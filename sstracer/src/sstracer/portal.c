#include "portal.h"

//
// PortalFindIntersecting
//
face_t *PortalFindIntersecting(bsp_t *node, vec3_t a, vec3_t b) {

	face_t	*face;
	vec3_t	res;
	vec3_t	dir;

	VectorSub(b, a, dir);
	VectorNormalize(dir);

	for(face=node->faces; face; face=face->next) {
		if(face->params&PARM_PORTAL) {
			if(VectorDotProduct(face->plane, dir)<0.0F) continue;
			SceneLinePlaneIntersection(face->plane, a, b, res);
			if(SceneVectorInFace(face, res)) return face;
		}
	}

	return NULL;
}

//
// PortalLineIntersection
//
int PortalLineIntersection(bsp_t *bsp, vec3_t a, vec3_t b) {

	bsp_t	*node;
	face_t	*face;

	node=SceneVectorIntersection(bsp, a);

	do {
		face=PortalFindIntersecting(node, a, b);
		if(face) node=face->portal;
	}while(face);
	
	return (SceneVectorIntersection(bsp, b)==node)?0:1;
}