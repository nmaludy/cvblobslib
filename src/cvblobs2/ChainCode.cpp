#include <cvblobs2/ChainCode.h>

#include <opencv2/core/core.hpp>

CVBLOBS_BEGIN_NAMESPACE

cv::Point movePoint(const cv::Point& origin,
                    const ChainCode& movement)
{
  // @note i profiled and using this switch statement is faster
  // than array lookup
  cv::Point moved_point = origin;
  switch (movement)
  {
    case CHAIN_CODE_RIGHT: // 1, 0
      ++moved_point.x;
      return moved_point;
    case CHAIN_CODE_UP_RIGHT: // 1, -1
      ++moved_point.x;
      --moved_point.y;
      return moved_point;
    case CHAIN_CODE_UP: // 0, -1
      --moved_point.y;
      return moved_point;
    case CHAIN_CODE_UP_LEFT: // -1, -1
      --moved_point.x;
      --moved_point.y;
      return moved_point;
    case CHAIN_CODE_LEFT: // -1, 0
      --moved_point.x;
      return moved_point;
    case CHAIN_CODE_DOWN_LEFT: // -1, 1
      --moved_point.x;
      ++moved_point.y;
      return moved_point;
    case CHAIN_CODE_DOWN: // 0, 1
      ++moved_point.y;
      return moved_point;
    case CHAIN_CODE_DOWN_RIGHT: // 1, 1
      ++moved_point.x;
      ++moved_point.y;
      return moved_point;
    default:
      return moved_point;
  }
}

ChainCode pointDiff2ChainCode(const cv::Point& fromPoint,
                              const cv::Point& toPoint)
{
  cv::Point diff(toPoint.x - fromPoint.x,
                 toPoint.y - fromPoint.y);
  return static_cast<ChainCode>( point2ChainCode(diff) );
}

cv::Point chainCode2Point(const ChainCode& movement)
{
  return movePoint(cv::Point(0, 0), movement);
}

ChainCode point2ChainCode(const cv::Point& point)
{
  int x = point.x + 1;
  int y = point.y + 1;
  if ((x == 1 && y == 1) || // the points isn't a movement
      x < 0 || x > 2 ||
      y < 0 || y > 2)
  {
    return CHAIN_CODE_INVALID;
  }

  switch (x)
  {
    case -1:
      switch (y)
      {
        case -1:
          return CHAIN_CODE_UP_LEFT;     // -1, -1
        case 0:
          return CHAIN_CODE_LEFT;        // -1, 0
        case 1:
          return CHAIN_CODE_DOWN_LEFT;   // -1, 1
        default:
          return CHAIN_CODE_INVALID;
      }
      break;
    case 0:
      switch (y)
      {
        case -1:
          return CHAIN_CODE_UP;          // 0, -1
        case 0:  
          return CHAIN_CODE_INVALID;     // 0, 0
        case 1:
          return CHAIN_CODE_DOWN;        // 0, 1
        default:
          return CHAIN_CODE_INVALID;
      }
      break;
    case 1:
      switch (y)
      {
        case -1:
          return CHAIN_CODE_UP_RIGHT;    // 1, -1
        case 0:
          return CHAIN_CODE_RIGHT;       // 1, 0
        case 1:
          return CHAIN_CODE_DOWN_RIGHT;  // 1, 1
        default:
          return CHAIN_CODE_INVALID;
      }
      break;
    default:
      return CHAIN_CODE_INVALID;
  }
  return CHAIN_CODE_INVALID;
}

CVBLOBS_END_NAMESPACE
