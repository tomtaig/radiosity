#ifndef _SCENE_H_
#define _SCENE_H_

//
// Local includes
#include "data.h"
#include "bsp.h"
#include "face.h"
#include "vector.h"

//
// Local functions
bsp_t	*SceneVector(bsp_t*, vec3_t);
int		SceneSphere(bsp_t*, vec3_t, vec_t);

//
// _SCENE_H_
#endif