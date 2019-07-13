#include "scene.h"

//
// SceneLinePlane
//
vec_t SceneLinePlane(vec3_t line1, vec3_t line2, plane_t plane, vec3_t res) {

	vec3_t	temp1;
	vec3_t	temp2;
	vec_t	numer;
	vec_t	denom;
	vec_t	time;

	VectorCopy(temp2, plane);
	VectorScaleInv(temp2, plane[3]);

	VectorSub(line1, temp2, temp1);
	VectorSub(line1, line2, temp2);

	numer=VectorDotProduct(plane, temp1);
	denom=VectorDotProduct(plane, temp2);

	if(denom==0) {
		return -1;
	}

	time=numer/denom;

	res[0]=line1[0]+((line2[0]-line1[0])*time);
	res[1]=line1[1]+((line2[1]-line1[1])*time);
	res[2]=line1[2]+((line2[2]-line1[2])*time);

	return time;
}

//
// SceneVector
//
bsp_t *SceneVector(bsp_t *src, vec3_t vec) {

	vec_t	 dis;

	while(!(src->params&PARM_ISLEAF)) {
		if(src->params&PARM_ONAXIS) {
			dis=src->plane[src->maxis]*vec[src->maxis]-src->plane[3];
		}else{
			dis=VectorDotProduct(src->plane, vec)-src->plane[3];
		}

		if(dis>=-BSP_ON_EPSILON) {
			src=src->front;
		}else{
			src=src->back;
		}
	}

	return (src->params&PARM_HOLLOW)?src:NULL;
}

//
// SceneLine
//
int SceneLine(bsp_t *src, vec3_t org, vec3_t end) {

	vec2_t	dis;
	vec3_t	mid;

	while(!(src->params&PARM_ISLEAF)) {
		if(src->params&PARM_ONAXIS) {
			dis[0]=src->plane[src->maxis]*org[src->maxis]-src->plane[3];
			dis[1]=src->plane[src->maxis]*end[src->maxis]-src->plane[3];
		}else{
			dis[0]=VectorDotProduct(src->plane, org)-src->plane[3];
			dis[1]=VectorDotProduct(src->plane, end)-src->plane[3];
		}

		if(dis[0]>=-BSP_ON_EPSILON && dis[1]>=-BSP_ON_EPSILON) {
			src=src->front;
		}else
		if(dis[0]<=+BSP_ON_EPSILON && dis[1]<=+BSP_ON_EPSILON) {
			src=src->back;
		}else{
			SceneLinePlane(org, end, src->plane, mid);

			if(dis[1]>=0) {
				if(!SceneLine(src->front, end, mid)) return 0;
				return SceneLine(src->back, mid, org);
			}else{
				if(!SceneLine(src->front, org, mid)) return 0;
				return SceneLine(src->back, mid, end);
			}
		}
	}

	return (src->params&PARM_HOLLOW)?1:0;
}

//
// SceneDestroyTree
//
void SceneDestroyTree(bsp_t *node) {

	if(node==NULL) {
		return;
	}

	SceneDestroyTree(node->front);
	SceneDestroyTree(node->back);

	while(node->faces) {
		free(FacePop(&node->faces));
	}

	free(node);

	return;
}