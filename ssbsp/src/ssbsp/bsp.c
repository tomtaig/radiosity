#include "bsp.h"

//
// bsp statistics
int		bspc_curdp=0;
int		bspc_faces=0;
int		bspc_nodes=0;
int		bspc_leafs=0;
int		bspc_depth=0;
int		bspc_backs=0;
int		bspc_fronts=0;
int		bspc_splits=0;

//
// bsp unique index
int		bspc_index=0;

//
// error margin
vec_t	bspc_epsilon=0.0F;

//
// BspAllocateNode
//
bsp_t *BspAllocateNode(void) {

	bsp_t	*node;

	node=calloc(1, sizeof(bsp_t));
	node->index=bspc_index++;

	node->front=NULL;
	node->back=NULL;
	node->faces=NULL;
	node->params=PARM_NULL;
	node->face=NULL;
	node->parent=NULL;
	
	return node;
}

//
// BspAxisPlaneSide
//
void BspAxisPlaneSide(plane_t plane, int axis, face_t *face, int sides[]) {

	int		i;
	vec_t	r;

	sides[0]=0;
	sides[1]=0;

	// early out using bounding sphere
	r=face->center[axis]*plane[axis]-plane[3];

	if(r>=+face->radius) {
		sides[0]=face->numverts;
		return;
	}

	if(r<=-face->radius) {
		sides[1]=face->numverts;
		return;
	}

	// side[0] is front side, side[1] is back side
	for(i=0; i<face->numverts; i++) {
		r=face->verts[i][axis]*plane[axis]-plane[3];
		if(r>=-bspc_epsilon) sides[0]++;
		if(r<=+bspc_epsilon) sides[1]++;
	}

	return;
}

//
// BspPlaneSide
//
void BspPlaneSide(plane_t plane, face_t *face, int sides[]) {

	int		i;
	vec_t	r;

	sides[0]=0;
	sides[1]=0;

	// early out using bounding sphere
	r=VectorDotProductInline(plane, face->center)-plane[3];

	if(r>=+face->radius) {
		sides[0]=face->numverts;
		return;
	}

	if(r<=-face->radius) {
		sides[1]=face->numverts;
		return;
	}

	// side[0] is front side, side[1] is back side
	for(i=0; i<face->numverts; i++) {
		r=VectorDotProductInline(plane, face->verts[i])-plane[3];
		if(r>=-bspc_epsilon) sides[0]++;
		if(r<=+bspc_epsilon) sides[1]++;
	}

	return;
}

//
// BspFindSplits
//
int BspFindSplits(face_t *faces, face_t *sp) {

	face_t	*face;
	int		side[2];
	int		numsplits;
	
	numsplits=0;
	
	for(face=faces; face; face=face->next) {
		if(sp->params&PARM_ONAXIS) {
			BspAxisPlaneSide(sp->plane, sp->maxis, face, side);
		}else{
			BspPlaneSide(sp->plane, face, side);
		}

		if(side[0]!=face->numverts && side[1]!=face->numverts) {
			numsplits++;
		}
	}

	return numsplits;
}

//
// BspFindPlane
//
void BspFindPlane(bsp_t *src, face_t *faces) {

	face_t	*face;
	face_t	*parp;
	vec_t	lead;
	vec_t	rate;

	lead=bspc_faces;
	parp=NULL;

	for(face=faces; face; face=face->next) {
		// dont bother rating used faces
		if(face->params&PARM_ISUSED) {
			continue;
		}

		// this squares the compile time
		rate=BspFindSplits(faces, face);

		// axial planes are good (tm)
		if(face->params&PARM_ONAXIS) {
			rate/=2;
		}

		// best splitter/face so far
		if(rate<lead) {
			parp=face;
			lead=rate;
		}

		// it doesnt get any better
		if(lead==0) {
			break;
		}
	}

	// check if there was a valid splitter
	if(parp==NULL || parp->params&PARM_ISUSED) {
		src->params|=PARM_ISLEAF;
		return;
	}
	
	// use the splitter
	parp->params|=PARM_ISUSED;

	// set the pointer
	src->face=parp;

	// set the nodes plane
	VectorCopy(src->plane, parp->plane);
	src->plane[3]=parp->plane[3];

	// copy over needed info
	if(parp->params&PARM_ONAXIS) {
		src->params|=PARM_ONAXIS;
	}

	src->maxis=parp->maxis;

	return;
}

//
// BspBuildTree
//
void BspBuildTree(bsp_t *src) {

	face_t	*face;
	int		side[2];

	bspc_nodes++;
		
	BspFindPlane(src, src->faces);

	// BspFindPlane determines leaf param
	if((src->params&PARM_ISLEAF)) {
		bspc_leafs++;
		return;
	}

	// bsp depth ignores leaf nodes
	if(bspc_depth<bspc_curdp) {
		bspc_depth=bspc_curdp;
	}

	// create front node
	src->front=BspAllocateNode();
	src->front->parent=src;

	// create back node
	src->back=BspAllocateNode();
	src->back->parent=src;
	
	while(src->faces) {
		face=FacePop(&src->faces);

		if(src->params&PARM_ONAXIS) {
			BspAxisPlaneSide(src->plane, src->maxis, face, side);
		}else{
			BspPlaneSide(src->plane, face, side);
		}
	
		if(side[0]==face->numverts && side[1]==face->numverts && VectorDotProduct(face->plane, src->plane)==1.0F) {
			face->params|=PARM_ISUSED;
			FacePush(&src->front->faces, face);
			bspc_backs++;
		}else
		if(side[0]==face->numverts) {
			FacePush(&src->front->faces, face);
			bspc_fronts++;
		}else
		if(side[1]==face->numverts) {
			FacePush(&src->back->faces, face);
			bspc_backs++;
		}else{
			FacePush(&src->front->faces, FaceCopy(face));
			FacePush(&src->back->faces, face);

			// face clip must be careful with face data
			FaceClip(face, src->plane, src->front->faces, src->back->faces);
			FaceBounds(src->front->faces);
			FaceBounds(src->back->faces);

			bspc_splits++;
		}
	}

	// build the sub-nodes
	bspc_curdp++;
	BspBuildTree(src->front);
	BspBuildTree(src->back);
	bspc_curdp--;

	// mark non-solid leafs
	if(src->front->params&PARM_ISLEAF) {
		src->front->params|=PARM_HOLLOW;
	}

	return;
}

//
// BspCompileTree
//
bsp_t *BspCompileTree(face_t *faces) {
	
	bsp_t	*node;
	face_t	*temp;

	printf("---- Solid BSP Tree ----\n");

	// reset statistics
	bspc_fronts=0;
	bspc_splits=0;
	bspc_faces=0;
	bspc_nodes=0;
	bspc_leafs=0;
	bspc_depth=0;
	bspc_backs=0;

	// allocate the root node
	node=BspAllocateNode();
	node->faces=faces;
	
	// error margin
	bspc_epsilon=DataReadPrecision("bsp_epsilon");

	// initialise faces
	for(temp=faces; temp; temp=temp->next) {
		temp->params=(temp->params|PARM_ISUSED)^PARM_ISUSED;
	}

	// count faces
	for(temp=faces; temp; temp=temp->next) {
		bspc_faces++;
	}

	// build the tree
	BspBuildTree(node);

	printf("faces in    : %d\n", bspc_faces);
	printf("total nodes : %d\n", bspc_nodes);
	printf("leaf nodes  : %d\n", bspc_leafs);
	printf("nodes       : %d\n", bspc_nodes-bspc_leafs);
	printf("worst depth : %d\n", bspc_depth);
	printf("front faces : %d\n", bspc_fronts);
	printf("back faces  : %d\n", bspc_backs);
	printf("split faces : %d\n", bspc_splits);
	
	return node;
}

//
// BspDestroyTree
//
void BspDestroyTree(bsp_t *src) {

	if(src==NULL) {
		return;
	}

	BspDestroyTree(src->front);
	BspDestroyTree(src->back);

	while(src->faces) {
		free(FacePop(&src->faces));
	}

	free(src);

	return;
}

//
// BspWriteName
// 
void BspWriteName(FILE *ons, char *name) {

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
// BspWriteFace
//
void BspWriteFace(FILE *ons, face_t *face) {

	int		i;

	FaceOptimiseVerts(face);

	fprintf(ons, "\n");
	fprintf(ons, "#poly\n");
	fprintf(ons, "{\n");

	BspWriteName(ons, "plane");
	fprintf(ons, "%f %f %f %f\n", face->plane[0], face->plane[1], face->plane[2], face->plane[3]);

	BspWriteName(ons, "numverts");
	fprintf(ons, "%d\n", face->numverts);

	for(i=0; i<face->numverts; i++) {
		BspWriteName(ons, "vertex");
		fprintf(ons, "%f %f %f %f %f\n", face->verts[i][0], face->verts[i][1], face->verts[i][2], face->terts[i][0], face->terts[i][1]);
	}

	BspWriteName(ons, "m_axis");
	fprintf(ons, "%d\n", face->maxis);

	BspWriteName(ons, "params");
	fprintf(ons, "%d\n", face->params);

	BspWriteName(ons, "index");
	fprintf(ons, "%d\n", face->index);

	BspWriteName(ons, "texture");
	fprintf(ons, "%s\n", face->texxer);

	fprintf(ons, "}\n");

	return;
}

//
// BspWriteNodeArray
//
void BspWriteNodeArray(FILE *ons, bsp_t *node) {
	
	face_t	*face;

	if(node==NULL) {
		return;
	}

	fprintf(ons, "\n");
	fprintf(ons, "#node\n");
	fprintf(ons, "{\n");

	BspWriteName(ons, "plane");
	fprintf(ons, "%f %f %f %f\n", node->plane[0], node->plane[1], node->plane[2], node->plane[3]);

	BspWriteName(ons, "m_axis");
	fprintf(ons, "%d\n", node->maxis);

	BspWriteName(ons, "params");
	fprintf(ons, "%d\n", node->params);

	BspWriteName(ons, "index");
	fprintf(ons, "%d\n", node->index);

	// write nested faces
	for(face=node->faces; face; face=face->next) {
		BspWriteFace(ons, face);
	}

	// write nested nodes
	BspWriteNodeArray(ons, node->front);
	BspWriteNodeArray(ons, node->back);

	fprintf(ons, "}\n");

	return;
}

//
// BspWriteFile
//
void BspWriteFile(char *filename, bsp_t *base) {

	FILE	*ons;

	ons=fopen(filename, "w");

	if(ons==NULL) {
		return;
	}

	BspWriteNodeArray(ons, base);

	fclose(ons);

	return;
}