/**
 * @class Blob
 * @brief Implementation of the Blob class and some helper classes to perform
 * some calculations on it
 * @author N. Maludy
 * @date 12/30/2013
 */
#ifndef _CVBLOBS2_BLOB_H_
#define _CVBLOBS2_BLOB_H_

#include <map>
#include <cvblobs2/BlobContour.h>

CVBLOBS_BEGIN_NAMESPACE

class Blob
{
 public:
	//! Type of blob properties: a map with variant types
	typedef std::map<std::string, double> PropertiesType;

	Blob();
	Blob(LabelType id,
       const cv::Point& startPoint,
       const cv::Size& originalImageSize);
	~Blob();

	//! Copy constructor
	Blob(const Blob& source);
	Blob(Blob* pSource);

	//! Assigment operator
	Blob& operator=(const Blob& source);

  //! swaps contents of this blob with other
  void swap(Blob& other);
	
	//! Adds a new internal contour to the blob
	void addInternalContour(const BlobContour& newContour);
	
	//! Retrieves contour in Freeman's chain code
	inline BlobContour* externalContour();
  
	//! Get label ID
	inline LabelType id() const;
  
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
	double mean(cv::Mat& image);

	//! Get standard deviation grey color
	double stdDev(cv::Mat& image);

	//! Indica si el blob està buit ( no té cap info associada )
	//! Shows if the blob has associated information
	bool isEmpty();

	//! Retorna el poligon convex del blob
	//! Calculates the convex hull of the blob
	void convexHull(PointContainer& hull);

	//! Pinta l'interior d'un blob d'un color determinat
	//! Paints the blob in an image
	void fillBlob(cv::Mat& image, const cv::Scalar& color);

	//! Join a blob to current one (add's contour
	void joinBlob(const Blob& blob);

	//! Get bounding box
  const cv::Rect& boundingBox();
  
	//! Get bounding ellipse
  cv::RotatedRect ellipse();

	//! Minimun X	
	inline double minX();
  
	//! Minimun Y
	inline double minY();
  
	//! Maximun X
	inline double maxX();
  
	//! Maximun Y
	inline double maxY();

	//! Get mutable blob properties
	inline PropertiesType* properties();

	//! Remove calculated property from results
	inline void resetProperty(const std::string& propertyName);
  
 private:

  // sort points top-to-bottom left-to-right order
  // y is compared before x for strict ordering
  // lhs is above rhs if lhs.y < rhs.y
  // lhs is left of rhs if lhs.x < rhs.x
	struct TopToBottomLeftToRightCmp
  {
    bool operator()(const cv::Point& lhs, const cv::Point& rhs) const
    {
      // is lhs above rhs?
      if (lhs.y < rhs.y)
      {
        return true;
      }
      // is lhs to the left of rhs?
      else if (lhs.x < rhs.x)
      {
        return true;
      }
      return false;
    }
  };

	//! Calculates mean and std deviation of blob in input image
	void meanAndStdDev(cv::Mat& image, double& mean, double& stdDev);
	
	//! Deallocates all contours
	void clearContours();
  
	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////

	//! External contour of the blob (crack codes)
	BlobContour mExternalContour;
	//! Internal contours (crack codes)
	ContourContainer mInternalContours;

	//////////////////////////////////////////////////////////////////////////
	// Blob features
	//////////////////////////////////////////////////////////////////////////

	// all calculated blob properties
	PropertiesType mProperties;

	//! Label number
	LabelType mId;
	//! Bounding box
  cv::Rect mBoundingBox;
	//! Sizes from image where blob is extracted
  cv::Size mOriginalImageSize;
};

inline BlobContour* Blob::externalContour()
{
  return &mExternalContour;
}

inline LabelType Blob::id() const
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

inline Blob::PropertiesType* Blob::properties()
{
  return &mProperties;
}

inline void Blob::resetProperty(const std::string& propertyName)
{
  mProperties.erase(propertyName);
}

CVBLOBS_END_NAMESPACE

namespace std {
template<>
void swap(cvblobs::Blob& lhs, cvblobs::Blob& rhs)
{
  lhs.swap(rhs);
}
} // namespace std

#endif // _CVBLOBS_BLOB_H_
