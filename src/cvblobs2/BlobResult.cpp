
/************************************************************************
  			BlobResult.cpp
  			
FUNCIONALITAT: Implementació de la classe BlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):
 
**************************************************************************/
#include <cvblobs2/BlobResult.h>

#include <limits.h>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include <cvblobs2/counted_ptr.h>

#include <cvblobs2/BlobOperators.h>

CVBLOBS_BEGIN_NAMESPACE

/**************************************************************************
		Constructors / Destructors
**************************************************************************/


/**
- FUNCIÓ: BlobResult
- FUNCIONALITAT: Constructor estandard.
- PARÀMETRES:
- RESULTAT:
- Crea un BlobResult sense cap blob
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 20-07-2004.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: BlobResult
- FUNCTIONALITY: Standard constructor
- PARAMETERS:
- RESULT:
	- creates an empty set of blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::BlobResult()
    : mBlobs()
{}

/**
- FUNCIÓ: BlobResult
- FUNCIONALITAT: Constructor a partir d'una imatge. Inicialitza la seqüència de blobs 
			   amb els blobs resultants de l'anàlisi de blobs de la imatge.
- PARÀMETRES:
	- source: imatge d'on s'extreuran els blobs
	- mask: màscara a aplicar. Només es calcularan els blobs on la màscara sigui 
			diferent de 0. Els blobs que toquin a un pixel 0 de la màscara seran 
			considerats exteriors.
	- threshold: llindar que s'aplicarà a la imatge source abans de calcular els blobs
	- findmoments: indica si s'han de calcular els moments de cada blob
	- blackBlobs: true per buscar blobs negres a la binaritzazió (it will join all extern white blobs).
				  false per buscar blobs negres a la binaritzazió (it will join all extern black blobs).

- RESULTAT:
	- objecte BlobResult amb els blobs de la imatge source
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
// @todo
// BlobResult::BlobResult(IplImage *source, IplImage *mask, uchar backgroundColor )
// {
// 	bool success;

// 	try
// 	{
// 		success = ComponentLabeling( source, mask, backgroundColor, mBlobs );
// 	}
// 	catch(...)
// 	{
// 		success = false;
// 	}

// 	if( !success ) throw EXCEPCIO_CALCUL_BLOBS;
// }

/**
- FUNCIÓ: BlobResult
- FUNCIONALITAT: Constructor de còpia. Inicialitza la seqüència de blobs 
			   amb els blobs del paràmetre.
- PARÀMETRES:
	- source: objecte que es copiarà
- RESULTAT:
	- objecte BlobResult amb els blobs de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: BlobResult
- FUNCTIONALITY: Copy constructor
- PARAMETERS:
	- source: object to copy
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::BlobResult(const BlobResult& source)
    : mBlobs(source.mBlobs)
{}


/**
- FUNCIÓ: ~BlobResult
- FUNCIONALITAT: Destructor estandard.
- PARÀMETRES:
- RESULTAT:
	- Allibera la memòria reservada de cadascun dels blobs de la classe
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: ~BlobResult
- FUNCTIONALITY: Destructor
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult::~BlobResult()
{
	clearBlobs();
}

/**************************************************************************
		Operadors / Operators
**************************************************************************/


/**
- FUNCIÓ: operador =
- FUNCIONALITAT: Assigna un objecte source a l'actual
- PARÀMETRES:
	- source: objecte a assignar
- RESULTAT:
	- Substitueix els blobs actuals per els de l'objecte source
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: Assigment operator
- FUNCTIONALITY: 
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult& BlobResult::operator=(const BlobResult& source)
{
	if (this != &source)
	{
    // use copy-swap idiom here to be safe in the face of allocation exceptions
    BlobResult tmp(source);
    // swap contents of tmp with this
    swap(tmp);
	}
	return *this;
}

// it's OK for result to be lhs or  rhs
template <typename T>
void concatVectors(std::vector<T>& result,
                   const std::vector<T>& lhs,
                   const std::vector<T>& rhs)
{
  if (&result == &lhs)
  {
    // allocate space for result and rhs
    result.reserve(result.size() + rhs.size());
    result.insert(result.end(), rhs.begin(), rhs.end());
  }
  else if (&result == &rhs)
  {
    // allocate space for result and lhs
    result.reserve(result.size() + lhs.size());
    result.insert(result.end(), lhs.begin(), lhs.end());
  }
  else
  {
    // allocate space for result, lhs, and rhs
    result.reserve(result.size() + lhs.size() + rhs.size());
    // append lhs first
    result.insert(result.end(), lhs.begin(), lhs.end());
    // append rhs second
    result.insert(result.end(), rhs.begin(), rhs.end());
  }
}

/**
- FUNCIÓ: operador +
- FUNCIONALITAT: Concatena els blobs de dos BlobResult
- PARÀMETRES:
	- source: d'on s'agafaran els blobs afegits a l'actual
- RESULTAT:
	- retorna un nou BlobResult amb els dos BlobResult concatenats
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- NOTA: per la implementació, els blobs del paràmetre es posen en ordre invers
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/**
- FUNCTION: + operator
- FUNCTIONALITY: Joins the blobs in source with the current ones
- PARAMETERS:
	- source: object to copy the blobs
- RESULT:
	- object with the actual blobs and the source blobs
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
BlobResult BlobResult::operator+(const BlobResult& source) const
{
  // Normally you would implement + in terms of += but in this case it is very
  // inefficient to do that because two copies of the mBlobs vector would be
  // made. Instead we make an empty BlobResult and add the blobs from this
  // and source into it at the same time. This requires only one allocation
  // resulting in a more efficient implementation.
	BlobResult result;
  concatVectors(result.mBlobs, source.mBlobs, mBlobs);
	return result;
}

BlobResult& BlobResult::operator+=(const BlobResult& source)
{
  // += is only needed if source has blobs to copy over otherwise we're making
  // an uncessary copy of our mBlobs for no reason.
  if (!source.mBlobs.empty())
  {
    // use copy-swap idiom here to be safe in the face of allocation exceptions
    BlobContainer tmp_blobs;
    concatVectors(tmp_blobs, source.mBlobs, mBlobs);
    
    // swap the containers so that mBlobs can be safely updated 
    std::swap(mBlobs, tmp_blobs);
  }
  return *this;
}

void BlobResult::swap(BlobResult& other)
{
  std::swap(mBlobs, other.mBlobs);
}

/**************************************************************************
		Operacions / Operations
**************************************************************************/

/**
- FUNCIÓ: AddBlob
- FUNCIONALITAT: Afegeix un blob al conjunt
- PARÀMETRES:
	- blob: blob a afegir
- RESULTAT:
	- modifica el conjunt de blobs actual
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 2006/03/01
- MODIFICACIÓ: Data. Autor. Descripció.
*/
void BlobResult::addBlob(BlobPtrType blob)
{
	mBlobs.push_back(blob);
}

/**
- FUNCIÓ: GetSTLResult
- FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
- PARÀMETRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
- RESULTAT:
	- Retorna un array de double's STL amb el resultat per cada blob
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
std::vector<double> BlobResult::result(BlobOperator* pOperator) const
{
  std::vector<double> result;
	if (numBlobs() <= 0)
	{
		return result;
	}

  // make enough space for each blob's result
  result.reserve(numBlobs());

  // evaluate pOperator for each blob
	for (BlobContainer::const_iterator it = mBlobs.begin();
       it != mBlobs.end();
       ++it)
  {
    result.push_back( pOperator->GetOperatorResult(**it) );
	}
	return result;
}

/**
- FUNCIÓ: GetNumber
- FUNCIONALITAT: Calcula el resultat especificat sobre un únic blob de la classe
- PARÀMETRES:
	- evaluador: Qualsevol objecte derivat de COperadorBlob
	- indexblob: número de blob del que volem calcular el resultat.
- RESULTAT:
	- Retorna un double amb el resultat
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
double BlobResult::number(int blobIndex, BlobOperator* pOperator) const
{
  // use range checked at() to be safe
  BlobPtrType p_blob = mBlobs.at(blobIndex);
	return pOperator->GetOperatorResult(*p_blob);
}

/////////////////////////// FILTRAT DE BLOBS ////////////////////////////////////


/**
- FUNCIÓ: Filter
- FUNCIONALITAT: Filtra els blobs de la classe i deixa el resultat amb només 
			   els blobs que han passat el filtre.
			   El filtrat es basa en especificar condicions sobre un resultat dels blobs
			   i seleccionar (o excloure) aquells blobs que no compleixen una determinada
			   condicio
- PARÀMETRES:
	- dst: variable per deixar els blobs filtrats
	- filterAction:	acció de filtrat. Incloure els blobs trobats (B_INCLUDE),
				    o excloure els blobs trobats (B_EXCLUDE)
	- evaluador: Funció per evaluar els blobs (qualsevol objecte derivat de COperadorBlob
	- Condition: tipus de condició que ha de superar la mesura (FilterType) 
				 sobre cada blob per a ser considerat.
				    B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
				    B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
	- LowLimit:  valor numèric per a la comparació (Condition) de la mesura (FilterType)
	- HighLimit: valor numèric per a la comparació (Condition) de la mesura (FilterType)
				 (només té sentit per a aquelles condicions que tenen dos valors 
				 (B_INSIDE, per exemple).
- RESULTAT:
	- Deixa els blobs resultants del filtrat a destination
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::filter(BlobResult& dst,
                        FilterAction filterAction, 
                        BlobOperator* pOperator,
                        FilterCondition condition, 
                        double lowLimit,
                        double highLimit /*=0*/)
{
	if (pOperator == NULL || numBlobs() == 0)
  {
    return;
  }
  
  bool b_inline_filter = (&dst == this);

  std::size_t num_blobs_erased = 0;
	bool b_result_passed = false;
  bool b_selected_blob = false;
  
	// evaluate blobs using specified operator
  std::vector<double> blob_results = result(pOperator);
  const std::size_t num_results = blob_results.size();
  double result = 0.0;
  
	for (std::size_t i = 0; i < num_results; ++i)
	{
    result = blob_results[i];
		switch (condition)
		{
			case CONDITION_EQUAL:
				b_result_passed = (result == lowLimit);
				break;
			case CONDITION_NOT_EQUAL:
				b_result_passed = (result != lowLimit);
				break;
			case CONDITION_GREATER:
				b_result_passed = (result > lowLimit);
				break;
			case CONDITION_LESS:
				b_result_passed = (result < lowLimit);
				break;
			case CONDITION_GREATER_OR_EQUAL:
				b_result_passed = (result >= lowLimit);
				break;
			case CONDITION_LESS_OR_EQUAL:
				b_result_passed = (result <= lowLimit);
				break;
			case CONDITION_INSIDE:
				b_result_passed = (result >= lowLimit) && (result <= highLimit); 
				break;
			case CONDITION_OUTSIDE:
				b_result_passed = (result < lowLimit) || (result > highLimit); 
				break;
		} // end switch condition
		
		// blob is selected?
		b_selected_blob = ( (b_result_passed && filterAction == ACTION_INCLUDE ) || 
                        (!b_result_passed && filterAction == ACTION_EXCLUDE ) );

		// add to dst if not inline operation
    // (for inline operation we don't have to do anything)
		if (!b_inline_filter && b_selected_blob)
		{
			dst.addBlob( mBlobs[i - num_blobs_erased] );
		}
		// remove from origin for inline operation
    // (for not inline we don't have to do anything)
		if (b_inline_filter && !b_selected_blob)
		{
			mBlobs.erase(mBlobs.begin() + i - num_blobs_erased);
			++num_blobs_erased;
		}
	} // end for each blob
}


/**
- FUNCIÓ: GetBlob
- FUNCIONALITAT: Retorna un blob si aquest existeix (index != -1)
- PARÀMETRES:
	- indexblob: index del blob a retornar
- RESULTAT:
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: GetBlob
- FUNCTIONALITY: Gets the n-th blob (without ordering the blobs)
- PARAMETERS:
	- indexblob: index in the blob array
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
const Blob& BlobResult::blob(std::size_t blobIndex) const
{
  // use at for safe range checking
	return *mBlobs.at(blobIndex);
}

/**
- FUNCIÓ: GetNthBlob
- FUNCIONALITAT: Retorna l'enèssim blob segons un determinat criteri
- PARÀMETRES:
	- criteri: criteri per ordenar els blobs (objectes derivats de COperadorBlob)
	- nBlob: index del blob a retornar
	- dst: on es retorna el resultat
- RESULTAT:
	- retorna el blob nBlob a dst ordenant els blobs de la classe segons el criteri
	  en ordre DESCENDENT. Per exemple, per obtenir el blob major:
		GetNthBlob( BlobGetArea(), 0, blobMajor );
		GetNthBlob( BlobGetArea(), 1, blobMajor ); (segon blob més gran)
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
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
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::nthBlob(BlobOperator* pCriteria,
                         std::size_t nBlob,
                         Blob& dst) const
{
  // range check
	if (nBlob >= numBlobs())
	{
		dst = Blob();
		return;
	}

  // @todo make this more efficient

	// evaluate the blobs with the pCriteria function
  std::vector<double> blob_results = result(pCriteria);
  std::vector<double> blob_results_sorted(numBlobs());

  // obtain the first nBlob results in descending order
	std::partial_sort_copy( blob_results.begin(), 
                          blob_results.end(),
                          blob_results_sorted.begin(), 
                          blob_results_sorted.end(),
                          std::greater<double>() );

	double nth_value = blob_results_sorted[nBlob];
  std::size_t blob_index = 0;
  
	// seek first blob that has the value n
	for (std::vector<double>::const_iterator iter = blob_results.begin();
       iter != blob_results.begin();
       ++iter, ++blob_index)
	{
		if (*iter == nth_value)
		{
			dst = Blob( blob(blob_index) );
      break;
		}
	}
}

/**
- FUNCIÓ: ClearBlobs
- FUNCIONALITAT: Elimina tots els blobs de l'objecte
- PARÀMETRES:
- RESULTAT: 
	- Allibera tota la memòria dels blobs
- RESTRICCIONS:
- AUTOR: Ricard Borràs Navarra
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: ClearBlobs
- FUNCTIONALITY: Clears all the blobs from the object and releases all its memory
- PARAMETERS:
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::clearBlobs()
{
	mBlobs.clear();
}

/**************************************************************************
		Auxiliars / Auxiliary functions
**************************************************************************/


/**
- FUNCIÓ: PrintBlobs
- FUNCIONALITAT: Escriu els paràmetres (àrea, perímetre, exterior, mitjana) 
			   de tots els blobs a un fitxer.
- PARÀMETRES:
	- nom_fitxer: path complet del fitxer amb el resultat
- RESULTAT:
- RESTRICCIONS:
- AUTOR: Ricard Borràs
- DATA DE CREACIÓ: 25-05-2005.
- MODIFICACIÓ: Data. Autor. Descripció.
*/
/*
- FUNCTION: PrintBlobs
- FUNCTIONALITY: Prints some blob features in an ASCII file
- PARAMETERS:
	- nom_fitxer: full path + filename to generate
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 25-05-2005.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::printBlobs(char* pFileName) const
{
  std::vector<double> area             = result( BlobGetArea() );
	std::vector<double> perimeter        = result( BlobGetPerimeter() );
  std::vector<double> exterior         = result( BlobGetExterior() );
  std::vector<double> compactness      = result( BlobGetCompactness() );
  std::vector<double> length           = result( BlobGetLength() );
  std::vector<double> extern_perimeter = result( BlobGetExternPerimeter() );
  std::vector<double> hull_perimeter   = result( BlobGetHullPerimeter() );

  FILE* p_file = fopen(pFileName, "w" );
  const std::size_t num_blobs = numBlobs();
  
	for (std::size_t i = 0; i < num_blobs; ++i)
	{
		fprintf(p_file, "blob %lu ->\t a=%7.0f\t p=%8.2f (%8.2f extern)\t pconvex=%8.2f\t ext=%.0f\t c=%3.2f\t l=%8.2f\n",
            i,
            area[i],
            perimeter[i],
            extern_perimeter[i],
            hull_perimeter[i],
            exterior[i],
            compactness[i],
            length[i] );
	}
	fclose(p_file);
}

/*
- FUNCTION: RemoveProperty
- FUNCTIONALITY: Remove a property from al contained blobs to allow recalculation
- PARAMETERS:
	- name: property name (BlobOperador name)
- RESULT:
- RESTRICTIONS:
- AUTHOR: Ricard Borràs
- CREATION DATE: 19-11-2009.
- MODIFICATION: Date. Author. Description.
*/
void BlobResult::removeProperty(const std::string& propertyName)
{
  for (BlobContainer::iterator iter = mBlobs.begin();
       iter != mBlobs.end();
       ++iter)
  {
    (*iter)->resetProperty(propertyName);
	}
}

CVBLOBS_END_NAMESPACE
