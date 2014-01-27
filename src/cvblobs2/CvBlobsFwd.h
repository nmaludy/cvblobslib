/**
 * @brief Header that formward declares all objects and typedefs in this
 *        library
 * @author N. Maludy
 * @date 01/02/2014
 */
#ifndef _CVBLOBS2_BLOBFWD_H_
#define _CVBLOBS2_BLOBFWD_H_

#include <list>
#include <vector>

#include <cvblobs2/CvBlobsDefs.h>
#include <cvblobs2/OpenCvFwd.h>
#include <cvblobs2/ChainCode.h>

//////////////
// CvBlobsLib forward declares
CVBLOBS_BEGIN_NAMESPACE

//! Class forward declares
class Blob;
class BlobContour;
class BlobOperator;
class BlobResult;

//! Actions performed by a filter (include or exclude blobs)
enum FilterAction
{
  ACTION_INCLUDE = 1,
  ACTION_EXCLUDE = 2
};

//! Conditions used during filtering to match results. Once a result is matched
//! using the condition then a FilterAction is performed.
// @todo Addability to use std::less and the like instead of these
enum FilterCondition
{
  CONDITION_EQUAL            = 3,
  CONDITION_NOT_EQUAL        = 4,
  CONDITION_GREATER		       = 5,
  CONDITION_LESS             = 6,
  CONDITION_GREATER_OR_EQUAL = 7,
  CONDITION_LESS_OR_EQUAL		 = 8,
  CONDITION_INSIDE			     = 9,
  CONDITION_OUTSIDE			     = 10
};

//! Type of chain codes
typedef ChainCode ChainCodeType;
//! Type of list of chain codes
typedef std::list<ChainCodeType> ChainCodeContainer;
//! Type of list of points
typedef std::vector<cv::Point> PointContainer;

//! Type of labelled images
typedef unsigned int LabelType;
//! Type of a list of contours
typedef std::list<BlobContour> ContourContainer;

//! Shared pointer to blob
// @todo change this to Blob* or std::shared_ptr<Blob>
template <class X> class counted_ptr;
typedef counted_ptr<Blob> BlobPtrType;

//! Blob vector definition
typedef std::vector<BlobPtrType> BlobContainer;

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_CVBLOBSFWD_H_
