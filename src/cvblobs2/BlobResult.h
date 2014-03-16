#ifndef _CVBLOBS2_BLOBRESULT_H_
#define _CVBLOBS2_BLOBRESULT_H_

#include <cvblobs2/CvBlobsFwd.h>
#include <string>

CVBLOBS_BEGIN_NAMESPACE

/**
 * @class BlobResult
 * @brief Class to calculate the blobs of an image and calculate some properties
 * on them. Also, the class provides functions to filter the blobs using
 * some criteria.
 * @author Ricard Borràs
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 03/16/2014
 */
class BlobResult  
{
 public:

	//! constructor estandard, crea un conjunt buit de blobs
	//! Standard constructor, it creates an empty set of blobs
	BlobResult();
	//! constructor a partir d'una imatge
	//! Image constructor, it creates an object with the blobs of the image
  // @todo
	// BlobResult(IplImage* pSource, IplImage* pMask, uchar backgroundColor);
	//! constructor de còpia
	//! Copy constructor
	BlobResult(const BlobResult& source);
	//! Destructor
	virtual ~BlobResult();

	//! operador = per a fer assignacions entre BlobResult
	//! Assigment operator
	BlobResult& operator=(const BlobResult& source);
	//! operador + per concatenar dos BlobResult
	//! Addition operator to concatenate two sets of blobs
	BlobResult operator+(const BlobResult& source) const;
  //! Inplace addition operator to concatenate another set of blobs to this one
	BlobResult& operator+=(const BlobResult& source);

  //! swaps the blobs in other with the blobs in this
  void swap(BlobResult& other);
  
  //! Afegeix un blob al conjunt
	//! Adds a blob to the set of blobs
	void addBlob(cv::Ptr<Blob> blob);

	//! Calcula un valor sobre tots els blobs de la classe retornant un std::vector<double>
	//! Computes some property on all the blobs of the class
  std::vector<double> result(BlobOperator* pOperator) const;
	
	//! Calcula un valor sobre un blob de la classe
	//! Computes some property on one blob of the class
	double number(int blobIndex, BlobOperator* pOperator) const;

	//! Retorna aquells blobs que compleixen les condicions del filtre en el destination 
	//! Filters the blobs of the class using some property
	void filter(BlobResult& dst,
              FilterAction filterAction,
              BlobOperator* pOperator,
              FilterCondition condition,
              double lowLimit,
              double highLimit = 0);
			
	//! Retorna l'enèssim blob segons un determinat criteri
	//! Sorts the blobs of the class acording to some criteria and returns the n-th blob
  cv::Ptr<Blob> nthBlob(BlobOperator* pCriteria,
                        std::size_t nBlob) const;
	
	//! Retorna el blob enèssim
	//! Gets the n-th blob of the class ( without sorting )
  cv::Ptr<Blob> blob(std::size_t i) const;
	
	//! Elimina tots els blobs de l'objecte
	//! Clears all the blobs of the class
	void clearBlobs();

	//! Escriu els blobs a un fitxer
	//! Prints some features of all the blobs in a file
	void printBlobs(char* pFileName) const;

  //Metodes GET/SET

	//! Retorna el total de blobs
	//! Gets the total number of blobs
  inline std::size_t numBlobs() const;

	//! Clears a calculated property from blobs
	void removeProperty(const std::string& propertyName);
	
 protected:

	//! Vector amb els blobs
	//! Vector with all the blobs
	BlobContainerType mBlobs;
};

inline std::size_t BlobResult::numBlobs() const 
{ 
  return mBlobs.size();
}

CVBLOBS_END_NAMESPACE

namespace std {
template<>
void swap(cvblobs::BlobResult& lhs, cvblobs::BlobResult& rhs);
} // namespace std

#endif // _CVBLOBS2_BLOBRESULT_H_
