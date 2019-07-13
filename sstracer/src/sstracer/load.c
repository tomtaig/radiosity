#include "load.h"

//
// Linker for faces
face_t	*prvface;

//
// Binder unique number
int		binder=0;

//
// LoadFaceOptimiseVerts
//
void LoadFaceOptimiseVerts(face_t *face) {

	face_t	a;
	int		i;
	int		n;

	// removes extra vertices after clipping (assumes adjacency)
	VectorCopy(a.verts[0], face->verts[0]);
	VectorCopy2(a.terts[0], face->terts[0]);

	for(i=1, n=1; i<face->numverts; i++) {
		if(VectorDistance(face->verts[i], face->verts[i-1])<0.01F) continue;
		VectorCopy(a.verts[n], face->verts[i]);
		VectorCopy2(a.terts[n], face->terts[i]);
		n++;
	}

	if(VectorDistance(a.verts[i-1], a.verts[0])<0.01F) {
		n--;
	}

	face->numverts=n;
	memcpy(face->verts, a.verts, n*sizeof(vec3_t));
	memcpy(face->terts, a.terts, n*sizeof(vec2_t));

	return;
}

//
// LoadFace
//
face_t *LoadFace(FILE *fp, face_t *face) {

	char	tex[FACE_MAXTEXLEN];
	char	*tp;
	int		i;

	do{
		TokenLoad(fp);
	}while(strcmp(TokenValue(), "{"));

	while(strcmp(TokenLoad(fp), "plane"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%f %f %f %f\n", &face->plane[0], &face->plane[1], &face->plane[2], &face->plane[3]);

	while(strcmp(TokenLoad(fp), "numverts"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%d", &face->numverts);

	for(i=0; i<face->numverts; i++) {
		while(strcmp(TokenLoad(fp), "vertex"));
		fscanf(fp, "%s", TokenValue());
		fscanf(fp, "%f %f %f", &face->verts[i][0], &face->verts[i][1], &face->verts[i][2]);
		fscanf(fp, "%f %f \n", &face->terts[i][0], &face->terts[i][1]);

		face->terts[i][0]/=16383.0F;
		face->terts[i][1]/=16383.0F;
	}
	
	while(strcmp(TokenLoad(fp), "m_axis"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%d\n", &face->maxis);

	while(strcmp(TokenLoad(fp), "params"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%d\n", &face->params);

	while(strcmp(TokenLoad(fp), "index"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%d\n", &face->index);

	while(strcmp(TokenLoad(fp), "texture"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%s\n", tex);
	
	tp=tex+1;

	while(*(tp-1)!=':' && *tp!='\0') {
		tp++;
	}

	strcpy(face->texxer, tp);

	tp--;
	*tp='\0';
	
	while((tp-1)!=tex && *(tp-1)!='\\' && *(tp-1)!='/') {
		tp--;
	}

	strcpy(face->texpak, tp);
	strcat(face->texpak, ".txp");

	if(face->texxer[0]=='\0') {
		face->texpak[0]='\0';
	}

	do{
		TokenLoad(fp);
	}while(strcmp(TokenValue(), "}"));

	// wait until finished parsing
	if(face->texxer[0]=='\0') {
		return NULL;
	}

	face->binder=binder;
	binder++;

	LoadFaceOptimiseVerts(face);

	return face;
}

//
// LoadFaceArray
//
int LoadFaceArray(face_t **scene, face_t *faces, bsp_t *node, int num) {

	int		n;
	int		i;

	for(n=0, i=0; i<num; i++) {
		if(faces[i].texxer[0]!='\0') n++;
	}

	if(n==0) {
		*scene=NULL;
		return 0;
	}

	*scene=malloc(sizeof(face_t)*n);

	for(n=0, i=0; i<num; i++) {
		if(faces[i].texxer[0]=='\0') continue;
		memcpy(&(*scene)[n++], &faces[i], sizeof(face_t));
		(*scene)[i].node=node;
		(*scene)[i].next=prvface;
		prvface=&(*scene)[i];
	}

	return n;
}

//
// LoadFaces
//
int LoadFaces(FILE *fp, bsp_t *node, face_t **scene) {

	static face_t	faces[128];
	int				n;
		
	TokenLoad(fp);

	n=0;

	while(strcmp(TokenValue(), "#node") && TokenValue()[0]!='}') {
		if(!strcmp(TokenValue(), "#poly")) if(LoadFace(fp, &faces[n])) n++;
		TokenLoad(fp);
	}

	TokenPreserve();

	return LoadFaceArray(scene, faces, node, n);
}

//
// LoadNode
//
bsp_t *LoadNode(FILE *fp) {

	bsp_t	*node;
	
	while(strcmp(TokenLoad(fp), "#node") && TokenValue()[0]!='}' && TokenValue()[0]!='{') {
	}

	if(TokenValue()[0]=='}') {
		TokenPreserve();
		return NULL;
	}

	node=malloc(sizeof(bsp_t));

	while(strcmp(TokenLoad(fp), "plane"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%f %f %f %f\n", &node->plane[0], &node->plane[1], &node->plane[2], &node->plane[3]);

	while(strcmp(TokenLoad(fp), "m_axis"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%d\n", &node->maxis);

	while(strcmp(TokenLoad(fp), "params"));
	fscanf(fp, "%s", TokenValue());
	fscanf(fp, "%d\n", &node->params);

	node->numfaces=LoadFaces(fp, node, &node->faces);

	node->front=LoadNode(fp);
	node->back=LoadNode(fp);

	while(strcmp(TokenLoad(fp), "}"));

	return node;
}

//
// LoadScene
//
bsp_t *LoadScene(char *name, face_t **faces) {

	bsp_t	*node;
	FILE	*fp;
	
	if(!(fp=fopen(name, "r"))) {
		ErrorInvoke(ERROR_MAP_NOT_FOUND);
		return NULL;
	}
	
	prvface=NULL;

	node=LoadNode(fp);

	fclose(fp);

	*faces=prvface;

	return node;
}
