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

#include <list>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <cvblobs2/CvBlobsFwd.h>

//! Max order of calculated moments
#define MAX_MOMENTS_ORDER		3

CVBLOBS_BEGIN_NAMESPACE;

//! Blob contour class (in crack code)
class BlobContour
{
	friend class Blob;
	
 public:
	//! Constructors
	BlobContour();
	BlobContour(CvPoint startPoint, CvMemStorage* pStorage);
	//! Copy constructor
	BlobContour(BlobContour* pSource);

	~BlobContour();
	//! Assigment operator
	BlobContour& operator=(const BlobContour& source);

	//! Add chain code to contour
	void addChainCode(t_chainCode code);

	//! Return freeman chain coded contour
	t_chainCodeList chainCode() const;

	inline bool isEmpty() const;

	//! Return all contour points
	t_PointList contourPoints();

 protected:	

  inline const CvPoint& startPoint() const;

	//! Clears chain code contour
	void resetChainCode();
	
	//! Computes area from contour
	double area();
	//! Computes perimeter from contour
	double perimeter();
	//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
	double moment(int p, int q);

	//! Crack code list
	t_chainCodeList mpContour; 	

 private:
	//! Starting point of the contour
	CvPoint mStartPoint;
	//! All points from the contour
	t_PointList mpContourPoints;

	//! Computed area from contour
	double mArea;
	//! Computed perimeter from contour
	double mPerimeter;
	//! Computed moments from contour
	CvMoments mMoments;

	//! Pointer to storage
	CvMemStorage* mpParentStorage;
};

inline t_chainCodeList BlobContour::chainCode() const
{
  return mpContour;
}

inline bool BlobContour::isEmpty() const
{
  return mpContour == NULL || mpContour->total == 0;
}

inline const CvPoint& BlobContour::startPoint() const
{
  return mStartPoint;
}

CVBLOBS_END_NAMESPACE;

#endif // _CVBLOBS_BLOBCONTOUR_H_

