#ifndef _CVBLOBS2_COMPONENTLABELING_H_
#define _CVBLOBS2_COMPONENTLABELING_H_

#include <vector>
#include <cvblobs2/BlobContour.h>
#include <cvblobs2/Blob.h>
#include <cvblobs2/counted_ptr.h>

CVBLOBS_BEGIN_NAMESPACE;

//! Shared pointer to blob
typedef counted_ptr<Blob> BlobPtr;

//! Blob vector definition
typedef std::vector<BlobPtr> Blob_vector;

//! Component labeling function
bool ComponentLabeling(	IplImage* inputImage,
						IplImage* maskImage,
						unsigned char backgroundColor,
						Blob_vector &blobs );

//! Auxiliary functions
void contourTracing( IplImage *image, IplImage *mask, CvPoint contourStart, t_labelType *labels, 
					 bool *visitedPoints, t_labelType label,
					 bool internalContour, unsigned char backgroundColor,
					 BlobContour *currentBlobContour );

CvPoint tracer( IplImage *image, IplImage *mask, CvPoint P, bool *visitedPoints,
				short initialMovement,
				unsigned char backgroundColor, short &movement );				

CVBLOBS_END_NAMESPACE;

#endif // _CVBLOBS2_COMPONENTLABELING_H_
