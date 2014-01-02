/************************************************************************
  			BlobResult.cpp
  			
FUNCIONALITAT: Implementaci� de la classe BlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):
 
**************************************************************************/
#if _MSC_VER > 1000
  // deactivate warning (identifier was truncated to '255' characters in the
  // debug information) is a problem of msvc6.5
  #pragma warning(disable: 4786)
#endif // _MSC_VER > 1000

#include <cvblobs2/BlobResult.h>

#include <limits.h>
#include <stdio.h>
#include <functional>
#include <algorithm>

//! Show errors functions: only works for windows releases
#ifdef _SHOW_ERRORS
	#include <afx.h>			//suport per a CStrings
	#include <afxwin.h>			//suport per a AfxMessageBox
#endif

CVBLOBS_BEGIN_NAMESPACE;

/**************************************************************************
		Constructors / Destructors
**************************************************************************/


/**
- FUNCI�: BlobResult
- FUNCIONALITAT: Constructor estandard.
- PAR�METRES:
- RESULTAT:
- Crea un BlobResult sense cap blob
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 20-07-2004.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: BlobResult
- FUNCTIONALITY: Standard constructor
- PARAMETERS:
- RESULT:
	- creates an empty set of blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::BlobResult()
{
	m_blobs = Blob_vector();
}

/**
- FUNCI�: BlobResult
- FUNCIONALITAT: Constructor a partir d'una imatge. Inicialitza la seq��ncia de blobs 
			   amb els blobs resultants de l'an�lisi de blobs de la imatge.
- PAR�METRES:
	- source: imatge d'on s'extreuran els blobs
	- mask: m�scara a aplicar. Nom�s es calcularan els blobs on la m�scara sigui 
			diferent de 0. Els blobs que toquin a un pixel 0 de la m�scara seran 
			considerats exteriors.
	- threshold: llindar que s'aplicar� a la imatge source abans de calcular els blobs
	- findmoments: indica si s'han de calcular els moments de cada blob
	- blackBlobs: true per buscar blobs negres a la binaritzazi� (it will join all extern white blobs).
				  false per buscar blobs negres a la binaritzazi� (it will join all extern black blobs).

- RESULTAT:
	- objecte BlobResult amb els blobs de la imatge source
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: Blob
- FUNCTIONALITY: Constructor from an image. Fills an object with all the blobs in
	the image
- PARAMETERS:
	- source: image to extract the blobs from
	- mask: optional mask to apply. The blobs will be extracted where the mask is
			not 0. All the neighbouring blobs where the mask is 0 will be extern blobs
	- threshold: threshold level to apply to the image before computing blobs
	- findmoments: true to calculate the blob moments (slower) (needed to calculate elipses!)
 	- blackBlobs: true to search for black blobs in the binarization (it will join all extern white blobs).
				  false to search for white blobs in the binarization (it will join all extern black blobs).
- RESULT:
	- object with all the blobs in the image. It throws an EXCEPCIO_CALCUL_BLOBS
	  if some error appears in the BlobAnalysis function
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::BlobResult(IplImage *source, IplImage *mask, uchar backgroundColor )
{
	bool success;

	try
	{
		success = ComponentLabeling( source, mask, backgroundColor, m_blobs );
	}
	catch(...)
	{
		success = false;
	}

	if( !success ) throw EXCEPCIO_CALCUL_BLOBS;
}

/**
- FUNCI�: BlobResult
- FUNCIONALITAT: Constructor de c�pia. Inicialitza la seq��ncia de blobs 
			   amb els blobs del par�metre.
- PAR�METRES:
	- source: objecte que es copiar�
- RESULTAT:
	- objecte BlobResult amb els blobs de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: BlobResult
- FUNCTIONALITY: Copy constructor
- PARAMETERS:
	- source: object to copy
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::BlobResult( const BlobResult &source )
{
	m_blobs = Blob_vector( source.GetNumBlobs() );
	
	// creem el nou a partir del passat com a par�metre
	m_blobs = Blob_vector( source.GetNumBlobs() );
	
	std::copy( source.m_blobs.begin(), source.m_blobs.end(), m_blobs.begin() );	
}



/**
- FUNCI�: ~BlobResult
- FUNCIONALITAT: Destructor estandard.
- PAR�METRES:
- RESULTAT:
	- Allibera la mem�ria reservada de cadascun dels blobs de la classe
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: ~BlobResult
- FUNCTIONALITY: Destructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::~BlobResult()
{
	ClearBlobs();
}

/**************************************************************************
		Operadors / Operators
**************************************************************************/


/**
- FUNCI�: operador =
- FUNCIONALITAT: Assigna un objecte source a l'actual
- PAR�METRES:
	- source: objecte a assignar
- RESULTAT:
	- Substitueix els blobs actuals per els de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: Assigment operator
- FUNCTIONALITY: 
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult& BlobResult::operator=(const BlobResult& source)
{
	// si ja s�n el mateix, no cal fer res
	if (this != &source)
	{
		// free previous blobs
		m_blobs.clear();
		// create a copy of source blobs
		m_blobs = Blob_vector( source.GetNumBlobs() );
		
		std::copy(source.m_blobs.begin(), source.m_blobs.end(), m_blobs.begin());
	}
	return *this;
}


/**
- FUNCI�: operador +
- FUNCIONALITAT: Concatena els blobs de dos BlobResult
- PAR�METRES:
	- source: d'on s'agafaran els blobs afegits a l'actual
- RESULTAT:
	- retorna un nou BlobResult amb els dos BlobResult concatenats
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- NOTA: per la implementaci�, els blobs del par�metre es posen en ordre invers
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: + operator
- FUNCTIONALITY: Joins the blobs in source with the current ones
- PARAMETERS:
	- source: object to copy the blobs
- RESULT:
	- object with the actual blobs and the source blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult BlobResult::operator+( const BlobResult& source ) const
{	
	// create a copy of current blobs
	BlobResult result( *this );
	int numblobs = result.GetNumBlobs();
		
	// allocate space for new blobs
	result.m_blobs.resize( result.GetNumBlobs() + source.GetNumBlobs() );
	// copy new blobs	
	std::copy( source.m_blobs.begin(), source.m_blobs.end(), result.m_blobs.begin() + numblobs );
	
	return result;
}

/**************************************************************************
		Operacions / Operations
**************************************************************************/

/**
- FUNCI�: AddBlob
- FUNCIONALITAT: Afegeix un blob al conjunt
- PAR�METRES:
	- blob: blob a afegir
- RESULTAT:
	- modifica el conjunt de blobs actual
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 2006/03/01
- MODIFICACI�: Data. Autor. Descripci�.
*/
void BlobResult::AddBlob( BlobPtr blob )
{
	m_blobs.push_back( blob );
}


#ifdef MATRIXCV_ACTIU

/**
- FUNCI�: GetResult
- FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
- PAR�METRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
- RESULTAT:
	- Retorna un array de double's amb el resultat per cada blob
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: GetResult
- FUNCTIONALITY: Computes the function evaluador on all the blobs of the class
				 and returns a vector with the result
- PARAMETERS:
	- evaluador: function to apply to each blob (any object derived from the 
				 COperadorBlob class )
- RESULT:
	- vector with all the results in the same order as the blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double_vector BlobResult::GetResult( funcio_calculBlob *evaluador ) const
{
	if( GetNumBlobs() <= 0 )
	{
		return double_vector();
	}

	// definim el resultat
	double_vector result = double_vector( GetNumBlobs() );
	// i iteradors sobre els blobs i el resultat
	double_vector::iterator itResult = result.GetIterator();
	Blob_vector::const_iterator itBlobs = m_blobs.begin();

	// avaluem la funci� en tots els blobs
	while( itBlobs != m_blobs.end() )
	{
		*itResult = (*evaluador).GetOperatorResult(**itBlobs);
		itBlobs++;
		itResult++;
	}
	return result;
}
#endif

/**
- FUNCI�: GetSTLResult
- FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
- PAR�METRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
- RESULTAT:
	- Retorna un array de double's STL amb el resultat per cada blob
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: GetResult
- FUNCTIONALITY: Computes the function evaluador on all the blobs of the class
				 and returns a vector with the result
- PARAMETERS:
	- evaluador: function to apply to each blob (any object derived from the 
				 COperadorBlob class )
- RESULT:
	- vector with all the results in the same order as the blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double_stl_vector BlobResult::GetSTLResult( funcio_calculBlob *evaluador ) const
{
	if( GetNumBlobs() <= 0 )
	{
		return double_stl_vector();
	}

	// definim el resultat
	double_stl_vector result = double_stl_vector( GetNumBlobs() );
	// i iteradors sobre els blobs i el resultat
	double_stl_vector::iterator itResult = result.begin();
	Blob_vector::const_iterator itBlobs = m_blobs.begin();

	// avaluem la funci� en tots els blobs
	while( itBlobs != m_blobs.end() )
	{
		*itResult = (*evaluador).GetOperatorResult(**itBlobs);
		itBlobs++;
		itResult++;
	}
	return result;
}

/**
- FUNCI�: GetNumber
- FUNCIONALITAT: Calcula el resultat especificat sobre un �nic blob de la classe
- PAR�METRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
	- indexblob: n�mero de blob del que volem calcular el resultat.
- RESULTAT:
	- Retorna un double amb el resultat
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: GetNumber
- FUNCTIONALITY: Computes the function evaluador on a blob of the class
- PARAMETERS:
	- indexBlob: index of the blob to compute the function
	- evaluador: function to apply to each blob (any object derived from the 
				 COperadorBlob class )
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobResult::GetNumber( int indexBlob, funcio_calculBlob *evaluador ) const
{
	if( indexBlob < 0 || indexBlob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
	return (*evaluador).GetOperatorResult( *m_blobs[indexBlob] );
}

/////////////////////////// FILTRAT DE BLOBS ////////////////////////////////////


/**
- FUNCI�: Filter
- FUNCIONALITAT: Filtra els blobs de la classe i deixa el resultat amb nom�s 
			   els blobs que han passat el filtre.
			   El filtrat es basa en especificar condicions sobre un resultat dels blobs
			   i seleccionar (o excloure) aquells blobs que no compleixen una determinada
			   condicio
- PAR�METRES:
	- dst: variable per deixar els blobs filtrats
	- filterAction:	acci� de filtrat. Incloure els blobs trobats (B_INCLUDE),
				    o excloure els blobs trobats (B_EXCLUDE)
	- evaluador: Funci� per evaluar els blobs (qualsevol objecte derivat de COperadorBlob
	- Condition: tipus de condici� que ha de superar la mesura (FilterType) 
				 sobre cada blob per a ser considerat.
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  valor num�ric per a la comparaci� (Condition) de la mesura (FilterType)
	- HighLimit: valor num�ric per a la comparaci� (Condition) de la mesura (FilterType)
				 (nom�s t� sentit per a aquelles condicions que tenen dos valors 
				 (B_INSIDE, per exemple).
- RESULTAT:
	- Deixa els blobs resultants del filtrat a destination
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/**
- FUNCTION: Filter
- FUNCTIONALITY: Get some blobs from the class based on conditions on measures
				 of the blobs. 
- PARAMETERS:
	- dst: where to store the selected blobs
	- filterAction:	B_INCLUDE: include the blobs which pass the filter in the result 
				    B_EXCLUDE: exclude the blobs which pass the filter in the result 
	- evaluador: Object to evaluate the blob
	- Condition: How to decide if  the result returned by evaluador on each blob
				 is included or not. It can be:
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  numerical value to evaluate the Condition on evaluador(blob)
	- HighLimit: numerical value to evaluate the Condition on evaluador(blob).
				 Only useful for B_INSIDE and B_OUTSIDE
- RESULT:
	- It returns on dst the blobs that accomplish (B_INCLUDE) or discards (B_EXCLUDE)
	  the Condition on the result returned by evaluador on each blob
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::Filter(BlobResult &dst, 
						 int filterAction, 
						 funcio_calculBlob *evaluador, 
						 int condition, 
						 double lowLimit, double highLimit /*=0*/)
							
{
   //int numBlobs = GetNumBlobs();

	// do the job
	DoFilter(dst, filterAction, evaluador, condition, &dst == this, lowLimit, highLimit );

	// inline operation: remove previous blobs
	/*if( &dst == this ) 
	{
		// delete original blob in inline operations ( it will be repeated if they pass the filter )
		m_blobs.erase( m_blobs.begin(), m_blobs.begin()+numBlobs );
	}*/
}


//! Does the Filter method job
void BlobResult::DoFilter(BlobResult &dst, int filterAction, funcio_calculBlob *evaluador, 
						   int condition, bool inlineFilter, double lowLimit, double highLimit/* = 0*/)
{
	int i, numBlobs, numBlobsErased;
	bool resultavaluacio = false, selectedBlob;
	double_stl_vector avaluacioBlobs;
	double_stl_vector::iterator itavaluacioBlobs;
	BlobPtr newblob;
	
	if( GetNumBlobs() <= 0 ) return;
	if( !evaluador ) return;
	//evaluate blobs using specified operator
	avaluacioBlobs = GetSTLResult(evaluador);
	itavaluacioBlobs = avaluacioBlobs.begin();
	
	numBlobs = GetNumBlobs();
	numBlobsErased = 0;

	for(i=0;i<numBlobs;i++, itavaluacioBlobs++)
	{
		switch(condition)
		{
			case B_EQUAL:
				resultavaluacio= *itavaluacioBlobs == lowLimit;
				break;
			case B_NOT_EQUAL:
				resultavaluacio= *itavaluacioBlobs != lowLimit;
				break;
			case B_GREATER:
				resultavaluacio= *itavaluacioBlobs > lowLimit;
				break;
			case B_LESS:
				resultavaluacio= *itavaluacioBlobs < lowLimit;
				break;
			case B_GREATER_OR_EQUAL:
				resultavaluacio= *itavaluacioBlobs >= lowLimit;
				break;
			case B_LESS_OR_EQUAL:
				resultavaluacio= *itavaluacioBlobs <= lowLimit;
				break;
			case B_INSIDE:
				resultavaluacio=( *itavaluacioBlobs >= lowLimit) && ( *itavaluacioBlobs <= highLimit); 
				break;
			case B_OUTSIDE:
				resultavaluacio=( *itavaluacioBlobs < lowLimit) || ( *itavaluacioBlobs > highLimit); 
				break;
		}
		
		// blob is selected?
		selectedBlob = (  resultavaluacio && filterAction == B_INCLUDE ) || 
					   ( !resultavaluacio && filterAction == B_EXCLUDE );

		// add to dst if not inline operation (for inline operation we don't have to do nothing)
		if( !inlineFilter && selectedBlob )
		{
			dst.AddBlob( m_blobs[i-numBlobsErased] );
		}
		// remove from origin for inline operation (for not inline we don't have to do nothing)
		if(inlineFilter && !selectedBlob ) 
		{
			m_blobs.erase(m_blobs.begin() + i - numBlobsErased );
			numBlobsErased++;
		}
	}
}
/**
- FUNCI�: GetBlob
- FUNCIONALITAT: Retorna un blob si aquest existeix (index != -1)
- PAR�METRES:
	- indexblob: index del blob a retornar
- RESULTAT:
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/*
- FUNCTION: GetBlob
- FUNCTIONALITY: Gets the n-th blob (without ordering the blobs)
- PARAMETERS:
	- indexblob: index in the blob array
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
Blob BlobResult::GetBlob(int indexblob) const
{	
	if( indexblob < 0 || indexblob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );

	return *m_blobs[indexblob];
}
BlobPtr BlobResult::GetBlobPtr(int indexblob)
{	
	if( indexblob < 0 || indexblob >= GetNumBlobs() )
		RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );

	return m_blobs[indexblob];
}

/**
- FUNCI�: GetNthBlob
- FUNCIONALITAT: Retorna l'en�ssim blob segons un determinat criteri
- PAR�METRES:
	- criteri: criteri per ordenar els blobs (objectes derivats de COperadorBlob)
	- nBlob: index del blob a retornar
	- dst: on es retorna el resultat
- RESULTAT:
	- retorna el blob nBlob a dst ordenant els blobs de la classe segons el criteri
	  en ordre DESCENDENT. Per exemple, per obtenir el blob major:
		GetNthBlob( BlobGetArea(), 0, blobMajor );
		GetNthBlob( BlobGetArea(), 1, blobMajor ); (segon blob m�s gran)
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/*
- FUNCTION: GetNthBlob
- FUNCTIONALITY: Gets the n-th blob ordering first the blobs with some criteria
- PARAMETERS:
	- criteri: criteria to order the blob array
	- nBlob: index of the returned blob in the ordered blob array
	- dst: where to store the result
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::GetNthBlob( funcio_calculBlob *criteri, int nBlob, Blob &dst ) const
{
	// verifiquem que no estem accedint fora el vector de blobs
	if( nBlob < 0 || nBlob >= GetNumBlobs() )
	{
		//RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
		dst = Blob();
		return;
	}

	double_stl_vector avaluacioBlobs, avaluacioBlobsOrdenat;
	double valorEnessim;

	//avaluem els blobs amb la funci� pertinent	
	avaluacioBlobs = GetSTLResult(criteri);

	avaluacioBlobsOrdenat = double_stl_vector( GetNumBlobs() );

	// obtenim els nBlob primers resultats (en ordre descendent)
	std::partial_sort_copy( avaluacioBlobs.begin(), 
						    avaluacioBlobs.end(),
						    avaluacioBlobsOrdenat.begin(), 
						    avaluacioBlobsOrdenat.end(),
						    std::greater<double>() );

	valorEnessim = avaluacioBlobsOrdenat[nBlob];

	// busquem el primer blob que t� el valor n-ssim
	double_stl_vector::const_iterator itAvaluacio = avaluacioBlobs.begin();

	bool trobatBlob = false;
	int indexBlob = 0;
	while( itAvaluacio != avaluacioBlobs.end() && !trobatBlob )
	{
		if( *itAvaluacio == valorEnessim )
		{
			trobatBlob = true;
			dst = Blob( GetBlob(indexBlob));
		}
		itAvaluacio++;
		indexBlob++;
	}
}

/**
- FUNCI�: ClearBlobs
- FUNCIONALITAT: Elimina tots els blobs de l'objecte
- PAR�METRES:
- RESULTAT: 
	- Allibera tota la mem�ria dels blobs
- RESTRICCIONS:
- AUTOR: Ricard Borr�s Navarra
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/*
- FUNCTION: ClearBlobs
- FUNCTIONALITY: Clears all the blobs from the object and releases all its memory
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::ClearBlobs()
{
	m_blobs.clear();
}

/**
- FUNCI�: RaiseError
- FUNCIONALITAT: Funci� per a notificar errors al l'usuari (en debug) i llen�a
			   les excepcions
- PAR�METRES:
	- errorCode: codi d'error
- RESULTAT: 
	- Ensenya un missatge a l'usuari (en debug) i llen�a una excepci�
- RESTRICCIONS:
- AUTOR: Ricard Borr�s Navarra
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/*
- FUNCTION: RaiseError
- FUNCTIONALITY: Error handling function
- PARAMETERS:
	- errorCode: reason of the error
- RESULT:
	- in _SHOW_ERRORS version, shows a message box with the error. In release is silent.
	  In both cases throws an exception with the error.
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::RaiseError(const int errorCode) const
{
//! Do we need to show errors?
#ifdef _SHOW_ERRORS
	CString msg, format = "Error en BlobResult: %s";

	switch (errorCode)
	{
	case EXCEPTION_BLOB_OUT_OF_BOUNDS:
		msg.Format(format, "Intentant accedir a un blob no existent");
		break;
	default:
		msg.Format(format, "Codi d'error desconegut");
		break;
	}

	AfxMessageBox(msg);

#endif
	throw errorCode;
}



/**************************************************************************
		Auxiliars / Auxiliary functions
**************************************************************************/


/**
- FUNCI�: PrintBlobs
- FUNCIONALITAT: Escriu els par�metres (�rea, per�metre, exterior, mitjana) 
			   de tots els blobs a un fitxer.
- PAR�METRES:
	- nom_fitxer: path complet del fitxer amb el resultat
- RESULTAT:
- RESTRICCIONS:
- AUTOR: Ricard Borr�s
- DATA DE CREACI�: 25-05-2005.
- MODIFICACI�: Data. Autor. Descripci�.
*/
/*
- FUNCTION: PrintBlobs
- FUNCTIONALITY: Prints some blob features in an ASCII file
- PARAMETERS:
	- nom_fitxer: full path + filename to generate
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::PrintBlobs( char *nom_fitxer ) const
{
	double_stl_vector area, /*perimetre,*/ exterior, compacitat, longitud, 
					  externPerimeter, perimetreConvex, perimetre;
	int i;
	FILE *fitxer_sortida;

 	area      = GetSTLResult( BlobGetArea());
	perimetre = GetSTLResult( BlobGetPerimeter());
	exterior  = GetSTLResult( BlobGetExterior());
	compacitat = GetSTLResult(BlobGetCompactness());
	longitud  = GetSTLResult( BlobGetLength());
	externPerimeter = GetSTLResult( BlobGetExternPerimeter());
	perimetreConvex = GetSTLResult( BlobGetHullPerimeter());

	fitxer_sortida = fopen( nom_fitxer, "w" );

	for(i=0; i<GetNumBlobs(); i++)
	{
		fprintf( fitxer_sortida, "blob %d ->\t a=%7.0f\t p=%8.2f (%8.2f extern)\t pconvex=%8.2f\t ext=%.0f\t c=%3.2f\t l=%8.2f\n",
				 i, area[i], perimetre[i], externPerimeter[i], perimetreConvex[i], exterior[i], compacitat[i], longitud[i] );
	}
	fclose( fitxer_sortida );

}


/*
- FUNCTION: RemoveProperty
- FUNCTIONALITY: Remove a property from al contained blobs to allow recalculation
- PARAMETERS:
	- name: property name (BlobOperador name)
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borr�s
- CREATION DATE: 19-11-2009.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::RemoveProperty( const std::string &name )
{
	Blob_vector::iterator pBlobs = m_blobs.begin();

	while( pBlobs != m_blobs.end() )
	{
		(*pBlobs)->resetProperty(name);
		pBlobs++;
	}
}

CVBLOBS_END_NAMESPACE;
