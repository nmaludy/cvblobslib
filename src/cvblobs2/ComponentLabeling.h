#ifndef _CVBLOBS2_COMPONENTLABELING_H_
#define _CVBLOBS2_COMPONENTLABELING_H_

#include <cvblobs2/CvBlobsFwd.h>

CVBLOBS_BEGIN_NAMESPACE

//! Component labeling functionx
bool ComponentLabeling(cv::Mat& inputImage,
                       cv::Mat& maskImage,
                       unsigned char backgroundColor,
                       BlobContainer& blobs);

//! Auxiliary functions
void contourTracing(cv::Mat& inputImage,
                    cv::Mat& maskImage,
                    cv::Point contourStart,
                    LabelType *labels, 
                    bool* pbVisitedPoints,
                    LabelType label,
                    bool bInternalContour,
                    unsigned char backgroundColor,
                    BlobContour* pCurrentBlobContour);

cv::Point tracer(cv::Mat& inputImage,
                 cv::Mat& maskImage,
                 cv::Point P,
                 bool* pbVisitedPoints,
                 ChainCode initialMovement,
                 unsigned char backgroundColor,
                 ChainCode& movement);

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_COMPONENTLABELING_H_
