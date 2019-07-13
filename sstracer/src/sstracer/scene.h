#ifndef _SCENE_H_
#define _SCENE_H_

// System includes
#include <malloc.h>
#include <stdlib.h>

//
// Local includes
#include "vector.h"
#include "data.h"

//
// Texture sizes
#define TEXTURE_X_SIZE		64
#define TEXTURE_Y_SIZE		64
#define TEXTURE_XY_SIZE		4096

//
// Patch sizes
#define PATCH_X_SIZE		64
#define PATCH_Y_SIZE		64
#define PATCH_XY_SIZE		4096

//
// Patch emmission sizes
#define PATCH_EMIT_X_SIZE	16
#define PATCH_EMIT_Y_SIZE	16
#define PATCH_EMIT_XY_SIZE	256

//
// Face limitations
#define FACE_MAXTEXLEN		132
#define FACE_MAXVERTS		12

//
// Bsp plane epsilon
#define	BSP_ON_EPSILON		0.05F

//
// Generic parameters
typedef enum {
	PARM_NULL	=0,
	PARM_ISLEAF	=1,
	PARM_ISUSED	=2,
	PARM_ONAXIS	=4,
	PARM_HOLLOW	=8,
	PARM_PORTAL =16
}param_t;

//
// Patch structure
typedef struct {
	vec3_t	drad;
	vec3_t	rrad;
	param_t	params;
	vec3_t	reflection;
	struct face_t *face;
}patch_t;

//
// Face structure
typedef struct face_t {
	patch_t	patches[PATCH_XY_SIZE];
	char	texxer[FACE_MAXTEXLEN];
	char	texpak[FACE_MAXTEXLEN];
	vec2_t	terts[FACE_MAXVERTS];
	vec3_t	verts[FACE_MAXVERTS];
	int		binder;
	int		numverts;
	plane_t	plane;
	int		maxis;
	int		index;
	vec_t	tarea;
	vec3_t	uaxis;
	vec3_t	vaxis;
	vec3_t	origin;
	param_t params;
	struct face_t	*next;
	struct bsp_t	*node;
	struct bsp_t	*portal;
}face_t;

//
// Bsp node structure
typedef struct bsp_t {
	int		numfaces;
	plane_t	plane;
	int		maxis;
	face_t	*faces;
	param_t params;
	struct bsp_t *front;
	struct bsp_t *back;
	struct bsp_t *parent;
}bsp_t;

//
// Local functions
int		SceneVectorInFace(face_t*, vec3_t);
vec_t	SceneLinePlaneIntersection(vec3_t, vec3_t, plane_t, vec3_t);
int		SceneLineIntersection(bsp_t*, vec3_t, vec3_t);
bsp_t*	SceneVectorIntersection(bsp_t*, vec3_t);
void	SceneDestroy(bsp_t*);

//
// _SCENE_H_
#endif