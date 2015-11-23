#include <iostream>
#include <stdio.h>
#include <math.h>

#include "Camera.h"
#include "SceneGraph.h"

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

// Camera
Camera camera; 
int	lastMouseX = -1, lastMouseY = -1;
int theButtonState = 0;
int theModifierState = 0;
int dx, dy;

// Types and Materials
#define NUM_OF_TYPE 2
#define NUM_OF_MATERIAL 4

// Display flags
bool showInfo = false;
bool showHelp = false;
bool showGround = true;

// Menu flags
bool showNodeMenu = false;
bool showMeshMenu = false;
bool showRaytraceMenu = false;

// String input
string filename = "";
string msg = "";
bool inputMeshFile = false;
bool showMsg = false;

// Ground
const float floorSize = 400.0f;
const float gridSize = 5.0f;

// Colors
const float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
const float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
const float blue[] = { 0.0f, 0.0f, 1.0f, 1.0f };
const float text[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const float textbg[] = { 1.0f, 1.0f, 1.0f, 0.2f };
const float lightGrid[] = { 0.5f, 0.5f, 0.5f, 1.0f };
const float darkGrid[] = {  0.1f, 0.1f, 0.1f, 1.0f };
const float specularGrid[] = { 0.1f, 0.25f, 0.4f, 1.0f };

// Render setting
bool render_setting[2] = { true, true };

SceneGraph *scene = new SceneGraph();

void init(void); // Initialize OpenGL settings

void renderInfo(void); // Render information
void renderHelp(void); // Render help
void renderInput(void); // Render file input
void renderMenu(void); // Render menu
void renderAxis(void); // Render XYZ axis
void renderGround(void); // Render ground
void renderDisplay(void); // Render display
void changeSize(int, int); // Change window size
void processKeyboard(unsigned char, int, int); // Keyboard keys
void processKeyboardSpecial(int, int, int);	// Special keys for the arrow keys
void processMouse(int, int, int, int); // Mouse motion
void processMouseMotion(int, int); // Mouse motion action

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Scene Renderer");
	
	glutDisplayFunc(renderDisplay);
	glutIdleFunc(renderDisplay);

	glutKeyboardFunc(processKeyboard);
	glutSpecialFunc(processKeyboardSpecial);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseMotion);

	glutReshapeFunc(changeSize);

	init();

	glutMainLoop();

	return 0;
}

void init(void)
{
	float l0_dif[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float l0_amb[] = {0.2f, 0.2f, 0.2f, 0.2f};
	float l0_pos[] = {0.5f, 0.6f, 0.624f, 0.0f};

	glClearColor(0.1, 0.1, 0.1, 1.0);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
	glEnable(GL_COLOR_MATERIAL);
	// glEnable(GL_CULL_FACE);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);
}

void renderInfo()
{
	// Display information
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 640, 0, 480);	
		glScalef(1, -1, 1); // Invert the y axis, down is positive
		glTranslatef(0, -480, 0); // Move the origin from the bottom left corner to the upper left corner
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
			glLoadIdentity();
			glDisable(GL_LIGHTING);
			glColor4fv(text);
			char info[1024];
			
			glRasterPos2f(520, 20);
			sprintf(info, "F1 (help) / F2 (info)");
			for (unsigned int ii = 0; ii < strlen(info); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);

			if (showInfo) {
				glRasterPos2f(10, 20);
				sprintf(info, "Translation: X: %.2f Y: %.2f Z: %.2f", scene->getSelectedNode()->getTranslation()[0],
																	  scene->getSelectedNode()->getTranslation()[1],
																	  scene->getSelectedNode()->getTranslation()[2]);
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			
				glRasterPos2f(10, 35);
				sprintf(info, "Rotation: X: %.2f Y: %.2f Z: %.2f", scene->getSelectedNode()->getRotation()[0],
																   scene->getSelectedNode()->getRotation()[1],
																   scene->getSelectedNode()->getRotation()[2]);
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			
				glRasterPos2f(10, 50);
				sprintf(info, "Scaling: X: %.2f Y: %.2f Z: %.2f", scene->getSelectedNode()->getScaling()[0],
																  scene->getSelectedNode()->getScaling()[1],
																  scene->getSelectedNode()->getScaling()[2]);
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);

				glRasterPos2f(10, 65);
				switch (scene->getSelectedNode()->getType()) {
					case NOTHING:	{ sprintf(info, "Object Type: Nothing"); break; }
					case SPHERE:	{ sprintf(info, "Object Type: Sphere"); break; }
					case CUBE:		{ sprintf(info, "Object Type: Cube"); break; }
					case MESH:		{ sprintf(info, "Object Type: Mesh Object"); break; }
				}
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
				
				glRasterPos2f(10, 80);
				switch (scene->getSelectedNode()->getMaterial()) {
					case MAT_DARK:	{ sprintf(info, "Object Material: Dark"); break; }
					case MAT_RED:	{ sprintf(info, "Object Material: Red"); break; }
					case MAT_GREEN:	{ sprintf(info, "Object Material: Green"); break; }
					case MAT_BLUE:	{ sprintf(info, "Object Material: Blue"); break; }
					case MAT_GLASS:	{ sprintf(info, "Object Material: Glass"); break; }
				}
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
				
				glRasterPos2f(10, 95);
				switch (scene->getSelectedNode()->getBvType()) {
					case NONE:	{ sprintf(info, "Bounding Volume Type: None"); break; }
					case AABB:	{ sprintf(info, "Bounding Volume Type: AABB"); break; }
					case OBB:	{ sprintf(info, "Bounding Volume Type: OBB"); break; }
					case BS:	{ sprintf(info, "Bounding Volume Type: Sphere"); break; }
				}
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			
				glRasterPos2f(10, 110);
				sprintf(info, "Eyep: X: %.2f Y: %.2f Z: %.2f", camera.GetEye()[0], camera.GetEye()[1], camera.GetEye()[2]);
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			
				glRasterPos2f(10, 125);
				sprintf(info, "Vdir: X: %.2f Y: %.2f Z: %.2f", camera.GetFrontVector()[0], camera.GetFrontVector()[1], camera.GetFrontVector()[2]);
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			
				glRasterPos2f(10, 140);
				sprintf(info, "Uvec: X: %.2f Y: %.2f Z: %.2f", camera.GetUpVector()[0], camera.GetUpVector()[1], camera.GetUpVector()[2]);
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			
				glRasterPos2f(10, 155);
				sprintf(info, "Fovy: %.2f", camera.GetFovY());
				for (unsigned int ii = 0; ii < strlen(info); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, info[ii]);
			}

			glEnable(GL_LIGHTING);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void renderHelp()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 640, 0, 480);	
		glScalef(1, -1, 1); // Invert the y axis, down is positive
		glTranslatef(0, -480, 0); // Move the origin from the bottom left corner to the upper left corner
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
			glLoadIdentity();
			glDisable(GL_LIGHTING);
			glColor4fv(text);
			char help_txt[1024];

			glRasterPos2f(220, 60);
			sprintf(help_txt, "'c/C' - create new node/copy selected node");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 75);
			sprintf(help_txt, "'e' - delete current node");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 90);
			sprintf(help_txt, "'s' - select child");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 105);
			sprintf(help_txt, "'w' - select parent");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 120);
			sprintf(help_txt, "'a' - select previous sibling");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 135);
			sprintf(help_txt, "'d' - selected next sibling");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 150);
			sprintf(help_txt, "'x/y/z'/'X/Y/Z' - +/- X/Y/Z translation by 1 unit");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 165);
			sprintf(help_txt, "'u/i/o'/'U/I/O' - +/- X/Y/Z rotation by 30 degrees");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 180);
			sprintf(help_txt, "'j/k/l'/'J/K/L' - +/- X/Y/Z scaling by 1 unit");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 195);
			sprintf(help_txt, "'g' - toggle ground on/off");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 210);
			sprintf(help_txt, "'b' - toggle bounding volume type");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 225);
			sprintf(help_txt, "'n' - node menu");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 240);
			sprintf(help_txt, "'m' - mesh menu");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 255);
			sprintf(help_txt, "'r' - ray tracer menu");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);

			glRasterPos2f(220, 270);
			sprintf(help_txt, "'up/down' - change node type");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 285);
			sprintf(help_txt, "'left/right' - change node material");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 300);
			sprintf(help_txt, "'1' - create table");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 315);
			sprintf(help_txt, "'2' - create chair");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 330);
			sprintf(help_txt, "'3' - create filing cabinet");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 345);
			sprintf(help_txt, "'4' - create mesh object");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 360);
			sprintf(help_txt, "'enter' - raytrace current scene");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 375);
			sprintf(help_txt, "'alt + mouse' - move/rotate/zoom camera");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 390);
			sprintf(help_txt, "'space' - reset scene graph");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glRasterPos2f(220, 405);
			sprintf(help_txt, "'esc' - quit");
			for (unsigned int ii = 0; ii < strlen(help_txt); ii++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, help_txt[ii]);
			
			glEnable(GL_LIGHTING);
			glColor4fv(textbg);
			glRectf(210.0, 35.0, 505.0, 415.0);
		glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void renderInput()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 640, 0, 480);	
		glScalef(1, -1, 1); // Invert the y axis, down is positive
		glTranslatef(0, -480, 0); // Move the origin from the bottom left corner to the upper left corner
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
			glLoadIdentity();
			glDisable(GL_LIGHTING);
			glColor4fv(text);
			char input_txt[1024];

			if (!showMsg) {
				glRasterPos2f(480, 450);
				sprintf(input_txt, "Enter file name:");
				for (unsigned int ii = 0; ii < strlen(input_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, input_txt[ii]);
				glRasterPos2f(480, 465);
				sprintf(input_txt, ">> %s_", filename.c_str());
				for (unsigned int ii = 0; ii < strlen(input_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, input_txt[ii]);
			}
			else {
				glRasterPos2f(480, 450);
				sprintf(input_txt, "%s", msg.c_str());
				for (unsigned int ii = 0; ii < strlen(input_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, input_txt[ii]);
			}
			
			glEnable(GL_LIGHTING);
			glColor4fv(textbg);
			glRectf(470.0, 435.0, 630.0, 475.0);
		glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void renderMenu()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 640, 0, 480);	
		glScalef(1, -1, 1); // Invert the y axis, down is positive
		glTranslatef(0, -480, 0); // Move the origin from the bottom left corner to the upper left corner
		glMatrixMode(GL_MODELVIEW);

		glPushMatrix();
			glLoadIdentity();
			glDisable(GL_LIGHTING);
			glColor4fv(text);
			char menu_txt[1024];

			if (showNodeMenu) {
				glRasterPos2f(220, 90);
				sprintf(menu_txt, "'1' - create empty node");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 105);
				sprintf(menu_txt, "'2' - create sphere");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 120);
				sprintf(menu_txt, "'3' - create cube");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 135);
				sprintf(menu_txt, "'4' - create mesh");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 150);
				sprintf(menu_txt, "'5' - create table");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 165);
				sprintf(menu_txt, "'6' - create chair");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);

				glRasterPos2f(220, 180);
				sprintf(menu_txt, "'7' - create filing cabinet");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glRasterPos2f(220, 195);
				sprintf(menu_txt, "'esc' - exit menu");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glEnable(GL_LIGHTING);
				glColor4fv(textbg);
				glRectf(210.0, 65.0, 505.0, 210.0);
			}
			if (showMeshMenu) {
				glRasterPos2f(220, 90);
				sprintf(menu_txt, "'1' - read from file");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 105);
				sprintf(menu_txt, "'2' - create extrusion");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 120);
				sprintf(menu_txt, "'3' - create surface revolution");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 135);
				sprintf(menu_txt, "'4' - create random mesh");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				glRasterPos2f(220, 150);
				sprintf(menu_txt, "'5' - create random extrusion");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);

				glRasterPos2f(220, 165);
				sprintf(menu_txt, "'6' - create random surface revolution");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glRasterPos2f(220, 180);
				sprintf(menu_txt, "'esc' - exit menu");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glEnable(GL_LIGHTING);
				glColor4fv(textbg);
				glRectf(210.0, 65.0, 505.0, 195.0);
			}
			if (showRaytraceMenu) {
				string state;
				
				if (render_setting[0]) state = "on";
				else state = "off";
				glRasterPos2f(220, 90);
				sprintf(menu_txt, "'1' - frensnel reflection: %s", state.c_str());
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
				
				if (render_setting[1]) state = "on";
				else state = "off";
				glRasterPos2f(220, 105);
				sprintf(menu_txt, "'2' - supersampling: %s", state.c_str());
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glRasterPos2f(220, 120);
				sprintf(menu_txt, "'enter' - render scene");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glRasterPos2f(220, 135);
				sprintf(menu_txt, "'esc' - exit menu");
				for (unsigned int ii = 0; ii < strlen(menu_txt); ii++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, menu_txt[ii]);
			
				glEnable(GL_LIGHTING);
				glColor4fv(textbg);
				glRectf(210.0, 65.0, 505.0, 150.0);
			}
		glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void renderAxis()
{
	// Draw the XYZ axis
	glPushAttrib(GL_LIGHTING_BIT | GL_LINE_BIT);
		glDisable(GL_LIGHTING);
		glLineWidth(1.0);
		glBegin(GL_LINES);
		
		glColor4fv(red);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1000.0, 0.0, 0.0);
		
		glColor4fv(green);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1000.0, 0.0);
		
		glColor4fv(blue);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1000.0);

		glEnd();
		glEnable(GL_LIGHTING);
	glPopAttrib();
}

void renderGround(void)
{	
	bool row, col;
	row = col = false;
	for (int i = -floorSize; i <= floorSize; i += gridSize) {
		for (int j = -floorSize; j <= floorSize; j += gridSize) {
			if (col) {
				glColor4fv(lightGrid);
			}
			else {
				glColor4fv(darkGrid);
			}

			glBegin(GL_QUADS);
			glVertex3f(float(i) - gridSize*0.5, 0 , float(j) - gridSize*0.5);
			glVertex3f(float(i) - gridSize*0.5, 0 , float(j) + gridSize*0.5);
			glVertex3f(float(i) + gridSize*0.5, 0 , float(j) + gridSize*0.5);
			glVertex3f(float(i) + gridSize*0.5, 0 , float(j) - gridSize*0.5);
			glEnd();
			col = !col;
		}
		row = !row;
		col = row;
	}
}

void renderDisplay(void)
{
	glClearColor(0.0, 0.0, 0.0, 1);
	// Clearing the depth buffer 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 10.0, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camera.LookAt();
	
	glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		renderAxis();
		if (showGround) renderGround();
		glEnable(GL_LIGHTING);
	glPopAttrib();

	scene->drawScene();
	
	renderInfo(); // Display info
	if (inputMeshFile || showMsg) { renderInput(); } // Display file input
	if (showHelp) { renderHelp(); } // Display help
	if (showNodeMenu || showMeshMenu || showRaytraceMenu) { renderMenu(); } // Display menu
	
	// swapping the buffers causes the rendering above to be shown
	glutSwapBuffers();
}

void changeSize(int width, int height)
{
	// Update viewport
	glViewport(0, 0, width, height);
}

void processKeyboard(unsigned char key, int x, int y)
{
	if (showMsg) { msg = ""; showMsg = false; }
	else if (inputMeshFile && key == 13) { // Enter key
		msg = scene->createMesh("data/" + filename);
		showMsg = true;
		inputMeshFile = false;
		filename = "";
	}
	else if (inputMeshFile && key == 27) { // Esc key
		inputMeshFile = false;
		filename = "";
	}
	else if (inputMeshFile && key == 8) { // Backspace key
		filename = filename.substr(0, filename.length() - 1);
	}
	else if (inputMeshFile) {
		filename += key;
	}
	else if (showNodeMenu) {
		switch (key) {
			case '1': { scene->createNode(); showNodeMenu = false; } break; // Create empty node
			case '2': { scene->createNode(); scene->getSelectedNode()->setType(SPHERE); showNodeMenu = false; } break; // Create sphere
			case '3': { scene->createNode(); scene->getSelectedNode()->setType(CUBE); showNodeMenu = false; } break; // Create cube
			case '4': { showMeshMenu = true; showNodeMenu = false; } break; // Create mesh
			case '5': { scene->createTable(); showNodeMenu = false; } break; // Create table
			case '6': { scene->createChair(); showNodeMenu = false; } break; // Create chair
			case '7': { scene->createCabinet(); showNodeMenu = false; } break; // Create filing cabinet
			case 27:  { showNodeMenu = false; } break; // Esc key
			default: break;
		}
	}
	else if (showMeshMenu) {
		switch (key) {
			case '1': { inputMeshFile = true; showMeshMenu = false; } break; // Create mesh from input file
			case '4': { scene->createMesh("random"); showMeshMenu = false; } break; // Create random mesh
			case '5': { scene->createMesh("random extrude"); showMeshMenu = false; } break; // Create random extrude
			case '6': { scene->createMesh("random surfrev"); showMeshMenu = false; } break; // Create random surfrev
			case 27:  { showMeshMenu = false; } break; // Esc key
			default: break;
		}
	}
	else if (showRaytraceMenu) {
		switch (key) {
			case '1': { render_setting[0] = !render_setting[0]; } break; // Toggle between regular reflection/refraction and Fresnel reflection
			case '2': { render_setting[1] = !render_setting[1]; } break; // Toggle Anti-aliasing with supersampling
			case 13: { scene->renderScene(camera.GetEye(), camera.GetFrontVector(), camera.GetUpVector(), camera.GetFovY(), render_setting); showRaytraceMenu = false; } break; // Enter key
			case 27:  { showRaytraceMenu = false; } break; // Esc key
			default: break;
		}
	}
	else {
		switch (key) {
			case 's': scene->selectChild();	break; // Select child
			case 'w': scene->selectParent(); break; // Select parent
			case 'a': scene->selectPrevSibling(); break; // Select previous sibling
			case 'd': scene->selectNextSibling(); break; // Select next sibling
			case 'c': scene->createNode(); break; // Create new child
			case 'C': scene->copyNode(); break; // Copy current node (does not work for root node)
			case 'e': scene->deleteNode(); break; // Erase current node (does not work for root node)
			case 'g': showGround = !showGround; break; // Toggle ground on/off
			case 'b': scene->updateBoundingVolumeType(); break; // Change bounding volume type
			case 'n': { showNodeMenu = !showNodeMenu; showMeshMenu = showRaytraceMenu = showHelp = false; } break; // Show node menu
			case 'm': { showMeshMenu = !showMeshMenu; showNodeMenu = showRaytraceMenu = showHelp = false; } break; // Show mesh menu
			case 'r': { showRaytraceMenu = !showRaytraceMenu; showNodeMenu = showMeshMenu = showHelp = false; } break; // Show raytrace menu
			case 'x': scene->getSelectedNode()->incTranslationX();	break; // Inc x translation
			case 'X': scene->getSelectedNode()->decTranslationX();	break; // Dec x translation
			case 'y': scene->getSelectedNode()->incTranslationY();	break; // Inc y translation
			case 'Y': scene->getSelectedNode()->decTranslationY();	break; // Dec y translation
			case 'z': scene->getSelectedNode()->incTranslationZ();	break; // Inc z translation
			case 'Z': scene->getSelectedNode()->decTranslationZ();	break; // Dec z translation
			case 'u': scene->getSelectedNode()->incRotationX(); break; // Inc x rotation
			case 'U': scene->getSelectedNode()->decRotationX(); break; // Dec x rotation
			case 'i': scene->getSelectedNode()->incRotationY(); break; // Inc y rotation
			case 'I': scene->getSelectedNode()->decRotationY(); break; // Dec y rotation
			case 'o': scene->getSelectedNode()->incRotationZ(); break; // Inc z rotation
			case 'O': scene->getSelectedNode()->decRotationZ(); break; // Dec z rotation
			case 'j': scene->getSelectedNode()->incScalingX();	break; // Inc x scaling
			case 'J': scene->getSelectedNode()->decScalingX();	break; // Dec x scaling
			case 'k': scene->getSelectedNode()->incScalingY();	break; // Inc y scaling
			case 'K': scene->getSelectedNode()->decScalingY();	break; // Dec y scaling
			case 'l': scene->getSelectedNode()->incScalingZ();	break; // Inc z scaling
			case 'L': scene->getSelectedNode()->decScalingZ();	break; // Dec z scaling
			case '1': scene->createTable();	break; // Create table
			case '2': scene->createChair();	break; // Create chair
			case '3': scene->createCabinet(); break; // Create filing cabinet
			case '4': if (!showNodeMenu && !showMeshMenu) inputMeshFile = true; break; // Create mesh object
			case 13: { scene->renderScene(camera.GetEye(), camera.GetFrontVector(), camera.GetUpVector(), camera.GetFovY(), render_setting); } break; // Enter key
			case ' ': scene->reset(); break; // Reset everything
			case 27: exit(0); break; // Esc key
			default: break;
		}
	}
	glutPostRedisplay();
}

void processKeyboardSpecial(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT: {
			// Root node is always nothing
			if (scene->getSelectedNode() != scene->getRootNode()) {
				int material = scene->getSelectedNode()->getMaterial();
				if (material == NUM_OF_MATERIAL) scene->updateMaterial(0, scene->getSelectedNode());
				else scene->updateMaterial(++material, scene->getSelectedNode());
			}
			break;
		}
		case GLUT_KEY_RIGHT: {
			// Root node is always nothing
			if (scene->getSelectedNode() != scene->getRootNode()) {
				int material = scene->getSelectedNode()->getMaterial();
				if (material == 0) scene->updateMaterial(NUM_OF_MATERIAL, scene->getSelectedNode());
				else scene->updateMaterial(--material, scene->getSelectedNode());
			}
			break;
		}
		case GLUT_KEY_UP: {
			// Root node is always nothing
			if (scene->getSelectedNode() != scene->getRootNode()) {
				int type = scene->getSelectedNode()->getType();
				if (scene->getSelectedNode()->getMeshChecksum()) {
					if (type == NUM_OF_TYPE + 1) scene->getSelectedNode()->setType(0);
					else scene->getSelectedNode()->setType(++type);
				}
				else {
					if (type == NUM_OF_TYPE) scene->getSelectedNode()->setType(0);
					else scene->getSelectedNode()->setType(++type);
				}
			}
			break;
		}
		case GLUT_KEY_DOWN: {
			// Root node is always nothing
			if (scene->getSelectedNode() != scene->getRootNode()) {
				int type = scene->getSelectedNode()->getType();
				if (scene->getSelectedNode()->getMeshChecksum()) {
					if (type == 0) scene->getSelectedNode()->setType(NUM_OF_TYPE + 1);
					else scene->getSelectedNode()->setType(--type);
				}
				else {
					if (type == 0) scene->getSelectedNode()->setType(NUM_OF_TYPE);
					else scene->getSelectedNode()->setType(--type);
				}
			}
			break;
		}
		case GLUT_KEY_F1: {
			if (!showNodeMenu && !showMeshMenu)
				showHelp = !showHelp;
			break;
		}
		case GLUT_KEY_F2: {
			showInfo = !showInfo;
			break;
		}
		default: break;
	}
}

void processMouse(int button, int state, int x, int y)
{
	theButtonState = button;
	theModifierState = glutGetModifiers();

	if (state == GLUT_DOWN) {
		processMouseMotion(x, y);
	}
	else {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}

void processMouseMotion(int x, int y)
{
	// Rotate camera
	if (theButtonState == GLUT_LEFT_BUTTON && theModifierState == GLUT_ACTIVE_ALT) {
		if (lastMouseX != -1) {
			dy = y - lastMouseY;
			dx = x - lastMouseX;
			camera.RotateCenter(-dx, -dy);
			renderDisplay();
		}
		lastMouseX = x;
		lastMouseY = y;
	}
	// Move camera
	else if (theButtonState == GLUT_MIDDLE_BUTTON && theModifierState == GLUT_ACTIVE_ALT) {
		if (lastMouseX != -1) {
			dx = (x - lastMouseX);
			dy = (y - lastMouseY);

			camera.MoveSWD(-dx, dy);
			renderDisplay();
		}
		lastMouseX = x;
		lastMouseY = y;
	}
	// Zoom camera
	else if (theButtonState == GLUT_RIGHT_BUTTON && theModifierState == GLUT_ACTIVE_ALT) {
		if (lastMouseY != -1) {			
			camera.Zoom(((y - lastMouseY) + (x - lastMouseX)) / 2.0f);
			renderDisplay();
		}
		lastMouseY = y;
		lastMouseX = x;
	}
	else {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}