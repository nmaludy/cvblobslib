#include <cvblobs2/BlobContour.h>

#include <cvblobs2/ChainCode.h>
#include <cvblobs2/SpatialMoments.h>

CVBLOBS_BEGIN_NAMESPACE

BlobContour::BlobContour()
    : mContour(),
      mStartPoint(-1, -1),
      mContourPoints(),
      mArea(-1),
      mPerimeter(-1),
      mMoments(),
      mBoundingBox(-1, -1, -1, -1)
{
	mMoments.m00 = -1;
}
               
BlobContour::BlobContour(const cv::Point& startPoint)
    : mContour(),
      mStartPoint(startPoint),
      mContourPoints(),
      mArea(-1),
      mPerimeter(-1),
      mMoments(),
      mBoundingBox(-1, -1, -1, -1)
{
	mMoments.m00 = -1;
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
}

//! Clears chain code contour and points
void BlobContour::resetChainCode()
{
	mContour.clear();
  mContourPoints.clear();
  mArea = -1;
  mPerimeter = -1;
  mMoments = cv::Moments();
  mMoments.m00 = -1;
  mBoundingBox = cv::Rect(-1, -1, -1, -1);
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
	if (mPerimeter != -1)
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
	if (mArea != -1)
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
	if (mMoments.m00 == -1)
	{
    mMoments = cv::moments(contourPoints(), true);
	}
		
	return spatialMoment(mMoments, p, q);
}

//! Calculate contour points from crack codes
const PointContainer& BlobContour::contourPoints()
{
	// it is calculated?
	if (!mContourPoints.empty())
  {
		return mContourPoints;
  }
  // mContourPoints is empty  
	if (mContour.empty())
	{
		return mContourPoints;
	}

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
  
  ChainCodeContainer::const_iterator end_iter = mContour.end();
	for (ChainCodeContainer::const_iterator iter = mContour.begin();
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

  bounding_box.width  -= bounding_box.x + 1;
  bounding_box.height -= bounding_box.y + 1;

	// assign calculated bounding box
	mBoundingBox = bounding_box;

	return mContourPoints;
}

const cv::Rect& BlobContour::boundingBox()
{
  // is it already calculated?
  if (mBoundingBox.width != 0 &&
      mBoundingBox.height != 0)
  {
		return mBoundingBox;
  }
  // computes bounding box
  contourPoints();
  return mBoundingBox;
}

CVBLOBS_END_NAMESPACE
