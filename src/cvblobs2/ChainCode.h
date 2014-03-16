#ifndef _CVBLOBS2_CHAINCODE_H_
#define _CVBLOBS2_CHAINCODE_H_

#include <cvblobs2/CvBlobsDefs.h>
#include <cvblobs2/OpenCvFwd.h>

CVBLOBS_BEGIN_NAMESPACE

/**
 * @enum ChainCode
 * @brief
 * Conversion from freeman code to coordinate increments (counterclockwise)
 * coordinates are in {x, y} offsets relative to the current pixel
 *
 * Below is a picture of how the indexes in this array map to offsets relative
 * to pixel P.
 @code
   3  2  1
   4  P  0
   5  6  7
 @endcode
 *
 * @todo operator overloads for cv::Point <-> ChainCode operations
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 01/21/2014
 */
enum ChainCode
{
  CHAIN_CODE_INVALID    = -1,
  CHAIN_CODE_RIGHT      = 0, // 1, 0
  CHAIN_CODE_UP_RIGHT   = 1, // 1, -1
  CHAIN_CODE_UP         = 2, // 0, -1
  CHAIN_CODE_UP_LEFT    = 3, // -1 -1
  CHAIN_CODE_LEFT       = 4, // -1, 0
  CHAIN_CODE_DOWN_LEFT  = 5, // -1, 1
  CHAIN_CODE_DOWN       = 6, // 0, 1
  CHAIN_CODE_DOWN_RIGHT = 7, // 1, 1
  CHAIN_CODE_MAX        = 8  // codes can't be >= to this
};

/** 
 * @brief Returns true of this movement is > INVALID and < MAX_MOVEMENT
 */
inline bool isChainCodeValid(const ChainCode& chainCode)
{
  return chainCode > CHAIN_CODE_INVALID &&
      chainCode < CHAIN_CODE_MAX;
}

/** 
 * @brief Creates a new point by moving \p origin using the chain code
 * \p movement
 * @param origin the origination point
 * @param movement the direction to move the point
 * @return The \p origin moved by this chain codes movement
 */
cv::Point movePoint(const cv::Point& origin,
                    const ChainCode& movement);

/** 
 * @brief Returns the chain code required to move from \p fromPoint to
 * \p toPoint
 * @param fromPoint the origin point to move from
 * @param toPoint the destination point to move to
 * @return The chain code required to move from \p fromPoint to \p toPoint.
 * If \p fromPoint is equal to \p toPoint then MOVEMENT_INVALID will be
 * returned.
 * If \p fromPoint and \p toPoint are not adjacent then MOVEMENT_INVALID will
 * be returned (ie if the points don't share an edge or a corner).
 */
ChainCode pointDiff2ChainCode(const cv::Point& fromPoint,
                              const cv::Point& toPoint);

/** 
 * @brief converts a chain code into a point with x and y values which will
 * represent the movement in each dimension.
 * If this is not a valid movement 0,0 is returned
 * @param movement 
 * @return 
 */
cv::Point chainCodePoint(const ChainCode& movement);

/** 
 * @brief Converts an offset point into a movement type.
 * @param point 
 * @return If the point's coordinates are 0, 0 or either x or y are
 *         greater than one or less than negative one then MOVEMENT_INVALID
 *         will be returned. Otherwise the movement representing this point's
 *         offset coordinates will be returned.
 */
ChainCode point2ChainCode(const cv::Point& point);

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_CHAINCODE_H_
