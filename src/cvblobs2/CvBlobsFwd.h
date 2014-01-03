/**
 * @brief Header that formward declares all objects and typedefs in this
 *        library
 * @author N. Maludy
 * @date 01/02/2014
 */
#ifndef _CVBLOBS2_BLOBFWD_H_
#define _CVBLOBS2_BLOBFWD_H_

#include <list>
#include <cvblobs2/CvBlobsDefs.h>

///////////////
// opencv forward declares
class CvSeq;

// namespace cv {

// // Point
// template <typename T> class Point_;
// typedef Point_<int> Point2i;
// typedef Point2i Point;
// typedef Point_<float> Point2f;
// typedef Point_<double> Point2d;

// } // end namespace cv

//////////////
// CvBlobsLib forward declares
CVBLOBS_BEGIN_NAMESPACE

class Blob;
class BlobContour;
class BlobResult;

// //! Type of chain codes
// typedef unsigned char t_chainCode;
// //! Type of list of chain codes
// typedef std::list<t_chainCode> t_chainCodeList;
// //! Type of list of points
// typedef std::list<cv::Point> t_PointList;

// //! Type of labelled images
// typedef unsigned int t_labelType;
// typedef std::list<BlobContour> t_contourList;

//! Type of chain codes
typedef unsigned char t_chainCode;
//! Type of list of chain codes
typedef CvSeq* t_chainCodeList;
//! Type of list of points
typedef CvSeq* t_PointList;

//! Type of labelled images
typedef unsigned int t_labelType;
typedef std::list<BlobContour> t_contourList;

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBSLIB_BLOBFWD_H_
