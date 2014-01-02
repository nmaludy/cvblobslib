/**
 * @class Blob
 * @brief Implementation of the Blob class and some helper classes to perform
 * some calculations on it
 * @author N. Maludy
 * @date 12/30/2013
 */
#ifndef _CVBLOBS2_BLOB_H_
#define _CVBLOBS2_BLOB_H_

#ifdef _MSC_VER
//! Disable warnings referred to 255 character truncation for the std:map
#pragma warning( disable : 4786 )
#endif

#include <opencv/cxcore.h>
#include <cvblobs2/BlobContour.h>
#include <map>

CVBLOBS_BEGIN_NAMESPACE;

class Blob
{
 public:
	//! Type of blob properties: a map with variant types
	typedef std::map<std::string, double> t_properties;

	Blob();
	Blob(t_labelType id, CvPoint startPoint, CvSize originalImageSize);
	~Blob();

	//! Copy constructor
	Blob(const Blob& src);
	Blob(const Blob* pSrc);

	//! Operador d'assignació
	//! Assigment operator
	Blob& operator=(const Blob& src);
	
	//! Adds a new internal contour to the blob
	void addInternalContour(const BlobContour& newContour);
	
	//! Retrieves contour in Freeman's chain code
	inline BlobContour* externalContour();

	//! Retrieves blob storage
  inline CvMemStorage* storage();
  
	//! Get label ID
	inline t_labelType id() const;
  
	//! > 0 for extern blobs, 0 if not
	int	exterior(IplImage *mask, 
               bool xBorderLeft = true,
               bool xBorderRight = true,
               bool yBorderTop = true,
               bool yBorderBottom = true);
  
	//! Compute blob's area
	double area();
  
	//! Compute blob's perimeter
	double perimeter();
  
	//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
	double moment(int p, int q);

	//! Compute extern perimeter 
	double externPerimeter(IplImage* pMask,
                         bool xBorderLeft = true,
                         bool xBorderRight = true,
                         bool yBorderTop = true,
                         bool yBorderBottom = true);
	
	//! Get mean grey color
	double mean(IplImage* pImage);

	//! Get standard deviation grey color
	double stdDev(IplImage* pImage);

	//! Indica si el blob està buit ( no té cap info associada )
	//! Shows if the blob has associated information
	bool isEmpty();

	//! Retorna el poligon convex del blob
	//! Calculates the convex hull of the blob
	t_PointList convexHull();

	//! Pinta l'interior d'un blob d'un color determinat
	//! Paints the blob in an image
	void fillBlob(IplImage* pImage, CvScalar color);

	//! Join a blob to current one (add's contour
	void joinBlob(Blob* pBlob);

	//! Get bounding box
	CvRect boundingBox();
  
	//! Get bounding ellipse
	CvBox2D ellipse();

	//! Minimun X	
	inline double minX();
  
	//! Minimun Y
	inline double minY();
  
	//! Maximun X
	inline double maxX();
  
	//! Maximun Y
	inline double maxY();

	//! Get mutable blob properties
	inline t_properties* properties();

	//! Remove calculated property from results
	inline void resetProperty(const std::string& propertyName);
  
 private:

	static int cmp_func(const void* pA, const void* pB, void* pUserdata)
	{
    CvPoint* p_a = (CvPoint*)pA;
    CvPoint* p_b = (CvPoint*)pB;
    int y_diff = p_a->y - p_b->y;
    int x_diff = p_a->x - p_b->x;
    return y_diff ? y_diff : x_diff;
	}

	//! Calculates mean and std deviation of blob in input image
	void meanAndStdDev(IplImage* pImage, double& mean, double& stdDev);
	
	//! Deallocates all contours
	void clearContours();
  
	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////

	//! Contour storage memory
	CvMemStorage* mpStorage;
	//! External contour of the blob (crack codes)
	BlobContour mExternalContour;
	//! Internal contours (crack codes)
	t_contourList mInternalContours;

	//////////////////////////////////////////////////////////////////////////
	// Blob features
	//////////////////////////////////////////////////////////////////////////

	// all calculated blob properties
	t_properties mProperties;

	//! Label number
	t_labelType mId;
	//! Bounding box
	CvRect mBoundingBox;
	//! Sizes from image where blob is extracted
	CvSize mOriginalImageSize;
};

inline BlobContour* Blob::externalContour()
{
  return &mExternalContour;
}

inline CvMemStorage* Blob::storage()
{
  return mpStorage;
}

inline t_labelType Blob::id() const
{
  return mId;
}

inline double Blob::minX()
{
  return boundingBox().x;
}

inline double Blob::minY()
{
  return boundingBox().y;
}

inline double Blob::maxX()
{
  return boundingBox().x + boundingBox().width;
}

inline double Blob::maxY()
{
  return boundingBox().y + boundingBox().height;
}

inline Blob::t_properties* Blob::properties()
{
  return &mProperties;
}

inline void Blob::resetProperty(const std::string& propertyName)
{
  mProperties.erase(propertyName);
}

CVBLOBS_END_NAMESPACE;

#endif // _CVBLOBS_BLOB_H_
