#ifndef _TEXPACK_H_
#define _TEXPACK_H_

// System includes
#include <string.h>
#include <stdio.h>
#include <assert.h>

//
// OpenGL includes
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

//
// Local includes
#include "scene.h"
#include "data.h"
#include "error.h"

//
// Return defines
#define	TEXPACK_LOAD_FAILED		0
#define	TEXPACK_LOAD_SUCCESS	!TEXPACK_LOAD_FAILED

//
// Parameter defines
#define TEXPACK_LOAD_TEXTURE	1
#define TEXPACK_LOAD_AMBIENCE	2

//
// Local functions
int  TexpackLoad(char*, char*, patch_t*, char*, int);

//
// _TEXPACK_H_
#endif
