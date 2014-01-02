/**
 * @brief Header that formward declares all objects and typedefs in this
 *        library
 * @author N. Maludy
 * @date 01/02/2014
 */
#ifndef _CVBLOBS_CVBLOBSFWD_H_
#define _CVBLOBS_CVBLOBSFWD_H_

#include <list>

// opencv forward declares
class CvSeq;

// CvBlobsLib forward declares
class CBlob;
class CBlobContour;

//! Type of chain codes
typedef unsigned char t_chainCode;
//! Type of list of chain codes
typedef CvSeq* t_chainCodeList;
//! Type of list of points
typedef CvSeq* t_PointList;

//! Type of labelled images
typedef unsigned int t_labelType;
typedef std::list<CBlobContour> t_contourList;

#endif // _CVBLOBS_CVBLOBSFWD_H_
