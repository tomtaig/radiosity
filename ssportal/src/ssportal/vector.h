#ifndef _VECTOR_H_
#define _VECTOR_H_

// System includes
#include <math.h>

//
// disable "double to float" warning
#pragma warning(disable:4244)

//
// M_PI as is in gcc v2
#define M_PI	3.14159265358979323846

//
// Radian <-> Degree Funcs
#define DEG2RAD(a) (((a)*M_PI)/180.0F)
#define RAD2DEG(a) (((a)*180.0f)/M_PI)

//
// Vector variable type
typedef double vec_t;

//
// Vector types
typedef vec_t vec4_t[4];
typedef vec_t vec3_t[3];
typedef vec_t vec2_t[2];

//
// Plane (dir & distance)
typedef vec4_t plane_t;

//
// Inlined Functions
#define VectorDotProductInline(a, b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])

//
// Functions
void  VectorMinus(vec3_t);
void  VectorMinus4(vec3_t);
void  VectorClear(vec3_t);
void  VectorCopy2(vec2_t, vec2_t);
void  VectorCopy(vec3_t, vec3_t);
void  VectorCopy4(vec2_t, vec2_t);
vec_t VectorDotProduct(vec3_t, vec3_t);
void  VectorCrossProduct(vec3_t, vec3_t, vec3_t);
void  VectorAdd(vec3_t, vec3_t, vec3_t);
void  VectorSub(vec3_t, vec3_t, vec3_t);
vec_t VectorLength(vec3_t);
vec_t VectorNormalize(vec3_t);
void  VectorScale(vec3_t, vec_t);
void  VectorScaleInv(vec3_t, vec_t);
vec_t VectorDistance(vec3_t, vec3_t);
int	  VectorMajor(vec3_t);

//
// _VECTOR_H_
#endif