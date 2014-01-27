/*
 * counted_ptr - simple reference counted pointer.
 *
 * The is a non-intrusive implementation that allocates an additional
 * int and pointer for every counted object.
 */

#ifndef _CVBLOBS2_COUNTED_PTR_H_
#define _CVBLOBS2_COUNTED_PTR_H_

#include <cvblobs2/CvBlobsDefs.h>
#include <cstddef> // for NULL

// #define NO_MEMBER_TEMPLATES
/* For ANSI-challenged compilers, you may want to #define
 * NO_MEMBER_TEMPLATES or explicit */

CVBLOBS_BEGIN_NAMESPACE

template <class X>
class counted_ptr
{
 public:
  typedef X element_type;

  explicit counted_ptr(X* pPtr = NULL) // allocate a new counter
      : mpCounter(NULL)
  {
    if (pPtr != NULL)
    {
      mpCounter = new Counter(pPtr);
    }
  }
  
  ~counted_ptr()
  {
    release();
  }
  
  counted_ptr(const counted_ptr& other) throw()
  {
    acquire(other.mpCounter);
  }
  
  counted_ptr& operator=(const counted_ptr& rhs)
  {
    if (this != &rhs)
    {
      release();
      acquire(rhs.mpCounter);
    }
    return *this;
  }

#ifndef NO_MEMBER_TEMPLATES
  template <class Y>
  counted_ptr(const counted_ptr<Y>& other) throw()
  {
    acquire(other.mpCounter);
  }
  
  template <class Y>
  counted_ptr& operator=(const counted_ptr<Y>& rhs)
  {
    if (this != &rhs)
    {
      release();
      acquire(rhs.mpCounter);
    }
    return *this;
  }
#endif // NO_MEMBER_TEMPLATES

  inline X& operator*() const throw()
  {
    return *mpCounter->mpPtr;
  }

  inline X* operator->() const throw()
  {
    return mpCounter->mpPtr;
  }
  
  inline X* get() const throw()
  {
    return (mpCounter != NULL) ? mpCounter->mpPtr : NULL;
  }
  
  inline bool unique() const throw()
  {
    return ((mpCounter != NULL) ? mpCounter->mCount == 1 : true);
  }

 private:

  struct Counter
  {
    Counter(X* pPtr = NULL, unsigned count = 1)
        : mpPtr(pPtr),
          mCount(count)
    {}
    
    X*           mpPtr;
    unsigned int mCount;
  };

  void acquire(Counter* pCounter) throw()
  {
    // increment the count
    mpCounter = pCounter;
    if (pCounter != NULL)
    {
      ++(pCounter->mCount);
    }
  }

  void release()
  {
    // decrement the count, delete if it is 0
    if (mpCounter != NULL)
    {
      if (--(mpCounter->mCount) == 0)
      {
        delete mpCounter->mpPtr;
        delete mpCounter;
      }
      mpCounter = NULL;
    }
  }

  Counter* mpCounter;
};

CVBLOBS_END_NAMESPACE

#endif // _CVBLOBS2_COUNTED_PTR_H_
