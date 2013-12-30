#ifndef BLOB_OPERATORS_H_INCLUDED
#define BLOB_OPERATORS_H_INCLUDED

#include "Blob.h"
/**************************************************************************
		Definició de les classes per a fer operacions sobre els blobs

		Helper classes to perform operations on blobs
**************************************************************************/

//! Factor de conversió de graus a radians
#define DEGREE2RAD		(CV_PI / 180.0)


//! Classe d'on derivarem totes les operacions sobre els blobs
//! Interface to derive all blob operations
class COperadorBlob
{
public:
	virtual ~COperadorBlob(){};

	double GetOperatorResult( CBlob &blob )
	{
		double result;

		// verify if operator is calculated
		CBlob::t_properties *props = blob.GetProperties();
		if( props->find(GetNom()) == props->end() )
		{
			// if not calculate it and add it to blob properties
			result = operator()(blob);
			(*props)[GetNom()] = result;

		}
		else
		{
			// if is calculate, get result (without calculating it again)
			result = (*props)[GetNom()];

		}

		return result;
	}

	//! Get operator name
	virtual std::string GetNom() = 0;

	operator COperadorBlob*()
	{
		return (COperadorBlob*)this;
	}

private:

	//! Aply operator to blob
	virtual double operator()(CBlob &blob) = 0;
};

typedef COperadorBlob funcio_calculBlob;

#ifdef BLOB_OBJECT_FACTORY
	/**
		Funció per comparar dos identificadors dins de la fàbrica de COperadorBlobs
	*/
	struct functorComparacioIdOperador
	{
	  bool operator()(const std::string &s1, const std::string &s2) const
	  {
		return s1.compare(s2) > 0;
	  }
	};

	//! Definition of Object factory type for COperadorBlob objects
	typedef ObjectFactory<COperadorBlob, std::string, functorComparacioIdOperador > t_OperadorBlobFactory;

	//! Funció global per a registrar tots els operadors definits a blob.h
	void RegistraTotsOperadors( t_OperadorBlobFactory &fabricaOperadorsBlob );

#endif


//! Classe per calcular l'etiqueta d'un blob
//! Class to get ID of a blob
class CBlobGetID : public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{ 
		return blob.GetID(); 
	}
	std::string GetNom()
	{
		return "CBlobGetID";
	}
};


//! Classe per calcular l'àrea d'un blob
//! Class to get the area of a blob
class CBlobGetArea : public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{ 
		return blob.Area(); 
	}
	std::string GetNom()
	{
		return "CBlobGetArea";
	}
};

//! Classe per calcular el perimetre d'un blob
//! Class to get the perimeter of a blob
class CBlobGetPerimeter: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{ 
		return blob.Perimeter(); 
	}
	std::string GetNom()
	{
		return "CBlobGetPerimeter";
	}
};

//! Classe que diu si un blob és extern o no
//! Class to get the extern flag of a blob
class CBlobGetExterior: public COperadorBlob
{
public:
	CBlobGetExterior()
	{
		m_mask = NULL;
		m_xBorderLeft = false;
		m_xBorderRight = false;
		m_yBorderTop = false;
		m_yBorderBottom = false;
	}
	CBlobGetExterior(IplImage *mask, 
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
    double operator()(CBlob &blob)
	{ 
		return blob.Exterior(m_mask, m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom); 
	}
	std::string GetNom()
	{
		return "CBlobGetExterior";
	}
private:
	IplImage *m_mask;
	bool m_xBorderLeft, m_xBorderRight, 
		 m_yBorderTop, m_yBorderBottom;
};

//! Classe per calcular la mitjana de nivells de gris d'un blob
//! Class to get the mean grey level of a blob
class CBlobGetMean: public COperadorBlob
{
public:
	CBlobGetMean()
	{
		m_image = NULL;
	}
	CBlobGetMean( IplImage *image )
	{
		m_image = image;
	};

    double operator()(CBlob &blob)
	{ 
		return blob.Mean(m_image); 
	}
	std::string GetNom()
	{
		return "CBlobGetMean";
	}
private:

	IplImage *m_image;
};

//! Classe per calcular la desviació estàndard dels nivells de gris d'un blob
//! Class to get the standard deviation of the grey level values of a blob
class CBlobGetStdDev: public COperadorBlob
{
public:
	CBlobGetStdDev()
	{
		m_image = NULL;
	}
	CBlobGetStdDev( IplImage *image )
	{
		m_image = image;
	};
    double operator()(CBlob &blob)
	{ 
		return blob.StdDev(m_image); 
	}
	std::string GetNom()
	{
		return "CBlobGetStdDev";
	}
private:

	IplImage *m_image;

};


//! Classe per calcular la mitjana de nivells de gris d'un blob respecte un valor de referència
//! Class to get the mean grey level of a blob relative to a value
class CBlobGetReferencedMean: public COperadorBlob
{
public:
	CBlobGetReferencedMean()
	{
		m_image = NULL;
		m_reference = 0;
	}
	CBlobGetReferencedMean( IplImage *image, double reference )
	{
		m_image = image;
		m_reference = reference;
	};

    double operator()(CBlob &blob)
	{ 
		return m_reference - blob.Mean(m_image);
	}
	std::string GetNom()
	{
		return "CBlobGetReferencedMean";
	}
private:
	//! Value to reference the mean
	double m_reference;
	IplImage *m_image;
};

//! Classe per calcular la compacitat d'un blob
//! Class to calculate the compactness of a blob
class CBlobGetCompactness: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetCompactness";
	}
};

//! Classe per calcular la longitud d'un blob
//! Class to calculate the length of a blob
class CBlobGetLength: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetLength";
	}
};

//! Classe per calcular l'amplada d'un blob
//! Class to calculate the breadth of a blob
class CBlobGetBreadth: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetBreadth";
	}
};

//! Classe per calcular la diferència en X del blob
class CBlobGetDiffX: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.GetBoundingBox().width;
	}
	std::string GetNom()
	{
		return "CBlobGetDiffX";
	}
};

//! Classe per calcular la diferència en X del blob
class CBlobGetDiffY: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.GetBoundingBox().height;
	}
	std::string GetNom()
	{
		return "CBlobGetDiffY";
	}
};

//! Classe per calcular el moment PQ del blob
//! Class to calculate the P,Q moment of a blob
class CBlobGetMoment: public COperadorBlob
{
public:
	//! Constructor estàndard
	//! Standard constructor (gets the 00 moment)
	CBlobGetMoment()
	{
		m_p = m_q = 0;
	}
	//! Constructor: indiquem el moment p,q a calcular
	//! Constructor: gets the PQ moment
	CBlobGetMoment( int p, int q )
	{
		m_p = p;
		m_q = q;
	};
	double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetMoment";
	}

private:
	//! moment que volem calcular
	int m_p, m_q;
};

//! Classe per calcular el perimetre del poligon convex d'un blob
//! Class to calculate the convex hull perimeter of a blob
class CBlobGetHullPerimeter: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetHullPerimeter";
	}
};

//! Classe per calcular l'àrea del poligon convex d'un blob
//! Class to calculate the convex hull area of a blob
class CBlobGetHullArea: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetHullArea";
	}
};

//! Classe per calcular la x minima en la y minima
//! Class to calculate the minimum x on the minimum y
class CBlobGetMinXatMinY: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetMinXatMinY";
	}
};

//! Classe per calcular la y minima en la x maxima
//! Class to calculate the minimum y on the maximum x
class CBlobGetMinYatMaxX: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetMinYatMaxX";
	}
};

//! Classe per calcular la x maxima en la y maxima
//! Class to calculate the maximum x on the maximum y
class CBlobGetMaxXatMaxY: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetMaxXatMaxY";
	}
};

//! Classe per calcular la y maxima en la x minima
//! Class to calculate the maximum y on the minimum y
class CBlobGetMaxYatMinX: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetMaxYatMinX";
	}
};

//! Classe per a calcular la x mínima
//! Class to get the minimum x
class CBlobGetMinX: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.MinX();
	}
	std::string GetNom()
	{
		return "CBlobGetMinX";
	}
};

//! Classe per a calcular la x màxima
//! Class to get the maximum x
class CBlobGetMaxX: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.MaxX();
	}
	std::string GetNom()
	{
		return "CBlobGetMaxX";
	}
};

//! Classe per a calcular la y mínima
//! Class to get the minimum y
class CBlobGetMinY: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.MinY();
	}
	std::string GetNom()
	{
		return "CBlobGetMinY";
	}
};

//! Classe per a calcular la y màxima
//! Class to get the maximum y
class CBlobGetMaxY: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.MaxY();
	}
	std::string GetNom()
	{
		return "CBlobGetMaxY";
	}
};


//! Classe per calcular l'elongacio d'un blob
//! Class to calculate the elongation of the blob
class CBlobGetElongation: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetElongation";
	}
};

//! Classe per calcular la rugositat d'un blob
//! Class to calculate the roughness of the blob
class CBlobGetRoughness: public COperadorBlob
{
public:
    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetRoughness";
	}
};

//! Classe per calcular la distància entre el centre del blob i un punt donat
//! Class to calculate the euclidean distance between the center of a blob and a given point
class CBlobGetDistanceFromPoint: public COperadorBlob
{
public:
	//! Standard constructor (distance to point 0,0)
	CBlobGetDistanceFromPoint()
	{
		m_x = m_y = 0.0;
	}
	//! Constructor (distance to point x,y)
	CBlobGetDistanceFromPoint( const double x, const double y )
	{
		m_x = x;
		m_y = y;
	}

    double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetDistanceFromPoint";
	}

private:
	// coordenades del punt on volem calcular la distància
	double m_x, m_y;
};

//! Classe per calcular el nombre de pixels externs d'un blob
//! Class to get the number of extern pixels of a blob
class CBlobGetExternPerimeter: public COperadorBlob
{
public:
	CBlobGetExternPerimeter()
	{
		m_mask = NULL;
		m_xBorderLeft= m_xBorderRight= m_yBorderTop= m_yBorderBottom = false;
	}
	CBlobGetExternPerimeter( IplImage *mask, bool xBorderLeft, bool xBorderRight, 
											 bool yBorderTop, bool yBorderBottom)
	{
		m_mask = mask;
		m_xBorderLeft = xBorderLeft;
		m_xBorderRight = xBorderRight;
		m_yBorderTop = yBorderTop;
		m_yBorderBottom = yBorderBottom;
	}
    double operator()(CBlob &blob)
	{
		return blob.ExternPerimeter(m_mask, m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom);
	}
	std::string GetNom()
	{
		return "CBlobGetExternPerimeter";
	}
private:
	IplImage *m_mask;
	bool m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom;
};

//! Classe per calcular el ratio entre el perimetre i nombre pixels externs
//! valors propers a 0 indiquen que la majoria del blob és intern
//! valors propers a 1 indiquen que la majoria del blob és extern
//! Class to calculate the ratio between the perimeter and the number of extern pixels
class CBlobGetExternPerimeterRatio: public COperadorBlob
{
public:
	CBlobGetExternPerimeterRatio()
	{
		m_mask = NULL;
		m_xBorderLeft= m_xBorderRight= m_yBorderTop= m_yBorderBottom = false;
	}
	CBlobGetExternPerimeterRatio( IplImage *mask, bool xBorderLeft = true, bool xBorderRight = true, 
								   bool yBorderTop = true, bool yBorderBottom = true )
	{
		m_mask = mask;
		m_xBorderLeft = xBorderLeft;
		m_xBorderRight = xBorderRight;
		m_yBorderTop = yBorderTop;
		m_yBorderBottom = yBorderBottom;
	}
    double operator()(CBlob &blob)
	{
		if( blob.Perimeter() != 0 )
			return blob.ExternPerimeter(m_mask, m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom) / blob.Perimeter();
		else
			return blob.ExternPerimeter(m_mask, m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom);
	}
	std::string GetNom()
	{
		return "CBlobGetExternPerimeterRatio";
	}
private:
	IplImage *m_mask;
	bool m_xBorderLeft, m_xBorderRight, m_yBorderTop, m_yBorderBottom;

};

//! Classe per calcular el ratio entre el perimetre convex i nombre pixels externs
//! valors propers a 0 indiquen que la majoria del blob és intern
//! valors propers a 1 indiquen que la majoria del blob és extern
//! Class to calculate the ratio between the perimeter and the number of extern pixels
class CBlobGetExternHullPerimeterRatio: public COperadorBlob
{
public:
	CBlobGetExternHullPerimeterRatio()
	{
		m_mask = NULL;
		m_xBorder = false;
		m_yBorder = false;
	}
	CBlobGetExternHullPerimeterRatio( IplImage *mask, bool xBorder = true, bool yBorder = true )
	{
		m_mask = mask;
		m_xBorder = xBorder;
		m_yBorder = yBorder;
	}
    double operator()(CBlob &blob)
	{
		CBlobGetHullPerimeter getHullPerimeter;
		double hullPerimeter;

		if( (hullPerimeter = getHullPerimeter( blob ) ) != 0 )
			return blob.ExternPerimeter(m_mask, m_xBorder, m_yBorder) / hullPerimeter;
		else
			return blob.ExternPerimeter(m_mask, m_xBorder, m_yBorder);
	}
	std::string GetNom()
	{
		return "CBlobGetExternHullPerimeterRatio";
	}
private:
	IplImage *m_mask;
	bool  m_xBorder, m_yBorder;

};

//! Classe per calcular el centre en el eix X d'un blob
//! Class to calculate the center in the X direction
class CBlobGetXCenter: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.MinX() + (( blob.MaxX() - blob.MinX() ) / 2.0);
	}
	std::string GetNom()
	{
		return "CBlobGetXCenter";
	}
};

//! Classe per calcular el centre en el eix Y d'un blob
//! Class to calculate the center in the Y direction
class CBlobGetYCenter: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		return blob.MinY() + (( blob.MaxY() - blob.MinY() ) / 2.0);
	}
	std::string GetNom()
	{
		return "CBlobGetYCenter";
	}
};

//! Classe per calcular la longitud de l'eix major d'un blob
//! Class to calculate the length of the major axis of the ellipse that fits the blob edges
class CBlobGetMajorAxisLength: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		CvBox2D elipse = blob.GetEllipse();

		return elipse.size.width;
	}
	std::string GetNom()
	{
		return "CBlobGetMajorAxisLength";
	}
};

//! Classe per calcular el ratio entre l'area de la elipse i la de la taca
//! Class 
class CBlobGetAreaElipseRatio: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		if( blob.Area()==0.0 ) return 0.0;

		CvBox2D elipse = blob.GetEllipse();
		double ratioAreaElipseAreaTaca = ( (elipse.size.width/2.0)
										   *
										   (elipse.size.height/2.0)
							               *CV_PI
						                 )
									     /
									     blob.Area();

		return ratioAreaElipseAreaTaca;
	}
	std::string GetNom()
	{
		return "CBlobGetAreaElipseRatio";
	}
};

//! Classe per calcular la longitud de l'eix menor d'un blob
//! Class to calculate the length of the minor axis of the ellipse that fits the blob edges
class CBlobGetMinorAxisLength: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		CvBox2D elipse = blob.GetEllipse();

		return elipse.size.height;
	}
	std::string GetNom()
	{
		return "CBlobGetMinorAxisLength";
	}
};

//! Classe per calcular l'orientació de l'ellipse del blob en radians
//! Class to calculate the orientation of the ellipse that fits the blob edges in radians
class CBlobGetOrientation: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		CvBox2D elipse = blob.GetEllipse();
		return elipse.angle;
	}
	std::string GetNom()
	{
		return "CBlobGetOrientation";
	}
};

//! Classe per calcular el centre en x de la elipse d'un blob
//! Class to calculate the x center of the ellipse that fits the blob edges
class CBlobGetElipseXCenter: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		CvBox2D elipse = blob.GetEllipse();
		return elipse.center.x;
	}
	std::string GetNom()
	{
		return "CBlobGetElipseXCenter";
	}
};


//! Classe per calcular el centre en y de la elipse d'un blob
//! Class to calculate the y center of the ellipse that fits the blob edges
class CBlobGetElipseYCenter: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		CvBox2D elipse = blob.GetEllipse();
		return elipse.center.y;
	}
	std::string GetNom()
	{
		return "CBlobGetElipseYCenter";
	}
};

//! Classe per calcular el cosinus de l'orientació de l'ellipse del blob
//! Class to calculate the cosinus of the orientation of the ellipse that fits the blob edges
class CBlobGetOrientationCos: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		CBlobGetOrientation getOrientation;
		return fabs( cos( getOrientation(blob)*DEGREE2RAD ));
	}
	std::string GetNom()
	{
		return "CBlobGetOrientationCos";
	}
};


//! Classe per calcular el ratio entre l'eix major i menor de la el·lipse
//! Class to calculate the ratio between both axes of the ellipse
class CBlobGetAxisRatio: public COperadorBlob
{
public:
    double operator()(CBlob &blob)
	{
		double major,minor;
		CBlobGetMajorAxisLength getMajor;
		CBlobGetMinorAxisLength getMinor;

		major = getMajor(blob);
		minor = getMinor(blob);

		if( major != 0 )
			return minor / major;
		else
			return 0;
	}
	std::string GetNom()
	{
		return "CBlobGetAxisRatio";
	}
};


//! Classe per calcular si un punt cau dins del blob
//! Class to calculate whether a point is inside a blob
class CBlobGetXYInside: public COperadorBlob
{
public:
	//! Constructor estàndard
	//! Standard constructor
	CBlobGetXYInside()
	{
		m_p.x = 0;
		m_p.y = 0;
	}
	//! Constructor: indiquem el punt
	//! Constructor: sets the point
	CBlobGetXYInside( CvPoint2D32f p )
	{
		m_p = p;
	};
	double operator()(CBlob &blob);
	std::string GetNom()
	{
		return "CBlobGetXYInside";
	}

private:
	//! punt que considerem
	//! point to be considered
	CvPoint2D32f m_p;
};



//! Classe per calcular l'àrea relativa del blob respecte un tamany teóric de l'objecte
//! Class to calculate the relative area of a blob with respect to the theoretical size of the object

class CBlobGetRelativeArea: public COperadorBlob
{
public:
	//! Constructor estàndard
	//! Standard constructor
	CBlobGetRelativeArea()
	{
		m_totalArea = 1;
	}
	//! Constructor: indiquem l'àrea total i la teórica
	//! Constructor: sets the total area and the theoretical size
	CBlobGetRelativeArea( double totalArea, int theoreticalArea )
	{
		m_totalArea = totalArea;
		m_theoreticalArea = theoreticalArea;
	};
	double operator()(CBlob &blob)
	{
		if( m_totalArea )
			return (blob.Area() / m_totalArea) * m_theoreticalArea;

		return 0;
	}
	std::string GetNom()
	{
		return "CBlobGetRelativeArea";
	}

private:
	//! Total area
	double m_totalArea;

	//! Theoretical area
	double m_theoreticalArea;
};

#endif	//!BLOB_OPERATORS_H_INCLUDED