#ifndef _FACE_H_
#define _FACE_H_

// System includes
#include <memory.h>
#include <malloc.h>

//
// Local includes
#include "vector.h"
#include "parm.h"

//
// Face limits
#define FACE_MAXTEXLEN	128
#define FACE_MAXVERTS	32

//
// Face structure
typedef struct face_t {
	vec2_t	terts[FACE_MAXVERTS];
	vec3_t	verts[FACE_MAXVERTS];
	int		numverts;
	vec_t	area;
	plane_t	plane;
	int		index;	// faces with the same index have been split from one original face
	int		maxis;
	int		hullid;
	vec3_t	center; // 
	vec_t	radius; // bounding sphere for bsp
	param_t params;
	char	texxer[FACE_MAXTEXLEN];
	struct face_t *next;
}face_t;

//
// Local functions
void	FacePush(face_t**, face_t*);
face_t	*FacePop(face_t**);
void	FaceRemove(face_t**, face_t*);
face_t	*FaceCopy(face_t*);
void	FaceArea(face_t*);
void	FaceBounds(face_t*);
void	FaceClip(face_t*, plane_t, face_t*, face_t*);
void	FaceOptimiseVerts(face_t*);

//
// _FACE_H_
#endif