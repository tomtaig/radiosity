#include "load.h"

//
// Token globals
char	token[128];
int		preservecycles=0;

//
// LoadToken
//
char *LoadToken(FILE *fp) {

	if(preservecycles) {
		preservecycles--;
	}else{
		fscanf(fp, "%s", token);
	}

	return token;
}

//
// LoadTokenPreserve
//
void LoadTokenPreserve(void) {

	preservecycles++;

	return;
}

//
// LoadFace
//
face_t *LoadFace(FILE *fp, bsp_t *node, face_t *face) {

	int		i;

	do{
		LoadToken(fp);
	}while(strcmp(token, "{"));

	while(strcmp(LoadToken(fp), "plane"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%lf %lf %lf %lf\n", &face->plane[0], &face->plane[1], &face->plane[2], &face->plane[3]);

	while(strcmp(LoadToken(fp), "numverts"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d", &face->numverts);

	for(i=0; i<face->numverts; i++) {
		while(strcmp(LoadToken(fp), "vertex"));
		fscanf(fp, "%s", token);
		fscanf(fp, "%lf %lf %lf", &face->verts[i][0], &face->verts[i][1], &face->verts[i][2]);
		fscanf(fp, "%lf %lf \n", &face->terts[i][0], &face->terts[i][1]);
	}
	
	// scale the texture coordinates
	for(i=0; i<face->numverts; i++) {
		face->terts[i][0]/=16383.0F;
		face->terts[i][1]/=16383.0F;
	}

	// edge planes, optimisation, etc.
	FaceExtendedInfo(face);

	while(strcmp(LoadToken(fp), "m_axis"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &face->maxis);

	while(strcmp(LoadToken(fp), "params"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &face->params);

	while(strcmp(LoadToken(fp), "index"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &face->index);

	while(strcmp(LoadToken(fp), "texture"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%s\n", token);
	
	do{
		LoadToken(fp);
	}while(strcmp(token, "}"));

	face->node=node;
	face->params|=PARM_ISUSED;

	return face;
}

//
// LoadFaces
//
int LoadFaces(FILE *fp, bsp_t *node, face_t **pl) {

	int		n=0;

	LoadToken(fp);

	while(strcmp(token, "#node") && token[0]!='}') {
		FacePush(pl, malloc(sizeof(face_t)));

		if(!strcmp(token, "#poly")) {
			LoadFace(fp, node, *pl);
		}

		LoadToken(fp);

		n++;
	}

	// rewind one token, effectively
	LoadTokenPreserve();

	return n;
}

//
// LoadNode
//
bsp_t *LoadNode(FILE *fp) {

	bsp_t	*node;
	
	while(strcmp(LoadToken(fp), "#node") && token[0]!='}' && token[0]!='{') {
	}

	if(token[0]=='}') {
		LoadTokenPreserve();
		return NULL;
	}

	node=malloc(sizeof(bsp_t));
	node->faces=NULL;

	while(strcmp(LoadToken(fp), "plane"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%lf %lf %lf %lf\n", &node->plane[0], &node->plane[1], &node->plane[2], &node->plane[3]);

	while(strcmp(LoadToken(fp), "m_axis"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &node->maxis);

	while(strcmp(LoadToken(fp), "params"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &node->params);

	while(strcmp(LoadToken(fp), "index"));
	fscanf(fp, "%s", token);
	fscanf(fp, "%d\n", &node->index);

	node->numfaces=LoadFaces(fp, node, &node->faces);

	node->front=LoadNode(fp);
	node->back=LoadNode(fp);

	if(node->front) {
		node->front->parent=node;
	}

	if(node->back) {
		node->back->parent=node;
	}
	
	while(strcmp(LoadToken(fp), "}"));

	return node;
}

//
// LoadScene
//
bsp_t *LoadScene(char *name) {

	bsp_t	*node;
	FILE	*fp;
	
	if(!(fp=fopen(name, "r"))) {
		ErrorInvoke(ERROR_MAP_NOT_FOUND);
		return NULL;
	}
	
	node=LoadNode(fp);

	node->parent=NULL;

	fclose(fp);

	return node;
}
