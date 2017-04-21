// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#define NOMINMAX
#include <windows.h>

// TODO: reference additional headers your program requires here
#define _SCL_SECURE_NO_WARNINGS

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/numeric/ublas/storage.hpp>

#define _UBLAS_BINDING

#ifdef _UBLAS_BINDING
#include <boost/numeric/bindings/blas/blas.hpp>
#include <boost/numeric/bindings/traits/ublas_vector.hpp>
#else
#include <boost/numeric/ublas/blas.hpp>
#include <boost/numeric/ublas/vector.hpp>
#endif

#include <boost/numeric/ublas/functional.hpp>
#include <boost/numeric/ublas/vector_expression.hpp>

#include <mkl_vml.h>
#include <mkl_vsl.h>
#include <mkl_service.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdarg.h>


