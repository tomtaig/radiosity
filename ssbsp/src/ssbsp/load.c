#include "load.h"

//
// Token cache
char token[1024];

//
// LoadToken
//
char *LoadToken(FILE *fp) {

	fscanf(fp, "%s", token);

	return token;
}

//
// LoadPVertex
//
void LoadPVertex(FILE *ins, vec3_t v, vec2_t s) {

	int		n[3];
	int		t[2];

	while(strcmp(LoadToken(ins), "vertex"));
	fscanf(ins, "%s", token);
	fscanf(ins, "%d %d %d", &n[0], &n[1], &n[2]);
	fscanf(ins, "%d %d \n", &t[0], &t[1]);

	v[0]=(vec_t)n[0];
	v[1]=(vec_t)n[1];
	v[2]=(vec_t)n[2];

	s[0]=(vec_t)t[0];
	s[1]=(vec_t)t[1];

	return;
}

//
// LoadVertex
//
void LoadVertex(FILE *ins, vec3_t v) {

	int		n[3];

	while(strcmp(LoadToken(ins), "vertex"));
	fscanf(ins, "%s", token);
	fscanf(ins, "%d %d %d", &n[0], &n[1], &n[2]);

	v[0]=(vec_t)n[0];
	v[1]=(vec_t)n[1];
	v[2]=(vec_t)n[2];

	return;
}

//
// LoadFaceParams
//
void LoadFaceParams(face_t *face) {

	face->params=PARM_NULL;

	if(fabs(face->plane[0])==1.0F || fabs(face->plane[1])==1.0F || fabs(face->plane[2])==1.0F) {
		face->params|=PARM_ONAXIS;
	}

	return;
}

//
// LoadFaceNormal
//
void LoadFaceNormal(face_t *face) {

	vec3_t	a;
	vec3_t	b;

	VectorSub(face->verts[1], face->verts[0], a);
	VectorSub(face->verts[2], face->verts[0], b);
	VectorCrossProduct(a, b, face->plane);
	VectorNormalize(face->plane);

	face->plane[0]=-face->plane[0];
	face->plane[1]=-face->plane[1];
	face->plane[2]=-face->plane[2];

	face->plane[3]=VectorDotProduct(face->plane, face->verts[0]);

	return;
}

//
// LoadPGroupCoords
//
void LoadPGroupCoords(int nx, int ny, int x, int y, vec3_t pcrd[], vec3_t real) {

	vec3_t	stepx;
	vec3_t	stepy;

	VectorSub(pcrd[1], pcrd[0], stepx);
	VectorScale(stepx, nx);
	VectorScaleInv(stepx, x);

	VectorSub(pcrd[2], pcrd[1], stepy);
	VectorScale(stepy, ny);
	VectorScaleInv(stepy, y);

	VectorAdd(pcrd[0], stepx, real);
	VectorAdd(real, stepy, real);

	return;
}

//
// LoadPGroup
//
int LoadPGroup(FILE *fp, face_t **faces, int index, int hull) {

	face_t	facecache[128];
	vec3_t	verts[4];
	int		i;
	int		nx;
	int		ny;

	do{
		LoadToken(fp);
	}while(strcmp(token, "{"));

	for(i=0; i<4; i++) {
		LoadVertex(fp, verts[i]);
	}

	while(strcmp(LoadToken(fp), "numx"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &nx);

	while(strcmp(LoadToken(fp), "numy"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &ny);

	for(i=0; i<nx*ny; i++) {
		LoadPGroupCoords(nx, ny, i%nx  , i/nx  , verts, facecache[i].verts[0]);
		LoadPGroupCoords(nx, ny, i%nx+1, i/nx  , verts, facecache[i].verts[1]);
		LoadPGroupCoords(nx, ny, i%nx+1, i/nx+1, verts, facecache[i].verts[2]);
		LoadPGroupCoords(nx, ny, i%nx  , i/nx+1, verts, facecache[i].verts[3]);
		
		facecache[i].terts[0][1]=0;	    facecache[i].terts[0][0]=0;
		facecache[i].terts[1][1]=0;	    facecache[i].terts[1][0]=16383;
		facecache[i].terts[2][1]=16383;	facecache[i].terts[2][0]=16383;
		facecache[i].terts[3][1]=16383;	facecache[i].terts[3][0]=0;

		facecache[i].index=index+i;
		facecache[i].numverts=4;
		facecache[i].hullid=hull;

		LoadFaceNormal(&facecache[i]);
		
		facecache[i].maxis=VectorMajor(facecache[i].plane);

		LoadFaceParams(&facecache[i]);
		FaceBounds(&facecache[i]);
	}

	for(i=0; i<nx*ny; i++) {
		while(strcmp(LoadToken(fp), "texture"));
		fscanf(fp, "%s", token);
		fscanf(fp, "%s\n", facecache[i].texxer);
	}

	do{
		LoadToken(fp);
	}while(strcmp(token, "}"));

	for(i=0; i<nx*ny; i++) {
		FacePush(faces, FaceCopy(&facecache[i]));
	}

	return nx*ny;
}

//
// LoadFace
//
int LoadFace(FILE *fp, face_t **faces, int index, int hull) {

	face_t	*face;
	int		i;

	face=malloc(sizeof(face_t));
	FacePush(faces, face);

	do{
		LoadToken(fp);
	}while(strcmp(token, "{"));

	while(strcmp(LoadToken(fp), "numverts"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d", &face->numverts);

	for(i=0; i<face->numverts; i++) {
		LoadPVertex(fp, face->verts[i], face->terts[i]);
	}

	while(strcmp(LoadToken(fp), "texture"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%s\n", &face->texxer);

	do{
		LoadToken(fp);
	}while(strcmp(token, "}"));

	LoadFaceNormal(face);
	LoadFaceParams(face);
	FaceBounds(face);

	face->maxis=VectorMajor(face->plane);
	face->index=index;
	face->hullid=hull;
	
	return 1;
}

//
// LoadScene
//
int LoadScene(char *name, face_t **faces) {

	FILE	*ins;
	int		ind;
	int		hul;

	ins=fopen(name, "r");

	if(ins==NULL) {
		return 0;
	}

	*faces=NULL;

	for(ind=hul=0; !feof(ins);) {
		LoadToken(ins);

		if(!strcmp(token, "#ohull") || !strcmp(token, "#ihull")) {
			hul++;
		}else
		if(!strcmp(token, "#poly")) {
			ind+=LoadFace(ins, faces, ind, hul);
		}else
		if(!strcmp(token, "#pgroup")) {
			ind+=LoadPGroup(ins, faces, ind, hul);
		}
	}

	fclose(ins);

	return 1;
}