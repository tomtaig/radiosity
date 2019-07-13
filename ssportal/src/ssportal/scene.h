#ifndef _SCENE_H_
#define _SCENE_H_

// System includes
#include <malloc.h>
#include <stdlib.h>
#include <memory.h>

//
// Local includes
#include "param.h"
#include "face.h"
#include "vector.h"

//
// Bsp plane elipson
#define	BSP_ON_EPSILON	0.0005F

//
// Bsp node structure
typedef struct bsp_t {
	int		numfaces;
	plane_t	plane;
	int		maxis;
	int		index;
	face_t	*faces;
	param_t params;
	struct bsp_t *front;
	struct bsp_t *back;
	struct bsp_t *parent;
}bsp_t;

//
// Local functions
vec_t	SceneLinePlane(vec3_t, vec3_t, plane_t, vec3_t);
int		SceneLine(bsp_t*, vec3_t, vec3_t);
bsp_t	*SceneVector(bsp_t*, vec3_t);
void	SceneDestroyTree(bsp_t*);

//
// _SCENE_H_
#endif