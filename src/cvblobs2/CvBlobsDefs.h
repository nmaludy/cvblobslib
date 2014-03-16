/**
 * @brief Header that declares all #defines for the library
 * @author Nick Maludy <nmaludy@gmail.com>
 * @date 01/02/2014
 */
#ifndef _CVBLOBS2_CVBLOBSDEFS_H_
#define _CVBLOBS2_CVBLOBSDEFS_H_

#undef CVBLOBS_NAMESPACE
#undef CVBLOBS_BEGIN_NAMESPACE
#undef CVBLOBS_END_NAMESPACE
#undef CVBLOBS_DISALLOW_EVIL_CONSTRUCTORS
#undef CVBLOBS_PI

//! namespace helpers
#define CVBLOBS_NAMESPACE cvblobs
#define CVBLOBS_BEGIN_NAMESPACE namespace CVBLOBS_NAMESPACE {
#define CVBLOBS_END_NAMESPACE }

//! helper for hiding copy constructors
#define CVBLOBS_DISALLOW_EVIL_CONSTRUCTORS(TypeName)  \
  TypeName(const TypeName&);                          \
  void operator=(const TypeName&);

//! pi
#define CVBLOBS_PI 3.1415926535897932384626433832795
#define CVBLOBS_DEGREE2RAD (CVBLOBS_PI / 180.0)

//! Max order of calculated moments
#define CVBLOBS_MAX_MOMENTS_ORDER 3

#endif // _CVBLOBS2_CVBLOBSDEFS_H_
