/**
 * @brief ComponentLabeling functions to perform the connected compoents
 * labelling (ie blobbing).
 * @author Ricard Borràs
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/15/2014
 */
#ifndef _CVBLOBS2_COMPONENTLABELING_H_
#define _CVBLOBS2_COMPONENTLABELING_H_

#include <cvblobs2/CvBlobsFwd.h>

CVBLOBS_BEGIN_NAMESPACE

//! Component labeling functionx
bool ComponentLabeling(const cv::Mat& inputImage,
                       const cv::Mat& maskImage,
                       unsigned char backgroundColor,
                       BlobContainerType& blobs);

//! Auxiliary functions
void contourTracing(const cv::Mat& inputImage,
                    const cv::Mat& maskImage,
                    const cv::Point& contourStart,
                    LabelType *labels, 
                    bool* pbVisitedPoints,
                    LabelType label,
                    bool bInternalContour,
                    unsigned char backgroundColor,
                    cv::Ptr<BlobContour> pCurrentBlobContour);

cv::Point tracer(const cv::Mat& inputImage,
                 const cv::Mat& maskImage,
                 const cv::Point& point,
                 bool* pbVisitedPoints,
                 ChainCode initialMovement,
                 unsigned char backgroundColor,
                 ChainCode& movement);

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_COMPONENTLABELING_H_
