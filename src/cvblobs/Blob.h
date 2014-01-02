/************************************************************************
  			Blob.h
  			
FUNCIONALITAT: Definició de la classe CBlob
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):

FUNCTIONALITY: Definition of the CBlob class and some helper classes to perform
			   some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/

#ifndef _CVBLOBS_BLOB_H_
#define _CVBLOBS_BLOB_H_

#ifdef _MSC_VER
	//! Disable warnings referred to 255 character truncation for the std:map
	#pragma warning( disable : 4786 )
#endif

#include <opencv/cxcore.h>
#include <cvblobs/BlobLibraryConfiguration.h>
#include <cvblobs/BlobContour.h>
#include <map>

#ifdef BLOB_OBJECT_FACTORY
	//! Object factory pattern implementation
	#include "..\inspecta\DesignPatterns\ObjectFactory.h"
#endif


//! Blob class
class CBlob
{
	struct mapComparator
	{
	  bool operator()(const std::string &s1, const std::string &s2) const
	  {
		return s1.compare(s2) > 0;
	  }
	};
	
public:
	//! Type of blob properties: a map with variant types
	typedef std::map<std::string, double, mapComparator> t_properties;


	CBlob();
	CBlob( t_labelType id, CvPoint startPoint, CvSize originalImageSize );
	~CBlob();

	//! Copy constructor
	CBlob( const CBlob &src );
	CBlob( const CBlob *src );

	//! Operador d'assignació
	//! Assigment operator
	CBlob& operator=(const CBlob &src );
	
	//! Adds a new internal contour to the blob
	void AddInternalContour( const CBlobContour &newContour );
	
	//! Retrieves contour in Freeman's chain code
	CBlobContour *GetExternalContour()
	{
		return &m_externalContour;
	}

	//! Retrieves blob storage
	CvMemStorage *GetStorage()
	{
		return m_storage;
	}

	//! Get label ID
	t_labelType GetID()
	{
		return m_id;
	}
	//! > 0 for extern blobs, 0 if not
	int	  Exterior( IplImage *mask, 
					bool xBorderLeft = true, bool xBorderRight = true,
					bool yBorderTop = true , bool yBorderBottom = true );
	//! Compute blob's area
	double Area();
	//! Compute blob's perimeter
	double Perimeter();
	//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
	double Moment(int p, int q);

	//! Compute extern perimeter 
	double ExternPerimeter( IplImage *mask,
							bool xBorderLeft = true, bool xBorderRight = true,
							bool yBorderTop = true , bool yBorderBottom = true );
	
	//! Get mean grey color
	double Mean( IplImage *image );

	//! Get standard deviation grey color
	double StdDev( IplImage *image );

	//! Indica si el blob està buit ( no té cap info associada )
	//! Shows if the blob has associated information
	bool IsEmpty();

	//! Retorna el poligon convex del blob
	//! Calculates the convex hull of the blob
	t_PointList GetConvexHull();

	//! Pinta l'interior d'un blob d'un color determinat
	//! Paints the blob in an image
	void FillBlob( IplImage *imatge, CvScalar color );

	//! Join a blob to current one (add's contour
	void JoinBlob( CBlob *blob );

	//! Get bounding box
	CvRect GetBoundingBox();
	//! Get bounding ellipse
	CvBox2D GetEllipse();

	//! Minimun X	
	double MinX()
	{
		return GetBoundingBox().x;
	}
	//! Minimun Y
	double MinY()
	{
		return GetBoundingBox().y;
	}
	//! Maximun X
	double MaxX()
	{
		return GetBoundingBox().x + GetBoundingBox().width;
	}
	//! Maximun Y
	double MaxY()
	{
		return GetBoundingBox().y + GetBoundingBox().height;
	}

	//! Get mutable blob properties
	t_properties *GetProperties()
	{
		return &m_properties;
	}

	//! Remove calculated property from results
	void ResetProperty( const std::string &name )
	{
		m_properties.erase( name );
	}
private:

	static int cmp_func( const void* _a, const void* _b, void* userdata )
	{
    	CvPoint* a = (CvPoint*)_a;
    	CvPoint* b = (CvPoint*)_b;
	    int y_diff = a->y - b->y;
	    int x_diff = a->x - b->x;
    	return y_diff ? y_diff : x_diff;
	}

	//! Calculates mean and std deviation of blob in input image
	void GetMeanAndStdDev( IplImage *image, double &mean, double &stddev );
	
	//! Deallocates all contours
	void ClearContours();
	//////////////////////////////////////////////////////////////////////////
	// Blob contours
	//////////////////////////////////////////////////////////////////////////


	//! Contour storage memory
	CvMemStorage *m_storage;
	//! External contour of the blob (crack codes)
  CBlobContour m_externalContour;
	//! Internal contours (crack codes)
	t_contourList m_internalContours;

	//////////////////////////////////////////////////////////////////////////
	// Blob features
	//////////////////////////////////////////////////////////////////////////

	// all calculated blob properties
	t_properties m_properties;


	//! Label number
	t_labelType m_id;
	//! Bounding box
	CvRect m_boundingBox;
	//! Sizes from image where blob is extracted
	CvSize m_originalImageSize;

};

#endif // _CVBLOBS_BLOB_H_
