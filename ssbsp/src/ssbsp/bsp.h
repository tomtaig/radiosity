#ifndef _BSPC_H_
#define _BSPC_H_

// System includes
#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

//
// Local includes
#include "vector.h"
#include "face.h"
#include "parm.h"
#include "data.h"

//
// Bsp node structure
typedef struct bsp_t {
	plane_t	plane;
	int		maxis;
	int		index;
	face_t	*face;
	face_t	*faces;
	param_t params;
	struct bsp_t *front;
	struct bsp_t *back;
	struct bsp_t *parent;
}bsp_t;

//
// Local functions
void	BspPlaneSide(plane_t, face_t*, int[]);
bsp_t	*BspCompileTree(face_t*);
void	BspDestroyTree(bsp_t*);
void	BspWriteFile(char *, bsp_t *);

//
// _BSPC_H_
#endif