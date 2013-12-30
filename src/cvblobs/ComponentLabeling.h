#ifndef _COMPONENT_LABELING_H_INCLUDED
#define _COMPONENT_LABELING_H_INCLUDED

#include <vector>
#include <cvblobs/BlobContour.h>
#include <cvblobs/Blob.h>
#include <cvblobs/counted_ptr.h>

//! Shared pointer to blob
typedef counted_ptr<CBlob> BlobPtr;

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
					 CBlobContour *currentBlobContour );

CvPoint tracer( IplImage *image, IplImage *mask, CvPoint P, bool *visitedPoints,
				short initialMovement,
				unsigned char backgroundColor, short &movement );
				

#endif	//!_COMPONENT_LABELING_H_INCLUDED
