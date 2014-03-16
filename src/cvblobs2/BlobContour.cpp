#include <cvblobs2/BlobContour.h>

#include <cvblobs2/ChainCode.h>
#include <cvblobs2/SpatialMoments.h>

#include <limits>

CVBLOBS_BEGIN_NAMESPACE

BlobContour::BlobContour()
    : mContour(),
      mStartPoint(std::numeric_limits<cv::Point::value_type>::min(),
                  std::numeric_limits<cv::Point::value_type>::min()),
      mContourPoints(),
      mArea(std::numeric_limits<cv::Point::value_type>::min()),
      mPerimeter(std::numeric_limits<cv::Point::value_type>::min()),
      mMoments(),
      mBoundingBox(std::numeric_limits<cv::Point::value_type>::min(),
                   std::numeric_limits<cv::Point::value_type>::min(),
                   std::numeric_limits<cv::Point::value_type>::min(),
                   std::numeric_limits<cv::Point::value_type>::min())
{
	mMoments.m00 = std::numeric_limits<cv::Point::value_type>::min();
}
               
BlobContour::BlobContour(const cv::Point& startPoint)
    : mContour(),
      mStartPoint(startPoint),
      mContourPoints(),
      mArea(std::numeric_limits<cv::Point::value_type>::min()),
      mPerimeter(std::numeric_limits<cv::Point::value_type>::min()),
      mMoments(),
      mBoundingBox(std::numeric_limits<cv::Point::value_type>::min(),
                   std::numeric_limits<cv::Point::value_type>::min(),
                   std::numeric_limits<cv::Point::value_type>::min(),
                   std::numeric_limits<cv::Point::value_type>::min())
{
	mMoments.m00 = std::numeric_limits<cv::Point::value_type>::min();
}

//! Copy constructor
BlobContour::BlobContour(BlobContour* pSource)
{
	if (pSource != NULL)
	{
		*this = *pSource;
	}
}

BlobContour::BlobContour(const BlobContour& source)
    : mContour(source.mContour),
      mStartPoint(source.mStartPoint),
      mContourPoints(source.mContourPoints),
      mArea(source.mArea),
      mPerimeter(source.mPerimeter),
      mMoments(source.mMoments),
      mBoundingBox(source.mBoundingBox)
{}

BlobContour::~BlobContour()
{
  // not responsible for any memory
}

//! Copy operator
BlobContour& BlobContour::operator=(const BlobContour& source)
{
	if (this != &source)
	{
    // copy-swap idiom
    BlobContour tmp(source);
    swap(tmp);
	}
	return *this;
}

//! swaps contents of this contour with other
void BlobContour::swap(BlobContour& other)
{
  std::swap(mContour, other.mContour);
  std::swap(mStartPoint, other.mStartPoint);
  std::swap(mContourPoints, other.mContourPoints);
  std::swap(mArea, other.mArea);
  std::swap(mPerimeter, other.mPerimeter);
  std::swap(mMoments, other.mMoments);
  std::swap(mBoundingBox, other.mBoundingBox);
}

/**
   - FUNCIÓ: AddChainCode
   - FUNCIONALITAT: Add chain code to contour
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
void BlobContour::addChainCode(ChainCodeType chaincode)
{
  mContour.push_back(chaincode);
  mBoundingBox = cv::Rect(std::numeric_limits<cv::Point::value_type>::min(),
                          std::numeric_limits<cv::Point::value_type>::min(),
                          std::numeric_limits<cv::Point::value_type>::min(),
                          std::numeric_limits<cv::Point::value_type>::min());
  mContourPoints.clear();
}

void BlobContour::clear()
{
	mContour.clear();
  mContourPoints.clear();
  mArea = std::numeric_limits<cv::Point::value_type>::min();
  mPerimeter = std::numeric_limits<cv::Point::value_type>::min();
  mMoments = cv::Moments();
  mMoments.m00 = std::numeric_limits<cv::Point::value_type>::min();
  mBoundingBox = cv::Rect(std::numeric_limits<cv::Point::value_type>::min(),
                          std::numeric_limits<cv::Point::value_type>::min(),
                          std::numeric_limits<cv::Point::value_type>::min(),
                          std::numeric_limits<cv::Point::value_type>::min());
}

/**
   - FUNCIÓ: GetPerimeter
   - FUNCIONALITAT: Get perimeter from chain code. Diagonals sum sqrt(2) and horizontal and vertical codes 1
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/30
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: Algorithm derived from "Methods to estimate area and perimeters of blob-like objects: A comparison", L.Yang
*/
double BlobContour::perimeter()
{
	// is calculated?
	if (mPerimeter != std::numeric_limits<cv::Point::value_type>::min())
	{
		return mPerimeter;
	}

	if (isEmpty())
  {
		return 0;
  }

  mPerimeter = cv::arcLength(contourPoints(), 1 );
	return mPerimeter;
}

/**
   - FUNCIÓ: GetArea
   - FUNCIONALITAT: Computes area from chain code
   - PARÀMETRES:
   - 
   - RESULTAT:
   - May give negative areas for clock wise contours
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/30
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: Algorithm derived from "Properties of contour codes", G.R. Wilson
*/
double BlobContour::area()
{
	// is calculated?
	if (mArea != std::numeric_limits<cv::Point::value_type>::min())
	{
		return mArea;
	}

	if (isEmpty())
  {
		return 0;
  }

	mArea = fabs( cv::contourArea(contourPoints()) );
	return mArea;
}

//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
double BlobContour::moment(int p, int q)
{
	if (isEmpty())
  {
		return 0;
  }
  
	// it is calculated?
	if (mMoments.m00 == std::numeric_limits<cv::Point::value_type>::min())
	{
    mMoments = cv::moments(contourPoints(), true);
	}
		
	return spatialMoment(mMoments, p, q);
}

//! Calculate contour points from crack codes
const PointContainerType& BlobContour::contourPoints()
{
	// it is calculated?
	if (!mContourPoints.empty())
  {
		return mContourPoints;
  }
  // mContourPoints is empty
	if (mContour.empty())
	{
    if (mStartPoint.x != std::numeric_limits<cv::Point::value_type>::min() &&
        mStartPoint.y != std::numeric_limits<cv::Point::value_type>::min())
    {
      mContourPoints = std::vector<cv::Point>(2, mStartPoint);
      mBoundingBox = cv::Rect(mStartPoint, cv::Size(1, 1));
    }
		return mContourPoints;
	}
  
  mContourPoints.clear();
  
  cv::Point actual_point = mStartPoint;
  cv::Rect::value_type min_value;
  cv::Rect::value_type max_value;
  if (std::numeric_limits<cv::Rect::value_type>::is_integer)
  {
    min_value = std::numeric_limits<cv::Rect::value_type>::min();
    max_value = std::numeric_limits<cv::Rect::value_type>::max();
  }
  else
  {
    min_value = -std::numeric_limits<cv::Rect::value_type>::min();
    max_value = std::numeric_limits<cv::Rect::value_type>::max();
  }
  
	// also calculate bounding box of the contour
  cv::Rect bounding_box(max_value,  // x
                        max_value,  // y
                        min_value,  // width
                        min_value); // height

  mContourPoints.reserve(mContour.size() + 1);
  mContourPoints.push_back(mStartPoint);
  
  ChainCodeContainerType::const_iterator end_iter = mContour.end();
	for (ChainCodeContainerType::const_iterator iter = mContour.begin();
       iter != end_iter;
       ++iter)
	{
    const ChainCode& chain_code = *iter;
		actual_point = movePoint(actual_point, chain_code);

		bounding_box.x = std::min( bounding_box.x, actual_point.x );
		bounding_box.y = std::min( bounding_box.y, actual_point.y );
		bounding_box.width  = std::max( bounding_box.width,  actual_point.x );
		bounding_box.height = std::max( bounding_box.height, actual_point.y );

    mContourPoints.push_back(actual_point);
	}

  bounding_box.width  -= bounding_box.x;
  bounding_box.height -= bounding_box.y;
  bounding_box.width  += 1;
  bounding_box.height += 1;

	// assign calculated bounding box
	mBoundingBox = bounding_box;

	return mContourPoints;
}

cv::Rect BlobContour::boundingBox()
{
  if (isEmpty())
  {
    return cv::Rect();
  }
  
  // is it already calculated?
  if (mBoundingBox.width != std::numeric_limits<cv::Point::value_type>::min() &&
      mBoundingBox.height != std::numeric_limits<cv::Point::value_type>::min())
  {
		return mBoundingBox;
  }

  // computes bounding box
  contourPoints();
  return mBoundingBox;
}

CVBLOBS_END_NAMESPACE

namespace std {
template<>
void swap(cvblobs::BlobContour& lhs, cvblobs::BlobContour& rhs)
{
  lhs.swap(rhs);
}
} // namespace std
