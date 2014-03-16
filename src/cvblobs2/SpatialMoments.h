/**
 * @brief @todo
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/16/2014
 */
#ifndef _CVBLOBS2_SPATIALMOMENTS_H_
#define _CVBLOBS2_SPATIALMOMENTS_H_

#include <cvblobs2/CvBlobsFwd.h>

CVBLOBS_BEGIN_NAMESPACE

//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
double spatialMoment(const cv::Moments& moments, int p, int q);

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_SPATIALMOMENTS_H_
