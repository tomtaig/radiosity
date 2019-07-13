#include "csg.h"

//
// CsgPlaneSide
//
void CsgPlaneSide(plane_t plane, face_t *face, int sides[]) {

	int		i;
	vec_t	r;

	sides[0]=0;
	sides[1]=0;

	// side[0] is front side, side[1] is back side
	for(i=0; i<face->numverts; i++) {
		r=VectorDotProductInline(plane, face->verts[i])-plane[3];
		if(r>=-0.05F) sides[0]++;
		if(r<=+0.05F) sides[1]++;
	}

	return;
}

//
// CsgBuildSkins
//
void CsgBuildSkins(face_t *scene, face_t **skin, int id) {

	for(; scene; scene=scene->next) {
		if(scene->hullid==id) FacePush(skin, FaceCopy(scene));
	}

	return;
}

//
// CsgClipFace
//
void CsgClipFace(bsp_t *node, face_t *face, face_t **res) {

	int		side[2];
	face_t	crap;

	if(node==NULL) {
		return;
	}

	if(node->params&PARM_ISLEAF && node->params&PARM_HOLLOW) {
		FacePush(res, FaceCopy(face));
		return;
	}

	CsgPlaneSide(node->plane, face, side);

	if(side[0]!=face->numverts && side[1]!=face->numverts) {
		FaceClip(face, node->plane, face, &crap);
		CsgClipFace(node->front, face, res);
		CsgClipFace(node->back, &crap, res);
	}else
	if(side[0]==face->numverts && side[1]==face->numverts) {
		CsgClipFace(node->back, face, res);
	}else
	if(side[1]==face->numverts) {
		CsgClipFace(node->back, face, res);
	}else
	if(side[0]==face->numverts) {
		CsgClipFace(node->front, face, res);
	}

	return;
}

//
// CsgSubtraction
//
void CsgSubtraction(face_t *faces, int id, int clipper, face_t **res) {

	face_t	*skin1=NULL;
	face_t	*skin2=NULL;
	bsp_t	*nodes=NULL;
	face_t	*face;

	CsgBuildSkins(faces, &skin1, id);
	CsgBuildSkins(faces, &skin2, clipper);

	nodes=BspCompileTree(skin2);

	for(face=skin1; face; face=face->next) {
		CsgClipFace(nodes, face, res);
	}

	BspDestroyTree(nodes);

	return;
}