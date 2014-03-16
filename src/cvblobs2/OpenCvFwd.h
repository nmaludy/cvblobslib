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

// Mat
class Mat;

// Matx
template <typename T, int m, int n> class Matx;
typedef Matx<float,  1, 2> Matx12f;
typedef Matx<double, 1, 2> Matx12d;
typedef Matx<float,  1, 3> Matx13f;
typedef Matx<double, 1, 3> Matx13d;
typedef Matx<float,  1, 4> Matx14f;
typedef Matx<double, 1, 4> Matx14d;
typedef Matx<float,  1, 6> Matx16f;
typedef Matx<double, 1, 6> Matx16d;

typedef Matx<float,  2, 1> Matx21f;
typedef Matx<double, 2, 1> Matx21d;
typedef Matx<float,  3, 1> Matx31f;
typedef Matx<double, 3, 1> Matx31d;
typedef Matx<float,  4, 1> Matx41f;
typedef Matx<double, 4, 1> Matx41d;
typedef Matx<float,  6, 1> Matx61f;
typedef Matx<double, 6, 1> Matx61d;

typedef Matx<float,  2, 2> Matx22f;
typedef Matx<double, 2, 2> Matx22d;
typedef Matx<float,  2, 3> Matx23f;
typedef Matx<double, 2, 3> Matx23d;
typedef Matx<float,  3, 2> Matx32f;
typedef Matx<double, 3, 2> Matx32d;

typedef Matx<float,  3, 3> Matx33f;
typedef Matx<double, 3, 3> Matx33d;

typedef Matx<float,  3, 4> Matx34f;
typedef Matx<double, 3, 4> Matx34d;
typedef Matx<float,  4, 3> Matx43f;
typedef Matx<double, 4, 3> Matx43d;

typedef Matx<float,  4, 4> Matx44f;
typedef Matx<double, 4, 4> Matx44d;
typedef Matx<float,  6, 6> Matx66f;
typedef Matx<double, 6, 6> Matx66d;

// Moments
class Moments;

// Point
template <typename T> class Point_;
typedef Point_<int> Point2i;
typedef Point2i Point;
typedef Point_<float> Point2f;
typedef Point_<double> Point2d;

// Point3
template<typename T> class Point3_;
typedef Point3_<int> Point3i;
typedef Point3_<float> Point3f;
typedef Point3_<double> Point3d;

// Rect
template<typename T> class Rect_;
typedef Rect_<int> Rect;

// Scalar
template <typename T> class Scalar_;
typedef Scalar_<double> Scalar;

// Size
template <typename T> class Size_;
typedef Size_<int> Size2i;
typedef Size2i Size;
typedef Size_<float> Size2f;

// Vec
template<typename T, int cn> class Vec;
typedef Vec<unsigned char, 2> Vec2b;
typedef Vec<unsigned char, 3> Vec3b;
typedef Vec<unsigned char, 4> Vec4b;

typedef Vec<short, 2> Vec2s;
typedef Vec<short, 3> Vec3s;
typedef Vec<short, 4> Vec4s;

typedef Vec<unsigned short, 2> Vec2w;
typedef Vec<unsigned short, 3> Vec3w;
typedef Vec<unsigned short, 4> Vec4w;

typedef Vec<int, 2> Vec2i;
typedef Vec<int, 3> Vec3i;
typedef Vec<int, 4> Vec4i;
typedef Vec<int, 6> Vec6i;
typedef Vec<int, 8> Vec8i;

typedef Vec<float, 2> Vec2f;
typedef Vec<float, 3> Vec3f;
typedef Vec<float, 4> Vec4f;
typedef Vec<float, 6> Vec6f;

typedef Vec<double, 2> Vec2d;
typedef Vec<double, 3> Vec3d;
typedef Vec<double, 4> Vec4d;
typedef Vec<double, 6> Vec6d;

// Mat_
template<typename T> class Mat_;
typedef Mat_<unsigned char> Mat1b;
typedef Mat_<Vec2b> Mat2b;
typedef Mat_<Vec3b> Mat3b;
typedef Mat_<Vec4b> Mat4b;

typedef Mat_<short> Mat1s;
typedef Mat_<Vec2s> Mat2s;
typedef Mat_<Vec3s> Mat3s;
typedef Mat_<Vec4s> Mat4s;

typedef Mat_<unsigned short> Mat1w;
typedef Mat_<Vec2w> Mat2w;
typedef Mat_<Vec3w> Mat3w;
typedef Mat_<Vec4w> Mat4w;

typedef Mat_<int>   Mat1i;
typedef Mat_<Vec2i> Mat2i;
typedef Mat_<Vec3i> Mat3i;
typedef Mat_<Vec4i> Mat4i;

typedef Mat_<float> Mat1f;
typedef Mat_<Vec2f> Mat2f;
typedef Mat_<Vec3f> Mat3f;
typedef Mat_<Vec4f> Mat4f;

typedef Mat_<double> Mat1d;
typedef Mat_<Vec2d> Mat2d;
typedef Mat_<Vec3d> Mat3d;
typedef Mat_<Vec4d> Mat4d;

// Ptr
template<typename _Tp> class Ptr;

} // end namespace cv

#endif // _CVBLOBSLIB_OPENCVFWD_H_
