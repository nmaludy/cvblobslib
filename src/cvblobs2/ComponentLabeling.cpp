#include <cvblobs2/ComponentLabeling.h>

#include <cvblobs2/counted_ptr.h>
#include <cvblobs2/Blob.h>

namespace {

/**
- FUNCIÓ: 
- FUNCIONALITAT: 
- PARÀMETRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACIÓ: 2008/04/29
- MODIFICACIÓ: Data. Autor. Descripció.
*/
inline unsigned char GET_ABOVE_IMAGEPIXEL(unsigned char* pCurrentPixel,
                                          IplImage* pImage)
{
	return *(pCurrentPixel - pImage->widthStep);
}
inline unsigned char GET_BELOW_IMAGEPIXEL(unsigned char* pCurrentPixel,
                                          IplImage* pImage)
{
	return *(pCurrentPixel + pImage->widthStep);
}


inline unsigned char GET_IMAGE_PIXEL(IplImage *pImage, CvPoint p)
{
	return *(pImage->imageData + p.x + (p.y * pImage->widthStep));
}

inline bool GET_IMAGEMASK_PIXEL(IplImage* pMask, CvPoint p)
{
	if (pMask == NULL)
  {
    return true;
  }
  return ((unsigned char)*(pMask->imageData + p.x + (p.y * pMask->widthStep))) > 0;
}
inline bool GET_BELOW_VISITEDPIXEL(bool* pCurrentPixel, int imageWidth)
{
	return *(pCurrentPixel + imageWidth);
}

/**
   - FUNCIÓ: ASSIGN_LABEL
   - FUNCIONALITAT: Assigns label value to label image
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/29
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
inline void ASSIGN_LABEL(CvPoint p,
                         cvblobs::LabelType* pLabels,
                         int imageWidth,
                         int newLabel)
{
	*(pLabels + p.y * imageWidth + p.x) = newLabel;
}


inline void ASSIGN_VISITED(CvPoint p,
                           bool* pVisitedPoints,
                           int imageWidth)
{
	*(pVisitedPoints + p.y * imageWidth + p.x) = true;
}

inline unsigned char* rowPtr(IplImage* pImage, int row)
{
  return (unsigned char*) pImage->imageData + (row * pImage->widthStep);
}

} // end anonymous namespace

CVBLOBS_BEGIN_NAMESPACE
/**
   - FUNCIÓ: ComponentLabeling
   - FUNCIONALITAT: Calcula els components binaris (blobs) d'una imatge amb connectivitat a 8
   - PARÀMETRES:
   - inputImage: image to segment (pixel values different than blobColor are treated as background)
   - maskImage: if not NULL, all the pixels equal to 0 in mask are skipped in input image
   - backgroundColor: color of background (ignored pixels)
   - blobs: blob vector destination
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/21
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: Algorithm based on "A linear-time component labeling algorithm using contour tracing technique", 
   F.Chang et al
*/
bool ComponentLabeling(IplImage* pInputImage,
                       IplImage* pMaskImage,
                       unsigned char backgroundColor,
                       BlobContainer& blobs)
{
  bool b_internal_contour = false;
  bool b_external_contour = false;

  // pointers into the input image
	unsigned char* p_input_image_iter       = NULL;
  unsigned char* p_above_input_image_iter = NULL;
  unsigned char* p_below_input_image_iter = NULL;

  // pointers into the mask image
  unsigned char* p_mask_iter = NULL;
  unsigned char* p_above_mask_iter = NULL;
  unsigned char* p_below_mask_iter = NULL;
  
  //! current blob pointer
  BlobPtrType p_current_blob;
  cv::Point current_point;

	// verify input image
  if (!CV_IS_IMAGE(pInputImage))
  {
		return false;
  }

	// verify that input image and mask image has same size
	if (pMaskImage != NULL)
	{
		if (!CV_IS_IMAGE(pMaskImage) || 
        pMaskImage->width != pInputImage->width || 
        pMaskImage->height != pInputImage->height)
    {
      return false;
    }
	}
	else
	{
		p_mask_iter = NULL;
		p_above_mask_iter = NULL;
		p_below_mask_iter = NULL;
	}

	const cv::Size image_sizes = cvSize(pInputImage->width, pInputImage->height);
	const int image_width  = pInputImage->width;
	const int image_height = pInputImage->height;

  const int num_pixels = pInputImage->width * pInputImage->height;
  
	// row major vector with labelled image (deleted at end of function)
	LabelType* p_labels = new LabelType[num_pixels];
	memset(p_labels, 0, num_pixels * sizeof(LabelType) ) ;
  LabelType* p_labels_iter = p_labels;

  // row major vector with visited points (deleted at end of function)
	bool* p_visited_points = new bool[num_pixels];
	memset(p_visited_points, false, num_pixels * sizeof(bool) ) ;
  bool* p_visited_points_iter = p_visited_points;

	// start labelling with 1 being the first label ID
  int current_label = 1;
  int contour_label = 0;

	for (int row = 0; row < image_height; ++row)
	{
		// don't verify if we area on first or last row,
    // it will verified on pointer access
		p_input_image_iter       = rowPtr(pInputImage, row);
		p_above_input_image_iter = rowPtr(pInputImage, row - 1);
		p_below_input_image_iter = rowPtr(pInputImage, row + 1);

		if (pMaskImage != NULL)
		{
			// don't verify if we area on first or last row,
      // it will verified on pointer access
			p_mask_iter       = rowPtr(pMaskImage, row);
			p_above_mask_iter = rowPtr(pMaskImage, row - 1);
			p_below_mask_iter = rowPtr(pMaskImage, row + 1);
		}
		
		for (int col = 0; col < image_width; ++col,
             ++p_input_image_iter,
             ++p_above_input_image_iter, ++p_below_input_image_iter,
             ++p_mask_iter,
             ++p_above_mask_iter, ++p_below_mask_iter)
		{
			// ignore background pixels or 0 pixels in mask
			if (*p_input_image_iter == backgroundColor ||
           (pMaskImage != NULL &&
            *p_mask_iter == 0 ))
			{
				++p_labels_iter;
				++p_visited_points_iter;
				continue;
			}
			
			// new external contour: current label == 0 and above pixel is background
			if (row > 0 )
			{
				b_external_contour = ((*p_above_input_image_iter == backgroundColor) || 
                           (pMaskImage && *p_above_mask_iter == 0)) && 
            (*p_labels_iter == 0);
			}
			else
      {
				b_external_contour = (*p_labels_iter == 0);
      }

			// new internal contour: below pixel is background and not visited
			if (!b_external_contour && row < image_height - 1)
			{
				b_internal_contour = *p_below_input_image_iter == backgroundColor &&
            !GET_BELOW_VISITEDPIXEL( p_visited_points_iter, image_width);
			}
			else
			{
				b_internal_contour = false;
			}
			
			
			if (b_external_contour)
			{
				current_point = cvPoint(col, row); // (x, y)
				// assign label to labelled image
				*p_labels_iter = current_label;
				
				// create new blob
				p_current_blob =
            BlobPtrType( new Blob(current_label, current_point, image_sizes ) );

				// contour tracing with current_label
				contourTracing(pInputImage,
                       pMaskImage,
                       current_point, 
                       p_labels,
                       p_visited_points, 
                       current_label,
                       false,
                       backgroundColor,
                       p_current_blob->externalContour());

				// add new created blob
				blobs.push_back(p_current_blob);

				++current_label;
			} // end if (b_external_contour)
			else 
			{
				if (b_internal_contour)
				{
          current_point = cvPoint(col, row); // (x, y)

					if (*p_labels_iter == 0)
					{
						// take left neightbour value as current
						if (col > 0)
            {
							contour_label = *(p_labels_iter - 1);
            }
					}
					else
					{
						contour_label = *p_labels_iter;
					}

					if (contour_label > 0)
					{
						p_current_blob = blobs[contour_label - 1];
						BlobContour new_contour(current_point);
						
						// contour tracing with contour_label
						contourTracing(pInputImage,
                           pMaskImage,
                           current_point,
                           p_labels,
                           p_visited_points,
                           contour_label,
                           true,
                           backgroundColor,
                           &new_contour); 

						p_current_blob->addInternalContour(new_contour);
					}
				} // end if (b_internal_contour)
				// neither internal nor external contour
				else
				{
					// take left neightbour value as current if it is not labelled
					if (col > 0 && *p_labels_iter == 0)
          {
						*p_labels_iter = *(p_labels_iter - 1);
          }
				}

			} // end else (if (b_external_contour))
			
			++p_labels_iter;
			++p_visited_points_iter;
		}
	}


 	// free auxiliary buffers
	delete [] p_labels;
	delete [] p_visited_points;

	return true;
}



/**
   - FUNCIÓ: 
   - FUNCIONALITAT: 
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/29
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
void contourTracing(IplImage* pImage,
                    IplImage* pMaskImage,
                    cv::Point contourStart,
                    LabelType* pLabels,
                    bool* pVisitedPoints,
                    LabelType label,
                    bool bInternalContour,
                    unsigned char backgroundColor,
                    BlobContour* pCurrentBlobContour)
{
  ChainCode movement = CHAIN_CODE_RIGHT;
	ChainCode initial_movement = CHAIN_CODE_RIGHT;
	if (bInternalContour)
	{
		initial_movement = CHAIN_CODE_DOWN_RIGHT; // old value: 3;
	}
	else
	{
		initial_movement = CHAIN_CODE_UP_LEFT; // old value: 7;
	}

  cv::Point tsecond = tracer(pImage,
                             pMaskImage,
                             contourStart,
                             pVisitedPoints,
                             initial_movement, 
                             backgroundColor,
                             movement); // out param
	
	// assign current label to tnext
	ASSIGN_LABEL( contourStart, pLabels, pImage->width, label );
	
	// contour corresponds to isolated pixel? 
	if (tsecond.x == contourStart.x &&
      tsecond.y == contourStart.y)
	{
		// we are finished with the contour
		return;
	}

	// add chain code to current contour
	pCurrentBlobContour->addChainCode(movement);
	
	// assign label to next point 
	ASSIGN_LABEL( tsecond, pLabels, pImage->width, label );
	
  cv::Point t     = tsecond;
  cv::Point tnext = tsecond;
	
	// while T is different than contourStart and Tnext is different than T
	// follow contour until start point is reached again
	while (t.x != contourStart.x ||
         t.y != contourStart.y || 
         tsecond.x != tnext.x ||
         tsecond.y != tnext.y)
	{
		
		t.x = tnext.x;
		t.y = tnext.y;
    // @todo remove nasty enum casts
		initial_movement = (ChainCode) (((int)movement + 5) % 8);
    
		// search for next contour point
		tnext = tracer(pImage,
                   pMaskImage,
                   t,
                   pVisitedPoints,
                   initial_movement, 
                   backgroundColor,
                   movement); // out param
		
		// assign label to contour point
		ASSIGN_LABEL( tnext, pLabels, pImage->width, label );

		// add chain code to current contour
		pCurrentBlobContour->addChainCode(movement);
	}
}

/**
   - FUNCIÓ: tracer
   - FUNCIONALITAT: Searches for next point of a contour
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/30
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
cv::Point tracer(IplImage* pImage,
                 IplImage* pMaskImage,
                 cv::Point point,
                 bool* pVisitedPoints,
                 ChainCode initialMovement,
                 unsigned char backgroundColor,
                 ChainCode& movement)
{
	int d;
  cv::Point next_point;
	
	for (d = 0; d <= 7; d++ )
	{
		movement = (ChainCode) (((int)initialMovement + d) % 8);

    next_point = movePoint(point, movement);

		// the point is inside image ?
		if (next_point.x < 0 || next_point.x >= pImage->width || 
        next_point.y < 0 || next_point.y >= pImage->height)
		{
			// try other movement
			continue;
		}
    
		// image has blobColor value in the new point?
		if (GET_IMAGE_PIXEL(pImage, next_point) != backgroundColor &&
        GET_IMAGEMASK_PIXEL(pMaskImage, next_point))
		{
			return next_point;
		}
		else
		{
			// mark point as visited
			ASSIGN_VISITED(next_point, pVisitedPoints, pImage->width);
		}
	}

	// no possible movement was found
	movement = CHAIN_CODE_INVALID;
  next_point = point;
	return next_point;
}

CVBLOBS_END_NAMESPACE
