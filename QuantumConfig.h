#ifndef _H_QUANTUM_CONFIG
#define _H_QUANTUM_CONFIG

#ifndef QUANTUM_TARGET_MAC
#define QUANTUM_TARGET_MAC		0
#endif
#ifndef QUANTUM_TARGET_UNIX
#define QUANTUM_TARGET_UNIX	0
#endif
#ifndef QUANTUM_TARGET_WIN
#define QUANTUM_TARGET_WIN		0
#endif
#ifndef QUANTUM_TARGET_BE
#define QUANTUM_TARGET_BE		0
#endif

#ifdef __APPLE__
#define GL_GL_H <OpenGL/gl.h>
#define GL_GLU_H <OpenGL/glu.h>
#define GL_GLUT_H <GLUT/glut.h>
#elif !QUANTUM_TARGET_WIN
#define GL_GL_H <GL/gl.h>
#define GL_GLU_H <GL/glu.h>
#define GL_GLUT_H <GL/glut.h>
#else
#define GL_GL_H <gl.h>
#define GL_GLU_H <glu.h>
#define GL_GLUT_H <glut.h>

	#ifdef __GNUC__
		#include <windef.h>
		#define _MSC_VER		/* tell glut.h that we are on windows */
	#endif
#endif
#include <stdlib.h>

#include <iosfwd>
using namespace std;

//#include <Windows32/Base.h>
#if QUANTUM_TARGET_MAC
#include <alloca.h>
#endif

#ifndef alloca
void *alloca(unsigned);
#endif

#if QUANTUM_TARGET_UNIX
	/* some versions of STL, for example the one distributed with
		my GNU C compiler, don't define mem_fun for functions with
		1 parameter, they define mem_fun1 instead....
		Metrowerks' MSL defines both cases with one name, mem_fun.
	*/

#define QUANTUM_EXPLICIT_MEMFUN1
#endif

#include <functional>
#ifdef QUANTUM_EXPLICIT_MEMFUN1
template <class S, class T, class A>
inline mem_fun1_t<S,T,A> mem_fun(S (T::*f)(A)) { 
  return mem_fun1_t<S,T,A>(f);
}

template <class S, class T, class A>
inline const_mem_fun1_t<S,T,A> mem_fun(S (T::*f)(A) const) {
  return const_mem_fun1_t<S,T,A>(f);
}

template <class S, class T, class A>
inline mem_fun1_ref_t<S,T,A> mem_fun_ref(S (T::*f)(A)) { 
  return mem_fun1_ref_t<S,T,A>(f);
}

template <class S, class T, class A>
inline const_mem_fun1_ref_t<S,T,A> mem_fun_ref(S (T::*f)(A) const) {
  return const_mem_fun1_ref_t<S,T,A>(f);
}

#endif

#endif
