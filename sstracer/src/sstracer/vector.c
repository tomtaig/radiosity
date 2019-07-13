#include "vector.h"

/*
INLINED (see vector.h)

//
// VectorCopy
//
void VectorCopy(vec3_t out, vec3_t in) {
	
	out[0]=in[0];
	out[1]=in[1];
	out[2]=in[2];
}
*/

//
// VectorClear
//
void VectorClear(vec3_t in) {
	
	in[0]=0;
	in[1]=0;
	in[2]=0;
}

//
// VectorAbs
//
void VectorAbs(vec3_t in) {

	in[0]=fabs(in[0]);
	in[1]=fabs(in[1]);
	in[2]=fabs(in[2]);
}

//
// VectorMinus
//
void VectorMinus(vec3_t in) {

	in[0]=-in[0];
	in[1]=-in[1];
	in[2]=-in[2];
}

//
// VectorCrossProduct
//
void VectorCrossProduct(vec3_t in1, vec3_t in2, vec3_t out) {
	
	vec3_t	tmp;

	tmp[0]=in1[1]*in2[2]-in1[2]*in2[1];
	tmp[1]=in1[2]*in2[0]-in1[0]*in2[2];
	tmp[2]=in1[0]*in2[1]-in1[1]*in2[0];

	VectorCopy(out, tmp);
}

/*
INLINED (see vector.h)

//
// VectorDotProduct
//
vec_t VectorDotProduct(vec3_t in1, vec3_t in2) {
	
	register vec_t a;

	a=in1[0]*in2[0]+in1[1]*in2[1]+in1[2]*in2[2];

	return a;
}
*/

//
// VectorLength
//
vec_t VectorLength(vec3_t in1) {
	
	register vec_t a;

	a=sqrt(VectorDotProduct(in1, in1));

	return a;
}

//
//VectorNormalize
//
vec_t VectorNormalize(vec3_t in1) {
	
	vec_t len=1.0F/VectorLength(in1);

	in1[0]*=len;
	in1[1]*=len;
	in1[2]*=len;
	
	return  len;
}

//
// VectorScale
//
void VectorScale(vec3_t out, vec_t scale) {
	
	if(scale==0) {
		return;
	}
	
	out[0]/=scale;
	out[1]/=scale;
	out[2]/=scale;
}

//
// VectorScaleInv
//
void VectorScaleInv(vec3_t out, vec_t scale) {
	
	out[0]*=scale;
	out[1]*=scale;
	out[2]*=scale;
}

//
// VectorAdd
//
void VectorAdd(vec3_t in1, vec3_t in2, vec3_t out) {
	
	out[0]=in1[0]+in2[0];
	out[1]=in1[1]+in2[1];
	out[2]=in1[2]+in2[2];
}

//
// VectorSub
//
void VectorSub(vec3_t in1, vec3_t in2, vec3_t out) { 
	
	out[0]=in1[0]-in2[0];
	out[1]=in1[1]-in2[1];
	out[2]=in1[2]-in2[2];
}

//
// VectorDistance
//
vec_t VectorDistance(vec3_t in1, vec3_t in2) {
	
	vec3_t tmp;
	
	VectorSub(in1, in2, tmp);

	return VectorLength(tmp);
}

//
// VectorMajor
//
int VectorMajor(vec3_t in) {

	vec3_t	tmp;

	VectorCopy(tmp, in);
	VectorAbs(tmp);
	
	if(tmp[0]>=tmp[1] && tmp[0]>=tmp[2]) {
		return 0;
	}
	else
	if(tmp[1]>=tmp[0] && tmp[1]>=tmp[2]) {
		return 1;
	}
	else{
		return 2;
	}
}
