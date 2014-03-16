#ifndef _CVBLOBS2_BLOBOPERATORS_H_
#define _CVBLOBS2_BLOBOPERATORS_H_

// std
#include <string>

// opencv
#include <opencv2/core/core.hpp>

// cvblobs
#include <cvblobs2/CvBlobsFwd.h>

CVBLOBS_BEGIN_NAMESPACE

/**
 * @class BlobOperator
 * @brief Functor interface for operating on Blob's in an efficient manner.
 * All functors inherit from the abstract BlobOperator class.
 * Sub-classes of BlobOperator MUST implement the name() and
 * double operator()(cv::Ptr<Blob>) functions. The name() function should return
 * the name of the operation and must be unique with respect to all other
 * BlobOperator sub-classes. The double operator()(cv::Ptr<Blob>) function
 * is the actual operation function that does the work.
 * @author Ricard Borràs
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/15/2014
 */
class BlobOperator
{
 public:
  
  /** 
   * @brief Virtual destructor for safe inheritance
   */
	virtual ~BlobOperator();

  /** 
   * @brief Cached acces to the Blob's property
   * Calls this->operator()(pBlob) that subclasses implement.
   *
   * The BlobOperator class (and functions that inherit from it) use a data
   * caching strategy to minimize redundant computations for blob properties.
   * Each Blob has a properties() function that returns a
   * std::map<std::string, double> that contains cached properties of the Blob.
   * When a BlobOperator is invoked the Blob's properties() map is checked for a
   * value using BlobOperator::name() as the key.
   * If the map contains a value for the BlobOperator::name() then the double
   * value that maps to the key is returned. Otherwise the property is computed
   * and then stored in the Blob's properties() map using the
   * BlobOperator::name() as the key.
   * @param pBlob The blob to operate on.
   * @return the value of the property this functor computes
   */
	double result(cv::Ptr<Blob> pBlob);

  /** 
   * @brief The name of this operation. 
   * @return Returns the name of the operation and must be unique with respect
   * to all other BlobOperator sub-classes.
   */
	virtual std::string name() = 0;
  
  /** 
   * @brief Conversion operator for turning the functor into a
   * BlobOperator*
   * @return Returns a BlobOperator* for this class
   */
	operator BlobOperator*();

 protected:

  /** 
   * @brief Performs the operation on pBlob.
   * Sub-classes must implement this function.
   * @param pBlob
   */
	virtual double operator()(cv::Ptr<Blob> pBlob) = 0;
};

/**
 * @class BlobGetID
 * @brief Functor that returns the ID of the Blob.
 */
class BlobGetID : public BlobOperator
{
 public:

  /**
   * @brief Returns the ID of \p pBlob.
   * @param pBlob the blob whose ID will be returned
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */
	virtual std::string name();
};

/**
 * @class BlobGetArea
 * @brief Functor that computes the area of the Blob
 */
class BlobGetArea : public BlobOperator
{
 public:

  /** 
   * @brief Returns the area of \p pBlob
   * @param pBlob the blob whose area will be returned
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */
	virtual std::string name();
};

/**
 * @class BlobGetPerimeter
 * @brief Functor that computes the perimeter of the Blob
 */
class BlobGetPerimeter : public BlobOperator
{
 public:

  /** 
   * @brief Returns the perimeter of \p pBlob
   * @param pBlob the blob whose perimeter will be returned 
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /** 
   * @brief Returns the name of this operation
   */
	virtual std::string name();
};

/**
 * @class BlobGetExterior
 * @brief Class to get the extern flag of a blob.
 * The extern flag of a Blob returns a number > 0 if the Blob touches the
 * image borders, or 0 of the Blob does NOT touch the image borders.
 */
class BlobGetExterior : public BlobOperator
{
 public:

  /** 
   * @brief Default constructor with no mask and no flags set.
   * operator()(cv::Ptr<Blob>) will return 0 every time.
   */
	BlobGetExterior();

  /** 
   * @brief Creates a functor to compute the extern flag of a Blob
   * using \p mask. Only the edges defined by the flags to this
   * constructor are considered when invoking operator()(cv::Ptr<Blob>).
   * @param mask the mask to use when computing the extern flag.
   * Only pixels in the mask with a value of 0 will be evaluated.
   * The mask image should be of type CV_8UC1.
   * @todo support alternative mask types
   * @todo use bitmasking or enum flags instead of bools?
   * @param bXBorderLeft flag to turn on/off considering the left border.
   * Default is true meaning consider the left border.
   * @param bXBorderRight flag to turn on/off considering the right border.
   * Default is true meaning consider the right border.
   * @param bYBorderTop  flag to turn on/off considering the top border.
   * Default is true meaning consider the top border.
   * @param bYBorderBottom  flag to turn on/off considering the bottom border.
   * Default is true meaning consider the bottom border.
   */
	BlobGetExterior(cv::Mat& mask, 
                  bool bXBorderLeft = true, 
                  bool bXBorderRight = true, 
                  bool bYBorderTop = true,
                  bool bYBorderBottom = true);

  /** 
   * @brief Determines if \p pBlob touches the exterior of the image.
   * @param pBlob the blob to operate on
   * @return A number > 0 if \p pBlob touches the image border.
   * Or 0 if \p pBlob does NOT touch the image borders
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */  
	virtual std::string name()
	{
		return "BlobGetExterior";
	}
  
 private:

  //! the mask to use during the operation
  cv::Mat mMask;
  
  //! true if the left border should be considered during the operation
	bool mbXBorderLeft;
  
  //! true if the right border should be considered during the operation
  bool mbXBorderRight;
  
  //! true if the top border should be considered during the operation
  bool mbYBorderTop;
  
  //! true if the bottom border should be considered during the operation
  bool mbYBorderBottom;
};

/**
 * @class BlobGetMean
 * @brief Functor that computes the mean of the grey level pixels inside of a
 * Blob
 * @todo ability to use a mask
 */
class BlobGetMean : public BlobOperator
{
 public:

  /** 
   * @brief Default constructor with an empty image.
   * The operator()(cv::Ptr<Blob>) will return 0 every time.
   */
	BlobGetMean();

  /** 
   * @brief Constructor that uses \p image when computing the mean value of
   * the pixels inside a Blob.
   * @param image the image to use for the values inside the Blob.
   */
	BlobGetMean(cv::Mat& image);

  /** 
   * @brief Computes the mean of all pixels inside \p pBlob using the image
   * given in the constructor.
   * @param pBlob the Blob to use to compute the mean.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMean";
	}
  
 private:

  //! the image to use during the operation
  cv::Mat mImage;
};

/**
 * @class BlobGetStdDev
 * @brief Functor that computes the standard deviation of the grey level pixels
 * of a Blob
 * @todo ability to use a mask
 */
class BlobGetStdDev : public BlobOperator
{
 public:
  
  /** 
   * @brief Default constructor with an empty image.
   * The operator()(cv::Ptr<Blob>) will return 0 every time.
   */
	BlobGetStdDev();

  /** 
   * @brief Constructor that uses \p image when computing the standard deviation
   * of the pixels inside a Blob.
   * @param image the image to use for the values inside the Blob.
   */
	BlobGetStdDev(cv::Mat& image);

  /** 
   * @brief Computes the standard deviation of all pixels inside \p pBlob using
   * the image given in the constructor.
   * @param pBlob the Blob to use to compute the standard deviation.
   */  
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetStdDev";
	}
  
 private:
  
  //! the image to use during the operation
  cv::Mat mImage;
};

/**
 * @class BlobGetStdDev
 * @brief Functor that computes the mean grey level of all pixels inside of a
 * Blob relative to a value.
 * @todo ability to use a mask
 */
class BlobGetReferencedMean : public BlobOperator
{
 public:
  
  /** 
   * @brief Default constructor with an empty image.
   * The operator()(cv::Ptr<Blob>) will return 0 every time.
   */
	BlobGetReferencedMean();

  /** 
   * @brief Constructor that uses \p image when computing the mean value of
   * the pixels inside a Blob.
   * @param image the image to use for the values inside the Blob.
   * @param reference the reference value for the mean.
   */
	BlobGetReferencedMean(cv::Mat& image, double reference);

  /** 
   * @brief Computes the mean of all pixels inside \p pBlob relative to
   * the reference given in the constructor using the image given in the
   * constructor.
   * @param pBlob the Blob to use to compute the mean relative to a value.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */
	virtual std::string name();
  
 private:

  //! the image to use during the operation
  cv::Mat mImage;
  
	//! Value to reference the mean
	double mReference;
};

/**
 * @class BlobGetCompactness
 * @brief Functor that computes the compactness of a Blob. <br>
 * Let L be the perimeter of the Blob. <br>
 * Let A be the area of the Blob. <br>
 * Compactness Q = L^2 / (4 * pi * A)
 * @see http://www.personal.psu.edu/nmc15/blogs/anthspace/2009/04/arcmap-field-calculations-for-polygons.html
 * @see http://progmat.uw.hu/oktseg/kepelemzes/lec13_shape_4.pdf
 * @see https://en.wikipedia.org/wiki/Shape_factor
 * @see https://gis.stackexchange.com/questions/85812/easily-calculate-roundness-compactness-of-a-polygon
 */
class BlobGetCompactness : public BlobOperator
{
 public:
  
  /** 
   * @brief Calculates the compactness of \p pBlob (maximum for circle shaped
   * blobs, minimum for the rest)
   * @param pBlob the Blob to compute the compactness of
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);

  /** 
   * @brief Returns the name of this operation
   */  
	virtual std::string name()
	{
		return "BlobGetCompactness";
	}
};
 
/**
 * @class BlobGetLength
 * @brief Functor that computes the length of a Blob
 * (the biggest axis of the blob).
 * @note The computed length is an aproximation to the real length.
 * @see https://en.wikipedia.org/wiki/Shape_factor_(image_analysis_and_microscopy)
 * @todo reference for algorithm
 */
class BlobGetLength : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the length (estimate) of \p pBlob.
   * @param pBlob the Blob to compute the length of.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetLength";
	}
};

/**
 * @class BlobGetBreadth
 * @brief Functor that computes the breadth of a Blob
 * (the smallest axis of the blob).
 * @note The computed bredth is an aproximation to the real breadth.
 * @todo reference for algorithm
 */
class BlobGetBreadth : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the breadth (estimate) of \p pBlob.
   * @param pBlob the Blob to compute the breadth of.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetBreadth";
	}
};

/**
 * @class BlobGetDiffX
 * @brief Functor that computes difference in the X axis of the Blob. This
 * is also the Blob's bounding box's width.
 */
class BlobGetDiffX : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the difference in X of \p pBlob. 
   * @param pBlob the Blob to compute the difference of X.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetDiffX";
	}
};

/**
 * @class BlobGetDiffY
 * @brief Functor that computes difference in the Y axis of the Blob. This
 * is also the Blob's bounding box's height.
 */
class BlobGetDiffY : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the difference in Y of \p pBlob. 
   * @param pBlob the Blob to compute the difference of Y.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetDiffY";
	}
};

/** 
 * @class BlobGetMoment
 * @brief Functor to compute the P,Q moment of a blob.
 * Moment P,Q is denoted Mpq below.
 * - M00 = area (number of pixels in image)
 * - M10 = sum over X
 * - M01 = sum over Y
 * - centroid x = M10 / M00
 * - centroid y = M01 / M00
 *
 * Moments where P + Q = 1 are denoted as the "first momenets" (1, 0), (0, 1).
 * Moments where P + Q = 2 are denoted as the "second moments" (2, 0), (0, 2),
 * (1, 1).
 * Momements can be calculated up to order defined by CVBLOBS_MAX_MOMENTS_ORDER
 * in cvblobs2/CvBlobsDefs.h
 *
 * @see https://en.wikipedia.org/wiki/Image_moment
 */
class BlobGetMoment : public BlobOperator
{
 public:
  
  /** 
   * @brief Default constructor that computes the 0,0 moment (area)
   */
	BlobGetMoment();
  
  /** 
   * @brief Computes that computes the P,Q moment using \p p , \p q.
   * This can compute moments up to order CVBLOBS_MAX_MOMENTS_ORDER
   * @param p 
   * @param q 
   */
	BlobGetMoment(int p, int q);

  /** 
   * @brief Computes the P,Q moment of \p pBlob
   * @param pBlob the Blob to compute the moment of.
   */
	virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMoment";
	}

 private:
  
  //! the P,Q moment to calculate
	int mP;
  int mQ;
};

/**
 * @class BlobGetHullPerimeter
 * @brief Functor to calculate the convex hull perimeter of a Blob
 * @todo algorithm reference
 */
class BlobGetHullPerimeter : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the convex hull perimeter of \p pBlob.
   * @param pBlob the Blob to use to compute the convex hull
   * @return the convex hull perimeter of the blob or the perimeter if the 
   * blob edges could not be retrieved.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetHullPerimeter";
	}
};

/**
 * @class BlobGetHullArea
 * @brief Functor to calculate the convex hull area of a Blob
 * @todo algorithm reference
 */
class BlobGetHullArea : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the convex hull perimeter of \p pBlob.
   * @param pBlob the Blob to use to compute the convex hull
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetHullArea";
	}
};

/**
 * @class BlobGetMinXatMinY
 * @brief Functor to to calculate the minimum x on the minimum y.
 */
class BlobGetMinXatMinY : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes minimum x on the minimum y of \p pBlob
   * @param pBlob the Blob to use to compute the minimum x on the minimum y
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMinXatMinY";
	}
};

/**
 * @class BlobGetMinYatMaxX
 * @brief Functor to calculate the minimum y on the maximum x.
 */
class BlobGetMinYatMaxX : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes minimum y on the maximum x of \p pBlob
   * @param pBlob the Blob to use to compute the minimum y on the maximum x
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMinYatMaxX";
	}
};

/**
 * @class BlobGetMaxXatMaxY
 * @brief Functor to calculate the maximum x on the maximum y.
 */
class BlobGetMaxXatMaxY : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes maximum x on the maximum y of \p pBlob
   * @param pBlob the Blob to use to compute the maximum x on the maximum y
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMaxXatMaxY";
	}
};

/**
 * @class BlobGetMaxYatMinX
 * @brief Functor to calculate the maximum y on the minimum x.
 */
class BlobGetMaxYatMinX : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes maximum y on the minimum x of \p pBlob
   * @param pBlob the Blob to use to compute the maximum y on the minimum x
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMaxYatMinX";
	}
};

/**
 * @class BlobGetMinX
 * @brief Functor to calculate the minimum x.
 */
class BlobGetMinX : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the minimum x of \p pBlob
   * @param pBlob the Blob to use to compute the minimum x
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMinX";
	}
};

/**
 * @class BlobGetMaxX
 * @brief Functor to calculate the maximum x.
 */
class BlobGetMaxX : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the maximum x of \p pBlob
   * @param pBlob the Blob to use to compute the maximum x
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMaxX";
	}
};

/**
 * @class BlobGetMinY
 * @brief Functor to calculate the minimum y.
 */
class BlobGetMinY : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the minimum y of \p pBlob
   * @param pBlob the Blob to use to compute the minimum y
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMinY";
	}
};

/**
 * @class BlobGetMaxY
 * @brief Functor to calculate the maximum y.
 */
class BlobGetMaxY : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the maximum y of \p pBlob
   * @param pBlob the Blob to use to compute the maximum y
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMaxY";
	}
};


/**
 * @class BlobGetElongation
 * @brief Functor to calculate the elongation of a Blob (length / breadth).
 * @see https://en.wikipedia.org/wiki/Shape_factor_(image_analysis_and_microscopy)#Elongation_shape_factor
 */
class BlobGetElongation : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the elongation of \p pBlob (length / breadth)
   * @param pBlob the Blob to use to compute the elongation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetElongation";
	}
};

/**
 * @class BlobGetRoughness
 * @brief Functor to calculate the roughness of a Blob (ratio between perimeter
 * and convex hull perimeter).
 */
class BlobGetRoughness : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the roughness of \p pBlob (ratio between perimeter
   * and convex hull perimeter)
   * @param pBlob the Blob to use to compute the roughness
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetRoughness";
	}
};

/**
 * @class BlobGetDistanceFromPoint
 * @brief Functor to calculate the euclidean distance between the center of a
 * Blob and a given point
 */
class BlobGetDistanceFromPoint: public BlobOperator
{
 public:
  
  /** 
   * @brief Default constructor (distance to point 0,0)
   */
	BlobGetDistanceFromPoint();

  /** 
   * @brief Default constructor (distance to point \p x, \p y )
   * @param x the x component of the point
   * @param y the y component of the point
   */
	BlobGetDistanceFromPoint(const double x, const double y);

  /** 
   * @brief Default constructor (distance to \p point)
   * @param point the point to compute the distance to
   */
	BlobGetDistanceFromPoint(const cv::Point& point);
  
  /** 
   * @brief Calculates the euclidean distance between the center of \p pBlob
   * and the point specified in the constructor.
   * @param pBlob 
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetDistanceFromPoint";
	}

 private:
  
  //! point to compute the distance to
  cv::Point2d mPoint;
};

/**
 * @class BlobGetExternPerimeter
 * @brief Functor to get the number of extern pixels of a blob.
 */
class BlobGetExternPerimeter : public BlobOperator
{
 public:

  /** 
   * @brief Default constructor with all border flags disabled.
   * An instance created with this constructor will always return 0
   * from operator()(cv::Ptr<Blob>).
   */
  BlobGetExternPerimeter();

  /**
   * @brief Creates a functor to compute the extern perimeter of a Blob
   * using \p mask. Only the edges defined by the flags to this
   * constructor are considered when invoking operator()(cv::Ptr<Blob>).
   * @param mask the mask to use when computing the extern perimeter.
   * Only pixels in the mask with a value of 0 will be evaluated.
   * The mask image should be of type CV_8UC1.
   * @todo support alternative mask types
   * @todo use bitmasking or enum flags instead of bools?
   * @param bXBorderLeft flag to turn on/off considering the left border.
   * Default is true meaning consider the left border.
   * @param bXBorderRight flag to turn on/off considering the right border.
   * Default is true meaning consider the right border.
   * @param bYBorderTop  flag to turn on/off considering the top border.
   * Default is true meaning consider the top border.
   * @param bYBorderBottom  flag to turn on/off considering the bottom border.
   * Default is true meaning consider the bottom border.
   */
	BlobGetExternPerimeter(cv::Mat& mask, 
                         bool bXBorderLeft,
                         bool bXBorderRight,
                         bool bYBorderTop,
                         bool bYBorderBottom);

  /** 
   * @brief Computes the extern perimeter of \p pBlob.
   * @param pBlob the blob to compute the extern perimeter of.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */
  virtual std::string name()
	{
		return "BlobGetExternPerimeter";
	}
  
 private:

  //! the mask to use during the operation
  cv::Mat mMask;
  
  //! true if the left border should be considered during the operation
	bool mbXBorderLeft;
  
  //! true if the right border should be considered during the operation
  bool mbXBorderRight;
  
  //! true if the top border should be considered during the operation
  bool mbYBorderTop;
  
  //! true if the bottom border should be considered during the operation
  bool mbYBorderBottom;
};

/**
 * @class BlobGetExternPerimeterRatio
 * @brief Class to calculate the ratio between the perimeter and the number of
 * extern pixels. Values ​​close to 0 indicate that most of the blob is
 * internal. Values ​​close to 1 indicate that most of the blob is external.
 */
class BlobGetExternPerimeterRatio : public BlobOperator
{
 public:
  
  /** 
   * @brief Default constructor with all border flags disabled.
   * An instance created with this constructor will always return 0
   * from operator()(cv::Ptr<Blob>).
   */
	BlobGetExternPerimeterRatio();

  /**
   * @brief Creates a functor to compute the extern perimeter ratio of a Blob
   * using \p mask. Only the edges defined by the flags to this
   * constructor are considered when invoking operator()(cv::Ptr<Blob>).
   * @param mask the mask to use when computing the extern permieter ratio.
   * Only pixels in the mask with a value of 0 will be evaluated.
   * The mask image should be of type CV_8UC1.
   * @todo support alternative mask types
   * @todo use bitmasking or enum flags instead of bools?
   * @param bXBorderLeft flag to turn on/off considering the left border.
   * Default is true meaning consider the left border.
   * @param bXBorderRight flag to turn on/off considering the right border.
   * Default is true meaning consider the right border.
   * @param bYBorderTop  flag to turn on/off considering the top border.
   * Default is true meaning consider the top border.
   * @param bYBorderBottom  flag to turn on/off considering the bottom border.
   * Default is true meaning consider the bottom border.
   */  
	BlobGetExternPerimeterRatio(cv::Mat& mask,
                              bool bXBorderLeft = true,
                              bool bXBorderRight = true, 
                              bool bYBorderTop = true,
                              bool bYBorderBottom = true);

  /** 
   * @brief 
   * @param pBlob  
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */  
	virtual std::string name()
	{
		return "BlobGetExternPerimeterRatio";
	}
  
 private:

  //! the mask to use during the operation
  cv::Mat mMask;
  
  //! true if the left border should be considered during the operation
	bool mbXBorderLeft;
  
  //! true if the right border should be considered during the operation
  bool mbXBorderRight;
  
  //! true if the top border should be considered during the operation
  bool mbYBorderTop;
  
  //! true if the bottom border should be considered during the operation
  bool mbYBorderBottom;
};

/**
 * @class BlobGetExternHullPerimeterRatio
 * @brief Class to calculate the ratio between the extern perimeter and the
 * perimeter of the convex hull. Values ​​close to 0 indicate that most of the
 * blob is internal. Values ​​close to 1 indicate that most of the blob is
 * external.
 */
class BlobGetExternHullPerimeterRatio : public BlobOperator
{
 public:
  
  /** 
   * @brief Default constructor with all border flags disabled.
   * An instance created with this constructor will always return 0
   * from operator()(cv::Ptr<Blob>).
   */
  BlobGetExternHullPerimeterRatio();

  /**
   * @brief Creates a functor to compute the extern hull perimeter ratio of a
   * Blob using \p mask. Only the edges defined by the flags to this
   * constructor are considered when invoking operator()(cv::Ptr<Blob>).
   * @param mask the mask to use when computing the extern hull permieter ratio.
   * Only pixels in the mask with a value of 0 will be evaluated.
   * The mask image should be of type CV_8UC1.
   * @todo support alternative mask types
   * @todo use bitmasking or enum flags instead of bools?
   * @param bXBorder flag to turn on/off considering the x border.
   * Default is true meaning consider the x border.
   * @param bYBorder flag to turn on/off considering the y border.
   * Default is true meaning consider the y border.
   */    
	BlobGetExternHullPerimeterRatio(cv::Mat& mask,
                                  bool bXBorder = true,
                                  bool bYBorder = true);

  /** 
   * @brief Computes the ratio between the extern perimeter and
   * the perimeter of the convex hull of \p pBlob
   * @param pBlob the blob to use in the computation.
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */  
	virtual std::string name()
	{
		return "BlobGetExternHullPerimeterRatio";
	}
  
 private:
  
  //! the mask to use during the operation
  cv::Mat mMask;
  
  //! true if the x border should be considered during the operation
	bool mbXBorder;
  
  //! true if the y border should be considered during the operation
  bool mbYBorder;
};

/**
 * @class BlobGetXCenter
 * @brief Functor to calculate the center of the blob in the X direction.
 * @note The center is calculated using the bounding box so it may not be the
 * true geometric centroid.
 * @todo proper centroid implementation
 */
class BlobGetXCenter : public BlobOperator
{
 public:

  /** 
   * @brief Computes the center of \p pBlob in the X direction using the
   * bounding box.
   * @param pBlob the blob to compute the center of
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetXCenter";
	}
};


/**
 * @class BlobGetYCenter
 * @brief Functor to calculate the center of the blob in the Y direction.
 * @note The center is calculated using the bounding box so it may not be the
 * true geometric centroid.
 * @todo proper centroid implementation
 */
class BlobGetYCenter : public BlobOperator
{
 public:
  
  /** 
   * @brief Computes the center of \p pBlob in the Y direction using the
   * bounding box.
   * @param pBlob the blob to compute the center of
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetYCenter";
	}
};

/**
 * @class BlobGetMajorAxisLength
 * @brief Functor to calculate the length of the major axis of the ellipse that
 * fits the blob edges (the width of the ellipse).
 */
class BlobGetMajorAxisLength : public BlobOperator
{
 public:

  /** 
   * @brief Compute the length of the "major" axis of the bounding ellipse of
   * \p pBlob (the width of the ellipse).
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMajorAxisLength";
	}
};

/**
 * @class BlobGetAreaElipseRatio
 * @brief Functor to calculate the ratio between the area of the bounding
 * ellipse and the area of the Blob.
 */
class BlobGetAreaElipseRatio : public BlobOperator
{
 public:
  
  /** 
   * @brief Compute the ratio between the area of \p pBlob's bounding ellipse
   * and the area of \p pBlob.
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetAreaElipseRatio";
	}
};

/**
 * @class BlobGetMinorAxisLength
 * @brief Class to calculate the length of the minor axis of the ellipse that
 * fits the blob edges (the height of the ellipse).
 */
class BlobGetMinorAxisLength : public BlobOperator
{
 public:
  
  /** 
   * @brief Compute the length of the "minor" axis of the bounding ellipse of
   * \p pBlob (the height of the ellipse).
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetMinorAxisLength";
	}
};

/**
 * @class BlobGetOrientation
 * @brief Functor to calculate the orientation of the ellipse that fits the
 * blob edges in radians (angle of the ellipse's major axis with respect to
 * the X axis, ie the meadured ratation of the ellipse).
 */
class BlobGetOrientation : public BlobOperator
{
 public:
  
  /** 
   * @brief Calculate the orientation of the ellipse that fits the
   * \p pBlob edges in radians (angle of the ellipse's major axis with respect
   * to the X axis, ie the meadured ratation of the ellipse).
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetOrientation";
	}
};

/**
 * @class BlobGetElipseXCenter
 * @brief Functor to calculate the X center of the ellipse that fits the Blob's
 * edges.
 */
class BlobGetElipseXCenter : public BlobOperator
{
 public:
  
  /** 
   * @brief Calculates the X center of the ellipse that fits \p pBlob's
   * edges.
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetElipseXCenter";
	}
};



/**
 * @class BlobGetElipseYCenter
 * @brief Functor to calculate the Y center of the ellipse that fits the Blob's
 * edges.
 */
class BlobGetElipseYCenter : public BlobOperator
{
 public:
  
  /** 
   * @brief Calculates the Y center of the ellipse that fits \p pBlob's
   * edges.
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetElipseYCenter";
	}
};

/**
 * @class BlobGetOrientationCos
 * @brief Class to calculate the cosine of the orientation of the ellipse that
 * fits the blob edges
 */
class BlobGetOrientationCos : public BlobOperator
{
 public:
  
  /** 
   * @brief Calculate the cosine of the orientation of the ellipse that
   * fits \p pBlob's edges.
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetOrientationCos";
	}
};


/**
 * @class BlobGetAxisRatio
 * @brief Functor to calculate the ratio between both axes of the ellipse
 * (minor / major).
 */
class BlobGetAxisRatio : public BlobOperator
{
 public:

  /** 
   * @brief Calculates the ratio between the minor and major axes of the ellipse
   * of \p pBlob, (minor / major)
   * @param pBlob the Blob to use in the operation
   */
  virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetAxisRatio";
	}
};

/**
 * @class BlobGetXYInside
 * @brief Class to calculate whether a point is inside of a blob.
 */
class BlobGetXYInside : public BlobOperator
{
 public:

  /** 
   * @brief Default constructor with point a point of (0, 0)
   */
	BlobGetXYInside();

  /** 
   * @brief Constructor that uses a test point of (\p x, \p y).
   * @param x the x component of the test point
   * @param y the y component of the test point
   */
	BlobGetXYInside(const double& x, const double& y);

  /** 
   * @brief Constructor that uses a test point of \p point.
   * @param point the point to test with.
   */
	BlobGetXYInside(const cv::Point& point);

  /** 
   * @brief Tests if the point given to the constructor is within
   * \p pBlob.
   * @param pBlob the Blob to use in the operation
   * @return Returns a 1.0 if the point is inside \p pBlob.
   * Returns 0.0 if the point lies on the edge of \p pBlob.
   * Returns a -1.0 if the point lies outside of \p pBlob.
   */
	virtual double operator()(cv::Ptr<Blob> pBlob);
  
  /**
   * @brief Returns the name of this operation
   */  
	virtual std::string name()
	{
		return "BlobGetXYInside";
	}

 private:

	//! punt que considerem
	//! point to be considered
  cv::Point2d mPoint;
};

/**
 * @class BlobGetRelativeArea
 * @brief Functor to calculate the relative area of a blob with respect to the
 * theoretical size of the object.
 */
class BlobGetRelativeArea : public BlobOperator
{
 public:
  
  /** 
   * @brief Default constrctor that sets the total area to 1.0
   * and sets the theoretical area to 0.
   * Instances created with this constructor will always return 0.0
   * from operator()(cv::Ptr<Blob)
   */
	BlobGetRelativeArea();

  /** 
   * @brief Constructor that uses \p totalArea and \p theoreticalArea
   * in the comparison of the blob's area.
   * @param totalArea the total area to compare
   * @param theoreticalArea the theoretical area to compare
   */
	BlobGetRelativeArea(double totalArea, int theoreticalArea);

  /** 
   * @brief Computes the relative area of \p pBlob with respect to the
   * theoretical area given in the constructor.
   * @param pBlob the Blob to use in the operation
   */
	virtual double operator()(cv::Ptr<Blob> pBlob);
    
  /**
   * @brief Returns the name of this operation
   */
	virtual std::string name()
	{
		return "BlobGetRelativeArea";
	}

 private:

	//! Total area
	double mTotalArea;

	//! Theoretical area
	double mTheoreticalArea;
};

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_BLOBOPERATORS_H_
