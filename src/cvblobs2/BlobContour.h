/**
 * Copyright (c) 2013 SilverBlock Systems, Inc. ALL RIGHTS RESERVED.
 * 
 * @class BlobContour
 * @brief 
 * @author N. Maludy
 * @date 12/31/2013
 */
#ifndef _CVBLOBS2_BLOBCONTOUR_H_
#define _CVBLOBS2_BLOBCONTOUR_H_

#include <cvblobs2/CvBlobsFwd.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

CVBLOBS_BEGIN_NAMESPACE

//! Blob contour class (in crack code)
class BlobContour
{
	friend class Blob;
	
 public:
	//! Constructors
	BlobContour();
	BlobContour(const cv::Point& startPoint);
	//! Copy constructor
  BlobContour(const BlobContour& source);
	BlobContour(BlobContour* pSource);

	~BlobContour();
	//! Assigment operator
	BlobContour& operator=(const BlobContour& source);

  //! swaps contents of this contour with other
  void swap(BlobContour& other);

	//! Add chain code to contour
	void addChainCode(ChainCodeType code);

	//! Return freeman chain coded contour
  const ChainCodeContainer& chainCode() const;

  //! returns true if this contour is empty
	inline bool isEmpty() const;

	//! Return all contour points
  const PointContainer& contourPoints();

  //! Returns the bounding box rectangle of this contour
  const cv::Rect& boundingBox();
  
 protected:	

  inline const cv::Point& startPoint() const;

	//! Clears chain code contour
	void resetChainCode();
	
	//! Computes area from contour
	double area();
	//! Computes perimeter from contour
	double perimeter();
	//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
	double moment(int p, int q);

	//! Crack code list
	ChainCodeContainer mContour;

 private:
	//! Starting point of the contour
  cv::Point mStartPoint;
	//! All points from the contour
	PointContainer mContourPoints;

	//! Computed area from contour
	double mArea;
	//! Computed perimeter from contour
	double mPerimeter;
	//! Computed moments from contour
  cv::Moments mMoments;

  //! bounding box
  cv::Rect mBoundingBox;
};

inline const ChainCodeContainer& BlobContour::chainCode() const
{
  return mContour;
}

inline bool BlobContour::isEmpty() const
{
  return mContour.empty();
}

inline const cv::Point& BlobContour::startPoint() const
{
  return mStartPoint;
}

CVBLOBS_END_NAMESPACE

namespace std {
template<>
void swap(cvblobs::BlobContour& lhs, cvblobs::BlobContour& rhs)
{
  lhs.swap(rhs);
}
} // namespace std

#endif // _CVBLOBS_BLOBCONTOUR_H_

