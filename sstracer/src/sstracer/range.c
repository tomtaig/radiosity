#include "range.h"

//
// RangeFaceMinMax
//
void RangeFaceMinMax(face_t *face, vec2_t r) {

	vec_t	n;
	int		i;

	r[0]=100000000.0F;
	r[1]=0.0F;

	for(i=0; i<PATCH_XY_SIZE; i++) {
		n=(face->patches[i].rrad[0]+face->patches[i].rrad[1]+face->patches[i].rrad[2])/3;
		r[0]=(n<r[0])?n:r[0];
		r[1]=(n>r[1])?n:r[1];
	}

	return;
}

//
// RangeSceneMinMax
//
void RangeSceneMinMax(face_t *scene, vec2_t r) {

	vec2_t	n;
	
	r[0]=100000000.0F;
	r[1]=0.0F;

	for(; scene; scene=scene->next) {
		RangeFaceMinMax(scene, n);
		r[0]=(n[0]<r[0])?n[0]:r[0];
		r[1]=(n[1]>r[1])?n[1]:r[1];
	}

	return;
}

//
// RangeScaleFace
//
void RangeScaleFace(face_t *face, vec_t upr) {

	int		i;

	for(i=0; i<PATCH_XY_SIZE; i++) {
		face->patches[i].rrad[0]/=upr;
		face->patches[i].rrad[1]/=upr;
		face->patches[i].rrad[2]/=upr;
	}

	return;
}

//
// RangeScaleScene
//
void RangeScaleScene(face_t *scene, vec_t upr) {

	face_t	*a;

	for(a=scene; a; a=a->next) {
		RangeScaleFace(a, upr);
	}

	return;
}

//
// RangeScaleFaceTowards
//
void RangeScaleFaceTowards(face_t *face, vec_t axis, vec_t qu) {

	int		i;

	for(i=0; i<PATCH_XY_SIZE; i++) {
		face->patches[i].rrad[0]+=(axis-face->patches[i].rrad[0])*qu;
		face->patches[i].rrad[1]+=(axis-face->patches[i].rrad[1])*qu;
		face->patches[i].rrad[2]+=(axis-face->patches[i].rrad[2])*qu;
	}

	return;
}

//
// RangeScaleSceneTowards
//
void RangeScaleSceneTowards(face_t *scene, vec_t axis, vec_t qu) {

	face_t	*a;

	for(a=scene; a; a=a->next) {
		RangeScaleFaceTowards(a, axis, qu);
	}

	return;
}
