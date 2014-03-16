/**
 * @author Ricard Borràs
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/15/2014
 */
#include <cvblobs2/BlobOperators.h>

// std
#include <limits>

// opencv
#include <opencv2/imgproc/imgproc.hpp>

// cvblobs
#include <cvblobs2/Blob.h>
#include <cvblobs2/BlobContour.h>

CVBLOBS_BEGIN_NAMESPACE

//////////////////
// BlobOperator //
//////////////////
BlobOperator::~BlobOperator()
{}

double BlobOperator::result(cv::Ptr<Blob> pBlob)
{
  double result;

  // verify if operator is calculated
  Blob::PropertiesType* p_props = pBlob->properties();
  Blob::PropertiesType::iterator iter = p_props->find(name());
  if (iter == p_props->end())
  {
    // if not calculate it and add it to blob properties
    result = operator()(pBlob);
    (*p_props)[name()] = result;
  }
  else
  {
    // if is calculate, get result (without calculating it again)
    result = iter->second;
  }

  return result;
}

BlobOperator::operator BlobOperator*()
{
  return (BlobOperator*)this;
}

///////////////
// BlobGetId //
///////////////
double BlobGetID::operator()(cv::Ptr<Blob> pBlob)
{ 
  return pBlob->id(); 
}

std::string BlobGetID::name()
{
  return "BlobGetID";
}

/////////////////
// BlobGetArea //
/////////////////
double BlobGetArea::operator()(cv::Ptr<Blob> pBlob)
{ 
  return pBlob->area(); 
}

std::string BlobGetArea::name()
{
  return "BlobGetArea";
}

//////////////////////
// BlobGetPerimeter //
//////////////////////
double BlobGetPerimeter::operator()(cv::Ptr<Blob> pBlob)
{ 
  return pBlob->perimeter(); 
}

std::string BlobGetPerimeter::name()
{
  return "BlobGetPerimeter";
}

/////////////////////
// BlobGetExterior //
/////////////////////
BlobGetExterior::BlobGetExterior()
    : mMask(),
      mbXBorderLeft(false),
      mbXBorderRight(false),
      mbYBorderTop(false),
      mbYBorderBottom(false)
{}
  
BlobGetExterior::BlobGetExterior(cv::Mat& mask, 
                                 bool bXBorderLeft, 
                                 bool bXBorderRight, 
                                 bool bYBorderTop,
                                 bool bYBorderBottom)
    : mMask(mask),
      mbXBorderLeft(bXBorderLeft),
      mbXBorderRight(bXBorderRight),
      mbYBorderTop(bYBorderTop),
      mbYBorderBottom(bYBorderBottom)
{}
  
double BlobGetExterior::operator()(cv::Ptr<Blob> pBlob)
{ 
  return pBlob->exterior(mMask,
                         mbXBorderLeft, mbXBorderRight,
                         mbYBorderTop,  mbYBorderBottom); 
}

/////////////////
// BlobGetMean //
/////////////////
BlobGetMean::BlobGetMean()
    : mImage()
{}

BlobGetMean::BlobGetMean(cv::Mat& image)
    : mImage(image)
{}

double BlobGetMean::operator()(cv::Ptr<Blob> pBlob)
{ 
  return pBlob->mean(mImage); 
}

///////////////////
// BlobGetStdDev //
///////////////////
BlobGetStdDev::BlobGetStdDev()
    : mImage()
{}
  
BlobGetStdDev::BlobGetStdDev(cv::Mat& image)
    : mImage(image)
{}
  
double BlobGetStdDev::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->stdDev(mImage);
}

//////////////////////////
// BlobGetReferenceMean //
//////////////////////////
BlobGetReferencedMean::BlobGetReferencedMean()
    : mImage(),
      mReference(0.0)
{}
  
BlobGetReferencedMean::BlobGetReferencedMean(cv::Mat& image, double reference)
    : mImage(image),
      mReference(reference)
{}

 
double BlobGetReferencedMean::operator()(cv::Ptr<Blob> pBlob)
{ 
  return mReference - pBlob->mean(mImage);
}

std::string BlobGetReferencedMean::name()
{
  return "BlobGetReferencedMean";
}

////////////////////////
// BlobGetCompactness //
////////////////////////
double BlobGetCompactness::operator()(cv::Ptr<Blob> pBlob)
{
  double area = pBlob->area();
	if (area != 0.0)
  {
		return pow(pBlob->perimeter(), 2.0) / (4.0 * CV_PI * area);
  }
	else
  {
		return 0.0;
  }
}

///////////////////
// BlobGetLength //
///////////////////
double BlobGetLength::operator()(cv::Ptr<Blob> pBlob)
{
  // @todo refactor common length and width computation
  
  double area      = pBlob->area();
  double perimeter = pBlob->perimeter();
  // @todo where does this function come from
	double tmp       = (perimeter * perimeter) - (16.0 * area);

  double width = 0.0;
	if (tmp > 0.0)
  {
		width = (perimeter + sqrt(tmp)) / 4.0;
    // @todo what does he mean by this?
    // error intrínsec en els càlculs de l'àrea i el perímetre
    // Translates to:
    // intrinsic error in calculating the area and perimeter
  }
	else
  {
		width = perimeter / 4.0;
  }

	if (width <= 0.0)
  {
    return 0.0;
  }
  
	double length = pBlob->area() / width;

	return std::max(length, width);
}

////////////////////
// BlobGetBreadth //
////////////////////
double BlobGetBreadth::operator()(cv::Ptr<Blob> pBlob)
{
  // @todo refactor common length and width computation
  
  double area      = pBlob->area();
  double perimeter = pBlob->perimeter();
  // @todo where does this function come from
	double tmp       = (perimeter * perimeter) - (16.0 * area);

  double width = 0.0;
	if (tmp > 0.0)
  {
		width = (perimeter + sqrt(tmp)) / 4.0;
    // @todo what does he mean by this?
    // error intrínsec en els càlculs de l'àrea i el perímetre
    // Translates to:
    // intrinsic error in calculating the area and perimeter
  }
	else
  {
		width = perimeter / 4.0;
  }

	if (width <= 0.0)
  {
    return 0.0;
  }
  
	double length = pBlob->area() / width;

	return std::min(length, width);
}

//////////////////
// BlobGetDiffX //
//////////////////
double BlobGetDiffX::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->boundingBox().width;
}

//////////////////
// BlobGetDiffY //
//////////////////
double BlobGetDiffY::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->boundingBox().height;
}

///////////////////
// BlobGetMoment //
///////////////////
BlobGetMoment::BlobGetMoment()
    : mP(0),
      mQ(0)
{}

BlobGetMoment::BlobGetMoment(int p, int q)
    : mP(p),
      mQ(q)
{}

double BlobGetMoment::operator()(cv::Ptr<Blob> pBlob)
{
	return pBlob->moment(mP, mQ);
}

//////////////////////////
// BlobGetHullPerimeter //
//////////////////////////
double BlobGetHullPerimeter::operator()(cv::Ptr<Blob> pBlob)
{
	PointContainerType convex_hull;
  pBlob->convexHull(convex_hull);
  double perimeter = 0.0;

  if (!convex_hull.empty())
  {
    perimeter = fabs( cv::arcLength(convex_hull, true) );  // true == closed
  }
  
	return perimeter;
}

/////////////////////
// BlobGetHullArea //
/////////////////////
double BlobGetHullArea::operator()(cv::Ptr<Blob> pBlob)
{
	PointContainerType convex_hull;
  pBlob->convexHull(convex_hull);
	double area = 0.0;

  if (!convex_hull.empty())
  {
		area = fabs( cv::contourArea(convex_hull) );
  }

	return area;
}

///////////////////////
// BlobGetMinXatMinY //
///////////////////////
double BlobGetMinXatMinY::operator()(cv::Ptr<Blob> pBlob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainerType& extern_contour =
      pBlob->externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainerType::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;
		if (actual_point.y == pBlob->minY() &&
        actual_point.x < result)
		{
			result = actual_point.x;
		}	
	}

	return result;
}

///////////////////////
// BlobGetMinYatMaxX //
///////////////////////
double BlobGetMinYatMaxX::operator()(cv::Ptr<Blob> pBlob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainerType& extern_contour =
      pBlob->externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainerType::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;	
		if (actual_point.x == pBlob->maxX() &&
        actual_point.y < result)
		{
			result = actual_point.y;
		}	
	}

	return result;
}

///////////////////////
// BlobGetMaxXatMaxY //
///////////////////////
double BlobGetMaxXatMaxY::operator()(cv::Ptr<Blob> pBlob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainerType& extern_contour =
      pBlob->externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainerType::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;

    if (actual_point.y == pBlob->maxY() &&
        actual_point.x > result)
		{
			result = actual_point.x;
		}	
	}

	return result;
}

///////////////////////
// BlobGetMaxYatMinX //
///////////////////////
double BlobGetMaxYatMinX::operator()(cv::Ptr<Blob> pBlob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainerType& extern_contour =
      pBlob->externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainerType::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;
    
		if (actual_point.x == pBlob->minX() &&
        actual_point.y > result)
		{
			result = actual_point.y;
		}	
	}

	return result;
}

/////////////////
// BlobGetMinX //
/////////////////
double BlobGetMinX::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->minX();
}

/////////////////
// BlobGetMaxX //
/////////////////
double BlobGetMaxX::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->maxX();
}

/////////////////
// BlobGetMinY //
/////////////////
double BlobGetMinY::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->minY();
}

/////////////////
// BlobGetMaxY //
/////////////////
double BlobGetMaxY::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->maxY();
}

///////////////////////
// BlobGetElongation //
///////////////////////
double BlobGetElongation::operator()(cv::Ptr<Blob> pBlob)
{
  // @todo refactor common length and width computation
  
  double area      = pBlob->area();
  double perimeter = pBlob->perimeter();
  // @todo where does this function come from
	double tmp       = (perimeter * perimeter) - (16.0 * area);

  double width = 0.0;
	if (tmp > 0.0)
  {
		width = (perimeter + sqrt(tmp)) / 4.0;
    // @todo what does he mean by this?
    // error intrínsec en els càlculs de l'àrea i el perímetre
    // Translates to:
    // intrinsic error in calculating the area and perimeter
  }
	else
  {
		width = perimeter / 4.0;
  }

	if (width <= 0.0)
  {
    return 0.0;
  }
  
	double length = pBlob->area() / width;

	double length_max = std::max(length, width);
	double width_min  = std::min(length, width);

	return length_max / width_min;
}

//////////////////////
// BlobGetRoughness //
//////////////////////
double BlobGetRoughness::operator()(cv::Ptr<Blob> pBlob)
{
	BlobGetHullPerimeter getHullPerimeter;
	double hull_perimeter = getHullPerimeter(pBlob);

	if (hull_perimeter != 0.0)
  {
		return pBlob->perimeter() / hull_perimeter;
  }
	return 0.0;
}

//////////////////////////////
// BlobGetDistanceFromPoint //
//////////////////////////////
BlobGetDistanceFromPoint::BlobGetDistanceFromPoint()
    : mPoint(0.0, 0.0)
{}

BlobGetDistanceFromPoint::BlobGetDistanceFromPoint(const double x,
                                                   const double y)
    : mPoint(x, y)
{}

BlobGetDistanceFromPoint::BlobGetDistanceFromPoint(const cv::Point& point)
    : mPoint()
{
  mPoint = point;
}

double BlobGetDistanceFromPoint::operator()(cv::Ptr<Blob> pBlob)
{
	BlobGetXCenter getXCenter;
	BlobGetYCenter getYCenter;

	double x_average = mPoint.x - getXCenter( pBlob );
	double y_average = mPoint.y - getYCenter( pBlob );

	return sqrt( (x_average * x_average) + (y_average * y_average) );
}

////////////////////////////
// BlobGetExternPerimeter //
////////////////////////////
BlobGetExternPerimeter::BlobGetExternPerimeter()
    : mMask(),
      mbXBorderLeft(false),
      mbXBorderRight(false),
      mbYBorderTop(false),
      mbYBorderBottom(false)
{}

BlobGetExternPerimeter::BlobGetExternPerimeter(cv::Mat& mask, 
                                               bool bXBorderLeft,
                                               bool bXBorderRight,
                                               bool bYBorderTop,
                                               bool bYBorderBottom)
    : mMask(mask),
      mbXBorderLeft(bXBorderLeft),
      mbXBorderRight(bXBorderRight),
      mbYBorderTop(bYBorderTop),
      mbYBorderBottom(bYBorderBottom)
{}

double BlobGetExternPerimeter::operator()(cv::Ptr<Blob> pBlob)
{
  return pBlob->externPerimeter(mMask,
                                mbXBorderLeft, mbXBorderRight,
                                mbYBorderTop,  mbYBorderBottom);
}

/////////////////////////////////
// BlobGetExternPerimeterRatio //
/////////////////////////////////
BlobGetExternPerimeterRatio::BlobGetExternPerimeterRatio()
    : mMask(),
      mbXBorderLeft(false),
      mbXBorderRight(false),
      mbYBorderTop(false),
      mbYBorderBottom(false)
{}

BlobGetExternPerimeterRatio::BlobGetExternPerimeterRatio(cv::Mat& mask,
                                                         bool bXBorderLeft,
                                                         bool bXBorderRight, 
                                                         bool bYBorderTop,
                                                         bool bYBorderBottom)
    : mMask(mask),
      mbXBorderLeft(bXBorderLeft),
      mbXBorderRight(bXBorderRight),
      mbYBorderTop(bYBorderTop),
      mbYBorderBottom(bYBorderBottom)
{}

double BlobGetExternPerimeterRatio::operator()(cv::Ptr<Blob> pBlob)
{
  const double perimeter = pBlob->perimeter();
  const double extern_perimeter =
      pBlob->externPerimeter(mMask,
                             mbXBorderLeft, mbXBorderRight,
                             mbYBorderTop, mbYBorderBottom);
  if (perimeter != 0.0)
  {
    return extern_perimeter / perimeter;
  }
  else
  {
    return extern_perimeter;
  }
}

/////////////////////////////////////
// BlobGetExternHullPerimeterRatio //
/////////////////////////////////////
BlobGetExternHullPerimeterRatio::BlobGetExternHullPerimeterRatio()
    : mMask(),
      mbXBorder(false),
      mbYBorder(false)
{}

BlobGetExternHullPerimeterRatio::BlobGetExternHullPerimeterRatio(cv::Mat& mask,
                                                                 bool bXBorder,
                                                                 bool bYBorder)
    : mMask(mask),
      mbXBorder(bXBorder),
      mbYBorder(bYBorder)
{}

double BlobGetExternHullPerimeterRatio::operator()(cv::Ptr<Blob> pBlob)
{
  BlobGetHullPerimeter getHullPerimeter;
  const double hull_perimeter   = getHullPerimeter(pBlob);
  const double extern_perimeter = pBlob->externPerimeter(mMask,
                                                         mbXBorder,
                                                         mbYBorder);
  if (hull_perimeter != 0.0)
  {
    return extern_perimeter / hull_perimeter;
  }
  else
  {
    return extern_perimeter;
  }
}

////////////////////
// BlobGetXCenter //
////////////////////
double BlobGetXCenter::operator()(cv::Ptr<Blob> pBlob)
{
  const double min_x = pBlob->minX();
  const double max_x = pBlob->maxX();
  return min_x + ((max_x - min_x) / 2.0);
}

////////////////////
// BlobGetYCenter //
////////////////////
double BlobGetYCenter::operator()(cv::Ptr<Blob> pBlob)
{
  const double min_y = pBlob->minY();
  const double max_y = pBlob->maxY();
  return min_y + ((max_y - min_y) / 2.0);
}

////////////////////////////
// BlobGetMajorAxisLength //
////////////////////////////
double BlobGetMajorAxisLength::operator()(cv::Ptr<Blob> pBlob)
{
  cv::RotatedRect elipse = pBlob->ellipse();
  return elipse.size.width;
}

////////////////////////////
// BlobGetAreaElipseRatio //
////////////////////////////
double BlobGetAreaElipseRatio::operator()(cv::Ptr<Blob> pBlob)
{
  const double area = pBlob->area();
  if (area == 0.0)
  {
    return 0.0;
  }

  const cv::RotatedRect elipse = pBlob->ellipse();
  // https://en.wikipedia.org/wiki/Ellipse#Area
  // area of ellipse = pi * A * B
  // A = 1/2 the "major" axis ie the width of the ellipse
  // B = 1/2 the "minor" axis ie the height of the ellipse
  const double ellipse_area = ( CV_PI *
                                (elipse.size.width / 2.0) *
                                (elipse.size.height / 2.0));
  const double ratioAreaElipseAreaTaca = ellipse_area / area;
  return ratioAreaElipseAreaTaca;
}

////////////////////////////
// BlobGetMinorAxisLength //
////////////////////////////
double BlobGetMinorAxisLength::operator()(cv::Ptr<Blob> pBlob)
{
  cv::RotatedRect elipse = pBlob->ellipse();
  return elipse.size.height;
}

////////////////////////
// BlobGetOrientation //
////////////////////////
double BlobGetOrientation::operator()(cv::Ptr<Blob> pBlob)
{
  cv::RotatedRect elipse = pBlob->ellipse();
  return elipse.angle;
}

//////////////////////////
// BlobGetElipseXCenter //
//////////////////////////
double BlobGetElipseXCenter::operator()(cv::Ptr<Blob> pBlob)
{
  cv::RotatedRect elipse = pBlob->ellipse();
  return elipse.center.x;
}

//////////////////////////
// BlobGetElipseYCenter //
//////////////////////////
double BlobGetElipseYCenter::operator()(cv::Ptr<Blob> pBlob)
{
  cv::RotatedRect elipse = pBlob->ellipse();
  return elipse.center.y;
}

///////////////////////////
// BlobGetOrientationCos //
///////////////////////////
double BlobGetOrientationCos::operator()(cv::Ptr<Blob> pBlob)
{
  BlobGetOrientation getOrientation;
  return fabs( cos(getOrientation(pBlob) * CVBLOBS_DEGREE2RAD) );
}

//////////////////////
// BlobGetAxisRatio //
//////////////////////
double BlobGetAxisRatio::operator()(cv::Ptr<Blob> pBlob)
{
  BlobGetMajorAxisLength getMajor;
  double major = getMajor(pBlob);

  if (major != 0.0)
  {
    BlobGetMinorAxisLength getMinor;
    double minor = getMinor(pBlob);
    return minor / major;
  }
  else
  {
    return 0.0;
  }
}

/////////////////////
// BlobGetXYInside //
/////////////////////

BlobGetXYInside::BlobGetXYInside()
    : mPoint(0, 0)
{}

BlobGetXYInside::BlobGetXYInside(const double& x, const double& y)
    : mPoint(x, y)
{}

BlobGetXYInside::BlobGetXYInside(const cv::Point& point)
    : mPoint()
{
  mPoint = point;
}

double BlobGetXYInside::operator()(cv::Ptr<Blob> pBlob)
{
  const PointContainerType& extern_contour =
      pBlob->externalContour()->contourPoints();
	if (!extern_contour.empty())
	{
    // false == don't measure distance
		return cv::pointPolygonTest(extern_contour, mPoint, false) >= 0;
	}

	return 0;
}

/////////////////////////
// BlobGetRelativeArea //
/////////////////////////
BlobGetRelativeArea::BlobGetRelativeArea()
    : mTotalArea(1.0),
      mTheoreticalArea(0)
{}

BlobGetRelativeArea::BlobGetRelativeArea(double totalArea, int theoreticalArea)
    : mTotalArea(totalArea),
      mTheoreticalArea(theoreticalArea)
{}
  
double BlobGetRelativeArea::operator()(cv::Ptr<Blob> pBlob)
{
  if (mTotalArea)
  {
    return (pBlob->area() / mTotalArea) * mTheoreticalArea;
  }
  return 0.0;
}


CVBLOBS_END_NAMESPACE
