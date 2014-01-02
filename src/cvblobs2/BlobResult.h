/************************************************************************
  			BlobResult.h
  			
FUNCIONALITAT: Definici� de la classe CBlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificaci�, Autor, Data):

FUNCTIONALITY: Definition of the CBlobResult class
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/


#ifndef _CVBLOBS2_BLOBRESULT_H_
#define _CVBLOBS2_BLOBRESULT_H_

#if _MSC_VER > 1000 
#pragma once
#endif // _MSC_VER > 1000


#include <cvblobs2/BlobLibraryConfiguration.h>
#include <math.h>
#include <opencv/cxcore.h>

#include <vector>		// vectors de la STL
#include <cvblobs2/Blob.h>
#include <cvblobs2/BlobOperators.h>
#include <cvblobs2/ComponentLabeling.h>

CVBLOBS_BEGIN_NAMESPACE;

#ifdef MATRIXCV_ACTIU
	#include "matrixCV.h"
#else
	// llibreria STL
	#include <vector>
	//! Vector de doubles
	typedef std::vector<double> double_stl_vector;
#endif

/**************************************************************************
	Filtres / Filters
**************************************************************************/

//! accions que es poden fer amb els filtres
//! Actions performed by a filter (include or exclude blobs)
#define B_INCLUDE				1L
#define B_EXCLUDE				2L

//! condicions sobre els filtres
//! Conditions to apply the filters
#define B_EQUAL					3L
#define B_NOT_EQUAL				4L
#define B_GREATER				5L
#define B_LESS					6L
#define B_GREATER_OR_EQUAL		7L
#define B_LESS_OR_EQUAL			8L
#define B_INSIDE			    9L
#define B_OUTSIDE			    10L


/**************************************************************************
	Excepcions / Exceptions
**************************************************************************/

//! Excepcions llen�ades per les funcions:
#define EXCEPTION_BLOB_OUT_OF_BOUNDS	1000
#define EXCEPCIO_CALCUL_BLOBS			1001

/** 
	Classe que cont� un conjunt de blobs i permet extreure'n propietats 
	o filtrar-los segons determinats criteris.
	Class to calculate the blobs of an image and calculate some properties 
	on them. Also, the class provides functions to filter the blobs using
	some criteria.
*/
class BlobResult  
{
public:

	//! constructor estandard, crea un conjunt buit de blobs
	//! Standard constructor, it creates an empty set of blobs
	BlobResult();
	//! constructor a partir d'una imatge
	//! Image constructor, it creates an object with the blobs of the image
	BlobResult(IplImage *source, IplImage *mask, uchar backgroundColor);
	//! constructor de c�pia
	//! Copy constructor
	BlobResult( const BlobResult &source );
	//! Destructor
	virtual ~BlobResult();

	//! operador = per a fer assignacions entre BlobResult
	//! Assigment operator
	BlobResult& operator=(const BlobResult& source);
	//! operador + per concatenar dos BlobResult
	//! Addition operator to concatenate two sets of blobs
	BlobResult operator+( const BlobResult& source ) const;
	
	//! Afegeix un blob al conjunt
	//! Adds a blob to the set of blobs
	void AddBlob( BlobPtr blob );

#ifdef MATRIXCV_ACTIU
	//! Calcula un valor sobre tots els blobs de la classe retornant una MatrixCV
	//! Computes some property on all the blobs of the class
	double_vector GetResult( funcio_calculBlob *evaluador ) const;
#endif
	//! Calcula un valor sobre tots els blobs de la classe retornant un std::vector<double>
	//! Computes some property on all the blobs of the class
	double_stl_vector GetSTLResult( funcio_calculBlob *evaluador ) const;
	
	//! Calcula un valor sobre un blob de la classe
	//! Computes some property on one blob of the class
	double GetNumber( int indexblob, funcio_calculBlob *evaluador ) const;

	//! Retorna aquells blobs que compleixen les condicions del filtre en el destination 
	//! Filters the blobs of the class using some property
	void Filter(BlobResult &dst,
				int filterAction, funcio_calculBlob *evaluador, 
				int condition, double lowLimit, double highLimit = 0 );
			
	//! Retorna l'en�ssim blob segons un determinat criteri
	//! Sorts the blobs of the class acording to some criteria and returns the n-th blob
	void GetNthBlob( funcio_calculBlob *criteri, int nBlob, Blob &dst ) const;
	
	//! Retorna el blob en�ssim
	//! Gets the n-th blob of the class ( without sorting )
	Blob GetBlob(int indexblob) const;
	
	BlobPtr GetBlobPtr(int indexblob);
	
	//! Elimina tots els blobs de l'objecte
	//! Clears all the blobs of the class
	void ClearBlobs();

	//! Escriu els blobs a un fitxer
	//! Prints some features of all the blobs in a file
	void PrintBlobs( char *nom_fitxer ) const;


//Metodes GET/SET

	//! Retorna el total de blobs
	//! Gets the total number of blobs
	int GetNumBlobs() const 
	{ 
		return(m_blobs.size()); 
	}

	//! Clears a calculated property from blobs
	void RemoveProperty( const std::string &name );
	
private:

	//! Funci� per gestionar els errors
	//! Function to manage the errors
	void RaiseError(const int errorCode) const;

	//! Does the Filter method job
	void DoFilter(BlobResult &dst,
				int filterAction, funcio_calculBlob *evaluador, 
				int condition, bool inlineFilter, double lowLimit, double highLimit = 0);

protected:

	//! Vector amb els blobs
	//! Vector with all the blobs
	Blob_vector		m_blobs;
};

CVBLOBS_END_NAMESPACE;

#endif // _CVBLOBS2_BLOBRESULT_H_
