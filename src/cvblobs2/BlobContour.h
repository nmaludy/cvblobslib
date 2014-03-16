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

  /** 
   * @brief Default constructor
   */
	BlobContour();
  
	BlobContour(const cv::Point& startPoint);
	//! Copy constructor
  BlobContour(const BlobContour& source);
	BlobContour(BlobContour* pSource);

	~BlobContour();
	//! Assigment operator
	BlobContour& operator=(const BlobContour& source);

  /** 
   * @brief Swaps contents of this contour with \p other.
   * One can clear a contour by doing
   * @code
   // Create a contour and add some chain codes to it
   cvblobs::BlobContour full_contour(cv::Point(10, 10));
   full_contour.addChainCode(cvblobs::CHAIN_CODE_RIGHT);
   full_contour.addChainCode(cvblobs::CHAIN_CODE_DOWN);
   full_contour.addChainCode(cvblobs::CHAIN_CODE_LEFT);
   full_contour.addChainCode(cvblobs::CHAIN_CODE_UP);

   // now clear full_contour using swap()
   {
     cvblobs::BlobContour empty_comtour;
     full_contour.swap(empty_contour);
     // full_contour is now empty, empty_contour is now full
     // empty_contour will be destructed when leaving the bracket scopes
   }
   @endcode
   * @param other the other contour to swap contents with.
   */
  void swap(BlobContour& other);

  /** 
   * @brief Adds a freeman chain code \p code to the contour.
   * @post After this call contourPoints() and boundingBox() will be reset.
   * One should call contourPOints() or boundingBox() again to recalculate
   * these properties (if necessary).
   * @param code the freeman chain code to add to the contour
   */
	void addChainCode(ChainCodeType code);

  /** 
   * @brief Return freeman chain coded contour
   */
  const ChainCodeContainerType& chainCode() const;

  /** 
   * @brief returns true if this contour is empty
   */
	inline bool isEmpty() const;

  /** 
   * @brief Return a container of cv::Points representing this contour
   */
  const PointContainerType& contourPoints();

  /** 
   * @brief Returns the bounding box rectangle of this contour
   */
  cv::Rect boundingBox();

  /** 
   * @brief The starting point of the BlobContour
   */
  inline const cv::Point& startPoint() const;

  /** 
   * @brief 
   * @param startPoint 
   */
  void setStartPoint(const cv::Point& startPoint) const;
  
  /** 
   * @brief Computes the area of the contour
   */
	double area();
  
  /** 
   * @brief Computes perimeter of the contour
   */
	double perimeter();
   
  /** 
   * @brief Get the contour moment \p p , \p q.
   * \p p and \p q must not be >= to CVBLOBS_MAX_MOMENTS_ORDER defined in
   * CvBlobsDefs.h.
   * @param p
   * @param q 
   * @return The value of the contour moment for \p p , \p q
   */
	double moment(int p, int q);

  /** 
   * @brief Clears the contour
   * @post isEmpty() will return true. <br>
   * chainCode() will return an empty container. <br>
   * contourPonts() will return an empty container. <br>
   * boundingBox() will return a cv::Rect with all elements set to
   * std::numeric_limits<cv::Rect::value_type>::min(). <br>
   * startPoint() will return a cv::Point with all elements set to
   * std::numeric_limits<cv::Rect::value_type>::min(). <br>
   */
  void clear();
  
 protected:	

	//! Crack code list
	ChainCodeContainerType mContour;

 private:
  
	//! Starting point of the contour
  cv::Point mStartPoint;
	//! All points from the contour
	PointContainerType mContourPoints;

	//! Computed area from contour
	double mArea;
	//! Computed perimeter from contour
	double mPerimeter;
	//! Computed moments from contour
  cv::Moments mMoments;

  //! bounding box
  cv::Rect mBoundingBox;
};

inline const ChainCodeContainerType& BlobContour::chainCode() const
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
void swap(cvblobs::BlobContour& lhs, cvblobs::BlobContour& rhs);
} // namespace std

#endif // _CVBLOBS_BLOBCONTOUR_H_

