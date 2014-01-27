#include <cvblobs2/BlobOperators.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <limits>

CVBLOBS_BEGIN_NAMESPACE

/***************************************************************************
  Implementaci� de les classes per al c�lcul de caracter�stiques sobre el blob

  Implementation of the helper classes to perform operations on blobs
**************************************************************************/

/**
- FUNCTION: Moment
- FUNCTIONALITY: Calculates the pq moment of the blob
- PARAMETERS:
- RESULT:
	- returns the pq moment or 0 if the moment it is not implemented
- RESTRICTIONS:
	- Currently, implemented moments up to 3
- AUTHOR: Ricard Borr�s
- CREATION DATE: 20-07-2004.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMoment::operator()(Blob &blob)
{
	return blob.moment(m_p, m_q);
}

/**
- FUNCI�: HullPerimeter
- FUNCIONALITAT: Calcula la longitud del perimetre convex del blob.
			   Fa servir la funci� d'OpenCV cvConvexHull2 per a 
			   calcular el perimetre convex.
			   
- PAR�METRES:
- RESULTAT:
	- retorna la longitud del per�metre convex del blob. Si el blob no t� coordenades
	  associades retorna el per�metre normal del blob.
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 20-07-2004.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: BlobGetHullPerimeter
- FUNCTIONALITY: Calculates the convex hull perimeter of the blob
- PARAMETERS:
- RESULT:
	- returns the convex hull perimeter of the blob or the perimeter if the 
	blob edges could not be retrieved
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetHullPerimeter::operator()(Blob &blob)
{
	PointContainer convex_hull;
  blob.convexHull(convex_hull);
  double perimeter = 0.0;

  if (!convex_hull.empty())
  {
    perimeter = fabs( cv::arcLength(convex_hull, true) );  // true == closed
  }
  
	return perimeter;
}

double BlobGetHullArea::operator()(Blob &blob)
{
	PointContainer convex_hull;
  blob.convexHull(convex_hull);
	double area = 0.0;

  if (!convex_hull.empty())
  {
		area = fabs( cv::contourArea(convex_hull) );
  }

	return area;
}

/**
- FUNCTION: BlobGetMinXatMinY
- FUNCTIONALITY: Calculates the minimum X on the minimum Y
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMinXatMinY::operator()(Blob &blob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainer& extern_contour = blob.externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainer::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;
		if (actual_point.y == blob.minY() &&
        actual_point.x < result)
		{
			result = actual_point.x;
		}	
	}

	return result;
}

/**
- FUNCTION: BlobGetMinXatMinY
- FUNCTIONALITY: Calculates the minimum Y on the maximum X
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMinYatMaxX::operator()(Blob &blob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainer& extern_contour = blob.externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainer::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;	
		if (actual_point.x == blob.maxX() &&
        actual_point.y < result)
		{
			result = actual_point.y;
		}	
	}

	return result;
}

/**
- FUNCTION: BlobGetMaxXatMaxY
- FUNCTIONALITY: Calculates the maximum X on the maximum Y
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMaxXatMaxY::operator()(Blob &blob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainer& extern_contour = blob.externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainer::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;

    if (actual_point.y == blob.maxY() &&
        actual_point.x > result)
		{
			result = actual_point.x;
		}	
	}

	return result;
}

/**
- FUNCTION: BlobGetMaxYatMinX
- FUNCTIONALITY: Calculates the maximum Y on the minimum X
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMaxYatMinX::operator()(Blob &blob)
{
	double result = std::numeric_limits<double>::max();
	const PointContainer& extern_contour = blob.externalContour()->contourPoints();
  
	if (extern_contour.empty())
  {
    return result;
  }

  typedef PointContainer::const_iterator PointIter;
  // define end outside for loop for performance and intent
  // we are not going to modify this container
  PointIter end_iter = extern_contour.end();
	for (PointIter iter = extern_contour.begin(); iter != end_iter; ++iter)
	{
    const cv::Point& actual_point = *iter;
    
		if (actual_point.x == blob.minX() &&
        actual_point.y > result)
		{
			result = actual_point.y;
		}	
	}

	return result;
}


/**
- FUNCTION: BlobGetElongation
- FUNCTIONALITY: Calculates the elongation of the blob ( length/breadth )
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
	- See below to see how the lenght and the breadth are aproximated
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetElongation::operator()(Blob &blob)
{
	double ampladaC,longitudC,amplada,longitud;

	double tmp;

	if ( blob.area() == 0 )
		return 0;

	tmp = blob.perimeter()*blob.perimeter() - 16*blob.area();

	if( tmp > 0.0 )
		ampladaC = (double) (blob.perimeter()+sqrt(tmp))/4;
	// error intr�nsec en els c�lculs de l'�rea i el per�metre 
	else
		ampladaC = (double) (blob.perimeter())/4;

	if(ampladaC<=0.0) return 0;

	longitudC=(double) blob.area()/ampladaC;

	longitud=MAX( longitudC , ampladaC );
	amplada=MIN( longitudC , ampladaC );

	return (double) longitud/amplada;
}

/**
	Retorna la compacitat del blob
*/
/**
- FUNCTION: BlobGetCompactness
- FUNCTIONALITY: Calculates the compactness of the blob 
			    ( maximum for circle shaped blobs, minimum for the rest)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetCompactness::operator()(Blob &blob)
{
	if( blob.area() != 0.0 )
		return (double) pow(blob.perimeter(),2)/(4*CV_PI*blob.area());
	else
		return 0.0;
}

/**
	Retorna la rugositat del blob
*/
/**
- FUNCTION: BlobGetRoughness
- FUNCTIONALITY: Calculates the roughness of the blob 
			    ( ratio between perimeter and convex hull perimeter)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetRoughness::operator()(Blob &blob)
{
	BlobGetHullPerimeter getHullPerimeter = BlobGetHullPerimeter();
	
	double hullPerimeter = getHullPerimeter(blob);

	if( hullPerimeter != 0.0 )
		return blob.perimeter() / hullPerimeter;//HullPerimeter();

	return 0.0;
}

/**
	Retorna la longitud del blob
*/
/**
- FUNCTION: BlobGetLength
- FUNCTIONALITY: Calculates the lenght of the blob (the biggest axis of the blob)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
	- The lenght is an aproximation to the real lenght
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetLength::operator()(Blob &blob)
{
	double ampladaC,longitudC;
	double tmp;

	tmp = blob.perimeter()*blob.perimeter() - 16*blob.area();

	if( tmp > 0.0 )
		ampladaC = (double) (blob.perimeter()+sqrt(tmp))/4;
	// error intr�nsec en els c�lculs de l'�rea i el per�metre 
	else
		ampladaC = (double) (blob.perimeter())/4;

	if(ampladaC<=0.0) return 0;
	longitudC=(double) blob.area()/ampladaC;

	return MAX( longitudC , ampladaC );
}

/**
	Retorna l'amplada del blob
*/
/**
- FUNCTION: BlobGetBreadth
- FUNCTIONALITY: Calculates the breadth of the blob (the smallest axis of the blob)
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
	- The breadth is an aproximation to the real breadth
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetBreadth::operator()(Blob &blob)
{
	double ampladaC,longitudC;
	double tmp;

	tmp = blob.perimeter()*blob.perimeter() - 16*blob.area();

	if( tmp > 0.0 )
		ampladaC = (double) (blob.perimeter()+sqrt(tmp))/4;
	// error intr�nsec en els c�lculs de l'�rea i el per�metre 
	else
		ampladaC = (double) (blob.perimeter())/4;

	if(ampladaC<=0.0) return 0;
	longitudC = (double) blob.area()/ampladaC;

	return MIN( longitudC , ampladaC );
}

/**
	Calcula la dist�ncia entre un punt i el centre del blob
*/
/**
- FUNCTION: BlobGetDistanceFromPoint
- FUNCTIONALITY: Calculates the euclidean distance between the blob center and 
				 the point specified in the constructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetDistanceFromPoint::operator()(Blob &blob)
{
	double xmitjana, ymitjana;
	BlobGetXCenter getXCenter;
	BlobGetYCenter getYCenter;

	xmitjana = m_x - getXCenter( blob );
	ymitjana = m_y - getYCenter( blob );

	return sqrt((xmitjana*xmitjana)+(ymitjana*ymitjana));
}

/**
- FUNCTION: BlobGetXYInside
- FUNCTIONALITY: Calculates whether a point is inside the
    rectangular bounding box of a blob
- PARAMETERS:
- RESULT:
	- returns 1 if it is inside; o if not
- RESTRICTIONS:
- AUTHOR: Francesc Pinyol Margalef
- CREATION DATE: 16-01-2006.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetXYInside::operator()(Blob &blob)
{
  const PointContainer& extern_contour = blob.externalContour()->contourPoints();
	if (!extern_contour.empty())
	{
    // false == don't measure distance
		return cv::pointPolygonTest(extern_contour, mPoint, false) >= 0;
	}

	return 0;
}

CVBLOBS_END_NAMESPACE
