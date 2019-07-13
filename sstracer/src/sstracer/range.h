#ifndef _RANGE_H_
#define _RANGE_H_

//
// Local includes
#include "vector.h"
#include "patch.h"
#include "scene.h"

//
// Local functions
void RangeFaceMinMax(face_t*, vec2_t);
void RangeSceneMinMax(face_t*, vec2_t);
void RangeScaleFaceTowards(face_t*, vec_t, vec_t);
void RangeScaleSceneTowards(face_t*, vec_t, vec_t);
void RangeScaleFace(face_t*, vec_t);
void RangeScaleScene(face_t*, vec_t);

//
// _RANGE_H_
#endif
