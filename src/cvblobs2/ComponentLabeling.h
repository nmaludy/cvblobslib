#ifndef _CVBLOBS2_COMPONENTLABELING_H_
#define _CVBLOBS2_COMPONENTLABELING_H_

#include <vector>
#include <opencv/cxcore.h>
#include <cvblobs2/CvBlobsFwd.h>
#include <cvblobs2/counted_ptr.h>

CVBLOBS_BEGIN_NAMESPACE

//! Shared pointer to blob
typedef counted_ptr<Blob> BlobPtr;

//! Blob vector definition
typedef std::vector<BlobPtr> Blob_vector;

//! Component labeling functionx
bool ComponentLabeling(IplImage* pInputImage,
                       IplImage* pMaskImage,
                       unsigned char backgroundColor,
                       Blob_vector& blobs);

//! Auxiliary functions
void contourTracing(IplImage* pImage,
                    IplImage* pMask,
                    CvPoint contourStart,
                    t_labelType *labels, 
                    bool* pbVisitedPoints,
                    t_labelType label,
                    bool bInternalContour,
                    unsigned char backgroundColor,
                    BlobContour* pCurrentBlobContour);

CvPoint tracer(IplImage* pImage,
               IplImage* pMask,
               CvPoint P,
               bool* pbVisitedPoints,
               short initialMovement,
               unsigned char backgroundColor,
               short& movement);

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_COMPONENTLABELING_H_
