/************************************************************************
  			Blob.cpp
  			
- FUNCIONALITAT: Implementaci� de la classe CBlob
- AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):

 
FUNCTIONALITY: Implementation of the CBlob class and some helper classes to perform
			   some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/

#include <cvblobs/Blob.h>
#include <cvblobs/BlobOperators.h>

CBlob::CBlob()
{
	m_storage = NULL;
}
CBlob::CBlob( t_labelType id, CvPoint startPoint, CvSize originalImageSize )
{
	m_storage = cvCreateMemStorage();
	m_externalContour = CBlobContour(startPoint, m_storage);
	m_boundingBox.width = -1;
	m_id = id;
	m_originalImageSize = originalImageSize;
}
//! Copy constructor
CBlob::CBlob( const CBlob &src )
{
	m_storage = NULL;
	*this = src;
}

CBlob::CBlob( const CBlob *src )
{
	if (src != NULL )
	{
		m_storage = NULL;
		*this = *src;
	}
}

CBlob& CBlob::operator=(const CBlob &src )
{
	if( this != &src )
	{
		m_id = src.m_id;
		m_originalImageSize = src.m_originalImageSize;
		m_boundingBox = src.m_boundingBox;

		// copy all blob properties
		m_properties.clear();
		std::copy(src.m_properties.begin(), src.m_properties.end(), std::inserter(m_properties, m_properties.begin()));

		// clear all current blob contours
		ClearContours();
		
		if( m_storage )
			cvReleaseMemStorage( &m_storage );

		m_storage = cvCreateMemStorage();

		m_externalContour = CBlobContour(src.m_externalContour.GetStartPoint(), m_storage );
		if( src.m_externalContour.m_contour )
			m_externalContour.m_contour = cvCloneSeq( src.m_externalContour.m_contour, m_storage);
		m_internalContours.clear();

		// copy all internal contours
		if( src.m_internalContours.size() )
		{
			m_internalContours = t_contourList( src.m_internalContours.size() );
			t_contourList::const_iterator itSrc;
			t_contourList::iterator it;

			itSrc = src.m_internalContours.begin();
			it = m_internalContours.begin();

			while (itSrc != src.m_internalContours.end())
			{
				*it = CBlobContour((*itSrc).GetStartPoint(), m_storage);
				if( (*itSrc).m_contour )
					(*it).m_contour = cvCloneSeq( (*itSrc).m_contour, m_storage);

				it++;
				itSrc++;
			}
		}
	}

	return *this;
}

CBlob::~CBlob()
{
	ClearContours();

	m_properties.clear();
	
	if( m_storage )
		cvReleaseMemStorage( &m_storage );
}

void CBlob::ClearContours()
{
	t_contourList::iterator it;

	it = m_internalContours.begin();

	while (it != m_internalContours.end())
	{
		(*it).ResetChainCode();
		it++;
	}	
	m_internalContours.clear();

	m_externalContour.ResetChainCode();
		
}
void CBlob::AddInternalContour( const CBlobContour &newContour )
{
	m_internalContours.push_back(newContour);
}

//! Indica si el blob est� buit ( no t� cap info associada )
//! Shows if the blob has associated information
bool CBlob::IsEmpty()
{
	return GetExternalContour()->m_contour == NULL;
}

/**
- FUNCI�: Area
- FUNCIONALITAT: Get blob area, ie. external contour area minus internal contours area
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/30
- MODIFICACI�: Data. Autor. Descripci�.
*/
double CBlob::Area()
{
	double area;
	t_contourList::iterator itContour; 

	area = m_externalContour.GetArea();

	itContour = m_internalContours.begin();
	
	while (itContour != m_internalContours.end() )
	{
		area -= (*itContour).GetArea();
		itContour++;
	}
	return area;
}

/**
- FUNCI�: Perimeter
- FUNCIONALITAT: Get blob perimeter, ie. sum of the lenght of all the contours
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/04/30
- MODIFICACI�: Data. Autor. Descripci�.
*/
double CBlob::Perimeter()
{
	double perimeter;
	t_contourList::iterator itContour; 

	perimeter = m_externalContour.GetPerimeter();

	itContour = m_internalContours.begin();
	
	while (itContour != m_internalContours.end() )
	{
		perimeter += (*itContour).GetPerimeter();
		itContour++;
	}
	return perimeter;

}

/**
- FUNCI�: Exterior
- FUNCIONALITAT: Return true for extern blobs
- PAR�METRES:
	- xBorder: true to consider blobs touching horizontal borders as extern
	- yBorder: true to consider blobs touching vertical borders as extern
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
*/
int	CBlob::Exterior(IplImage *mask, 
					bool xBorderLeft /* = true */, bool xBorderRight /* = true */,
					bool yBorderTop /* = true */, bool yBorderBottom /* = true */ )
{
	if (ExternPerimeter(mask, xBorderLeft, xBorderRight, yBorderTop, yBorderBottom ) > 0 )
	{
		return 1;
	}
	
	return 0;	 
}
/**
- FUNCI�: ExternPerimeter
- FUNCIONALITAT: Get extern perimeter (perimeter touching image borders)
- PAR�METRES:
	- maskImage: if != NULL, counts maskImage black pixels as external pixels and contour points touching
				 them are counted as external contour points.
	- xBorderLeft: true to consider blobs touching left horizontal borders as extern
	- xBorderRight: true to consider blobs touching right horizontal borders as extern
	- yBorderTop: true to consider blobs touching vertical top borders as extern
	- yBorderBottom: true to consider blobs touching vertical bottom borders as extern
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/05
- MODIFICACI�: Data. Autor. Descripci�.
- NOTA: If CBlobContour::GetContourPoints aproximates contours with a method different that NONE,
		this function will not give correct results
*/
double CBlob::ExternPerimeter( IplImage *maskImage, 
							  bool xBorderLeft /* = true */, bool xBorderRight /* = true */,
							  bool yBorderTop /* = true */, bool yBorderBottom /* = true */ )
{
	t_PointList externContour, externalPoints;
	CvSeqReader reader;
	CvSeqWriter writer;
	CvPoint actualPoint, previousPoint;
	bool find = false;
	int i,j;
	int delta = 0;
	double externPerimeter;
	
	// get contour pixels
	externContour = m_externalContour.GetContourPoints();

	externPerimeter = 0;

	// there are contour pixels?
	if( externContour == NULL )
	{
		return 0;
	}

	cvStartReadSeq( externContour, &reader);

	// create a sequence with the external points of the blob
	externalPoints = cvCreateSeq( externContour->flags, externContour->header_size, externContour->elem_size, 
								  m_storage );
	cvStartAppendToSeq( externalPoints, &writer );
	previousPoint.x = -1;

	// which contour pixels touch border?
	for( j=0; j< externContour->total; j++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		find = false;

		// pixel is touching border?
		if ( (xBorderLeft && (actualPoint.x == 0)) || 
			 (xBorderRight && (actualPoint.x == m_originalImageSize.width - 1 )) ||
			 (yBorderTop  && (actualPoint.y == 0)) || 
			 (yBorderBottom && (actualPoint.y == m_originalImageSize.height - 1 ))
			 )
		{
			find = true;
		}
		else
		{
			if( maskImage != NULL )
			{
				// verify if some of 8-connected neighbours is black in mask
				char *pMask;
				
				pMask = (maskImage->imageData + actualPoint.x - 1 + (actualPoint.y - 1) * maskImage->widthStep);
				
				for ( i = 0; i < 3; i++, pMask++ )
				{
					if(*pMask == 0 && !find ) 
					{
						find = true;
						break;
					}						
				}
				
				if(!find)
				{
					pMask = (maskImage->imageData + actualPoint.x - 1 + (actualPoint.y ) * maskImage->widthStep);
				
					for ( i = 0; i < 3; i++, pMask++ )
					{
						if(*pMask == 0 && !find ) 
						{
							find = true;
							break;
						}
					}
				}
			
				if(!find)
				{
					pMask = (maskImage->imageData + actualPoint.x - 1 + (actualPoint.y + 1) * maskImage->widthStep);

					for ( i = 0; i < 3; i++, pMask++ )
					{
						if(*pMask == 0 && !find ) 
						{
							find = true;
							break;
						}
					}
				}
			}
		}

		if( find )
		{
			if( previousPoint.x > 0 )
				delta = abs(previousPoint.x - actualPoint.x) + abs(previousPoint.y - actualPoint.y);

			// calculate separately each external contour segment 
			if( delta > 2 )
			{
				cvEndWriteSeq( &writer );
				externPerimeter += cvArcLength( externalPoints, CV_WHOLE_SEQ, 0 );
				
				cvClearSeq( externalPoints );
				cvStartAppendToSeq( externalPoints, &writer );
				delta = 0;
				previousPoint.x = -1;
			}

			CV_WRITE_SEQ_ELEM( actualPoint, writer );
			previousPoint = actualPoint;
		}
	}

	cvEndWriteSeq( &writer );
	cvSeqSort(externalPoints, cmp_func, 0);
	externPerimeter += cvArcLength( externalPoints, CV_WHOLE_SEQ, 0 );

	cvClearSeq( externalPoints );

	// divide by two because external points have one side inside the blob and the other outside
	// Perimeter of external points counts both sides, so it must be divided
	externPerimeter /= 2.0;

	return externPerimeter;
}

//! Compute blob's moment (p,q up to MAX_CALCULATED_MOMENTS)
double CBlob::Moment(int p, int q)
{
	double moment;
	t_contourList::iterator itContour; 

	moment = m_externalContour.GetMoment(p,q);

	itContour = m_internalContours.begin();
	
	while (itContour != m_internalContours.end() )
	{
		moment -= (*itContour).GetMoment(p,q);
		itContour++;
	}
	return moment;
}

/**
- FUNCI�: Mean
- FUNCIONALITAT: Get blob mean color in input image
- PAR�METRES:
	- image: image from gray color are extracted
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
*/
double CBlob::Mean( IplImage *image )
{
	double mean, stddev;
	
	// call mean calculation (where also standard deviation is calculated)
	GetMeanAndStdDev( image, mean, stddev );

	return mean;
}

double CBlob::StdDev( IplImage *image )
{
	double mean, stddev;
	
	// call mean calculation (where also standard deviation is calculated)
	GetMeanAndStdDev( image, mean, stddev );

	return stddev;
}
//! Calculates mean and std deviation of blob in input image
void CBlob::GetMeanAndStdDev( IplImage *image, double &mean, double &stddev )
{
	IplImage *mask;
	CvScalar meanscalar, stdscalar;
	CvPoint offset;

	// Create a mask with same size as blob bounding box
	GetBoundingBox();

	if (m_boundingBox.height <= 0 ||m_boundingBox.width <= 0 || !CV_IS_IMAGE( image ) ||
		m_boundingBox.height > image->height || m_boundingBox.width > image->width )
	{
		mean = 0;
		stddev = 0;
		return;
	}

	// apply ROI and mask to input image to compute mean gray and standard deviation
	mask = cvCreateImage( cvSize(m_boundingBox.width, m_boundingBox.height), IPL_DEPTH_8U, 1);
	cvSetZero(mask);

	offset.x = -m_boundingBox.x;
	offset.y = -m_boundingBox.y;

	// draw contours on mask
	cvDrawContours( mask, m_externalContour.GetContourPoints(), CV_RGB(255,255,255), CV_RGB(255,255,255),0, CV_FILLED, 8,
					offset );

	// draw internal contours
	t_contourList::iterator it = m_internalContours.begin();
	while(it != m_internalContours.end() )
	{
		cvDrawContours( mask, (*it).GetContourPoints(), CV_RGB(0,0,0), CV_RGB(0,0,0),0, CV_FILLED, 8,
					offset );
		it++;
	}

	cvSetImageROI( image, m_boundingBox );
	cvAvgSdv( image, &meanscalar, &stdscalar, mask );
	
	mean = meanscalar.val[0];
	stddev = stdscalar.val[0];

	cvReleaseImage( &mask );
	cvResetImageROI( image );

}

/**
- FUNCI�: GetBoundingBox
- FUNCIONALITAT: Get bounding box (without rotation) of a blob
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
*/
CvRect CBlob::GetBoundingBox()
{
	// it is calculated?
/*	if( m_boundingBox.width != -1 )
	{
		return m_boundingBox;
	}
*/
	t_PointList externContour;
	CvSeqReader reader;
	CvPoint actualPoint;
	
	// get contour pixels
	externContour = m_externalContour.GetContourPoints();
	
	// it is an empty blob?
	if( !externContour )
	{
		m_boundingBox.x = 0;
		m_boundingBox.y = 0;
		m_boundingBox.width = 0;
		m_boundingBox.height = 0;

		return m_boundingBox;
	}

	cvStartReadSeq( externContour, &reader);

	m_boundingBox.x = m_originalImageSize.width;
	m_boundingBox.y = m_originalImageSize.height;
	m_boundingBox.width = 0;
	m_boundingBox.height = 0;

	for( int i=0; i< externContour->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		m_boundingBox.x = MIN( actualPoint.x, m_boundingBox.x );
		m_boundingBox.y = MIN( actualPoint.y, m_boundingBox.y );
		
		m_boundingBox.width = MAX( actualPoint.x, m_boundingBox.width );
		m_boundingBox.height = MAX( actualPoint.y, m_boundingBox.height );
	}

	//m_boundingBox.x = max( m_boundingBox.x , 0 );
	//m_boundingBox.y = max( m_boundingBox.y , 0 );

	m_boundingBox.width -= m_boundingBox.x + 1;
	m_boundingBox.height -= m_boundingBox.y + 1;
	
	return m_boundingBox;
}

/**
- FUNCI�: GetEllipse
- FUNCIONALITAT: Calculates bounding ellipse of external contour points
- PAR�METRES:
	- 
- RESULTAT:
	- 
- RESTRICCIONS:
	- 
- AUTOR: rborras
- DATA DE CREACI�: 2008/05/06
- MODIFICACI�: Data. Autor. Descripci�.
- NOTA: Calculation is made using second order moment aproximation
*/
CvBox2D CBlob::GetEllipse()
{
	CvBox2D elipse;
	double u00,u11,u01,u10,u20,u02, delta, temp;


	// it is calculated?
	if( m_properties.find( CBlobGetMajorAxisLength().GetNom() )  != m_properties.end() )
	{
		// build a ellipse from calculated properties
		elipse.size.width = m_properties[CBlobGetMajorAxisLength().GetNom()];
		elipse.size.height = m_properties[CBlobGetMinorAxisLength().GetNom()];
		elipse.angle = m_properties[CBlobGetOrientation().GetNom()];
		elipse.center.x = m_properties[CBlobGetElipseXCenter().GetNom()];
		elipse.center.y = m_properties[CBlobGetElipseYCenter().GetNom()];
	}	

	// initialize properties
	m_properties[CBlobGetMajorAxisLength().GetNom()] = 0;
	m_properties[CBlobGetMinorAxisLength().GetNom()] = 0;
	m_properties[CBlobGetOrientation().GetNom()] = 0;
	m_properties[CBlobGetElipseXCenter().GetNom()] = 0;
	m_properties[CBlobGetElipseYCenter().GetNom()] = 0;
	elipse.angle = 0;
	elipse.size.height = 0;
	elipse.size.width = 0;
	elipse.center.x = 0;
	elipse.center.y = 0;
	
	// central moments calculation
	u00 = Moment(0,0);

	// empty blob?
	if ( u00 <= 0 )
	{
		return elipse;
	}

	u10 = Moment(1,0) / u00;
	u01 = Moment(0,1) / u00;

	u11 = -(Moment(1,1) - Moment(1,0) * Moment(0,1) / u00 ) / u00;
	u20 = (Moment(2,0) - Moment(1,0) * Moment(1,0) / u00 ) / u00;
	u02 = (Moment(0,2) - Moment(0,1) * Moment(0,1) / u00 ) / u00;


	// elipse calculation
	delta = sqrt( 4*u11*u11 + (u20-u02)*(u20-u02) );
	elipse.center.x = u10;
	elipse.center.y = u01;

	m_properties[CBlobGetElipseXCenter().GetNom()] = elipse.center.x;
	m_properties[CBlobGetElipseYCenter().GetNom()] = elipse.center.y;
	
	temp = u20 + u02 + delta;
	if( temp > 0 )
	{
		elipse.size.width = sqrt( 2*(u20 + u02 + delta ));
		m_properties[CBlobGetMajorAxisLength().GetNom()] = elipse.size.width;
	}	
	else
	{
		return elipse;
	}

	temp = u20 + u02 - delta;
	if( temp > 0 )
	{
		elipse.size.height = sqrt( 2*(u20 + u02 - delta ) );
		m_properties[CBlobGetMinorAxisLength().GetNom()] = elipse.size.height;
	}
	else
	{
		return elipse;
	}

	// elipse orientation
	elipse.angle = atan2( 2*u11, u20 - u02 + sqrt( (u20 - u02)*(u20 - u02) + (2*u11)*(2*u11)));

	if( elipse.angle == 0 && u02 > u20 )
	{
		elipse.angle = CV_PI/2.0;
	}
	// convert to degrees
	elipse.angle = (180.0 / CV_PI) *elipse.angle;
	elipse.angle+=180;
	m_properties[CBlobGetOrientation().GetNom()] = elipse.angle;

	        
	return elipse;

}

/**
- FUNCTION: FillBlob
- FUNCTIONALITY: 
	- Fills the blob with a specified colour
- PARAMETERS:
	- imatge: where to paint
	- color: colour to paint the blob
- RESULT:
	- modifies input image and returns the seed point used to fill the blob
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlob::FillBlob( IplImage *imatge, CvScalar color ) 					  
{
	cvDrawContours( imatge, m_externalContour.GetContourPoints(), color, color,0, CV_FILLED, 8 );
}


/**
- FUNCTION: GetConvexHull
- FUNCTIONALITY: Calculates the convex hull polygon of the blob
- PARAMETERS:
	- dst: where to store the result
- RESULT:
	- true if no error ocurred
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
t_PointList CBlob::GetConvexHull()
{
	CvSeq *convexHull = NULL;

	if( m_externalContour.GetContourPoints() )
		convexHull = cvConvexHull2( m_externalContour.GetContourPoints(), m_storage,
					   CV_COUNTER_CLOCKWISE, 1 );

	return convexHull;
}

/**
- FUNCTION: JoinBlob
- FUNCTIONALITY: Add's external contour to current external contour
- PARAMETERS:
	- blob: blob from which extract the added external contour
- RESULT:
	- true if no error ocurred
- RESTRICTIONS: Only external contours are added
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void CBlob::JoinBlob( CBlob *blob )
{
	CvSeqWriter writer;
	CvSeqReader reader;
	t_chainCode chainCode;

	cvStartAppendToSeq( m_externalContour.GetChainCode(), &writer );
	cvStartReadSeq( blob->GetExternalContour()->GetChainCode(), &reader );

	for (int i = 0; i < blob->GetExternalContour()->GetChainCode()->total; i++ )
	{
		CV_READ_SEQ_ELEM( chainCode, reader );
		CV_WRITE_SEQ_ELEM( chainCode, writer );
	}	
	cvEndWriteSeq( &writer );

	// reset stats for the blob
	m_properties.clear();
	
}
