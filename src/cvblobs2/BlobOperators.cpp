// deactivation of warning 4786 
// (identifier was truncated to '255' characters in the debug information)
#if _MSC_VER > 1000
  #pragma warning(disable: 4786)
  #pragma warning(disable: 4503)
#endif

#include <limits.h>
#include <cvblobs2/BlobOperators.h>

CVBLOBS_BEGIN_NAMESPACE;

/***************************************************************************
  Implementació de les classes per al càlcul de característiques sobre el blob

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
- AUTHOR: Ricard Borràs
- CREATION DATE: 20-07-2004.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMoment::operator()(Blob &blob)
{
	return blob.moment(m_p, m_q);
}

/**
- FUNCIÓ: HullPerimeter
- FUNCIONALITAT: Calcula la longitud del perimetre convex del blob.
			   Fa servir la funció d'OpenCV cvConvexHull2 per a 
			   calcular el perimetre convex.
			   
- PARÀMETRES:
- RESULTAT:
	- retorna la longitud del perímetre convex del blob. Si el blob no té coordenades
	  associades retorna el perímetre normal del blob.
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 20-07-2004.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: BlobGetHullPerimeter
- FUNCTIONALITY: Calculates the convex hull perimeter of the blob
- PARAMETERS:
- RESULT:
	- returns the convex hull perimeter of the blob or the perimeter if the 
	blob edges could not be retrieved
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetHullPerimeter::operator()(Blob &blob)
{
	CvSeq* p_convex_hull = blob.convexHull();
  double perimeter = 0.0;

  if (p_convex_hull != NULL)
  {
    perimeter = fabs(cvArcLength(p_convex_hull, CV_WHOLE_SEQ, 1));
    cvClearSeq(p_convex_hull);
  }
  
	return perimeter;
}

double BlobGetHullArea::operator()(Blob &blob)
{
	CvSeq* p_convex_hull = blob.convexHull();
	double area = 0.0;

  if (p_convex_hull != NULL)
  {
		area = fabs(cvContourArea(p_convex_hull));
    cvClearSeq(p_convex_hull);
  }

	return area;
}

/**
- FUNCTION: BlobGetMinXatMinY
- FUNCTIONALITY: Calculates the minimum X on the minimum Y
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMinXatMinY::operator()(Blob &blob)
{
	double result = LONG_MAX;
	
	CvSeqReader reader;
	CvPoint actual_point;
	t_PointList p_extern_contour = blob.externalContour()->contourPoints();
  
	if (p_extern_contour == NULL)
  {
    return result;
  }
	cvStartReadSeq( p_extern_contour, &reader);

	for (int i = 0; i < p_extern_contour->total; ++i)
	{
		CV_READ_SEQ_ELEM( actual_point, reader);

		if ((actual_point.y == blob.minY()) && (actual_point.x < result) )
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMinYatMaxX::operator()(Blob &blob)
{
	double result = LONG_MAX;
	
	CvSeqReader reader;
	CvPoint actual_point;
	t_PointList externContour;
	
	externContour = blob.externalContour()->contourPoints();
	if( !externContour ) return result;
	cvStartReadSeq( externContour, &reader);

	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actual_point, reader);

		if( (actual_point.x == blob.maxX()) && (actual_point.y < result) )
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMaxXatMaxY::operator()(Blob &blob)
{
	double result = LONG_MIN;
	
	CvSeqReader reader;
	CvPoint actual_point;
	t_PointList externContour;
	
	externContour = blob.externalContour()->contourPoints();
	if( !externContour ) return result;

	cvStartReadSeq( externContour, &reader);

	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actual_point, reader);

		if( (actual_point.y == blob.maxY()) && (actual_point.x > result) )
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobGetMaxYatMinX::operator()(Blob &blob)
{
	double result = LONG_MIN;
	
	CvSeqReader reader;
	CvPoint actual_point;
	t_PointList externContour;
	
	externContour = blob.externalContour()->contourPoints();
	if( !externContour ) return result;

	cvStartReadSeq( externContour, &reader);

	
	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actual_point, reader);

		if( (actual_point.x == blob.minX()) && (actual_point.y > result) )
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
- AUTHOR: Ricard Borràs
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
	// error intrínsec en els càlculs de l'àrea i el perímetre 
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
- AUTHOR: Ricard Borràs
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
- AUTHOR: Ricard Borràs
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
- AUTHOR: Ricard Borràs
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
	// error intrínsec en els càlculs de l'àrea i el perímetre 
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
- AUTHOR: Ricard Borràs
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
	// error intrínsec en els càlculs de l'àrea i el perímetre 
	else
		ampladaC = (double) (blob.perimeter())/4;

	if(ampladaC<=0.0) return 0;
	longitudC = (double) blob.area()/ampladaC;

	return MIN( longitudC , ampladaC );
}

/**
	Calcula la distància entre un punt i el centre del blob
*/
/**
- FUNCTION: BlobGetDistanceFromPoint
- FUNCTIONALITY: Calculates the euclidean distance between the blob center and 
				 the point specified in the constructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
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
	if( blob.externalContour()->contourPoints() )
	{
		return cvPointPolygonTest( blob.externalContour()->contourPoints(), m_p,0) >= 0;
	}

	return 0;
}
#ifdef BLOB_OBJECT_FACTORY

/**
- FUNCIÓ: RegistraTotsOperadors
- FUNCIONALITAT: Registrar tots els operadors definits a blob.h
- PARÀMETRES:
	- fabricaOperadorsBlob: fàbrica on es registraran els operadors
- RESULTAT:
	- Modifica l'objecte fabricaOperadorsBlob
- RESTRICCIONS:
	- Només es registraran els operadors de blob.h. Si se'n volen afegir, cal afegir-los amb 
	  el mètode Register de la fàbrica.
- AUTOR: rborras
- DATA DE CREACIÓ: 2006/05/18
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void RegistraTotsOperadors( t_OperadorBlobFactory &fabricaOperadorsBlob )
{
	// blob shape
	fabricaOperadorsBlob.Register( BlobGetArea().GetNom(), Type2Type<BlobGetArea>());
	fabricaOperadorsBlob.Register( BlobGetRelativeArea().GetNom(), Type2Type<BlobGetRelativeArea>());
	fabricaOperadorsBlob.Register( BlobGetBreadth().GetNom(), Type2Type<BlobGetBreadth>());
	fabricaOperadorsBlob.Register( BlobGetCompactness().GetNom(), Type2Type<BlobGetCompactness>());
	fabricaOperadorsBlob.Register( BlobGetElongation().GetNom(), Type2Type<BlobGetElongation>());
	fabricaOperadorsBlob.Register( BlobGetExterior().GetNom(), Type2Type<BlobGetExterior>());
	fabricaOperadorsBlob.Register( BlobGetLength().GetNom(), Type2Type<BlobGetLength>());
	fabricaOperadorsBlob.Register( BlobGetPerimeter().GetNom(), Type2Type<BlobGetPerimeter>());
	fabricaOperadorsBlob.Register( BlobGetRoughness().GetNom(), Type2Type<BlobGetRoughness>());

	// blob color
	fabricaOperadorsBlob.Register( BlobGetMean(NULL).GetNom(), Type2Type<BlobGetMean>());
	fabricaOperadorsBlob.Register( BlobGetReferencedMean(NULL,0).GetNom(), Type2Type<BlobGetReferencedMean>());
	fabricaOperadorsBlob.Register( BlobGetStdDev(NULL).GetNom(), Type2Type<BlobGetStdDev>());

	// extern pixels
	fabricaOperadorsBlob.Register( BlobGetExternPerimeterRatio().GetNom(), Type2Type<BlobGetExternPerimeterRatio>());
	fabricaOperadorsBlob.Register( BlobGetExternHullPerimeterRatio().GetNom(), Type2Type<BlobGetExternHullPerimeterRatio>());
	fabricaOperadorsBlob.Register( BlobGetExternPerimeter().GetNom(), Type2Type<BlobGetExternPerimeter>());
	

	// hull 
	fabricaOperadorsBlob.Register( BlobGetHullPerimeter().GetNom(), Type2Type<BlobGetHullPerimeter>());
	fabricaOperadorsBlob.Register( BlobGetHullArea().GetNom(), Type2Type<BlobGetHullArea>());
	

	// elipse info
	fabricaOperadorsBlob.Register( BlobGetMajorAxisLength().GetNom(), Type2Type<BlobGetMajorAxisLength>());
	fabricaOperadorsBlob.Register( BlobGetMinorAxisLength().GetNom(), Type2Type<BlobGetMinorAxisLength>());
	fabricaOperadorsBlob.Register( BlobGetAxisRatio().GetNom(), Type2Type<BlobGetAxisRatio>());
	fabricaOperadorsBlob.Register( BlobGetOrientation().GetNom(), Type2Type<BlobGetOrientation>());
	fabricaOperadorsBlob.Register( BlobGetOrientationCos().GetNom(), Type2Type<BlobGetOrientationCos>());
	fabricaOperadorsBlob.Register( BlobGetAreaElipseRatio().GetNom(), Type2Type<BlobGetAreaElipseRatio>());

	// min an max
	fabricaOperadorsBlob.Register( BlobGetMaxX().GetNom(), Type2Type<BlobGetMaxX>());
	fabricaOperadorsBlob.Register( BlobGetMaxY().GetNom(), Type2Type<BlobGetMaxY>());
	fabricaOperadorsBlob.Register( BlobGetMinX().GetNom(), Type2Type<BlobGetMinX>());
	fabricaOperadorsBlob.Register( BlobGetMinY().GetNom(), Type2Type<BlobGetMinY>());

	fabricaOperadorsBlob.Register( BlobGetMaxXatMaxY().GetNom(), Type2Type<BlobGetMaxXatMaxY>());
	fabricaOperadorsBlob.Register( BlobGetMaxYatMinX().GetNom(), Type2Type<BlobGetMaxYatMinX>());
	fabricaOperadorsBlob.Register( BlobGetMinXatMinY().GetNom(), Type2Type<BlobGetMinXatMinY>());
	fabricaOperadorsBlob.Register( BlobGetMinYatMaxX().GetNom(), Type2Type<BlobGetMinYatMaxX>());

	// coordinate info
	fabricaOperadorsBlob.Register( BlobGetXYInside().GetNom(), Type2Type<BlobGetXYInside>());
	fabricaOperadorsBlob.Register( BlobGetDiffY().GetNom(), Type2Type<BlobGetDiffY>());
	fabricaOperadorsBlob.Register( BlobGetDiffX().GetNom(), Type2Type<BlobGetDiffX>());
	fabricaOperadorsBlob.Register( BlobGetXCenter().GetNom(), Type2Type<BlobGetXCenter>());
	fabricaOperadorsBlob.Register( BlobGetYCenter().GetNom(), Type2Type<BlobGetYCenter>());
	fabricaOperadorsBlob.Register( BlobGetDistanceFromPoint().GetNom(), Type2Type<BlobGetDistanceFromPoint>());

	// moments
	fabricaOperadorsBlob.Register( BlobGetMoment().GetNom(), Type2Type<BlobGetMoment>());
}

#endif	//BLOB_OBJECT_FACTORY

CVBLOBS_END_NAMESPACE;
