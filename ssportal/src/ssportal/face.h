#ifndef _FACE_H_
#define _FACE_H_

// System includes
#include <memory.h>
#include <malloc.h>

//
// Local includes
#include "error.h"
#include "param.h"
#include "vector.h"

//
// Face limitations
#define FACE_MAXVERTS		32
#define	FACE_CLIP_EPSILON	0.05F

//
// Face structure
typedef struct face_t {
	vec2_t	terts[FACE_MAXVERTS];
	vec3_t	verts[FACE_MAXVERTS];
	plane_t	edges[FACE_MAXVERTS];
	int		numverts;
	plane_t	plane;
	int		maxis;
	int		index;
	param_t params;
	struct face_t	*next;
	struct bsp_t	*node;
	struct bsp_t	*portal;
}face_t;

//
// Local functions
void	FacePush(face_t**, face_t*);
face_t	*FacePop(face_t**);
void	FaceRemove(face_t**, face_t*);
face_t	*FaceCopy(face_t*);
void	FaceExtendedInfo(face_t*);
void	FaceClip(face_t*, plane_t, face_t*, face_t*);
vec_t	FaceArea(face_t*);
void	FaceEdgePlane(face_t*, int);
int		FaceIntersection(face_t*, face_t*, face_t*);
int		FaceCoPlanar(face_t*, face_t*);
void	FaceOnPlaneSide(plane_t, face_t*, int*);
void	FaceOptimiseVerts(face_t*);
void	FaceReverseWinding(face_t*);
void	FaceReverseWinding(face_t*);

//
// _FACE_H_
#endif