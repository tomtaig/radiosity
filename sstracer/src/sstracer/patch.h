#ifndef _PATCH_H_
#define _PATCH_H_

// System includes
#include <stdio.h>

//
// Local includes
#include "data.h"
#include "scene.h"
#include "texpack.h"
#include "portal.h"

//
// Local functions
void	PatchPrepareFace(bsp_t*, face_t*);
int		PatchFindAndDistribute(bsp_t*, face_t*t);
void	PatchXYToXYZQuick(face_t*, vec_t, vec_t, vec3_t);
void	PatchXYToXYZ(face_t*, vec_t, vec_t, vec3_t);

//
// _PATCH_H_
#endif