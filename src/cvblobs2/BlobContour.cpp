#include <cvblobs2/BlobContour.h>
#include <opencv/cxcore.h>

CVBLOBS_BEGIN_NAMESPACE;

BlobContour::BlobContour()
{
	mStartPoint.x = 0;
	mStartPoint.y = 0;
	mArea = -1;
	mPerimeter = -1;
	mpContourPoints = NULL;
	mMoments.m00 = -1;
	mpContour = NULL;
	mpParentStorage = NULL;
}
BlobContour::BlobContour(CvPoint startPoint, CvMemStorage *storage )
{
	mStartPoint.x = startPoint.x;
	mStartPoint.y = startPoint.y;
	mArea = -1;
	mPerimeter = -1;
	mMoments.m00 = -1;

	mpParentStorage = storage;

	mpContourPoints = NULL;

	// contour sequence: must be compatible with opencv functions
	mpContour = cvCreateSeq( CV_SEQ_ELTYPE_CODE | CV_SEQ_KIND_CURVE | CV_SEQ_FLAG_CLOSED,
                           sizeof(CvContour),
                           sizeof(t_chainCode),mpParentStorage);

}


//! Copy constructor
BlobContour::BlobContour( BlobContour *source )
{
	if (source != NULL )
	{
		*this = *source;
	}
}

BlobContour::~BlobContour()
{
	// let parent blob deallocate all contour and contour point memory
	mpContour = NULL;
	mpContourPoints = NULL;
}


//! Copy operator
BlobContour& BlobContour::operator=( const BlobContour &source )
{
	if( this != &source )
	{		
		mStartPoint = source.mStartPoint;

		mpParentStorage = source.mpParentStorage;
		
		if (mpContour)
		{
			cvClearSeq( mpContour );
		}

		if (source.mpContour)
		{
			mpContour =	cvCloneSeq( source.mpContour, mpParentStorage);
		}
		
		if( source.mpContourPoints )
		{
			if( mpContourPoints )
				cvClearSeq( mpContourPoints );
			mpContourPoints = cvCloneSeq( source.mpContourPoints, mpParentStorage);
		}

		mArea = source.mArea;
		mPerimeter = source.mArea;
		mMoments = source.mMoments;
	}
	return *this;
}


/**
   - FUNCIÓ: AddChainCode
   - FUNCIONALITAT: Add chain code to contour
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/05/06
   - MODIFICACIÓ: Data. Autor. Descripció.
*/
void BlobContour::addChainCode(t_chainCode chaincode)
{
	cvSeqPush(mpContour, &chaincode);
}

//! Clears chain code contour and points
void BlobContour::resetChainCode()
{
	if( mpContour )
	{
		cvClearSeq( mpContour );
		mpContour = NULL;
	}
	if( mpContourPoints )
	{
		cvClearSeq( mpContourPoints );
		mpContourPoints = NULL;
	}
}

/**
   - FUNCIÓ: GetPerimeter
   - FUNCIONALITAT: Get perimeter from chain code. Diagonals sum sqrt(2) and horizontal and vertical codes 1
   - PARÀMETRES:
   - 
   - RESULTAT:
   - 
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/30
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: Algorithm derived from "Methods to estimate area and perimeters of blob-like objects: A comparison", L.Yang
*/
double BlobContour::perimeter()
{
	// is calculated?
	if (mPerimeter != -1)
	{
		return mPerimeter;
	}

	if( isEmpty() )
		return 0;

	mPerimeter = cvContourPerimeter( contourPoints() );

	return mPerimeter;
}

/**
   - FUNCIÓ: GetArea
   - FUNCIONALITAT: Computes area from chain code
   - PARÀMETRES:
   - 
   - RESULTAT:
   - May give negative areas for clock wise contours
   - RESTRICCIONS:
   - 
   - AUTOR: rborras
   - DATA DE CREACIÓ: 2008/04/30
   - MODIFICACIÓ: Data. Autor. Descripció.
   - NOTA: Algorithm derived from "Properties of contour codes", G.R. Wilson
*/
double BlobContour::area()
{
	// is calculated?
	if (mArea != -1)
	{
		return mArea;
	}

	if( isEmpty() )
		return 0;

	mArea = fabs( cvContourArea( contourPoints() ));
	
	return mArea;
}

//! Get contour moment (p,q up to MAX_CALCULATED_MOMENTS)
double BlobContour::moment(int p, int q)
{
	// is a valid moment?
	if ( p < 0 || q < 0 || p > MAX_MOMENTS_ORDER || q > MAX_MOMENTS_ORDER )
	{
		return -1;
	}

	if( isEmpty() )
		return 0;

	// it is calculated?
	if( mMoments.m00 == -1)
	{
		cvMoments( contourPoints(), &mMoments );
	}
		
	return cvGetSpatialMoment( &mMoments, p, q );

	
}

//! Calculate contour points from crack codes
t_PointList BlobContour::contourPoints()
{
	// it is calculated?
	if( mpContourPoints != NULL )
		return mpContourPoints;

	if ( mpContour == NULL || mpContour->total <= 0 )
	{
		return NULL;
	}

	CvSeq *tmpPoints;
	CvSeqReader reader;
	CvSeqWriter writer;
	CvPoint actualPoint;
	CvRect boundingBox;

	// if aproximation is different than simple extern perimeter will not work
	tmpPoints = cvApproxChains( mpContour, mpParentStorage, CV_CHAIN_APPROX_NONE);


	// apply an offset to contour points to recover real coordinates
	
	cvStartReadSeq( tmpPoints, &reader);

	mpContourPoints = cvCreateSeq( tmpPoints->flags, tmpPoints->header_size, tmpPoints->elem_size, mpParentStorage );
	cvStartAppendToSeq(mpContourPoints, &writer );

	// also calculate bounding box of the contour to allow cvPointPolygonTest
	// work correctly on the generated polygon
	boundingBox.x = boundingBox.y = 10000;
	boundingBox.width = boundingBox.height = 0;
	
	for( int i=0; i< tmpPoints->total; i++)
	{
		CV_READ_SEQ_ELEM( actualPoint, reader);

		actualPoint.x += mStartPoint.x;
		actualPoint.y += mStartPoint.y;

		boundingBox.x = MIN( boundingBox.x, actualPoint.x );
		boundingBox.y = MIN( boundingBox.y, actualPoint.y );
		boundingBox.width = MAX( boundingBox.width, actualPoint.x );
		boundingBox.height = MAX( boundingBox.height, actualPoint.y );
		
		CV_WRITE_SEQ_ELEM( actualPoint, writer );
	}
	cvEndWriteSeq( &writer );
	cvClearSeq( tmpPoints );

	// assign calculated bounding box
	((CvContour*)mpContourPoints)->rect = boundingBox;


	return mpContourPoints;
}

CVBLOBS_END_NAMESPACE;
