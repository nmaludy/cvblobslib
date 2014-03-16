/**
 * @author N. Maludy
 * @date 12/30/2013
 */
#include <cvblobs2/Blob.h>
#include <cvblobs2/BlobContour.h>
#include <cvblobs2/BlobOperators.h>

CVBLOBS_BEGIN_NAMESPACE

Blob::Blob()
    : mpExternalContour(new BlobContour()),
      mInternalContours(),
      mProperties(),
      mId(0),
      mOriginalImageSize(std::numeric_limits<cv::Point::value_type>::min(),
                         std::numeric_limits<cv::Point::value_type>::min())
{}

Blob::Blob(LabelType id,
           const cv::Point& startPoint,
           const cv::Size& originalImageSize)
    : mpExternalContour(new BlobContour(startPoint)),
      mInternalContours(),
      mProperties(),
      mId(id),
      mOriginalImageSize(originalImageSize)
{}

Blob::Blob(const Blob& source)
    : mpExternalContour(source.mpExternalContour),
      mInternalContours(source.mInternalContours),
      mProperties(source.mProperties),
      mId(source.mId),
      mOriginalImageSize(source.mOriginalImageSize)
{}

Blob::~Blob()
{
  // not responsible for any memory
}

Blob& Blob::operator=(const Blob& source)
{
	if (this != &source)
	{
    // copy-swap idiom
    Blob tmp(source);
    swap(tmp);
	}
	return *this;
}

//! swaps contents of this blob with other
void Blob::swap(Blob& other)
{
  std::swap(mpExternalContour,  other.mpExternalContour);
  std::swap(mInternalContours,  other.mInternalContours);
  std::swap(mProperties,        other.mProperties);
  std::swap(mId,                other.mId);
  std::swap(mOriginalImageSize, other.mOriginalImageSize);
}

void Blob::clear()
{
  ContourContainerType::iterator end_iter = mInternalContours.end();
	for (ContourContainerType::iterator iter = mInternalContours.begin();
       iter != end_iter;
       ++iter)
	{
		(*iter)->clear();
	}	
	mInternalContours.clear();
	mpExternalContour->clear();
  mProperties.clear();
}

void Blob::addInternalContour(cv::Ptr<BlobContour> pInternalContour)
{
	mInternalContours.push_back(pInternalContour);
}

//! Indica si el blob està buit ( no té cap info associada )
//! Shows if the blob has associated information
bool Blob::isEmpty()
{
	return externalContour()->isEmpty();
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
	double area = mpExternalContour->area();

  // minus all of the internal contours
  ContourContainerType::iterator end_iter = mInternalContours.end();
	for (ContourContainerType::iterator iter = mInternalContours.begin();
       iter != end_iter;
       ++iter)
	{
		area -= (*iter)->area();
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
	double perimeter = mpExternalContour->perimeter();

  // plus perimeter of all internal contours
  ContourContainerType::iterator end_iter = mInternalContours.end();
	for (ContourContainerType::iterator iter = mInternalContours.begin();
       iter != end_iter;
       ++iter)
	{
		perimeter += (*iter)->perimeter();
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
int	Blob::exterior(cv::Mat& mask, 
                   bool bXBorderLeft   /* = true */,
                   bool bXBorderRight  /* = true */,
                   bool bYBorderTop    /* = true */,
                   bool bYBorderBottom /* = true */)
{
	if (externPerimeter(mask,
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
double Blob::externPerimeter(cv::Mat& mask,
                             bool bXBorderLeft   /* = true */,
                             bool bXBorderRight  /* = true */,
                             bool bYBorderTop    /* = true */,
                             bool bYBorderBottom /* = true */)
{
  cv::Point previous_point;
	bool find = false;
	int delta = 0;
	double extern_perimeter = 0.0;
	
	// get contour pixels
	const PointContainerType& extern_contour = mpExternalContour->contourPoints();

	// there are contour pixels?
	if (extern_contour.empty())
	{
		return 0;
	}

  // @todo check Mask size

	// create a sequence with the external points of the blob
  // this must be a std::vector for use with cv::arcLength
  std::vector<cv::Point> external_points;
  external_points.reserve(extern_contour.size());
	previous_point.x = std::numeric_limits<cv::Point::value_type>::min();

	// which contour pixels touch border?
  PointContainerType::const_iterator end_iter = extern_contour.end();
	for (PointContainerType::const_iterator iter = extern_contour.begin();
       iter != end_iter;
       ++iter)
	{
		const cv::Point& actual_point = *iter;

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
			if (!mask.empty())
			{
				// verify if some of 8-connected neighbours is black in mask
				uchar* p_mask;
				
				p_mask = (mask.data + actual_point.x - 1 +
                  (actual_point.y - 1) * mask.step);
				
				for (int i = 0; i < 3; ++i, ++p_mask)
				{
					if (*p_mask == 0 && !find) 
					{
						find = true;
						break;
					}						
				} // end for each pixel
				
				if (!find)
				{
					p_mask = (mask.data + actual_point.x - 1 +
                    (actual_point.y ) * mask.step);
				
					for (int i = 0; i < 3; ++i, ++p_mask)
					{
						if (*p_mask == 0 && !find)
						{
							find = true;
							break;
						}
					} // end for each pixel
				} // end if (!find)
			
				if (!find)
				{
					p_mask = (mask.data + actual_point.x - 1 +
                    (actual_point.y + 1) * mask.step);

					for (int i = 0; i < 3; ++i, ++p_mask)
					{
						if (*p_mask == 0 && !find)
						{
							find = true;
							break;
						}
					} // end for each pixel
				} // end if (!find)
			} // end if pMaskImage != NULL
		} // end if pixel touching border

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
        // false == not closed
				extern_perimeter += cv::arcLength(external_points, false);
				
				external_points.clear();
				delta = 0;
				previous_point.x = std::numeric_limits<cv::Point::value_type>::min();
			}

			external_points.push_back(actual_point);
			previous_point = actual_point;
		} // end if (find)
	} // end for each point in extern_contour

  std::sort(external_points.begin(), external_points.end(),
            TopToBottomLeftToRightCmp());
  
  // false == not closed
	extern_perimeter += cv::arcLength(external_points, false);

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
	double moment = mpExternalContour->moment(p,q);

  // minus all of the internal moments
	ContourContainerType::iterator end_iter = mInternalContours.end();
	for (ContourContainerType::iterator iter = mInternalContours.begin();
       iter != end_iter;
       ++iter)
	{
		moment -= (*iter)->moment(p,q);
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
double Blob::mean(cv::Mat& image)
{
	double mean   = 0.0;
  double stddev = 0.0;
	
	// call mean calculation (where also standard deviation is calculated)
	meanAndStdDev(image, mean, stddev);

	return mean;
}

double Blob::stdDev(cv::Mat& image)
{
	double mean   = 0.0;
  double stddev = 0.0;
	
	// call mean calculation (where also standard deviation is calculated)
	meanAndStdDev(image, mean, stddev);

	return stddev;
}

//! Calculates mean and std deviation of blob in input image
void Blob::meanAndStdDev(cv::Mat& image, double& mean, double& stddev)
{
 	// Create a mask with same size as blob bounding box
  cv::Rect bounding_box = boundingBox();

	if (bounding_box.height <= 0 ||
      bounding_box.width <= 0 ||
      image.empty() ||
      bounding_box.height > image.size().height ||
      bounding_box.width > image.size().width)
	{
		mean = 0.0;
		stddev = 0.0;
		return;
	}

  cv::Point offset;

	// apply ROI and mask to input image to compute mean gray and standard deviation
  cv::Mat mask = cv::Mat::zeros(bounding_box.size(), CV_8UC1);

	offset.x = -bounding_box.x;
	offset.y = -bounding_box.y;

  std::vector<cv::Mat> contours(1);
  contours[0] = cv::Mat(mpExternalContour->contourPoints(), false);
  
	// draw contours on mask
  cv::drawContours(mask,
                   contours,
                   -1, // draw all contours
                   cv::Scalar(255, 255, 255), // fill color
                   CV_FILLED,     // thickness
                   8,             // line type
                   cv::noArray(), // heirarchy
                   0,             // draw only the specified contour
                   offset);       // offset to shift each point

	// draw internal contours
  ContourContainerType::iterator end_iter = mInternalContours.end();
	for (ContourContainerType::iterator iter = mInternalContours.begin();
       iter != end_iter;
       ++iter)
	{
    const PointContainerType& contour_points = (*iter)->contourPoints();
    contours[0] = cv::Mat(contour_points, false);
    cv::drawContours(mask,
                     contours,
                     -1, // draw all contours
                     cv::Scalar(0, 0, 0), // fill color
                     CV_FILLED,     // thickness
                     8,             // line type
                     cv::noArray(), // heirarchy
                     0,             // draw only the specified contour
                     offset);       // offset to shift each point
	}

  // extracts ROI from image
  cv::Mat roi = image(bounding_box);
  
  cv::Scalar meanscalar;
  cv::Scalar stdscalar;
  cv::meanStdDev(roi, meanscalar, stdscalar, mask);
	
	mean   = meanscalar[0];
	stddev = stdscalar[0];
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
cv::Rect Blob::boundingBox()
{
	return mpExternalContour->boundingBox();
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
cv::RotatedRect Blob::ellipse()
{
	cv::RotatedRect elipse;
	double u00;
  double u11;
  double u01;
  double u10;
  double u20;
  double u02;
  double delta;
  double temp;

	// it is calculated?
	if (mProperties.find(BlobGetMajorAxisLength().name()) != mProperties.end())
	{
		// build a ellipse from calculated properties
		elipse.size.width = mProperties[BlobGetMajorAxisLength().name()];
		elipse.size.height = mProperties[BlobGetMinorAxisLength().name()];
		elipse.angle = mProperties[BlobGetOrientation().name()];
		elipse.center.x = mProperties[BlobGetElipseXCenter().name()];
		elipse.center.y = mProperties[BlobGetElipseYCenter().name()];
	}	

	// initialize properties
	mProperties[BlobGetMajorAxisLength().name()] = 0;
	mProperties[BlobGetMinorAxisLength().name()] = 0;
	mProperties[BlobGetOrientation().name()] = 0;
	mProperties[BlobGetElipseXCenter().name()] = 0;
	mProperties[BlobGetElipseYCenter().name()] = 0;
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

	mProperties[BlobGetElipseXCenter().name()] = elipse.center.x;
	mProperties[BlobGetElipseYCenter().name()] = elipse.center.y;
	
	temp = u20 + u02 + delta;
	if (temp > 0)
	{
		elipse.size.width = sqrt( 2*(u20 + u02 + delta ));
		mProperties[BlobGetMajorAxisLength().name()] = elipse.size.width;
	}	
	else
	{
		return elipse;
	}

	temp = u20 + u02 - delta;
	if( temp > 0 )
	{
		elipse.size.height = sqrt( 2*(u20 + u02 - delta ) );
		mProperties[BlobGetMinorAxisLength().name()] = elipse.size.height;
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
	elipse.angle += 180;
	mProperties[BlobGetOrientation().name()] = elipse.angle;

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
void Blob::fillBlob(cv::Mat& image, const cv::Scalar& color) 					  
{
  const PointContainerType& contour_points = mpExternalContour->contourPoints();
  
  std::vector<cv::Mat> contours(1);
  contours[0] = cv::Mat(contour_points, false);
  cv::drawContours(image,
                   contours,
                   -1,            // draw all contours
                   color,         // fill color
                   CV_FILLED,     // thickness
                   8,             // line type
                   cv::noArray(), // heirarchy
                   0);            // draw only the specified contour
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
void Blob::convexHull(PointContainerType& hull)
{
  const PointContainerType& contour_points = mpExternalContour->contourPoints();
	if (contour_points.empty())
  {
    cv::convexHull(contour_points,
                   hull,
                   true);  // clockwise?
  }
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
void Blob::joinBlob(const Blob& blob)
{
  // @todo proper set join implementation
  ChainCodeContainerType& contour = mpExternalContour->mContour;
  const ChainCodeContainerType& src_contour = blob.mpExternalContour->mContour;
  contour.insert(contour.end(), src_contour.begin(), src_contour.end());

	// reset stats for the blob
	mProperties.clear();
}

CVBLOBS_END_NAMESPACE

namespace std {
template<>
void swap(cvblobs::Blob& lhs, cvblobs::Blob& rhs)
{
  lhs.swap(rhs);
}
} // namespace std

