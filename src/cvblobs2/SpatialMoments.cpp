/**
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/16/2014
 */
#include <cvblobs2/SpatialMoments.h>
#include <opencv2/imgproc/imgproc.hpp>

CVBLOBS_BEGIN_NAMESPACE

double spatialMoment(const cv::Moments& moments, int p, int q)
{
  int order = p + q;
	// is a valid moment?
	if (p < 0 ||
      q < 0 ||
      order > CVBLOBS_MAX_MOMENTS_ORDER)
	{
		return -1.0;
	}

  switch (p)
  {
    case 0: // moments m0q
      switch (q)
      {
        case 0:
          return moments.m00;
        case 1:
          return moments.m01;
        case 2:
          return moments.m02;
      }
      break;
    case 1: // moments m1q
      switch (q)
      {
        case 0:
          return moments.m10;
        case 1:
          return moments.m11;
        case 2:
          return moments.m12;
      }
      break;
    case 2: // moments m2q
      switch (q)
      {
        case 0:
          return moments.m20;
        case 1:
          return moments.m21;
      }
      break;
    case 3: // moments m3q
      switch (q)
      {
        case 0:
          return moments.m30;
      }
      break;
  };
  return -1.0;
}

CVBLOBS_END_NAMESPACE

