#include "compile.h"

//
// camera & momentum
vec_t	g_camera[16]={1,0,0,0,0,-1,0,0,0,0,1,0,0,0,0,1};
vec_t	g_mo_cam_fwd=0;
vec_t	g_mo_cam_sde=0;

//
// solid bsp tree
bsp_t	*g_bsp;

//
// csg faces
face_t	*g_faces=NULL;
face_t	*g_csg=NULL;

//
// key press buffer
char	g_keys[256];

//
// ExitFunc
//
void ExitFunc(int exvalue) {

	glutSetCursor(GLUT_CURSOR_LEFT_ARROW);

	if(!DataReadInteger("app_windowed")) {
		glutLeaveGameMode();
	}

	glDeleteLists(0, 5);

	DataDestroyPrefixed("");
	BspDestroyTree(g_bsp);
	exit(exvalue);
}

//
// DisplayFaceNormal
//
void DisplayFaceNormal(face_t face, vec3_t col) {

	vec3_t	mid;
	vec3_t	pla;
	vec3_t	a;
	vec3_t	b;
	int		i;

	VectorSub(face.verts[1], face.verts[0], a);
	VectorSub(face.verts[2], face.verts[0], b);
	VectorCrossProduct(a, b, pla);
	VectorScale(pla, 10000);
	
	VectorAdd(face.verts[0], face.verts[1], mid);

	for(i=2; i<face.numverts; i++) {
		VectorAdd(mid, face.verts[i], mid);
	}

	VectorScale(mid, face.numverts);
	VectorSub(mid, pla, pla);

	glColor3d(1-col[0], 1-col[1], 1-col[2]);

	glBegin(GL_LINES);
	glVertex3dv(mid);
	glVertex3dv(pla);
	glEnd();

	return;
}

//
// DisplayFace
//
void DisplayFace(face_t *face, vec3_t col) {

	int		i;

	if(!face) {
		return;
	}

	if(DataReadInteger("rnd_faces")) {
		col[0]=1.0F-((vec_t)(((int)face*2)%10000))/10000.0F;
		col[1]=1.0F-((vec_t)(((int)face*4)%10000))/10000.0F;
		col[2]=1.0F-((vec_t)(((int)face*8)%10000))/10000.0F;
	}

	glColor3d(col[0], col[1], col[2]);

	glBegin(GL_POLYGON);

	for(i=0; i<face->numverts; i++) {
		glVertex3dv(face->verts[i]);
	}

	glEnd();

	if(DataReadInteger("rnd_normals")) {
		DisplayFaceNormal(*face, col);
	}

	return;
}

//
// DisplayBsp
//
void DisplayBsp(bsp_t *node) {

	vec3_t	col;
	face_t	*face;
	
	if(node==NULL) {
		return;
	}

	DisplayBsp(node->front);
	DisplayBsp(node->back);

	if(!(node->params&PARM_ISLEAF)) {
		return;
	}

	if(!(node->params&PARM_HOLLOW)) {
		return;
	}

	if(DataReadInteger("rnd_hulls")) {
		col[0]=1.0F-((vec_t)(((int)node*2)%10000))/10000.0F;
		col[1]=1.0F-((vec_t)(((int)node*4)%10000))/10000.0F;
		col[2]=1.0F-((vec_t)(((int)node*8)%10000))/10000.0F;
	}

	for(face=node->faces; face; face=face->next) {
		DisplayFace(face, col);
	}

	return;
}

//
// DisplayBspSorted
//
void DisplayBspSorted(bsp_t *node, vec3_t co) {

	vec_t	d=VectorDotProduct(node->plane, co)-node->plane[3];

	if(node->params&PARM_ISLEAF) {
		DisplayBsp(node);
		return;
	}

	if(d>0.0F) {
		DisplayBspSorted(node->back, co);
		DisplayBspSorted(node->front, co);
	}else{
		DisplayBspSorted(node->front, co);
		DisplayBspSorted(node->back, co);
	}

	return;
}

//
// DisplayFunc
//
void DisplayFunc(void) {

	face_t	*face;
	vec3_t	cam;
	vec_t	scl;

	scl=DataReadPrecision("rnd_scenescale");

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadMatrixd(g_camera);
	glScalef(scl, scl, scl);

	if(DataReadInteger("rnd_axis")) {
		glBegin(GL_LINES);
		glColor3f(0, 0, 1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,1/scl);
		glColor3f(0, 1, 0);
		glVertex3f(0,0,0);
		glVertex3f(0,1/scl,0);
		glColor3f(1, 0, 0);
		glVertex3f(0,0,0);
		glVertex3f(1/scl,0,0);
		glEnd();
	}

	cam[0]=-VectorDotProduct(&g_camera[0], &g_camera[12])/scl;
	cam[1]=-VectorDotProduct(&g_camera[4], &g_camera[12])/scl;
	cam[2]=-VectorDotProduct(&g_camera[8], &g_camera[12])/scl;

	if(DataReadInteger("rnd_culling")) {
		glEnable(GL_CULL_FACE);
	}else{
		glDisable(GL_CULL_FACE);
	}

	if(SceneVector(g_bsp, cam)==0) {
		glCullFace(GL_BACK);
	}else{
		glCullFace(GL_FRONT);
	}

	if(!g_csg) {
	if(DataReadInteger("rnd_bspsorting")) {
		glDisable(GL_DEPTH_TEST);
		DisplayBspSorted(g_bsp, cam);
	}else{
		glEnable(GL_DEPTH_TEST);
		glCallList(1);
	}
	}

	for(face=g_csg; face; face=face->next) {
		DisplayFace(face, cam);
	}

	glutSwapBuffers();
}

//
// KeyboardFunc
//
void KeyboardFunc(unsigned char key, int x, int y) {

	g_keys[key]=1;

	return;
}

//
// KeyupFunc
//
void KeyupFunc(unsigned char key, int x, int y) {

	g_keys[key]=0;

	return;
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

	glLoadIdentity();
	glRotatef((x-cx)/3, -g_camera[4], -g_camera[5], -g_camera[6]);
	glMultMatrixd(g_camera);
	glGetDoublev(GL_MODELVIEW_MATRIX, g_camera);

	glLoadIdentity();
	glRotatef((y-cy)/3, 1, 0, 0);
	glMultMatrixd(g_camera);
	glGetDoublev(GL_MODELVIEW_MATRIX, g_camera);

	glutWarpPointer(cx, cy);

	ox=x;
	oy=y;
}

//
// IdleFunc
//
void IdleFunc(void) {

	static bsp_t *node1=NULL;
	static bsp_t *node2=NULL;
	static bsp_t *node3=NULL;

	vec_t	scl;
	int		cul;
	vec3_t	cam;

	scl=DataReadPrecision("rnd_scenescale");
	cul=DataReadInteger("rnd_culling");

	cam[0]=-VectorDotProduct(&g_camera[0], &g_camera[12])/scl;
	cam[1]=-VectorDotProduct(&g_camera[4], &g_camera[12])/scl;
	cam[2]=-VectorDotProduct(&g_camera[8], &g_camera[12])/scl;

	if(g_keys[27]) {
		ExitFunc(0x01);
	}

	if(g_keys['w']) {
		g_mo_cam_fwd+=0.05;
	}

	if(g_keys['s']) {
		g_mo_cam_fwd-=0.05;
	}

	if(g_keys['a']) {
		g_mo_cam_sde+=0.05;
	}

	if(g_keys['d']) {
		g_mo_cam_sde-=0.05;
	}

	if(g_keys['-']) {
		DataWritePrecision("rnd_scenescale", scl/1.05F);
	}

	if(g_keys['=']) {
		DataWritePrecision("rnd_scenescale", scl*1.05F);
	}

	if(g_keys['c']) {		
		DataWriteInteger("rnd_culling", cul^1);
		g_keys['c']=0;
	}

	if(g_keys['1']) {
		node1=SceneVector(g_bsp, cam);
		g_keys['1']=0;
	}

	if(g_keys['2']) {
		node2=SceneVector(g_bsp, cam);
		g_keys['2']=0;
	}

	if(g_keys['3']) {
		node3=SceneVector(g_bsp, cam);
		g_keys['3']=0;
	}

	if(g_keys['4']) {
		CsgSubtraction(g_faces, node1->parent->face->hullid, node2->parent->face->hullid, &g_csg);
		CsgSubtraction(g_faces, node1->parent->face->hullid, node3->parent->face->hullid, &g_csg);
		g_keys['4']=0;
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
// ParseCmdLine
//
int ParseCmdLine(int argc, char **argv) {

	int		i;

	if(argc==1) {
		return 0;
	}

	for(i=1; argv[i][0]=='-'; i++) {
		if(!strcmp(argv[i], "-render")) {
			DataWriteInteger("rnd_enabled", 1);
		}else
		if(!strcmp(argv[i], "-axis")) {
			DataWriteInteger("rnd_axis", 1);
		}else
		if(!strcmp(argv[i], "-normals")) {
			DataWriteInteger("rnd_normals", 1);
		}else
		if(!strcmp(argv[i], "-windowed")) {
			DataWriteInteger("app_windowed", 1);
		}else
		if(!strcmp(argv[i], "-csg")) {
			DataWriteInteger("bsp_csg", 1);
		}else
		if(!strcmp(argv[i], "-hulls")) {
			DataWriteInteger("rnd_hulls", 1);
		}else
		if(!strcmp(argv[i], "-faces")) {
			DataWriteInteger("rnd_faces", 1);
		}else
		if(!strcmp(argv[i], "-bspsort")) {
			DataWriteInteger("rnd_bspsorting", 1);
		}else{
			ErrorInvoke(ERROR_CMD_LINE_PARSE);
		}

		argv[i][0]='\n';
	}

	if((argc-i)<2) {
		ErrorInvoke(ERROR_CMD_LINE_PARSE);
	}

	DataWriteString("app_input", argv[i+1]);
	DataWriteString("app_output", argv[i]);

	return 1;
}

//
// InitVariables
//
void InitVariables(void) {

	// bsp epsilon
	DataCreate("bsp_epsilon", DATA_PRECISION);
	DataWritePrecision("bsp_epsilon", 0.05F);

	// bsp csg state
	DataCreate("bsp_csg", DATA_INTEGER);
	DataWriteInteger("bsp_csg", 0);

	// render state
	DataCreate("rnd_enabled", DATA_INTEGER);
	DataWriteInteger("rnd_enabled", 0);

	// render depth sorting state
	DataCreate("rnd_bspsorting", DATA_INTEGER);
	DataWriteInteger("rnd_bspsorting", 0);

	// render normals
	DataCreate("rnd_normals", DATA_INTEGER);
	DataWriteInteger("rnd_normals", 0);

	// culling state
	DataCreate("rnd_culling", DATA_INTEGER);
	DataWriteInteger("rnd_culling", 0);

	// render as coloured hulls
	DataCreate("rnd_hulls", DATA_INTEGER);
	DataWriteInteger("rnd_hulls", 1);

	// render as coloured faces
	DataCreate("rnd_faces", DATA_INTEGER);
	DataWriteInteger("rnd_faces", 0);

	// show world space axis
	DataCreate("rnd_axis", DATA_INTEGER);
	DataWriteInteger("rnd_axis", 0);

	// scene scale factor
	DataCreate("rnd_scenescale", DATA_PRECISION);
	DataWritePrecision("rnd_scenescale", 0.008F);

	// application windowing mode
	DataCreate("app_windowed", DATA_INTEGER);
	DataWriteInteger("app_windowed", 0);

	// input file for the application
	DataCreate("app_input", DATA_STRING);
	DataWriteString("app_input", "");

	// output file for the application
	DataCreate("app_output", DATA_STRING);
	DataWriteString("app_output", "");

	return;
}

//
// main
//
int main(int argc, char** argv) {
	
	time_t	curtime[2];
	face_t	*scene;
	char	str[DATA_STRING_LEN];
	
	// memory leak checking
#ifndef NDEBUG
	int		i;

	i=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	i|=_CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(i);
#endif

	// init key press buffer
	memset(g_keys, 0, 255);

	// initialise vars
	InitVariables();

	// parse command line
	if(ParseCmdLine(argc, argv)==0) {
		return 0;
	}

	// load the scene
	DataReadString("app_input", str);
	if(LoadScene(str, &scene)==0) {
		return 0;
	}

	if(LoadScene(str, &g_faces)==0) {
		return 0;
	}

	// create & write the bsp
	time(&curtime[0]);
	g_bsp=BspCompileTree(scene);
	time(&curtime[1]);

	DataReadString("app_output", str);
	BspWriteFile(str, g_bsp);

	printf("------------------------\n");
	printf("bsp compile time = %d seconds\n", curtime[1]-curtime[0]);

	// do not display the scene
	if(!DataReadInteger("rnd_enabled")) {
		BspDestroyTree(g_bsp);
		return 1;
	}

	// setup pixel type via GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
 
	if(DataReadInteger("app_windowed")) {
		// init windowed mode
		glutInitWindowPosition(100, 100);
		glutInitWindowSize(700, 700);
		glutCreateWindow("ssbsp v2");
	}else{
		// init fullscreen mode
		glutGameModeString("800x600:16@60");
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

	glEnable(GL_DEPTH_TEST);

	// create display lists
	glGenLists(1);
	glNewList(1, GL_COMPILE);
	DisplayBsp(g_bsp);
	glEndList();

	glutMainLoop();

	return 0;
}
