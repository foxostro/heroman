#ifndef _STDAFX_H_
#define _STDAFX_H_

#define PROJECT_NAME "HeroMan"
#define PROJECT_AUTHOR "Andrew Fox and James Huff"

#if defined(_MSC_VER)
#	if _MSC_VER == 1500 // 2008
#		pragma warning(disable: 4180) // qualifier applied to function type has no meaning; ignored
#	elif _MSC_VER == 1400 // 2005
#		pragma warning(disable: 4180) // qualifier applied to function type has no meaning; ignored
#		pragma warning(disable: 4312) // type cast conversion from uintptr_t to void* of greater size in xlocnum
#	elif _MSC_VER == 1310 // 2003
#		pragma warning(disable: 4702) // unreachable code in xtree and list
#		pragma warning(disable: 4512) // cannot generate assignment operator
#	elif _MSC_VER == 1300 // 2002
#		// untested
#	elif _MSC_VER < 1300 // Visual Studio 6 and below
#		pragma error("Compiler not supported!")
#	endif
#
#	// necessary for boost (_s "safe" functions are not standard yet)
#	define _CRT_NONSTDC_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE
#	define _SCL_SECURE_NO_DEPRECATE
#
#	ifdef NDEBUG // disable checked iterators in Release mode
#		ifdef _SECURE_SCL
#			undef _SECURE_SCL
#		endif
#
#		ifdef _SECURE_SCL
#			undef _HAS_ITERATOR_DEBUGGING
#		endif
#
#		define _SECURE_SCL 0
#		define _HAS_ITERATOR_DEBUGGING 0
#	endif
#endif

#include <SDL/SDL.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

/* OpenGL Library */
#include <GL/gl.h>
#include <GL/glu.h>

/* Standard C Library */
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cfloat>
#include <cerrno>

/* STL */
#include <list>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <map>
#include <sstream>
#include <new>
#include <algorithm>
#include <exception>
using namespace std;

/* Boost Library */
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <boost/tokenizer.hpp>
#include <boost/function.hpp>
#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
using namespace boost;

/* Boost Lambda Library */
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
using namespace boost::lambda;

/* ODE Physics Engine */
#include <ode/ode.h>

/* DevIL Image Library */
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "macros.h"        // useful macros
#include "mytypes.h"       // types of specific sizes
#include "logger.h"        // Logger
#include "myassert.h"      // Custom assertion routine
#include "random.h"        // Various Random number routines
#include "tstring.h"       // functions to operate on strings
#include "FileName.h"      // File name wrapper
#include "FileFuncs.h"     // Useful file and filesystem utilities
#include "Kernel.h"        // Game task kernel

#include "ivec2.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include "mat4.h"

/** Convert a boost::any to a string */
string toString(const any &value);

/**
Loads an image and returns the DevIL image handle
@param fileName File name of the font image
@return DevIL image handle
*/
unsigned int devil_loadImage(const FileName &fileName);

/**
Calls DevIL's ilLoadImage, but allowing FileName type parameter
@param fileName File name of the font image
@return DevIL image handle
*/
ILboolean ilLoadImage(const FileName &fileName);

/** Wraps an angle around so that it lies between 0 and 2*pi */
float angle_clamp(const float rads);

/**
Gets the radian angle between two 2D vectors
@param a a 2D vector that is different from b
@param b a 2D vector that is different from a
@return radian angle wrapped to [0, 2*pi]
*/
float getAngle(const vec2 &a, const vec2 &b);

#endif

