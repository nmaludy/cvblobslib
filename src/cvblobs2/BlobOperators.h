#ifndef _CVBLOBS2_BLOBOPERATORS_H_
#define _CVBLOBS2_BLOBOPERATORS_H_

#include <cvblobs2/Blob.h>

/**************************************************************************
		Definició de les classes per a fer operacions sobre els blobs

		Helper classes to perform operations on blobs
**************************************************************************/

//! Factor de conversió de graus a radians
#define DEGREE2RAD		(CV_PI / 180.0)

CVBLOBS_BEGIN_NAMESPACE;

//! Classe d'on derivarem totes les operacions sobre els blobs
//! Interface to derive all blob operations
class BlobOperator
{
 public:
	virtual ~BlobOperator(){};

	double GetOperatorResult( Blob &blob )
	{
		double result;

		// verify if operator is calculated
		Blob::t_properties* p_props = blob.properties();
		if (p_props->find(GetNom()) == p_props->end())
		{
			// if not calculate it and add it to blob properties
			result = operator()(blob);
			(*p_props)[GetNom()] = result;
		}
		else
		{
			// if is calculate, get result (without calculating it again)
			result = (*p_props)[GetNom()];
		}

		return result;
	}

	//! Get operator name
	virtual std::string GetNom() = 0;

	operator BlobOperator*()
	{
		return (BlobOperator*)this;
	}

 private:

	//! Aply operator to blob
	virtual double operator()(Blob &blob) = 0;
};

typedef BlobOperator funcio_calculBlob;

#ifdef BLOB_OBJECT_FACTORY
/**
   Funció per comparar dos identificadors dins de la fàbrica de BlobOperators
*/
struct functorComparacioIdOperador
{
  bool operator()(const std::string &s1, const std::string &s2) const
  {
		return s1.compare(s2) > 0;
  }
};

//! Definition of Object factory type for BlobOperator objects
typedef ObjectFactory<BlobOperator, std::string, functorComparacioIdOperador > t_OperadorBlobFactory;

//! Funció global per a registrar tots els operadors definits a blob.h
void RegistraTotsOperadors( t_OperadorBlobFactory &fabricaOperadorsBlob );

#endif


//! Classe per calcular l'etiqueta d'un blob
//! Class to get ID of a blob
class BlobGetID : public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{ 
		return blob.id(); 
	}
	std::string GetNom()
	{
		return "BlobGetID";
	}
};


//! Classe per calcular l'àrea d'un blob
//! Class to get the area of a blob
class BlobGetArea : public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{ 
		return blob.area(); 
	}
	std::string GetNom()
	{
		return "BlobGetArea";
	}
};

//! Classe per calcular el perimetre d'un blob
//! Class to get the perimeter of a blob
class BlobGetPerimeter: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{ 
		return blob.perimeter(); 
	}
	std::string GetNom()
	{
		return "BlobGetPerimeter";
	}
};

//! Classe que diu si un blob és extern o no
//! Class to get the extern flag of a blob
class BlobGetExterior: public BlobOperator
{
 public:
	BlobGetExterior()
	{
		m_mask = NULL;
		m_xBorderLeft = false;
		m_xBorderRight = false;
		m_yBorderTop = false;
		m_yBorderBottom = false;
	}
	BlobGetExterior(IplImage *mask, 
                  bool xBorderLeft = true, 
                  bool xBorderRight = true, 
                  bool yBorderTop = true,
                  bool yBorderBottom = true)
	{
		m_mask = mask;
		m_xBorderLeft = xBorderLeft;
		m_xBorderRight = xBorderRight;
		m_yBorderTop = yBorderTop;
		m_yBorderBottom = yBorderBottom;
	}
  double operator()(Blob &blob)
	{ 
		return blob.exterior(m_mask,
                         m_xBorderLeft, m_xBorderRight,
                         m_yBorderTop,  m_yBorderBottom); 
	}
	std::string GetNom()
	{
		return "BlobGetExterior";
	}
 private:
	IplImage *m_mask;
	bool m_xBorderLeft, m_xBorderRight, 
    m_yBorderTop, m_yBorderBottom;
};

//! Classe per calcular la mitjana de nivells de gris d'un blob
//! Class to get the mean grey level of a blob
class BlobGetMean: public BlobOperator
{
 public:
	BlobGetMean()
	{
		m_image = NULL;
	}
	BlobGetMean( IplImage *image )
	{
		m_image = image;
	};

  double operator()(Blob &blob)
	{ 
		return blob.mean(m_image); 
	}
	std::string GetNom()
	{
		return "BlobGetMean";
	}
 private:

	IplImage *m_image;
};

//! Classe per calcular la desviació estàndard dels nivells de gris d'un blob
//! Class to get the standard deviation of the grey level values of a blob
class BlobGetStdDev: public BlobOperator
{
 public:
	BlobGetStdDev()
	{
		m_image = NULL;
	}
	BlobGetStdDev( IplImage *image )
	{
		m_image = image;
	};
  double operator()(Blob &blob)
	{ 
		return blob.stdDev(m_image); 
	}
	std::string GetNom()
	{
		return "BlobGetStdDev";
	}
 private:

	IplImage *m_image;

};


//! Classe per calcular la mitjana de nivells de gris d'un blob respecte un valor de referència
//! Class to get the mean grey level of a blob relative to a value
class BlobGetReferencedMean: public BlobOperator
{
 public:
	BlobGetReferencedMean()
	{
		m_image = NULL;
		m_reference = 0;
	}
	BlobGetReferencedMean( IplImage *image, double reference )
	{
		m_image = image;
		m_reference = reference;
	};

  double operator()(Blob &blob)
	{ 
		return m_reference - blob.mean(m_image);
	}
	std::string GetNom()
	{
		return "BlobGetReferencedMean";
	}
 private:
	//! Value to reference the mean
	double m_reference;
	IplImage *m_image;
};

//! Classe per calcular la compacitat d'un blob
//! Class to calculate the compactness of a blob
class BlobGetCompactness: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetCompactness";
	}
};

//! Classe per calcular la longitud d'un blob
//! Class to calculate the length of a blob
class BlobGetLength: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetLength";
	}
};

//! Classe per calcular l'amplada d'un blob
//! Class to calculate the breadth of a blob
class BlobGetBreadth: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetBreadth";
	}
};

//! Classe per calcular la diferència en X del blob
class BlobGetDiffX: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.boundingBox().width;
	}
	std::string GetNom()
	{
		return "BlobGetDiffX";
	}
};

//! Classe per calcular la diferència en X del blob
class BlobGetDiffY: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.boundingBox().height;
	}
	std::string GetNom()
	{
		return "BlobGetDiffY";
	}
};

//! Classe per calcular el moment PQ del blob
//! Class to calculate the P,Q moment of a blob
class BlobGetMoment: public BlobOperator
{
 public:
	//! Constructor estàndard
	//! Standard constructor (gets the 00 moment)
	BlobGetMoment()
	{
		m_p = m_q = 0;
	}
	//! Constructor: indiquem el moment p,q a calcular
	//! Constructor: gets the PQ moment
	BlobGetMoment( int p, int q )
	{
		m_p = p;
		m_q = q;
	};
	double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetMoment";
	}

 private:
	//! moment que volem calcular
	int m_p, m_q;
};

//! Classe per calcular el perimetre del poligon convex d'un blob
//! Class to calculate the convex hull perimeter of a blob
class BlobGetHullPerimeter: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetHullPerimeter";
	}
};

//! Classe per calcular l'àrea del poligon convex d'un blob
//! Class to calculate the convex hull area of a blob
class BlobGetHullArea: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetHullArea";
	}
};

//! Classe per calcular la x minima en la y minima
//! Class to calculate the minimum x on the minimum y
class BlobGetMinXatMinY: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetMinXatMinY";
	}
};

//! Classe per calcular la y minima en la x maxima
//! Class to calculate the minimum y on the maximum x
class BlobGetMinYatMaxX: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetMinYatMaxX";
	}
};

//! Classe per calcular la x maxima en la y maxima
//! Class to calculate the maximum x on the maximum y
class BlobGetMaxXatMaxY: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetMaxXatMaxY";
	}
};

//! Classe per calcular la y maxima en la x minima
//! Class to calculate the maximum y on the minimum y
class BlobGetMaxYatMinX: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetMaxYatMinX";
	}
};

//! Classe per a calcular la x mínima
//! Class to get the minimum x
class BlobGetMinX: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.minX();
	}
	std::string GetNom()
	{
		return "BlobGetMinX";
	}
};

//! Classe per a calcular la x màxima
//! Class to get the maximum x
class BlobGetMaxX: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.maxX();
	}
	std::string GetNom()
	{
		return "BlobGetMaxX";
	}
};

//! Classe per a calcular la y mínima
//! Class to get the minimum y
class BlobGetMinY: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.minY();
	}
	std::string GetNom()
	{
		return "BlobGetMinY";
	}
};

//! Classe per a calcular la y màxima
//! Class to get the maximum y
class BlobGetMaxY: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.maxY();
	}
	std::string GetNom()
	{
		return "BlobGetMaxY";
	}
};


//! Classe per calcular l'elongacio d'un blob
//! Class to calculate the elongation of the blob
class BlobGetElongation: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetElongation";
	}
};

//! Classe per calcular la rugositat d'un blob
//! Class to calculate the roughness of the blob
class BlobGetRoughness: public BlobOperator
{
 public:
  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetRoughness";
	}
};

//! Classe per calcular la distància entre el centre del blob i un punt donat
//! Class to calculate the euclidean distance between the center of a blob and a given point
class BlobGetDistanceFromPoint: public BlobOperator
{
 public:
	//! Standard constructor (distance to point 0,0)
	BlobGetDistanceFromPoint()
	{
		m_x = m_y = 0.0;
	}
	//! Constructor (distance to point x,y)
	BlobGetDistanceFromPoint( const double x, const double y )
	{
		m_x = x;
		m_y = y;
	}

  double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetDistanceFromPoint";
	}

 private:
	// coordenades del punt on volem calcular la distància
	double m_x, m_y;
};

//! Classe per calcular el nombre de pixels externs d'un blob
//! Class to get the number of extern pixels of a blob
class BlobGetExternPerimeter: public BlobOperator
{
 public:
	BlobGetExternPerimeter()
	{
		m_mask = NULL;
		m_xBorderLeft= m_xBorderRight= m_yBorderTop= m_yBorderBottom = false;
	}
	BlobGetExternPerimeter( IplImage *mask, bool xBorderLeft, bool xBorderRight, 
                          bool yBorderTop, bool yBorderBottom)
	{
		m_mask = mask;
		m_xBorderLeft = xBorderLeft;
		m_xBorderRight = xBorderRight;
		m_yBorderTop = yBorderTop;
		m_yBorderBottom = yBorderBottom;
	}
  double operator()(Blob &blob)
	{
		return blob.externPerimeter(m_mask,
                                m_xBorderLeft, m_xBorderRight,
                                m_yBorderTop,  m_yBorderBottom);
	}
	std::string GetNom()
	{
		return "BlobGetExternPerimeter";
	}
 private:
	IplImage *m_mask;
	bool m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom;
};

//! Classe per calcular el ratio entre el perimetre i nombre pixels externs
//! valors propers a 0 indiquen que la majoria del blob és intern
//! valors propers a 1 indiquen que la majoria del blob és extern
//! Class to calculate the ratio between the perimeter and the number of extern pixels
class BlobGetExternPerimeterRatio: public BlobOperator
{
 public:
	BlobGetExternPerimeterRatio()
	{
		m_mask = NULL;
		m_xBorderLeft= m_xBorderRight= m_yBorderTop= m_yBorderBottom = false;
	}
	BlobGetExternPerimeterRatio( IplImage *mask,
                               bool xBorderLeft = true,
                               bool xBorderRight = true, 
                               bool yBorderTop = true,
                               bool yBorderBottom = true )
	{
		m_mask = mask;
		m_xBorderLeft = xBorderLeft;
		m_xBorderRight = xBorderRight;
		m_yBorderTop = yBorderTop;
		m_yBorderBottom = yBorderBottom;
	}
  double operator()(Blob &blob)
	{
		if (blob.perimeter() != 0)
			return blob.externPerimeter(m_mask,
                                  m_xBorderLeft, m_xBorderRight,
                                  m_yBorderTop,  m_yBorderBottom) / blob.perimeter();
		else
			return blob.externPerimeter(m_mask,
                                  m_xBorderLeft, m_xBorderRight,
                                  m_yBorderTop, m_yBorderBottom);
	}
	std::string GetNom()
	{
		return "BlobGetExternPerimeterRatio";
	}
 private:
	IplImage *m_mask;
	bool m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom;

};

//! Classe per calcular el ratio entre el perimetre convex i nombre pixels externs
//! valors propers a 0 indiquen que la majoria del blob és intern
//! valors propers a 1 indiquen que la majoria del blob és extern
//! Class to calculate the ratio between the perimeter and the number of extern pixels
class BlobGetExternHullPerimeterRatio: public BlobOperator
{
 public:
	BlobGetExternHullPerimeterRatio()
	{
		m_mask = NULL;
		m_xBorder = false;
		m_yBorder = false;
	}
	BlobGetExternHullPerimeterRatio( IplImage *mask, bool xBorder = true, bool yBorder = true )
	{
		m_mask = mask;
		m_xBorder = xBorder;
		m_yBorder = yBorder;
	}
  double operator()(Blob &blob)
	{
		BlobGetHullPerimeter getHullPerimeter;
		double hullPerimeter;

		if( (hullPerimeter = getHullPerimeter( blob ) ) != 0 )
			return blob.externPerimeter(m_mask, m_xBorder, m_yBorder) / hullPerimeter;
		else
			return blob.externPerimeter(m_mask, m_xBorder, m_yBorder);
	}
	std::string GetNom()
	{
		return "BlobGetExternHullPerimeterRatio";
	}
 private:
	IplImage *m_mask;
	bool  m_xBorder, m_yBorder;

};

//! Classe per calcular el centre en el eix X d'un blob
//! Class to calculate the center in the X direction
class BlobGetXCenter: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.minX() + (( blob.maxX() - blob.minX() ) / 2.0);
	}
	std::string GetNom()
	{
		return "BlobGetXCenter";
	}
};

//! Classe per calcular el centre en el eix Y d'un blob
//! Class to calculate the center in the Y direction
class BlobGetYCenter: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		return blob.minY() + (( blob.maxY() - blob.minY() ) / 2.0);
	}
	std::string GetNom()
	{
		return "BlobGetYCenter";
	}
};

//! Classe per calcular la longitud de l'eix major d'un blob
//! Class to calculate the length of the major axis of the ellipse that fits the blob edges
class BlobGetMajorAxisLength: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		CvBox2D elipse = blob.ellipse();

		return elipse.size.width;
	}
	std::string GetNom()
	{
		return "BlobGetMajorAxisLength";
	}
};

//! Classe per calcular el ratio entre l'area de la elipse i la de la taca
//! Class 
class BlobGetAreaElipseRatio: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		if (blob.area() == 0.0)
    {
      return 0.0;
    }

		CvBox2D elipse = blob.ellipse();
		double ratioAreaElipseAreaTaca = ( ( (elipse.size.width/2.0) *
                                         (elipse.size.height/2.0) * CV_PI) /
                                       blob.area() );

		return ratioAreaElipseAreaTaca;
	}
	std::string GetNom()
	{
		return "BlobGetAreaElipseRatio";
	}
};

//! Classe per calcular la longitud de l'eix menor d'un blob
//! Class to calculate the length of the minor axis of the ellipse that fits the blob edges
class BlobGetMinorAxisLength: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		CvBox2D elipse = blob.ellipse();

		return elipse.size.height;
	}
	std::string GetNom()
	{
		return "BlobGetMinorAxisLength";
	}
};

//! Classe per calcular l'orientació de l'ellipse del blob en radians
//! Class to calculate the orientation of the ellipse that fits the blob edges in radians
class BlobGetOrientation: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		CvBox2D elipse = blob.ellipse();
		return elipse.angle;
	}
	std::string GetNom()
	{
		return "BlobGetOrientation";
	}
};

//! Classe per calcular el centre en x de la elipse d'un blob
//! Class to calculate the x center of the ellipse that fits the blob edges
class BlobGetElipseXCenter: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		CvBox2D elipse = blob.ellipse();
		return elipse.center.x;
	}
	std::string GetNom()
	{
		return "BlobGetElipseXCenter";
	}
};


//! Classe per calcular el centre en y de la elipse d'un blob
//! Class to calculate the y center of the ellipse that fits the blob edges
class BlobGetElipseYCenter: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		CvBox2D elipse = blob.ellipse();
		return elipse.center.y;
	}
	std::string GetNom()
	{
		return "BlobGetElipseYCenter";
	}
};

//! Classe per calcular el cosinus de l'orientació de l'ellipse del blob
//! Class to calculate the cosinus of the orientation of the ellipse that fits the blob edges
class BlobGetOrientationCos: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		BlobGetOrientation getOrientation;
		return fabs( cos( getOrientation(blob)*DEGREE2RAD ));
	}
	std::string GetNom()
	{
		return "BlobGetOrientationCos";
	}
};


//! Classe per calcular el ratio entre l'eix major i menor de la el·lipse
//! Class to calculate the ratio between both axes of the ellipse
class BlobGetAxisRatio: public BlobOperator
{
 public:
  double operator()(Blob &blob)
	{
		double major,minor;
		BlobGetMajorAxisLength getMajor;
		BlobGetMinorAxisLength getMinor;

		major = getMajor(blob);
		minor = getMinor(blob);

		if( major != 0 )
			return minor / major;
		else
			return 0;
	}
	std::string GetNom()
	{
		return "BlobGetAxisRatio";
	}
};


//! Classe per calcular si un punt cau dins del blob
//! Class to calculate whether a point is inside a blob
class BlobGetXYInside: public BlobOperator
{
 public:
	//! Constructor estàndard
	//! Standard constructor
	BlobGetXYInside()
	{
		m_p.x = 0;
		m_p.y = 0;
	}
	//! Constructor: indiquem el punt
	//! Constructor: sets the point
	BlobGetXYInside( CvPoint2D32f p )
	{
		m_p = p;
	};
	double operator()(Blob &blob);
	std::string GetNom()
	{
		return "BlobGetXYInside";
	}

 private:
	//! punt que considerem
	//! point to be considered
	CvPoint2D32f m_p;
};



//! Classe per calcular l'àrea relativa del blob respecte un tamany teóric de l'objecte
//! Class to calculate the relative area of a blob with respect to the theoretical size of the object

class BlobGetRelativeArea: public BlobOperator
{
 public:
	//! Constructor estàndard
	//! Standard constructor
	BlobGetRelativeArea()
	{
		m_totalArea = 1;
	}
	//! Constructor: indiquem l'àrea total i la teórica
	//! Constructor: sets the total area and the theoretical size
	BlobGetRelativeArea( double totalArea, int theoreticalArea )
	{
		m_totalArea = totalArea;
		m_theoreticalArea = theoreticalArea;
	};
	double operator()(Blob &blob)
	{
		if( m_totalArea )
			return (blob.area() / m_totalArea) * m_theoreticalArea;

		return 0;
	}
	std::string GetNom()
	{
		return "BlobGetRelativeArea";
	}

 private:
	//! Total area
	double m_totalArea;

	//! Theoretical area
	double m_theoreticalArea;
};

CVBLOBS_END_NAMESPACE;

#endif // _CVBLOBS2_BLOBOPERATORS_H_
