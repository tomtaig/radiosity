#include "patch.h"

//
// Patch calculation cache
vec_t	g_pvispr[PATCH_XY_SIZE];

//
// PatchXYToXYZQuick
//
void PatchXYToXYZQuick(face_t *face, vec_t x, vec_t y, vec3_t res) {

	res[0]=face->uaxis[0]*x+face->vaxis[0]*y+face->origin[0];
	res[1]=face->uaxis[1]*x+face->vaxis[1]*y+face->origin[1];
	res[2]=face->uaxis[2]*x+face->vaxis[2]*y+face->origin[2];
	
	return;
}

//
// PatchXYToXYZ
//
void PatchXYToXYZ(face_t *face, vec_t x, vec_t y, vec3_t res) {

	vec3_t	a2;
	vec3_t	b2;
	vec3_t	c2;
	vec_t	b[4];

	x/=PATCH_X_SIZE;
	y/=PATCH_Y_SIZE;

	#define x0 x
	#define x1 face->terts[0][0]
	#define x2 face->terts[1][0]
	#define x3 face->terts[face->numverts-1][0]

	#define y0 y
	#define y1 face->terts[0][1]
	#define y2 face->terts[1][1]
	#define y3 face->terts[face->numverts-1][1]
	
	b[0]=(x2-x1)*(y3-y1)-(x3-x1)*(y2-y1);

	b[1]=((x2-x0)*(y3-y0)-(x3-x0)*(y2-y0))/b[0];
	b[2]=((x3-x0)*(y1-y0)-(x1-x0)*(y3-y0))/b[0];
	b[3]=((x1-x0)*(y2-y0)-(x2-x0)*(y1-y0))/b[0];

	VectorCopy(a2, face->verts[0]);
	VectorCopy(b2, face->verts[1]);
	VectorCopy(c2, face->verts[face->numverts-1]);

	VectorScaleInv(a2, b[1]);
	VectorScaleInv(b2, b[2]);
	VectorScaleInv(c2, b[3]);

	VectorAdd(a2, b2, res);
	VectorAdd(res, c2, res);

	#undef x0
	#undef x1
	#undef x2
	#undef x3

	#undef y0
	#undef y1
	#undef y2
	#undef y3
}

//
// PatchCalcVisPool
//
void PatchCalcVisPool(bsp_t *bsp, face_t *src, face_t *dst, int s) {

	vec3_t	n1;
	vec3_t	n2;
	vec2_t	rd;
	vec_t	x;
	vec_t	y;
	int		i;

	x=PATCH_X_SIZE/PATCH_EMIT_X_SIZE;
	y=PATCH_Y_SIZE/PATCH_EMIT_Y_SIZE;

	VectorScale(dst->plane, sqrt(dst->tarea));
	VectorScale(src->plane, sqrt(src->tarea));

	for(i=0; i<PATCH_XY_SIZE; i++) {
		if(!(dst->patches[i].params&PARM_ISUSED)) continue;

		rd[0]=(vec_t)(rand()%1000)/1000.0F*x;
		rd[1]=(vec_t)(rand()%1000)/1000.0F*y;
		PatchXYToXYZQuick(src, (s%PATCH_X_SIZE)+rd[0], (s/PATCH_X_SIZE)+rd[1], n2);

		rd[0]=(vec_t)(rand()%1000)/1000.0F;
		rd[1]=(vec_t)(rand()%1000)/1000.0F;
		PatchXYToXYZQuick(dst, (i%PATCH_X_SIZE)+rd[0], (i/PATCH_X_SIZE)+rd[1], n1);

		VectorAdd(n2, src->plane, n2);
		VectorAdd(n1, dst->plane, n1);

		g_pvispr[i]+=(vec_t)SceneLineIntersection(bsp, n1, n2);
	}

	VectorScaleInv(dst->plane, sqrt(dst->tarea));
	VectorScaleInv(src->plane, sqrt(src->tarea));

	return;
}

//
// PatchChooseDistributor
//
void PatchChooseDistributor(face_t *face, face_t **brs, int *bri, vec_t *brv) {

	vec_t	a;
	int		i;

	for(i=0; i<PATCH_XY_SIZE; i++) {
		a=(face->patches[i].drad[0]+face->patches[i].drad[1]+face->patches[i].drad[2])/3.0F;
		*bri=(a>=*brv)?i:*bri;
		*brv=(a>=*brv)?a:*brv;
		*brs=(a>=*brv)?face:*brs;
	}

	return;
}

//
// PatchFromFace
//
void PatchFromFace(bsp_t *bsp, face_t *face, int s) {

	int		vviss[4];
	vec3_t	verts[4];
	vec_t	vx;
	vec_t	vy;
	patch_t	*src;

	vx=(vec_t)(s%PATCH_X_SIZE);
	vy=(vec_t)(s/PATCH_X_SIZE);

	src=&face->patches[s];
	src->face=face;

	src->params|=PARM_ISUSED;

	PatchXYToXYZQuick(src->face, vx,   vy,   verts[0]);
	PatchXYToXYZQuick(src->face, vx+1, vy,   verts[1]);
	PatchXYToXYZQuick(src->face, vx+1, vy+1, verts[2]);
	PatchXYToXYZQuick(src->face, vx,   vy+1, verts[3]);

	vviss[0]=SceneVectorInFace(src->face, verts[0]);
	vviss[1]=SceneVectorInFace(src->face, verts[1]);
	vviss[2]=SceneVectorInFace(src->face, verts[2]);
	vviss[3]=SceneVectorInFace(src->face, verts[3]);

	if(vviss[0]+vviss[1]+vviss[2]+vviss[3]==0) {
		src->params=(src->params|PARM_ISUSED)^PARM_ISUSED;
	}

	VectorScaleInv(src->face->plane, sqrt(src->face->tarea));

	VectorAdd(verts[0], src->face->plane, verts[0]);
	VectorAdd(verts[1], src->face->plane, verts[1]);
	VectorAdd(verts[2], src->face->plane, verts[2]);
	VectorAdd(verts[3], src->face->plane, verts[3]);

	VectorScale(src->face->plane, sqrt(src->face->tarea));

	vviss[0]=(int)SceneVectorIntersection(bsp, verts[0]);
	vviss[1]=(int)SceneVectorIntersection(bsp, verts[1]);
	vviss[2]=(int)SceneVectorIntersection(bsp, verts[2]);
	vviss[3]=(int)SceneVectorIntersection(bsp, verts[3]);

	if(vviss[0]+vviss[1]+vviss[2]+vviss[3]==0) {
		src->params=(src->params|PARM_ISUSED)^PARM_ISUSED;
	}

	if(!(src->params&PARM_ISUSED)) {
		VectorClear(src->rrad);
		VectorClear(src->drad);
	}

	return;
}

//
// PatchPrepareFace
//
void PatchPrepareFace(bsp_t *bsp, face_t *face) {

	vec3_t	verts[4];
	int		i;

	// 3d patch representation
	PatchXYToXYZ(face, 0, 1, verts[0]);
	PatchXYToXYZ(face, 1, 1, verts[1]);
	PatchXYToXYZ(face, 1, 0, verts[2]);
	PatchXYToXYZ(face, 0, 0, verts[3]);

	// copy over axis origin
	VectorCopy(face->origin, verts[3]);

	// calculate u/v axis
	VectorSub(verts[2], face->origin, face->uaxis);
	VectorSub(verts[0], face->origin, face->vaxis);

	VectorCrossProduct(face->uaxis, face->vaxis, verts[0]);

	// texel area from quad
	face->tarea=VectorLength(verts[0]);

	// load patches from ambience map
	TexpackLoad(face->texpak, face->texxer, face->patches, NULL, TEXPACK_LOAD_AMBIENCE);

	// intialise patches
	for(i=0; i<PATCH_XY_SIZE; i++) {
		PatchFromFace(bsp, face, i);
	}

	return;
}

//
// PatchFaceBehind
//
int PatchFaceBehind(face_t *org, face_t *face) {

	int		i;

	// is one face behind the other's plane
	for(i=0; i<face->numverts; i++) {
		if(VectorDotProduct(face->verts[i], org->plane)-org->plane[3]>0.0F) break;
	}

	return (i==face->numverts);
}

//
// PatchDeltaEmmission
//
void PatchDeltaEmmission(face_t *src, int s, vec3_t res) {

	int		x, x2, x3;
	int		y, y2;

	// elements imp. calcs a patches delta emmission
	x=(s%PATCH_X_SIZE);
	y=(s/PATCH_X_SIZE);

	x2=x+(PATCH_X_SIZE/PATCH_EMIT_X_SIZE);
	y2=y+(PATCH_Y_SIZE/PATCH_EMIT_Y_SIZE);

	VectorClear(res);

	for(; y<y2; y++) {
		for(x3=x; x3<x2; x3++) {
			VectorAdd(src->patches[y*PATCH_X_SIZE+x3].drad, res, res);
		}
	}

	VectorScale(res, PATCH_XY_SIZE/PATCH_EMIT_XY_SIZE);

	return;
}

//
// PatchZeroEmmission
//
void PatchZeroEmmission(face_t *src, int s) {

	int		x, x2, x3;
	int		y, y2;

	// elements imp. nulls a patches delta emmission
	x=(s%PATCH_X_SIZE);
	y=(s/PATCH_X_SIZE);

	x2=x+(PATCH_X_SIZE/PATCH_EMIT_X_SIZE);
	y2=y+(PATCH_Y_SIZE/PATCH_EMIT_Y_SIZE);

	for(; y<y2; y++) {
		for(x3=x; x3<x2; x3++) {
			VectorClear(src->patches[y*PATCH_X_SIZE+x3].drad);
		}
	}

	return;
}

//
// PatchFormFactor
//
vec_t PatchFormFactor(patch_t *sp, int s, patch_t *dp, int d, vec3_t sv) {

	vec3_t		cen;
	vec_t		san;
	vec_t		dan;
	vec_t		dsq;
	vec3_t		dv;

	// calculate element center
	PatchXYToXYZQuick(dp->face, (d%PATCH_X_SIZE)+0.5F, (d/PATCH_X_SIZE)+0.5F, dv);

	// find direction and distance between the patches
	VectorSub(dv, sv, cen);
	dsq=VectorDotProduct(cen, cen);
	VectorScale(cen, sqrt(dsq));

	// calculate angles for src and destination
	san=VectorDotProduct(sp->face->plane, cen);
	dan=VectorDotProduct(dp->face->plane, cen);

	// form factor!
	return (san*-dan)/(M_PI*dsq)*g_pvispr[d];
}

//
// PatchTransfer
//
void PatchTransfer(patch_t *sp, int s, patch_t *dp, int d, vec_t area, vec3_t cent) {

	static vec2_t bounds={0.0F,0.0F};

	vec3_t	drad;
	vec_t	form;

	// destination patch used and is visible?
	if(!(dp->params&PARM_ISUSED) || g_pvispr[d]==0.0F) {
		return;
	}

	// form factor error margin bounds
	if(bounds[0]==0.0F) {
		bounds[0]=-DataReadPrecision("rad_formerror");
		bounds[1]=-bounds[0]+1.0F;
	}

	// calculate the form factor
	form=PatchFormFactor(sp, s, dp, d, cent);

	// check if form is in bounds
//	if(form>bounds[1]) {
//		ErrorInvoke(ERROR_FORM_FACTOR_ONE);
//	}else
//	if(form<bounds[0]) {
//		ErrorInvoke(ERROR_FORM_FACTOR_ZERO);
//	}

	// clamp form factor
	form=(form>1.0F)?1.0F:(form<0.0F)?0.0F:form;

	// when form is 0, no need to calculate
	if(form==0.0F) return;

	// scale quantity to area
	form=form*area;

	// calculate drad for grouped patches
	PatchDeltaEmmission(sp->face, s, drad);

	// calculate the energy to be recieved
	drad[0]*=dp->reflection[0]*form;
	drad[1]*=dp->reflection[1]*form;
	drad[2]*=dp->reflection[2]*form;

	// send it to the destination
	VectorAdd(dp->drad, drad, dp->drad);
	VectorAdd(dp->rrad, drad, dp->rrad);

	return;
}

//
// PatchTransferVisibleFace
//
void PatchTransferVisibleFace(patch_t *sp, int s, face_t *face) {

	vec_t	a;
	int		i;
	vec_t	n;
	vec_t	x;
	vec_t	y;
	vec3_t	cent;

	// initialise the vis buffer
	for(i=0; i<PATCH_XY_SIZE; i++) {
		g_pvispr[i]=1.0F;
	}

	// calculate center of patch
	x=(vec_t)(PATCH_X_SIZE/PATCH_EMIT_X_SIZE)/2.0F;
	y=(vec_t)(PATCH_X_SIZE/PATCH_EMIT_X_SIZE)/2.0F;

	PatchXYToXYZQuick(sp->face, (s%PATCH_X_SIZE)+x, (s/PATCH_X_SIZE)+y, cent);

	// an element:patch ratio
	n=PATCH_XY_SIZE/PATCH_EMIT_XY_SIZE;

	// area ratio between src and destination
	a=sp->face->tarea*n;
	a=(a*a)/face->tarea;

	// transfer energy between src and destination
	for(i=0; i<PATCH_XY_SIZE; i++) {
		PatchTransfer(sp, s, &face->patches[i], i, a, cent);
	}

	return;
}

//
// PatchTransferFace
//
void PatchTransferFace(bsp_t *bsp, patch_t *sp, int s, face_t *face) {

	static int d=0;

	vec_t	a;
	int		i;
	vec_t	n;
	vec_t	x;
	vec_t	y;
	vec3_t	cent;

	// read the vispasses
	if(d==0) {
		d=DataReadInteger("rad_vispasses");
	}

	// special case of a face being behind (or coplanar)
	if(VectorDotProduct(sp->face->plane, face->plane)==1.0F) {
		return;
	}

	// source face behind destination face or vice versa?
	if(PatchFaceBehind(sp->face, face) || PatchFaceBehind(face, sp->face)) {
		return;
	}

	// can we do a quick transfer?
	if(sp->face->node==face->node) {
		PatchTransferVisibleFace(sp, s, face);
		return;
	}

	// initialise the vis buffer
	for(i=0; i<PATCH_XY_SIZE; i++) {
		g_pvispr[i]=0.0F;
	}

	// iterate vis calculations
	for(i=0; i<d; i++) {
		PatchCalcVisPool(bsp, sp->face, face, s);
	}

	// divide by vispasses (0.0<vis<1.0)
	for(i=0; i<PATCH_XY_SIZE; i++) {
		g_pvispr[i]/=(vec_t)d;
	}

	// calculate center of patch
	x=(vec_t)(PATCH_X_SIZE/PATCH_EMIT_X_SIZE)/2.0F;
	y=(vec_t)(PATCH_X_SIZE/PATCH_EMIT_X_SIZE)/2.0F;

	PatchXYToXYZQuick(sp->face, (s%PATCH_X_SIZE)+x, (s/PATCH_X_SIZE)+y, cent);

	// an element:patch ratio
	n=PATCH_XY_SIZE/PATCH_EMIT_XY_SIZE;

	// area ratio between src and destination
	a=sp->face->tarea*n;
	a=(a*a)/face->tarea;

	// transfer energy between src and destination
	for(i=0; i<PATCH_XY_SIZE; i++) {
		PatchTransfer(sp, s, &face->patches[i], i, a, cent);
	}

	return;
}

//
// PatchDistribute
//
void PatchDistribute(bsp_t *bsp, face_t *scene, patch_t *sp, int s) {

	face_t	*face;

	// if the source patch isnt visible, dont bother
	if(!(sp->params&PARM_ISUSED)) {
		return;
	}

	// transfer energy to faces
	for(face=scene; face!=sp->face; face=face->next) {
		PatchTransferFace(bsp, sp, s, face);
	}

	// skip source patches face
	face=face->next;

	for(; face; face=face->next) {
		PatchTransferFace(bsp, sp, s, face);
	}

	return;
}

//
// PatchDistributeFace
//
void PatchDistributeFace(bsp_t *bsp, face_t *scene, face_t *src, int index) {

	int		x;
	int		y;
	int		i;

	// snap to lower resolution patches, not elements
	x=(index%PATCH_X_SIZE)/(PATCH_X_SIZE/PATCH_EMIT_X_SIZE)*(PATCH_X_SIZE/PATCH_EMIT_X_SIZE);
	y=(index/PATCH_X_SIZE)/(PATCH_Y_SIZE/PATCH_EMIT_Y_SIZE)*(PATCH_Y_SIZE/PATCH_EMIT_Y_SIZE);

	// calculate snapped position in element grid
	i=(y*PATCH_X_SIZE)+x;

	// distribute source patch to the scene!
	PatchDistribute(bsp, scene, &src->patches[i], i);

	// then zero the delta emmissions of the patches
	PatchZeroEmmission(src, i);

	return;
}

//
// PatchFindAndDistribute
//
int PatchFindAndDistribute(bsp_t *bsp, face_t *faces) {

	static vec_t r=-1.0F;

	face_t	*a;
	face_t	*b;
	int		i;
	vec_t	v;

	// load the lowerbound margin once
	if(r==-1.0F) {
		r=DataReadPrecision("rad_lowerbound");
	}

	// a null pointer will screw up everything
	if(!faces) {
		return 0;
	}

	// find the patch with the most unshot energy
	for(v=0.0F, a=faces; a; a=a->next) {
		PatchChooseDistributor(a, &b, &i, &v);
	}

	// distribute if energy is above lowerbound
	if(v>=r) {
		PatchDistributeFace(bsp, faces, b, i);
	}

	return (v>=r);
}