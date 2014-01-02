/**
 * @brief Header that declares all #defines for the library
 * @author N. Maludy
 * @date 01/02/2014
 */
#ifndef _CVBLOBS2_CVBLOBSDEFS_H_
#define _CVBLOBS2_CVBLOBSDEFS_H_

#undef CVBLOBS_NAMESPACE
#undef CVBLOBS_BEGIN_NAMESPACE
#undef CVBLOBS_END_NAMESPACE
#undef CVBLOBS_DISALLOW_EVIL_CONSTRUCTORS

#define CVBLOBS_NAMESPACE cvblobs
#define CVBLOBS_BEGIN_NAMESPACE namespace CVBLOBS_NAMESPACE {
#define CVBLOBS_END_NAMESPACE }

#define CVBLOBS_DISALLOW_EVIL_CONSTRUCTORS(TypeName)  \
  TypeName(const TypeName&);                          \
  void operator=(const TypeName&);

#endif // _CVBLOBS2_CVBLOBSDEFS_H_
