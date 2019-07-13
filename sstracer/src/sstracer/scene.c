#include "scene.h"

//
// SceneTriangleBarycentric
//
vec_t SceneTriangleBarycentric(vec3_t a, vec3_t b, vec3_t c) {

	vec3_t	b2;
	vec3_t	c2;
	vec3_t	d2;

	b2[0]=b[0]-a[0];
	b2[1]=b[1]-a[1];
	b2[2]=b[2]-a[2];

	c2[0]=c[0]-a[0];
	c2[1]=c[1]-a[1];
	c2[2]=c[2]-a[2];

	// inlined cross product (3d)
	d2[0]=b2[1]*c2[2]-b2[2]*c2[1];
	d2[1]=b2[2]*c2[0]-b2[0]*c2[2];
	d2[2]=b2[0]*c2[1]-b2[1]*c2[0];

	// works for a relative triangle area
	return sqrt(VectorDotProduct(d2, d2));
}

//
// SceneVectorInTriangle
//
int SceneVectorInTriangle(vec3_t a[], int i, int k, int m, vec3_t d) {

	static vec_t ermr=0.0F;

	vec_t	b[4];
	
	// only loads error margin once
	if(ermr==0.0F) {
		ermr=1.0F+DataReadPrecision("rad_centricerror");
	}

	b[0]=SceneTriangleBarycentric(a[i], a[k], a[m]);
	b[1]=SceneTriangleBarycentric(a[k], a[m], d)/b[0];
	b[2]=SceneTriangleBarycentric(a[i], a[m], d)/b[0];
	b[3]=SceneTriangleBarycentric(a[i], a[k], d)/b[0];

	return ((b[1]+b[2]+b[3])<=ermr);
}

//
// SceneVectorInFace
//
int SceneVectorInFace(face_t *face, vec3_t vec) {

	int		i;

	for(i=1; i<face->numverts-1; i++)
		if(SceneVectorInTriangle(face->verts, 0, i, i+1, vec))
			break;

	return (i!=face->numverts-1);
}

//
// SceneLinePlaneIntersection
//
vec_t SceneLinePlaneIntersection(vec3_t line1, vec3_t line2, plane_t plane, vec3_t res) {

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
// SceneVectorIntersection
//
bsp_t *SceneVectorIntersection(bsp_t *src, vec3_t vec) {

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
// SceneLineIntersection
//
int SceneLineIntersection(bsp_t *src, vec3_t org, vec3_t end) {

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
			SceneLinePlaneIntersection(org, end, src->plane, mid);

			if(dis[1]>=0) {
				if(!SceneLineIntersection(src->front, end, mid)) return 0;
				return SceneLineIntersection(src->back, mid, org);
			}else{
				if(!SceneLineIntersection(src->front, org, mid)) return 0;
				return SceneLineIntersection(src->back, mid, end);
			}
		}
	}

	return (src->params&PARM_HOLLOW)?1:0;
}

//
// SceneDestroy
//
void SceneDestroy(bsp_t *node) {

	if(node==NULL) {
		return;
	}

	SceneDestroy(node->front);
	SceneDestroy(node->back);

	free(node->faces);
	free(node);

	return;
}