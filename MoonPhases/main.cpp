/*Published under The MIT License (MIT)

See LICENSE.TXT*/

// Christopher Hernandez chris00hernandez@gmail.com


//Building upon https://github.com/RyanPridgeon/solarsystem to emphasize the phases of the moon.


#include <cmath>
#include <cstdlib>
#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif
#include <iostream>

#include "tga.h"
#include "solarsystem.h"
#include "camera.h"
#include "globals.h"

// the screen size
int screenWidth, screenHeight;

// The TGA texture containing the help dialogue and starfield and moon texture
TGA *stars, *moon, *newMoon;

// toggles if orbits are drawn
bool showOrbits = false;
// holds the index of the last planet that was selected with the 1 to 9 number keys
int planetSelected = 1;

// The main instance of the solar system
SolarSystem solarSystem;

// The instance of the camera
Camera camera;

// These control the simulation of time
double _time;
double timeSpeed;

// holds the state of the controls for the camera - when true, the key for that control is being pressed
struct ControlStates
{
	bool forward, backward, left, right, yawLeft, yawRight, pitchUp,
		pitchDown, rollLeft, rollRight;
} controls;


// timer function called every 10ms or more
void timer(int)
{
	glutPostRedisplay(); // post for display func
	glutTimerFunc(10, timer, 0); // limit frame drawing to 100fps
}

// creates a random number up to the max specified
float random(float max)
{
	return (float)(std::rand() % 1000) * max * 0.001;
}

// adds a moon to the selected planet
void addMoon()
{
	// make a moon using random values
	solarSystem.addMoon(planetSelected,
		(500 + random(1500)) * solarSystem.getRadiusOfPlanet(planetSelected),
		10 + random(100), 0.5 + random(20),
		solarSystem.getRadiusOfPlanet(planetSelected) * (0.05f + random(0.2f)), moon->getTextureHandle());
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);

	// set up lighting
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat matAmbience[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat matShininess[] = { 20.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbience);

	GLfloat lightAmbient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat lightDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	// Load all image data
	stars = new TGA("images/stars.tga");
	moon = new TGA("images/moon.tga");

	TGA* sun = new TGA("images/sun.tga");
	TGA* earth = new TGA("images/earth.tga");


	// Add all the planets with accurate data. Distance measured in km, time measured in earth days.
	solarSystem.addPlanet(0, 1, 500, .8 * 69500, sun->getTextureHandle()); // sun
	solarSystem.addPlanet(149600000, 365, 1, 13 * 5000, earth->getTextureHandle()); // earth
	solarSystem.addMoon(1, 11 * 7000000, 27.3, 60, 6 * 10000, moon->getTextureHandle()); // test moon for the earth

	// set up time
	_time = 1.0f;
	timeSpeed = 0.015f;

	// reset controls
	controls.forward = false;
	controls.backward = false;
	controls.left = false;
	controls.right = false;
	controls.rollRight = false;
	controls.rollLeft = false;
	controls.pitchDown = false;
	controls.pitchUp = false;
	controls.yawLeft = false;
	controls.yawRight = false;
	

	timer(0);
}

void drawCube(void);

void drawImage(void) {
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(200.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(200.0f, 200.0f);
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.0f, 200.0f);
	glEnd();
}

void display(void)
{
	// update the logic and simulation
	_time += timeSpeed;
	if (_time >= 60) _time = 0; // Only show 1 month

	solarSystem.calculatePositions(_time);
	
	if (controls.forward) camera.forward();		if (controls.backward) camera.backward();
	if (controls.left) camera.left();			if (controls.right) camera.right();
	if (controls.yawLeft) camera.yawLeft();		if (controls.yawRight) camera.yawRight();
	if (controls.rollLeft) camera.rollLeft();	if (controls.rollRight) camera.rollRight();
	if (controls.pitchUp) camera.pitchUp();		if (controls.pitchDown) camera.pitchDown();
	
	// clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);


	// set up the perspective matrix for rendering the 3d world
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, (float)screenWidth / (float)screenHeight, 0.001f, 500.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// perform the camera orientation transform
	camera.transformOrientation();

	// draw the skybox
	glBindTexture(GL_TEXTURE_2D, stars->getTextureHandle());
	drawCube();

	// perform the camera translation transform
	camera.transformTranslation();



	GLfloat lightPosition[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// render the solar system
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	solarSystem.render();
	glDisable(GL_LIGHTING);

	// possibly render orbits
	if (showOrbits)
		solarSystem.renderOrbits();

	glDisable(GL_DEPTH_TEST);

	// set up ortho matrix for showing the UI (help dialogue)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)screenWidth, (GLdouble)screenHeight, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glFlush();
	glutSwapBuffers();
}

void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '-':
		timeSpeed /= 2.0f; // half the rate of time passing
		break;
	case '=':
		timeSpeed *= 2.0f; // double the rate of time passing
		break;
	case 'o':
		showOrbits = !showOrbits; // toggle show orbits
		break;
	case ',':
		camera.slowDown(); // slow down camera
		break;
	case '.':
		camera.speedUp(); // speed up camera
		break;
	// these are all camera controls
	case 'w':
		controls.forward = true;
		break;
	case 's':
		controls.backward = true;
		break;
	case 'a':
		controls.left = true;
		break;
	case 'd':
		controls.right = true;
		break;
	case 'l':
		controls.rollRight = true;
		break;
	case 'j':
		controls.rollLeft = true;
		break;
	case 'i':
		controls.pitchDown = true;
		break;
	case 'k':
		controls.pitchUp = true;
		break;
	case 'q':
		controls.yawLeft = true;
		break;
	case 'e':
		controls.yawRight = true;
		break;
	}

}

void keyUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		controls.forward = false;
		break;
	case 's':
		controls.backward = false;
		break;
	case 'a':
		controls.left = false;
		break;
	case 'd':
		controls.right = false;
		break;
	case 'l':
		controls.rollRight = false;
		break;
	case 'j':
		controls.rollLeft = false;
		break;
	case 'i':
		controls.pitchDown = false;
		break;
	case 'k':
		controls.pitchUp = false;
		break;
	case 'q':
		controls.yawLeft = false;
		break;
	case 'e':
		controls.yawRight = false;
		break;
	}
}

void reshape(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void printInstructions() {
	std::cout << "======== Instructions ========" << std::endl;
	std::cout << "+	increases speed" << std::endl;
	std::cout << "-	decreases speed" << std::endl;
	std::cout << "o	show orbits" << std::endl;
	std::cout << "w	camera forward" << std::endl;
	std::cout << "s	camera backward" << std::endl;
	std::cout << "a	camera left" << std::endl;
	std::cout << "d	camera right" << std::endl;
	std::cout << "l	camera roll right" << std::endl;
	std::cout << "j	camera roll left" << std::endl;
	std::cout << "i	camera pitch down" << std::endl;
	std::cout << "k	camera pitch up" << std::endl;
	std::cout << "q	camera yaw left" << std::endl;
	std::cout << "e	camera yaw right" << std::endl;
	std::cout << ".	increase camera speed" << std::endl;
	std::cout << ",	decrease camera speed" << std::endl;
	std::cout << "==============================" << std::endl;
}

int main(int argc, char** argv)
{
	printInstructions();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1200, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

	glutMainLoop();
	return 0;
}

void drawCube(void)
{
	glBegin(GL_QUADS);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(1.0f, -1.0f, 1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(1.0f, -1.0f, -1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, -1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd();
}
