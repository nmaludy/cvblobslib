/**
 * @class Blob
 * @brief Implementation of the Blob class and some helper classes to perform
 * some calculations on it
 * @author N. Maludy
 * @date 12/30/2013
 */
#ifndef _CVBLOBS2_BLOB_H_
#define _CVBLOBS2_BLOB_H_

// std
#include <map>
#include <string>

// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// cvblobs
#include <cvblobs2/CvBlobsFwd.h>

CVBLOBS_BEGIN_NAMESPACE

class Blob
{
 public:
	//! Type of blob properties: a map with variant types
	typedef std::map<std::string, double> PropertiesType;

  /** 
   * @brief Creates an empty blob.
   */
	Blob();

  /** 
   * @brief Creates a new blob with an identifier of \p id starting its
   * external contour at \p startPoint with an original image size of
   * \p originalImageSize
   * @param id 
   * @param startPoint 
   * @param originalImageSize 
   */
	Blob(LabelType id,
       const cv::Point& startPoint,
       const cv::Size& originalImageSize);

  /** 
   * @brief Destroys a blob and all of its contours
   */
	~Blob();

	//! Copy constructor
	Blob(const Blob& source);

	//! Assigment operator
	Blob& operator=(const Blob& source);

  //! swaps contents of this blob with other
  void swap(Blob& other);
	
	//! Adds a new internal contour to the blob
	void addInternalContour(cv::Ptr<BlobContour> pInternalContour);
	
	//! Retrieves external contour in Freeman's chain code
	inline cv::Ptr<BlobContour> externalContour();

  /** 
   * @brief Returns the number of internal contours of the blob
   */
	inline std::size_t numInternalContours() const;

  /** 
   * @brief Returns the internal BlobContour at index \p i 
   * @param i the index of the internal contour to retrieve.
   * @return A pointer to the internal contour at index \p i.
   * If \p i is >= numInternalContours() then NULL is returned.
   */
  inline cv::Ptr<BlobContour> internalContour(std::size_t i) const;
  
  
	//! Retrieves an internal contour in Freeman's chain code at index i
	// inline BlobContour* internalContour(std::size_t);
  
	//! Get label ID
	inline LabelType id() const;
  
	//! > 0 for extern blobs, 0 if not
	int	exterior(cv::Mat& mask, 
               bool xBorderLeft = true,
               bool xBorderRight = true,
               bool yBorderTop = true,
               bool yBorderBottom = true);
  
	//! Compute blob's area
	double area();
  
	//! Compute blob's perimeter
	double perimeter();
  
	//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
//! @see https://en.wikipedia.org/wiki/Image_moment
//! M00 = area (number of pixels in image)
//! M10 = sum over X
//! M01 = sum over Y
//! centroid x = M10 / M00
//! centroid y = M01 / M00
	double moment(int p, int q);

	//! Compute extern perimeter 
	double externPerimeter(cv::Mat& mask,
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
	void convexHull(PointContainerType& hull);

	//! Pinta l'interior d'un blob d'un color determinat
	//! Paints the blob in an image
	void fillBlob(cv::Mat& image, const cv::Scalar& color);

	//! Join a blob to current one (add's contour
	void joinBlob(const Blob& blob);

	//! Get bounding box
  cv::Rect boundingBox();
  
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
    
	//! Calculates mean and std deviation of blob in input image
	void meanAndStdDev(cv::Mat& image, double& mean, double& stdDev);
	
	//! Deallocates all contours
	void clear();
  
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
  
	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////

	//! External contour of the blob (crack codes)
  cv::Ptr<BlobContour> mpExternalContour;

	//! Internal contours (crack codes)
	ContourContainerType mInternalContours;

	//////////////////////////////////////////////////////////////////////////
	// Blob features
	//////////////////////////////////////////////////////////////////////////

	// all calculated blob properties
	PropertiesType mProperties;

	//! Label number
	LabelType mId;
  
	//! Sizes from image where blob is extracted
  cv::Size mOriginalImageSize;
};

inline cv::Ptr<BlobContour> Blob::externalContour()
{
  return mpExternalContour;
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
void swap(cvblobs::Blob& lhs, cvblobs::Blob& rhs);
} // namespace std

#endif // _CVBLOBS2_BLOB_H_
