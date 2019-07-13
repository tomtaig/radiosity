#include "compile.h"

//
// keyboard buffer
char	g_keys[256];

//
// scene variables
bsp_t	*g_bsp=NULL;

//
// camera & momentum
vec_t	g_camera[16]={1,0,0,0,0,-1,0,0,0,0,1,0,0,0,0,1};
vec_t	g_mo_cam_fwd=0;
vec_t	g_mo_cam_sde=0;

//
// ExitFunc
//
void ExitFunc(int exvalue) {

	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

#ifdef SSTRACER_FULLSCREEN
	glutLeaveGameMode();
#endif

	glDeleteLists(0, 1);

	printf("-- destroying bsp --\n");
	SceneDestroyTree(g_bsp);

	printf("-- destroying variables --\n");
	DataDestroyPrefixed("");

	exit(exvalue);
}

//
// DisplayFaceNormal
//
void DisplayFaceNormal(face_t face, vec3_t col) {

	vec3_t	mid;
	vec3_t	pla;
	int		i;

	if(!(face.params&PARM_ISUSED)) {
		return;
	}

	VectorCopy(pla, face.plane);
	VectorScaleInv(pla, 200);
	
	VectorAdd(face.verts[0], face.verts[1], mid);

	for(i=2; i<face.numverts; i++) {
		VectorAdd(mid, face.verts[i], mid);
	}

	VectorScale(mid, face.numverts);
	VectorAdd(mid, pla, pla);

	glColor3d(1.0F-col[0], 1.0F-col[1], 1.0F-col[2]);

	glBegin(GL_LINES);
	glVertex3dv(mid);
	glVertex3dv(pla);
	glEnd();

	return;
}

//
// DisplayFacePortal
//
void DisplayFacePortal(face_t *face, vec3_t col) {

	int		i;

	if(!(face->params&PARM_PORTAL)) {
		return;
	}

	if(!(face->params&PARM_ISUSED)) {
		return;
	}

	col[0]=((vec_t)(((int)face)*2%1000))/1000;
	col[1]=((vec_t)(((int)face)*4%1000))/1000;
	col[2]=((vec_t)(((int)face)*8%1000))/1000;

	glColor3f(col[0], col[1], col[2]);
	glBegin(GL_POLYGON);

	for(i=0; i<face->numverts; i++) {
		glVertex3dv(face->verts[i]);
	}

	glEnd();

	DisplayFaceNormal(*face, col);

	return;
}

//
// DisplayFace
//
void DisplayFace(face_t *face, vec3_t col) {

	int		i;

	if((face->params&PARM_PORTAL)) {
		return;
	}

	if(!(face->params&PARM_ISUSED)) {
		return;
	}

//	col[0]=((vec_t)(((int)face)*2%1000))/1000;
//	col[1]=((vec_t)(((int)face)*4%1000))/1000;
//	col[2]=((vec_t)(((int)face)*8%1000))/1000;

	glColor3f(col[0], col[1], col[2]);
	glBegin(GL_POLYGON);

	for(i=0; i<face->numverts; i++) {
		glVertex3dv(face->verts[i]);
	}

	glEnd();

	return;
}

//
// DisplayTreePortal
//
void DisplayTreePortal(bsp_t *node) {

	vec3_t	col;
	face_t	*face;

	if(!node) {
		return;
	}

	if(!(node->params&PARM_ISLEAF)) {
		DisplayTreePortal(node->front);
		DisplayTreePortal(node->back);
		return;
	}

	if(!(node->params&PARM_HOLLOW)) {
		return;
	}

	col[0]=((vec_t)(((int)node)*2%1000))/1000;
	col[1]=((vec_t)(((int)node)*4%1000))/1000;
	col[2]=((vec_t)(((int)node)*8%1000))/1000;

	for(face=node->faces; face; face=face->next) {
		DisplayFacePortal(face, col);
	}

	return;
}

//
// DisplayTree
//
void DisplayTree(bsp_t *node) {

	vec3_t	col;
	face_t	*face;

	if(!node) {
		return;
	}

	if(!(node->params&PARM_ISLEAF)) {
		DisplayTree(node->front);
		DisplayTree(node->back);
		return;
	}

	if(!(node->params&PARM_HOLLOW)) {
		return;
	}

	col[0]=((vec_t)(((int)node)*2%1000))/1000;
	col[1]=((vec_t)(((int)node)*4%1000))/1000;
	col[2]=((vec_t)(((int)node)*8%1000))/1000;

	for(face=node->faces; face; face=face->next) {
		DisplayFace(face, col);
	}

	return;
}

//
// DisplayFunc
//
void DisplayFunc(void) {

	bsp_t	*nd;
	vec_t	scl;
	vec3_t	org;
	
	glEnable(GL_TEXTURE_2D);

	scl=DataReadPrecision("rnd_scenescale");
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadMatrixd(g_camera);
	glScaled(scl, scl, scl);

	glDisable(GL_TEXTURE_2D);

	if(DataReadInteger("rnd_showaxis")) {
		glBegin(GL_LINES);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1/scl);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1/scl, 0);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(1/scl, 0, 0);
		glEnd();
	}

	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if(DataReadInteger("rnd_cullbacks")) {
		glEnable(GL_CULL_FACE);
	}else{
		glDisable(GL_CULL_FACE);
	}

	org[0]=-VectorDotProduct(&g_camera[0], &g_camera[12])/scl;
	org[1]=-VectorDotProduct(&g_camera[4], &g_camera[12])/scl;
	org[2]=-VectorDotProduct(&g_camera[8], &g_camera[12])/scl;

	nd=SceneVector(g_bsp, org);

	if(nd) {
		glCullFace(GL_BACK);
	}else{
		glCullFace(GL_FRONT);
	}

	glCallList(1);

//	DisplayTreePortal(g_bsp);

	glutSwapBuffers();
}

//
// KeyboardFunc
//
void KeyboardFunc(unsigned char key, int x, int y) {

	if(key>='A' && key<='Z') {
		key-=('A'-'a');
	}

	g_keys[key]=!0;

	return;
}

//
// KeyupFunc
//
void KeyupFunc(unsigned char key, int x, int y) {

	if(key>='A' && key<='Z') {
		key-=('A'-'a');
	}

	g_keys[key]=0;

	return;
}

//
// MotionFunc
//
void MotionFunc(int x, int y) {

	static	ox;
	static	oy;
	int		cx;
	int		cy;

	if(x==ox && y==oy) {
		return;
	}

	cx=glutGet(GLUT_WINDOW_WIDTH )/2;
	cy=glutGet(GLUT_WINDOW_HEIGHT)/2;

	glutWarpPointer(cx, cy);

	glLoadIdentity();
	glRotated((x-cx)/3, -g_camera[4], -g_camera[5], -g_camera[6]);
	glMultMatrixd(g_camera);
	glGetDoublev(GL_MODELVIEW_MATRIX, g_camera);

	glLoadIdentity();
	glRotated((y-cy)/3, 1, 0, 0);
	glMultMatrixd(g_camera);
	glGetDoublev(GL_MODELVIEW_MATRIX, g_camera);

	ox=x;
	oy=y;
}

//
// ResizeFunc
//
void ResizeFunc(int w, int h) {

	float Fw=w, Fh=h;

	glViewport(0, 0, Fw, Fh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, (double)w/(double)h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

//
// IdleFunc
//
void IdleFunc(void) {

	static bsp_t *node1;
	static bsp_t *node2;

	vec3_t	org;
	vec_t	scl;
	int		cul;
	int		axi;

	scl=DataReadPrecision("rnd_scenescale");
	cul=DataReadInteger("rnd_cullbacks");
	axi=DataReadInteger("rnd_showaxis");

	org[0]=-VectorDotProduct(&g_camera[0], &g_camera[12])/scl;
	org[1]=-VectorDotProduct(&g_camera[4], &g_camera[12])/scl;
	org[2]=-VectorDotProduct(&g_camera[8], &g_camera[12])/scl;

	if(g_keys[27]==!0) {
		ExitFunc(0x01);
	}

	if(g_keys['1']==!0) {
		node1=SceneVector(g_bsp, org);
		g_keys['1']=0;
	}

	if(g_keys['2']==!0) {
		node2=SceneVector(g_bsp, org);
		g_keys['2']=0;
	}

	if(g_keys['z']==!0) {
		PortalFromNodes(node1, node2);
		g_keys['z']=0;
	}

	if(g_keys['3']==!0) {
		DataWriteInteger("rnd_showaxis", axi^1);
		g_keys['3']=0;
	}

	if(g_keys['4']==!0) {
		DataWriteInteger("rnd_cullbacks", cul^1);
		g_keys['4']=0;
	}

	if(g_keys['w']==!0) {
		g_mo_cam_fwd+=0.05;
	}

	if(g_keys['s']==!0) {
		g_mo_cam_fwd-=0.05;
	}

	if(g_keys['a']==!0) {
		g_mo_cam_sde+=0.05;
	}

	if(g_keys['d']==!0) {
		g_mo_cam_sde-=0.05;
	}

	if(g_keys['-']==!0) {
		DataWritePrecision("rnd_scenescale", scl/1.05F);
	}

	if(g_keys['=']==!0) {
		DataWritePrecision("rnd_scenescale", scl*1.05F);
	}

	glLoadIdentity();
	glTranslatef(g_mo_cam_sde, 0, 0);
	glMultMatrixd(g_camera);
	glGetDoublev(GL_MODELVIEW_MATRIX, g_camera);

	glLoadIdentity();
	glTranslatef(0, 0, g_mo_cam_fwd);
	glMultMatrixd(g_camera);
	glGetDoublev(GL_MODELVIEW_MATRIX, g_camera);

	// cap speed, 0.5 unit max
	g_mo_cam_fwd=(g_mo_cam_fwd>0.5)?0.5:(g_mo_cam_fwd<-0.5)?-0.5:g_mo_cam_fwd;

	// reduce camera momentum
	if(g_mo_cam_fwd!=0) {
		g_mo_cam_fwd=(g_mo_cam_fwd<0)?g_mo_cam_fwd+0.01:g_mo_cam_fwd-0.01;
	}

	// clamp speed when near 0
	if(g_mo_cam_fwd>-0.01 && g_mo_cam_fwd<0.01) g_mo_cam_fwd=0;

	// cap speed, 0.5 unit max
	g_mo_cam_sde=(g_mo_cam_sde>0.5)?0.5:(g_mo_cam_sde<-0.5)?-0.5:g_mo_cam_sde;

	// reduce camera momentum
	if(g_mo_cam_sde!=0) {
		g_mo_cam_sde=(g_mo_cam_sde<0)?g_mo_cam_sde+0.01:g_mo_cam_sde-0.01;
	}

	// clamp speed when near 0
	if(g_mo_cam_sde>-0.01 && g_mo_cam_sde<0.01) g_mo_cam_sde=0;

	glutPostRedisplay();
}

//
// ExtractPortals
//
void ExtractPortals(bsp_t *node, face_t **portals) {

	face_t	*face;

	if(!node) {
		return;
	}

	if(!(node->params&PARM_ISLEAF)) {
		ExtractPortals(node->front, portals);
		ExtractPortals(node->back, portals);
		return;
	}

	if(!(node->params&PARM_HOLLOW)) {
		return;
	}

	for(face=node->faces; face; face=face->next) { 
		if(face->params&PARM_PORTAL) FacePush(portals, FaceCopy(face));
	}

	return;
}

//
// CountLeafs
//
void CountLeafs(bsp_t *node, int *num) {

	if(!node) {
		return;
	}

	if(!(node->params&PARM_ISLEAF)) {
		CountLeafs(node->front, num);
		CountLeafs(node->back, num);
		return;
	}

	if(!(node->params&PARM_HOLLOW)) {
		return;
	}

	(*num)++;
	
	return;
}

//
// BuildLeafReferences
//
void BuildLeafReferences(bsp_t *node, bsp_t **nodes, int *num) {

	if(!node) {
		return;
	}

	if(!(node->params&PARM_ISLEAF)) {
		BuildLeafReferences(node->front, nodes, num);
		BuildLeafReferences(node->back, nodes, num);
		return;
	}

	if(node->params&PARM_HOLLOW) {
		nodes[(*num)++]=node;
	}

	return;
}

//
// BuildPortals
//
void BuildPortals(bsp_t *nodes[], int numnodes) {

	int		i;
	int		n;

	for(i=0; i<numnodes; i++) {
		for(n=i; n<numnodes; n++) {
			PortalFromNodes(nodes[i], nodes[n]);
		}
	}

	return;
}

//
// InitVariables
//
void InitVariables(void) {

	// screen mode
	DataCreate("app_windowed", DATA_INTEGER);
	DataWriteInteger("app_windowed", 1);

	// render axis
	DataCreate("rnd_showaxis", DATA_INTEGER);
	DataWriteInteger("rnd_showaxis", 1);

	// render cull state
	DataCreate("rnd_cullbacks", DATA_INTEGER);
	DataWriteInteger("rnd_cullbacks", 0);

	// render scene scale
	DataCreate("rnd_scenescale", DATA_PRECISION);
	DataWritePrecision("rnd_scenescale", 0.008F);

	return;
}

//
// Portalize
//
void Portalize(char *str) {

	face_t	*portals;
	bsp_t	**nodes;
	int		num=0;
	int		ncn=0;

	CountLeafs(g_bsp, &ncn);
	nodes=malloc(sizeof(bsp_t*)*ncn);
	BuildLeafReferences(g_bsp, nodes, &num);

	BuildPortals(nodes, ncn);

	portals=NULL;

	ExtractPortals(g_bsp, &portals);
	PortalWriteFile("rampssb.ssp", portals);

	while(portals) {
		free(FacePop(&portals));
	}

	return;
}

//
// main
//
int main(int argc, char** argv) {

	// memory leak checking
#ifndef NDEBUG
	int		i;

	i=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	i|=_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(i);
#endif

	// initialising
	printf("-- initialising application --\n");

	// initialise keys
	memset(g_keys, 0, 255);

	// initialise all variables
	InitVariables();

	// load bsp scene & faces
	g_bsp=LoadScene("rampssb.ssb");

	// terminate if errors
	if(!g_bsp) {
		return !0;
	}

	// terminate if errors
	if(ErrorWasInvoked()) {
		return !0;
	}

	// build the damn portals
	Portalize("rampssb.ssp");

	// terminate if errors
	if(ErrorWasInvoked()) {
		return !0;
	}

	// setup pixel type thru GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);

	// initialise GLUT window
	if(DataReadInteger("app_windowed")) {
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(700, 700);
		glutCreateWindow("ssportal v1");
	}else{
		glutGameModeString("1152x864:16@40");
		glutEnterGameMode();
		glutSetCursor(GLUT_CURSOR_NONE);
	}

	// init GLUT callbacks
	glutKeyboardFunc(KeyboardFunc);
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ResizeFunc);
	glutMotionFunc(MotionFunc);
	glutPassiveMotionFunc(MotionFunc);
	glutKeyboardUpFunc(KeyupFunc);
	glutIdleFunc(IdleFunc);

	// init OpenGL states
	glEnable(GL_DEPTH_TEST);

	printf("-- creating display lists --\n");
	glGenLists(1);
	glNewList(1, GL_COMPILE);
	DisplayTree(g_bsp);
	glEndList();

	printf("-- finished initialisation --\n");
	glutMainLoop();

	return 0;
}