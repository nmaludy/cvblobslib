/**
 * @author N. Maludy
 * @date 12/30/2013
 */
#include <cvblobs2/Blob.h>
#include <cvblobs2/BlobOperators.h>

CVBLOBS_BEGIN_NAMESPACE;

Blob::Blob()
    : mpStorage(NULL),
      mExternalContour(),
      mInternalContours(),
      mProperties(),
      mId(0),
      mBoundingBox(cvRect(-1, -1, -1, -1)),
      mOriginalImageSize(cvSize(-1, -1))
{}

Blob::Blob(t_labelType id, CvPoint startPoint, CvSize originalImageSize)
    : mpStorage(cvCreateMemStorage()),
      mExternalContour(startPoint, mpStorage),
      mInternalContours(),
      mProperties(),
      mId(id),
      mBoundingBox(cvRect(-1, -1, -1, -1)),
      mOriginalImageSize(originalImageSize)
{}

Blob::Blob(const Blob& src)
{
	mpStorage = NULL;
	*this = src;
}

Blob::Blob(const Blob* pSrc)
{
	if (pSrc != NULL &&
      this != pSrc)
	{
		mpStorage = NULL;
		*this = *pSrc;
	}
}

Blob& Blob::operator=(const Blob &src)
{
	if (this != &src)
	{
		mId = src.mId;
		mOriginalImageSize = src.mOriginalImageSize;
		mBoundingBox = src.mBoundingBox;

		// copy all blob properties
    mProperties = src.mProperties;

		// clear all current blob contours
		clearContours();
		
		if (mpStorage != NULL)
    {
			cvReleaseMemStorage(&mpStorage);
    }

		mpStorage = cvCreateMemStorage();

		mExternalContour = BlobContour(src.mExternalContour.startPoint(),
                                   mpStorage);
		if (src.mExternalContour.mpContour != NULL)
    {
			mExternalContour.mpContour = cvCloneSeq(src.mExternalContour.mpContour,
                                              mpStorage);
    }
		mInternalContours.clear();

		// copy all internal contours
		if (!src.mInternalContours.empty())
		{
			mInternalContours = t_contourList( src.mInternalContours.size() );
			t_contourList::const_iterator itSrc;
			t_contourList::iterator it;

			itSrc = src.mInternalContours.begin();
			it = mInternalContours.begin();

			while (itSrc != src.mInternalContours.end())
			{
				*it = BlobContour((*itSrc).startPoint(), mpStorage);
				if ((*itSrc).mpContour != NULL)
        {
					(*it).mpContour = cvCloneSeq( (*itSrc).mpContour, mpStorage );
        }

				++it;
				++itSrc;
			}
		}
	}

	return *this;
}

Blob::~Blob()
{
	clearContours();

	mProperties.clear();
	
	if (mpStorage != NULL)
  {
		cvReleaseMemStorage( &mpStorage );
  }
}

void Blob::clearContours()
{
	for (t_contourList::iterator it = mInternalContours.begin();
       it != mInternalContours.end();
       ++it)
	{
		(*it).resetChainCode();
	}	
	mInternalContours.clear();
	mExternalContour.resetChainCode();
		
}
void Blob::addInternalContour(const BlobContour& newContour)
{
	mInternalContours.push_back(newContour);
}

//! Indica si el blob està buit ( no té cap info associada )
//! Shows if the blob has associated information
bool Blob::isEmpty()
{
	return externalContour()->mpContour == NULL;
}

/**
   - FUNCIÓ: Area
   - FUNCIONALITAT: Get blob area, ie. external contour area minus internal contours area
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
double Blob::area()
{
  // total area of external contour
	double area = mExternalContour.area();

  // minus all of the internal contours
	for (t_contourList::iterator it_contour = mInternalContours.begin();
       it_contour != mInternalContours.end();
       ++it_contour)
	{
		area -= (*it_contour).area();
	}
	return area;
}

/**
   - FUNCIÓ: Perimeter
   - FUNCIONALITAT: Get blob perimeter, ie. sum of the lenght of all the contours
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
double Blob::perimeter()
{
  // perimeter of external contour
	double perimeter = mExternalContour.perimeter();

  // plus perimeter of all internal contours
	for (t_contourList::iterator it_contour = mInternalContours.begin();
       it_contour != mInternalContours.end();
       ++it_contour)
	{
		perimeter += (*it_contour).perimeter();
	}
	return perimeter;
}

/**
   - FUNCIÓ: Exterior
   - FUNCIONALITAT: Return true for extern blobs
   - PARÀMETRES:
   - xBorder: true to consider blobs touching horizontal borders as extern
   - yBorder: true to consider blobs touching vertical borders as extern
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/05/06
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
int	Blob::exterior(IplImage* pMask, 
                   bool bXBorderLeft   /* = true */,
                   bool bXBorderRight  /* = true */,
                   bool bYBorderTop    /* = true */,
                   bool bYBorderBottom /* = true */)
{
	if (externPerimeter(pMask,
                      bXBorderLeft, bXBorderRight,
                      bYBorderTop,  bYBorderBottom) > 0)
	{
		return 1;
	}
	return 0;	 
}

/**
   - FUNCIÓ: ExternPerimeter
   - FUNCIONALITAT: Get extern perimeter (perimeter touching image borders)
   - PARÀMETRES:
   - maskImage: if != NULL, counts maskImage black pixels as external pixels and contour points touching
   them are counted as external contour points.
   - xBorderLeft: true to consider blobs touching left horizontal borders as extern
   - xBorderRight: true to consider blobs touching right horizontal borders as extern
   - yBorderTop: true to consider blobs touching vertical top borders as extern
   - yBorderBottom: true to consider blobs touching vertical bottom borders as extern
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/05/05
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: If BlobContour::GetContourPoints aproximates contours with a method different that NONE,
   this function will not give correct results
*/
double Blob::externPerimeter(IplImage* pMaskImage, 
                             bool bXBorderLeft   /* = true */,
                             bool bXBorderRight  /* = true */,
                             bool bYBorderTop    /* = true */,
                             bool bYBorderBottom /* = true */)
{
	t_PointList p_extern_contour;
  t_PointList p_external_points;
	CvSeqReader reader;
	CvSeqWriter writer;
	CvPoint actual_point;
  CvPoint previous_point;
	bool find = false;
	int delta = 0;
	double extern_perimeter = 0.0;
	
	// get contour pixels
	p_extern_contour = mExternalContour.contourPoints();

	// there are contour pixels?
	if (p_extern_contour == NULL)
	{
		return 0;
	}

	cvStartReadSeq(p_extern_contour, &reader);

	// create a sequence with the external points of the blob
	p_external_points = cvCreateSeq(p_extern_contour->flags,
                                  p_extern_contour->header_size,
                                  p_extern_contour->elem_size, 
                                  mpStorage);
	cvStartAppendToSeq( p_external_points, &writer );
	previous_point.x = -1;

	// which contour pixels touch border?
	for (int j = 0; j < p_extern_contour->total; ++j)
	{
		CV_READ_SEQ_ELEM( actual_point, reader);

		find = false;

		// pixel is touching border?
		if ((bXBorderLeft   && (actual_point.x == 0)) || 
        (bXBorderRight  && (actual_point.x == mOriginalImageSize.width - 1 )) ||
        (bYBorderTop    && (actual_point.y == 0)) || 
        (bYBorderBottom && (actual_point.y == mOriginalImageSize.height - 1 )))
		{
			find = true;
		}
		else
		{
			if (pMaskImage != NULL)
			{
				// verify if some of 8-connected neighbours is black in mask
				char* p_mask;
				
				p_mask = (pMaskImage->imageData + actual_point.x - 1 +
                  (actual_point.y - 1) * pMaskImage->widthStep);
				
				for (int i = 0; i < 3; ++i, ++p_mask)
				{
					if (*p_mask == 0 && !find) 
					{
						find = true;
						break;
					}						
				}
				
				if (!find)
				{
					p_mask = (pMaskImage->imageData + actual_point.x - 1 +
                    (actual_point.y ) * pMaskImage->widthStep);
				
					for (int i = 0; i < 3; ++i, ++p_mask)
					{
						if (*p_mask == 0 && !find)
						{
							find = true;
							break;
						}
					}
				}
			
				if (!find)
				{
					p_mask = (pMaskImage->imageData + actual_point.x - 1 +
                    (actual_point.y + 1) * pMaskImage->widthStep);

					for (int i = 0; i < 3; ++i, ++p_mask)
					{
						if (*p_mask == 0 && !find)
						{
							find = true;
							break;
						}
					}
				}
			}
		}

		if (find)
		{
			if (previous_point.x > 0)
      {
				delta = (abs(previous_point.x - actual_point.x) +
                 abs(previous_point.y - actual_point.y));
      }
      
			// calculate separately each external contour segment 
			if (delta > 2)
			{
				cvEndWriteSeq( &writer );
				extern_perimeter += cvArcLength( p_external_points, CV_WHOLE_SEQ, 0 );
				
				cvClearSeq( p_external_points );
				cvStartAppendToSeq( p_external_points, &writer );
				delta = 0;
				previous_point.x = -1;
			}

			CV_WRITE_SEQ_ELEM( actual_point, writer );
			previous_point = actual_point;
		}
	}

	cvEndWriteSeq( &writer );
	cvSeqSort(p_external_points, cmp_func, 0);
	extern_perimeter += cvArcLength( p_external_points, CV_WHOLE_SEQ, 0 );

	cvClearSeq( p_external_points );

	// divide by two because external points have one side inside the blob and
  // the other outside Perimeter of external points counts both sides, so it
  // must be divided
	extern_perimeter /= 2.0;

	return extern_perimeter;
}

//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
double Blob::moment(int p, int q)
{
  // external moment
	double moment = mExternalContour.moment(p,q);

  // minus all of the internal moments
	for (t_contourList::iterator it_contour = mInternalContours.begin();
       it_contour != mInternalContours.end();
       ++it_contour)
	{
		moment -= (*it_contour).moment(p,q);
	}
	return moment;
}

/**
   - FUNCIÓ: Mean
   - FUNCIONALITAT: Get blob mean color in input image
   - PARÀMETRES:
   - image: image from gray color are extracted
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/05/06
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
double Blob::mean(IplImage* pImage)
{
	double mean   = 0.0;
  double stddev = 0.0;
	
	// call mean calculation (where also standard deviation is calculated)
	meanAndStdDev(pImage, mean, stddev);

	return mean;
}

double Blob::stdDev(IplImage* pImage)
{
	double mean   = 0.0;
  double stddev = 0.0;
	
	// call mean calculation (where also standard deviation is calculated)
	meanAndStdDev(pImage, mean, stddev);

	return stddev;
}

//! Calculates mean and std deviation of blob in input image
void Blob::meanAndStdDev(IplImage* pImage, double& mean, double& stddev)
{
	IplImage* p_mask = NULL;
	CvScalar meanscalar;
  CvScalar stdscalar;
	CvPoint offset;

	// Create a mask with same size as blob bounding box
	boundingBox();

	if (mBoundingBox.height <= 0 ||
      mBoundingBox.width <= 0 ||
      !CV_IS_IMAGE( pImage ) ||
      mBoundingBox.height > pImage->height ||
      mBoundingBox.width > pImage->width)
	{
		mean = 0.0;
		stddev = 0.0;
		return;
	}

	// apply ROI and mask to input image to compute mean gray and standard deviation
	p_mask = cvCreateImage( cvSize(mBoundingBox.width, mBoundingBox.height),
                          IPL_DEPTH_8U, 1);
	cvSetZero(p_mask);

	offset.x = -mBoundingBox.x;
	offset.y = -mBoundingBox.y;

	// draw contours on mask
	cvDrawContours( p_mask,
                  mExternalContour.contourPoints(),
                  CV_RGB(255,255,255),
                  CV_RGB(255,255,255),
                  0,
                  CV_FILLED,
                  8,
                  offset );

	// draw internal contours
	for (t_contourList::iterator it = mInternalContours.begin();
       it != mInternalContours.end();
       ++it)
	{
		cvDrawContours( p_mask,
                    (*it).contourPoints(),
                    CV_RGB(0, 0, 0),
                    CV_RGB(0, 0, 0),
                    0,
                    CV_FILLED,
                    8,
                    offset );
	}

	cvSetImageROI( pImage, mBoundingBox );
	cvAvgSdv( pImage, &meanscalar, &stdscalar, p_mask );
	
	mean   = meanscalar.val[0];
	stddev = stdscalar.val[0];

	cvReleaseImage( &p_mask );
	cvResetImageROI( pImage );
}

/**
   - FUNCIÓ: GetBoundingBox
   - FUNCIONALITAT: Get bounding box (without rotation) of a blob
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/05/06
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
CvRect Blob::boundingBox()
{
	// it is calculated?
  /*	if( m_boundingBox.width != -1 )
      {
      return m_boundingBox;
      }
  */
  
	// get contour pixels
	t_PointList extern_contour = mExternalContour.contourPoints();
  
	CvSeqReader reader;
	CvPoint actual_point;	
	
	// it is an empty blob?
	if (extern_contour == NULL)
	{
		mBoundingBox.x = 0;
		mBoundingBox.y = 0;
		mBoundingBox.width = 0;
		mBoundingBox.height = 0;

		return mBoundingBox;
	}

	cvStartReadSeq( extern_contour, &reader);

	mBoundingBox.x = mOriginalImageSize.width;
	mBoundingBox.y = mOriginalImageSize.height;
	mBoundingBox.width  = 0;
	mBoundingBox.height = 0;

	for (int i = 0; i < extern_contour->total; ++i)
	{
		CV_READ_SEQ_ELEM( actual_point, reader);

		mBoundingBox.x = MIN( actual_point.x, mBoundingBox.x );
		mBoundingBox.y = MIN( actual_point.y, mBoundingBox.y );
		
		mBoundingBox.width  = MAX( actual_point.x, mBoundingBox.width );
		mBoundingBox.height = MAX( actual_point.y, mBoundingBox.height );
	}

	//mBoundingBox.x = max( mBoundingBox.x , 0 );
	//mBoundingBox.y = max( mBoundingBox.y , 0 );

	mBoundingBox.width  -= mBoundingBox.x + 1;
	mBoundingBox.height -= mBoundingBox.y + 1;
	
	return mBoundingBox;
}

/**
   - FUNCIÓ: GetEllipse
   - FUNCIONALITAT: Calculates bounding ellipse of external contour points
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/05/06
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: Calculation is made using second order moment aproximation
*/
CvBox2D Blob::ellipse()
{
	CvBox2D elipse;
	double u00;
  double u11;
  double u01;
  double u10;
  double u20;
  double u02;
  double delta;
  double temp;

	// it is calculated?
	if (mProperties.find(BlobGetMajorAxisLength().GetNom()) != mProperties.end())
	{
		// build a ellipse from calculated properties
		elipse.size.width = mProperties[BlobGetMajorAxisLength().GetNom()];
		elipse.size.height = mProperties[BlobGetMinorAxisLength().GetNom()];
		elipse.angle = mProperties[BlobGetOrientation().GetNom()];
		elipse.center.x = mProperties[BlobGetElipseXCenter().GetNom()];
		elipse.center.y = mProperties[BlobGetElipseYCenter().GetNom()];
	}	

	// initialize properties
	mProperties[BlobGetMajorAxisLength().GetNom()] = 0;
	mProperties[BlobGetMinorAxisLength().GetNom()] = 0;
	mProperties[BlobGetOrientation().GetNom()] = 0;
	mProperties[BlobGetElipseXCenter().GetNom()] = 0;
	mProperties[BlobGetElipseYCenter().GetNom()] = 0;
	elipse.angle = 0;
	elipse.size.height = 0;
	elipse.size.width = 0;
	elipse.center.x = 0;
	elipse.center.y = 0;
	
	// central moments calculation
	u00 = moment(0,0);

	// empty blob?
	if (u00 <= 0)
	{
		return elipse;
	}

	u10 = moment(1,0) / u00;
	u01 = moment(0,1) / u00;

	u11 = -(moment(1,1) - moment(1,0) * moment(0,1) / u00 ) / u00;
	u20 = (moment(2,0) - moment(1,0) * moment(1,0) / u00 ) / u00;
	u02 = (moment(0,2) - moment(0,1) * moment(0,1) / u00 ) / u00;


	// elipse calculation
	delta = sqrt( 4*u11*u11 + (u20-u02)*(u20-u02) );
	elipse.center.x = u10;
	elipse.center.y = u01;

	mProperties[BlobGetElipseXCenter().GetNom()] = elipse.center.x;
	mProperties[BlobGetElipseYCenter().GetNom()] = elipse.center.y;
	
	temp = u20 + u02 + delta;
	if (temp > 0)
	{
		elipse.size.width = sqrt( 2*(u20 + u02 + delta ));
		mProperties[BlobGetMajorAxisLength().GetNom()] = elipse.size.width;
	}	
	else
	{
		return elipse;
	}

	temp = u20 + u02 - delta;
	if( temp > 0 )
	{
		elipse.size.height = sqrt( 2*(u20 + u02 - delta ) );
		mProperties[BlobGetMinorAxisLength().GetNom()] = elipse.size.height;
	}
	else
	{
		return elipse;
	}

	// elipse orientation
	elipse.angle = atan2( 2*u11, u20 - u02 + sqrt( (u20 - u02) * (u20 - u02) +
                                                 (2*u11) * (2*u11)) );

	if( elipse.angle == 0 && u02 > u20 )
	{
		elipse.angle = CV_PI/2.0;
	}
	// convert to degrees
	elipse.angle = (180.0 / CV_PI) *elipse.angle;
	elipse.angle+=180;
	mProperties[BlobGetOrientation().GetNom()] = elipse.angle;

	return elipse;
}

/**
   - FUNCTION: FillBlob
   - FUNCTIONALITY: 
   - Fills the blob with a specified colour
   - PARAMETERS:
   - imatge: where to paint
   - color: colour to paint the blob
   - RESULT:
   - modifies input image and returns the seed point used to fill the blob
   - RESTRICTIONS:
   - AUTHOR: Ricard Borràs
   - CREATION DATE: 25-05-2005.
   - MODIFICATION: Date. Author. Description.
*/
void Blob::fillBlob(IplImage* pImage, CvScalar color) 					  
{
	cvDrawContours( pImage,
                  mExternalContour.contourPoints(),
                  color,
                  color,
                  0,
                  CV_FILLED,
                  8 );
}


/**
   - FUNCTION: GetConvexHull
   - FUNCTIONALITY: Calculates the convex hull polygon of the blob
   - PARAMETERS:
   - dst: where to store the result
   - RESULT:
   - true if no error ocurred
   - RESTRICTIONS:
   - AUTHOR: Ricard Borràs
   - CREATION DATE: 25-05-2005.
   - MODIFICATION: Date. Author. Description.
*/
t_PointList Blob::convexHull()
{
	CvSeq* p_convex_hull = NULL;
	if (mExternalContour.contourPoints())
  {
		p_convex_hull = cvConvexHull2( mExternalContour.contourPoints(),
                                   mpStorage,
                                   CV_COUNTER_CLOCKWISE,
                                   1);
  }
	return p_convex_hull;
}

/**
   - FUNCTION: JoinBlob
   - FUNCTIONALITY: Add's external contour to current external contour
   - PARAMETERS:
   - blob: blob from which extract the added external contour
   - RESULT:
   - true if no error ocurred
   - RESTRICTIONS: Only external contours are added
   - AUTHOR: Ricard Borràs
   - CREATION DATE: 25-05-2005.
   - MODIFICATION: Date. Author. Description.
*/
void Blob::joinBlob(Blob* pBlob)
{
	CvSeqWriter writer;
	CvSeqReader reader;
	t_chainCode chainCode;

	cvStartAppendToSeq( mExternalContour.chainCode(), &writer );
	cvStartReadSeq( pBlob->externalContour()->chainCode(), &reader );

	for (int i = 0; i < pBlob->externalContour()->chainCode()->total; ++i)
	{
		CV_READ_SEQ_ELEM( chainCode, reader );
		CV_WRITE_SEQ_ELEM( chainCode, writer );
	}	
	cvEndWriteSeq( &writer );

	// reset stats for the blob
	mProperties.clear();
}

CVBLOBS_END_NAMESPACE;
