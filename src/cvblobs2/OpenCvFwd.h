/**
 * @brief Header that formward declares common OpenCV types used by this library
 * @author N. Maludy
 * @date 01/27/2014
 */
#ifndef _CVBLOBS2_OPENCVFWD_H_
#define _CVBLOBS2_OPENCVFWD_H_

///////////////
// opencv forward declares
namespace cv {

// Point
template <typename T> class Point_;
typedef Point_<int> Point2i;
typedef Point2i Point;
typedef Point_<float> Point2f;
typedef Point_<double> Point2d;

// Moments
class Moments;

} // end namespace cv

#endif // _CVBLOBSLIB_OPENCVFWD_H_
