/**
 * @author Ricard Borràs
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/15/2014
 */
#include <cvblobs2/ComponentLabeling.h>

#include <cvblobs2/Blob.h>
#include <cvblobs2/BlobContour.h>

namespace {

inline bool GET_IMAGEMASK_PIXEL(const cv::Mat& maskImage, const cv::Point& point)
{
	if (maskImage.empty())
  {
    return true;
  }
  // @todo make this more generic so we can use any type and channel combo
  // for our mask image
  return maskImage.at<unsigned char>(point) > 0;
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
inline void ASSIGN_LABEL(const cv::Point& point,
                         cvblobs::LabelType* pLabels,
                         int imageWidth,
                         int newLabel)
{
	*(pLabels + point.y * imageWidth + point.x) = newLabel;
}


inline void ASSIGN_VISITED(const cv::Point& point,
                           bool* pVisitedPoints,
                           int imageWidth)
{
	*(pVisitedPoints + point.y * imageWidth + point.x) = true;
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
bool ComponentLabeling(const cv::Mat& inputImage,
                       const cv::Mat& maskImage,
                       unsigned char backgroundColor,
                       BlobContainerType& blobs)
{
  bool b_internal_contour = false;
  bool b_external_contour = false;

  // pointers into the input image
	const unsigned char* p_input_image_iter       = NULL;
  const unsigned char* p_above_input_image_iter = NULL;
  const unsigned char* p_below_input_image_iter = NULL;

  // pointers into the mask image
  const unsigned char* p_mask_iter       = NULL;
  const unsigned char* p_above_mask_iter = NULL;
  const unsigned char* p_below_mask_iter = NULL;
  
  //! current blob pointer
  cv::Ptr<Blob> p_current_blob;
  cv::Point current_point;

	// verify input image
  if (inputImage.empty())
  {
		return false;
  }

  const cv::Size image_size = inputImage.size();
  const cv::Size mask_size  = maskImage.size();
  
	// verify that input image and mask image has same size
	if (!maskImage.empty())
	{
		if (mask_size != image_size)
    {
      return false;
    }
	}
	else
	{
		p_mask_iter       = NULL;
		p_above_mask_iter = NULL;
		p_below_mask_iter = NULL;
	}

  const int num_pixels = image_size.width * image_size.height;
  
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

	for (int row = 0; row < image_size.height; ++row)
	{
		// don't verify if we area on first or last row,
    // it will verified on pointer access
		p_input_image_iter       = inputImage.ptr(row);
		p_above_input_image_iter = inputImage.ptr(row - 1);
		p_below_input_image_iter = inputImage.ptr(row + 1);

		if (!maskImage.empty())
		{
			// don't verify if we area on first or last row,
      // it will verified on pointer access
			p_mask_iter       = maskImage.ptr(row);
			p_above_mask_iter = maskImage.ptr(row - 1);
			p_below_mask_iter = maskImage.ptr(row + 1);
		}
		
		for (int col = 0; col < image_size.width; ++col,
             ++p_input_image_iter,
             ++p_above_input_image_iter, ++p_below_input_image_iter,
             ++p_mask_iter,
             ++p_above_mask_iter, ++p_below_mask_iter)
		{
			// ignore background pixels or 0 pixels in mask
			if (*p_input_image_iter == backgroundColor ||
          (!maskImage.empty() &&
           *p_mask_iter == 0 ))
			{
				++p_labels_iter;
				++p_visited_points_iter;
				continue;
			}
			
			// new external contour: current label == 0 and above pixel is background
			if (row > 0 )
			{
				b_external_contour = ( ((*p_above_input_image_iter == backgroundColor) || 
                                (!maskImage.empty() &&
                                 *p_above_mask_iter == 0)) && 
                               (*p_labels_iter == 0));
			}
			else
      {
				b_external_contour = (*p_labels_iter == 0);
      }

			// new internal contour: below pixel is background and not visited
			if (!b_external_contour && row < image_size.height - 1)
			{
				b_internal_contour = *p_below_input_image_iter == backgroundColor &&
            !GET_BELOW_VISITEDPIXEL( p_visited_points_iter, image_size.width);
			}
			else
			{
				b_internal_contour = false;
			}
			
			
			if (b_external_contour)
			{
				current_point = cv::Point(col, row); // (x, y)
				// assign label to labelled image
				*p_labels_iter = current_label;
				
				// create new blob
				p_current_blob = cv::Ptr<Blob>(new Blob(current_label,
                                                current_point,
                                                image_size));
        
				// contour tracing with current_label
				contourTracing(inputImage,
                       maskImage,
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
          current_point = cv::Point(col, row); // (x, y)

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
            cv::Ptr<BlobContour> p_new_contour(new BlobContour(current_point));
						
						// contour tracing with contour_label
						contourTracing(inputImage,
                           maskImage,
                           current_point,
                           p_labels,
                           p_visited_points,
                           contour_label,
                           true,
                           backgroundColor,
                           p_new_contour); 

						p_current_blob->addInternalContour(p_new_contour);
					} // end if (contour_label > 0)
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
		} // for each column in image
	} // for each row in image


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
void contourTracing(const cv::Mat& inputImage,
                    const cv::Mat& maskImage,
                    const cv::Point& contourStart,
                    LabelType* pLabels,
                    bool* pVisitedPoints,
                    LabelType label,
                    bool bInternalContour,
                    unsigned char backgroundColor,
                    cv::Ptr<BlobContour> pCurrentBlobContour)
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

  const cv::Point tsecond = tracer(inputImage,
                                   maskImage,
                                   contourStart,
                                   pVisitedPoints,
                                   initial_movement, 
                                   backgroundColor,
                                   movement); // out param

  
  const cv::Size image_size = inputImage.size();
  
	// assign current label to tnext
	ASSIGN_LABEL( contourStart, pLabels, image_size.width, label );
	
	// contour corresponds to isolated pixel? 
	if (tsecond == contourStart)
	{
		// we are finished with the contour
		return;
	}

	// add chain code to current contour
	pCurrentBlobContour->addChainCode(movement);
	
	// assign label to next point 
	ASSIGN_LABEL( tsecond, pLabels, image_size.width, label );
	
  cv::Point t     = tsecond;
  cv::Point tnext = tsecond;
	
	// while T is different than contourStart and Tnext is different than T
	// follow contour until start point is reached again
	while (t != contourStart ||
         tsecond != tnext)
	{
		
		t = tnext;
		initial_movement = (ChainCode) (((int)movement + 5) % 8);
    
		// search for next contour point
		tnext = tracer(inputImage,
                   maskImage,
                   t,
                   pVisitedPoints,
                   initial_movement, 
                   backgroundColor,
                   movement); // out param
		
		// assign label to contour point
		ASSIGN_LABEL( tnext, pLabels, image_size.width, label );

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
cv::Point tracer(const cv::Mat& inputImage,
                 const cv::Mat& maskImage,
                 const cv::Point& point,
                 bool* pVisitedPoints,
                 ChainCode initialMovement,
                 unsigned char backgroundColor,
                 ChainCode& movement)
{
  cv::Point next_point;

  const cv::Size image_size = inputImage.size();
	
	for (int direction = 0; direction <= 7; ++direction)
	{
		movement = (ChainCode) (((int)initialMovement + direction) % 8);

    next_point = movePoint(point, movement);

		// the point is inside image ?
		if (next_point.x < 0 || next_point.x >= image_size.width || 
        next_point.y < 0 || next_point.y >= image_size.height)
		{
			// try other movement
			continue;
		}
    
		// image has blobColor value in the new point?
    // @todo support any type/depth combo 
		if (inputImage.at<unsigned char>(next_point) != backgroundColor &&
        GET_IMAGEMASK_PIXEL(maskImage, next_point))
		{
			return next_point;
		}
		else
		{
			// mark point as visited
			ASSIGN_VISITED(next_point, pVisitedPoints, image_size.width);
		}
	}

	// no possible movement was found
	movement = CHAIN_CODE_INVALID;
  next_point = point;
	return next_point;
}

CVBLOBS_END_NAMESPACE
