#ifndef _SAVE_H_
#define _SAVE_H_

// System includes
#include <stdio.h>
#include <assert.h>

//
// Local includes
#include "scene.h"
#include "patch.h"
#include "error.h"

//
// Local defines
#define	SAVE_FAILED		0
#define	SAVE_SUCCESS	!SAVE_FAILED

//
// Local functions
int  SaveLmp(char*, face_t*);

//
// _SAVE_H_
#endif
