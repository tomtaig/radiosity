#include "scene.h"

//
// SceneVector
//
bsp_t *SceneVector(bsp_t *src, vec3_t vec) {

	vec_t	 dis;
	vec_t	 eps;

	eps=DataReadPrecision("bsp_epsilon");

	while(!(src->params&PARM_ISLEAF)) {
		if(src->params&PARM_ONAXIS) {
			dis=src->plane[src->maxis]*vec[src->maxis]-src->plane[3];
		}else{
			dis=VectorDotProduct(src->plane, vec)-src->plane[3];
		}

		if(dis>=-eps) {
			src=src->front;
		}else{
			src=src->back;
		}
	}

	return !(src->params&PARM_HOLLOW)?src:NULL;
}

//
// SceneSphere
//
int SceneSphere(bsp_t *src, vec3_t vec, vec_t rad) {

	vec_t	 dis;

	while(!(src->params&PARM_ISLEAF)) {
		if(src->params&PARM_ONAXIS) {
			dis=src->plane[src->maxis]*vec[src->maxis]-src->plane[3];
		}else{
			dis=VectorDotProduct(src->plane, vec)-src->plane[3];
		}

		if(fabs(dis)<rad) {
			if(SceneSphere(src->front, vec, rad)) return !0;
			if(SceneSphere(src->back, vec, rad)) return !0;
		}else{	
			src=(dis>=0.0F)?src->front:src->back;
		}
	}

	return !(src->params&PARM_HOLLOW);
}