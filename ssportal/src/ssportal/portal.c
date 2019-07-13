#include "portal.h"

//
// portal-building cache
face_t	g_portals[1024];

//
// PortalClipFaceBSide
//
void PortalClipFaceBSide(face_t *face, plane_t plane) {

	face_t	crap;
	int		sides[2];

	FaceOnPlaneSide(plane, face, sides);

	if(sides[0]!=face->numverts && sides[1]!=face->numverts) {
		FaceClip(face, plane, &crap, face);
	}

	return;
}

//
// PortalClipFaceFSide
//
void PortalClipFaceFSide(face_t *face, plane_t plane) {

	face_t	crap;
	int		sides[2];

	FaceOnPlaneSide(plane, face, sides);

	if(sides[0]!=face->numverts && sides[1]!=face->numverts) {
		FaceClip(face, plane, face, &crap);
	}
	
	return;
}

//
// PortalDecal
//
void PortalDecal(face_t *decal, plane_t plane) {

	vec3_t	uax;
	vec3_t	rax;
	vec3_t	mar;
	vec3_t	cpv;
	int		com;

	VectorClear(mar);

	com=VectorMajor(plane);
	if(fabs(plane[com])>=0.999F) {
		com=(com+1)%3;
	}
	
	mar[com]=1.0F;

	VectorCopy(cpv, plane);
	VectorScaleInv(cpv, plane[3]);

	VectorCopy(rax, plane);
	VectorScaleInv(rax, plane[3]-plane[com]);
	VectorAdd(rax, mar, rax);
	VectorSub(rax, cpv, rax);
	VectorNormalize(rax);

	VectorCrossProduct(rax, plane, uax);

	// make this a variable or something
	VectorScaleInv(uax, 10000000);
	VectorScaleInv(rax, 10000000);

	VectorCopy(decal->verts[0], cpv);
	VectorAdd(decal->verts[0], uax, decal->verts[0]);
	VectorAdd(decal->verts[0], rax, decal->verts[0]);

	VectorCopy(decal->verts[1], cpv);
	VectorSub(decal->verts[1], uax, decal->verts[1]);
	VectorAdd(decal->verts[1], rax, decal->verts[1]);

	VectorCopy(decal->verts[2], cpv);
	VectorSub(decal->verts[2], uax, decal->verts[2]);
	VectorSub(decal->verts[2], rax, decal->verts[2]);

	VectorCopy(decal->verts[3], cpv);
	VectorAdd(decal->verts[3], uax, decal->verts[3]);
	VectorSub(decal->verts[3], rax, decal->verts[3]);

	VectorCopy(decal->plane, plane);
	decal->plane[3]=plane[3];

	decal->numverts=4;
	decal->params|=PARM_ISUSED;

	return;
}

//
// PortalBoundingCube
//
int PortalBoundingCube(void) {

	face_t	*face;

	face=&g_portals[0];

	face->maxis=0;
	face->plane[0]=1;
	face->plane[1]=0;
	face->plane[2]=0;
	face->plane[3]=-10000;
	PortalDecal(face, face->plane);

	face=&g_portals[1];

	face->maxis=0;
	face->plane[0]=-1;
	face->plane[1]=0;
	face->plane[2]=0;
	face->plane[3]=-10000;
	PortalDecal(face, face->plane);

	face=&g_portals[2];

	face->maxis=1;
	face->plane[0]=0;
	face->plane[1]=1;
	face->plane[2]=0;
	face->plane[3]=-10000;
	PortalDecal(face, face->plane);

	face=&g_portals[3];

	face->maxis=1;
	face->plane[0]=0;
	face->plane[1]=-1;
	face->plane[2]=0;
	face->plane[3]=-10000;
	PortalDecal(face, face->plane);

	face=&g_portals[4];

	face->maxis=2;
	face->plane[0]=0;
	face->plane[1]=0;
	face->plane[2]=-1;
	face->plane[3]=-10000;
	PortalDecal(face, face->plane);

	face=&g_portals[5];

	face->maxis=2;
	face->plane[0]=0;
	face->plane[1]=0;
	face->plane[2]=1;
	face->plane[3]=-10000;
	PortalDecal(face, face->plane);

	return 6;
}

//
// PortalNodeAverage
//
void PortalNodeAverage(bsp_t *node, vec3_t org) {

	face_t	*face;
	int		n;
	int		i;

	VectorClear(org);

	for(n=0, face=node->faces; face; face=face->next) {
		for(i=0; i<face->numverts; i++, n++) VectorAdd(face->verts[i], org, org);
	}

	VectorScale(org, (vec_t)n);

	return;
}

//
// PortalFaceForPlane
//
void PortalFaceForPlane(plane_t plane, int numplanes) {

	int		i;

	PortalDecal(&g_portals[numplanes], plane);

	for(i=0; i<numplanes; i++) {
		PortalClipFaceFSide(&g_portals[numplanes], g_portals[i].plane);
	}

	return;
}

//
// PortalAddPlane
//
int PortalAddPlane(plane_t p, vec3_t center, int numplanes) {

	plane_t		plane;
	int			i;
	
	VectorCopy4(plane, p);

	// flip the plane to be pointing out from the hull
	if((VectorDotProduct(plane, center)-plane[3])<0.0F) {
		VectorMinus4(plane);
	}

	// clip existing hull
	for(i=0; i<numplanes; i++) {
		PortalClipFaceFSide(&g_portals[i], plane);
	}
	
	// create the new planes polygon
	PortalFaceForPlane(plane, numplanes);

	return numplanes+1;
}

//
// PortalFaceUsed
//
void PortalFaceUsed(face_t *faces, face_t *used, int numplanes) {

	int		sides[2];
	int		i;

	// flag any degenerate polys
	if(used->numverts==0 || FaceArea(used)<=0.1F) {
		used->params=PARM_NULL;
	}

	// flag any plane/poly that we do not need in the final hull
	for(i=0; i<numplanes; i++) {
		if(&g_portals[i]==used || !(g_portals[i].params&PARM_ISUSED)) continue;
		FaceOnPlaneSide(g_portals[i].plane, used, sides);
		if(sides[1]==used->numverts) used->params=PARM_NULL;
	}

	return;
}

//
// PortalCopyToFaces
//
int PortalCopyToFaces(face_t **faces, bsp_t *node, int index) {

	if(!(g_portals[index].params&PARM_ISUSED)) {
		return 0;
	}

	FaceExtendedInfo(&g_portals[index]);

	g_portals[index].node=node;

	FacePush(faces, FaceCopy(&g_portals[index]));

	return 1;
}

//
// PortalCalculateHull
//
int PortalCalculateHull(bsp_t *node, face_t **faces) {

	int		numplanes;
	vec3_t	center;
	int		i;
	int		n;

	// calculate a cube
	numplanes=PortalBoundingCube();

	// hulls barycentric center
	PortalNodeAverage(node, center);
	
	// chop it down to wrap the hull
	for(node=node->parent; node; node=node->parent) {
		numplanes=PortalAddPlane(node->plane, center, numplanes);
	}

	// remove all the degenerates
	for(i=0; i<numplanes; i++) {
		PortalFaceUsed(g_portals, &g_portals[i], numplanes);
	}

	// copy over the non degenerates
	for(n=0, i=0; i<numplanes; i++) {
		n+=PortalCopyToFaces(faces, node, i);
	}

	return n;
}

//
// PortalFindConnector
//
face_t *PortalFindConnector(face_t **portals, face_t *faces, face_t *face, bsp_t *connection) {

	face_t	*a;

	// if i can guarantee one portal per node pair, i dont need to keep reallocating
	FacePush(portals, FaceCopy(face));

	(*portals)->numverts=0;

	// csg intersection
	for(a=faces; a; a=a->next) {
		if(!(a->params&PARM_PORTAL) && FaceIntersection(face, a, *portals)) goto success;
	}

	free(FacePop(portals));

	return NULL;

success:
	(*portals)->params|=PARM_PORTAL;
	(*portals)->portal=connection;

	FaceOptimiseVerts(*portals);

	return *portals;
}

//
// PortalFromNodes
//
void PortalFromNodes(bsp_t *node1, bsp_t *node2) {

	face_t	*skin1=NULL;
	face_t	*skin2=NULL;
	face_t	*face;
	
	if(!node1 || !node2 || node1==node2) {
		return;
	}

	// calculate a "skin" for the bsp leaf nodes
	PortalCalculateHull(node1, &skin1);
	PortalCalculateHull(node2, &skin2);

	// find the portals for node1
	for(face=skin1; face; face=face->next) {
		PortalFindConnector(&node1->faces, skin2, face, node2);
	}

	// find the portals for node2
	for(face=skin2; face; face=face->next) {
		PortalFindConnector(&node2->faces, skin1, face, node1);
	}
	
	// we now have portals, so free the skins
	while(skin1) {
		free(FacePop(&skin1));
	}

	while(skin2) {
		free(FacePop(&skin2));
	}

	return;
}

//
// PortalWriteName
// 
void PortalWriteName(FILE *ons, char *name) {

	int		i;

	fprintf(ons, "%s", name);

	// shut up compiler
	for(i=0; i<(10-(int)strlen(name)); i++) {
		fprintf(ons, " ");
	}

	fprintf(ons, ": ");

	return;
}

//
// PortalWrite
//
void PortalWrite(FILE *ons, face_t *face) {

	int		i;

	FaceOptimiseVerts(face);

	fprintf(ons, "\n");
	fprintf(ons, "#poly\n");
	fprintf(ons, "{\n");

	PortalWriteName(ons, "plane");
	fprintf(ons, "%f %f %f %f\n", face->plane[0], face->plane[1], face->plane[2], face->plane[3]);

	PortalWriteName(ons, "numverts");
	fprintf(ons, "%d\n", face->numverts);

	for(i=0; i<face->numverts; i++) {
		PortalWriteName(ons, "vertex");
		fprintf(ons, "%f %f %f %f %f\n", face->verts[i][0], face->verts[i][1], face->verts[i][2], face->terts[i][0], face->terts[i][1]);
	}

	PortalWriteName(ons, "m_axis");
	fprintf(ons, "%d\n", face->maxis);

	PortalWriteName(ons, "params");
	fprintf(ons, "%d\n", face->params);

	PortalWriteName(ons, "index");
	fprintf(ons, "%d\n", face->index);

	PortalWriteName(ons, "node");
	fprintf(ons, "%d\n", face->portal->index);

	fprintf(ons, "}\n");

	return;
}

//
// PortalWriteFile
//
void PortalWriteFile(char *str, face_t *faces) {

	FILE	*ons;
	face_t	*por;

	ons=fopen(str, "w");

	for(por=faces; por; por=por->next) {
		PortalWrite(ons, por);
	}

	fclose(ons);
	
	return;
}
