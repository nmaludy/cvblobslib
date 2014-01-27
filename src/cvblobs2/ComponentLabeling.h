#ifndef _CVBLOBS2_COMPONENTLABELING_H_
#define _CVBLOBS2_COMPONENTLABELING_H_

#include <opencv/cxcore.h>
#include <cvblobs2/CvBlobsFwd.h>

#include <cvblobs2/ChainCode.h>

CVBLOBS_BEGIN_NAMESPACE

//! Component labeling functionx
bool ComponentLabeling(IplImage* pInputImage,
                       IplImage* pMaskImage,
                       unsigned char backgroundColor,
                       BlobContainer& blobs);

//! Auxiliary functions
void contourTracing(IplImage* pImage,
                    IplImage* pMask,
                    cv::Point contourStart,
                    LabelType *labels, 
                    bool* pbVisitedPoints,
                    LabelType label,
                    bool bInternalContour,
                    unsigned char backgroundColor,
                    BlobContour* pCurrentBlobContour);

cv::Point tracer(IplImage* pImage,
                 IplImage* pMask,
                 cv::Point P,
                 bool* pbVisitedPoints,
                 ChainCode initialMovement,
                 unsigned char backgroundColor,
                 ChainCode& movement);

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_COMPONENTLABELING_H_
