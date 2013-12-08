#include <fstream>
#include <iostream>
#include <time.h>
#include "sceneGraph.h"
#include "raytrace.h"
using namespace glm;

unsigned int eyePositionShaderIndex;
unsigned int lightColorShaderIndex;
unsigned int lightLocation;
unsigned int ambientColor;
glm::vec3 eyeVector;
glm::vec3 upVector;
glm::vec3 viewVector;
glm::vec3 lightPos;
glm::vec3 ambColor;
raytrace *raytrace1;
float xCamera;

//vertex arrays needed for drawing
unsigned int vbo;
unsigned int cbo;
unsigned int nbo;
unsigned int ibo;

//attributes
glm::vec3 lightColor = glm::vec3(1.0,1.0,1.0);
unsigned int positionLocation = 0;
unsigned int colorLocation = 1;
unsigned int normalLocation = 2;

//uniforms
unsigned int u_modelMatrixLocation;
unsigned int u_projMatrixLocation;
	
//needed to compile and link and use the shaders
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

//Animation/transformation stuff
clock_t old;
float rotation = 0.0f;

//helper function to read shader source and put it in a char array
//thanks to Swiftless
char* textFileRead(const char*);

//some other helper functions from CIS 565
void printLinkInfoLog(int);
void printShaderInfoLog(int);

//standard glut-based program functions
void init(void);
void resize(int, int);
void display(void);
void keypress(unsigned char, int, int);
void cleanup(void);

void createFloor(mat4 modelView);

sceneGraph* myScene;
void performRaytrace();

void displayText();

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Starter Code");

	//Call GLEW only _after_ you get the window
	//I had to tell the author of your textbook that ;-)  -Cory
	glewInit();

	init();
	raytrace1 = new raytrace("raytracer_config_sampleTEST.txt");
	sceneGraph *sc1 = new sceneGraph(argv[1], raytrace1->materials);
	myScene = sc1;

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keypress);
	glutIdleFunc(display);

	glutMainLoop();
	return 0;
}

void init() {
	xCamera = 0.f;
	// Set up the camera attributes
	eyeVector = glm::vec3(10,10,0);
	viewVector = glm::vec3(0,0,0);
	upVector = glm::vec3(0,1,0);

	//Create the VBOs and IBO we'll be using to render images in OpenGL
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &cbo);
	glGenBuffers(1, &nbo);
	glGenBuffers(1, &ibo);
	
	//Everybody does this
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);

	//here is stuff for setting up our shaders
	const char* fragFile = "diffuseFrag.frag";
	const char* vertFile = "diffuseVert.vert";
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();
	
	//load up the source, compile and link the shader program
	const char* vertSource = textFileRead(vertFile);
	const char* fragSource = textFileRead(fragFile);
	glShaderSource(vertexShader, 1, &vertSource, 0);
	glShaderSource(fragmentShader, 1, &fragSource, 0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	//For your convenience, i decided to throw in some compiler/linker output helper functions
	//from CIS 565
	GLint compiled;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printShaderInfoLog(vertexShader);
	} 
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		printShaderInfoLog(fragmentShader);
	} 

	//set the attribute locations for our shaders
	glBindAttribLocation(shaderProgram, positionLocation, "vs_position");
	glBindAttribLocation(shaderProgram, normalLocation, "vs_normal");
	glBindAttribLocation(shaderProgram, colorLocation, "vs_color");

	//finish shader setup
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//check for linking success
	GLint linked;
	glGetProgramiv(shaderProgram,GL_LINK_STATUS, &linked);
	if (!linked) 
	{
		printLinkInfoLog(shaderProgram);
	}

	//Get the uniform locations for our shaders, unfortunately they can not be set by us, we have
	//to ask OpenGL for them
	u_modelMatrixLocation = glGetUniformLocation(shaderProgram, "u_modelMatrix");
	u_projMatrixLocation = glGetUniformLocation(shaderProgram, "u_projMatrix");
	
	lightLocation = glGetUniformLocation(shaderProgram, "lightPos");
	ambientColor = glGetUniformLocation(shaderProgram, "ambient");
	eyePositionShaderIndex = glGetUniformLocation(shaderProgram, "vEyePosition");
	lightColorShaderIndex = glGetUniformLocation(shaderProgram, "vSpecularMaterial");
	
	//Always remember that it doesn't do much good if you don't have OpenGL actually use the shaders
	glUseProgram(shaderProgram);

	// set the light color
	glUniform3fv(lightColorShaderIndex,1,&lightColor[0]);

	lightPos = glm::vec3(0.f,5.f,0.f);
	glUniform3fv(lightLocation,1,&lightPos[0]);

	ambColor = glm::vec3(0.f,0.f,0.f);
	glUniform3fv(ambientColor,1,&ambColor[0]);

	glUniform3fv(eyePositionShaderIndex,1,&eyeVector[0]);

	resize(1280, 720);
	old = clock();
}

void cleanup() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &cbo);
	glDeleteBuffers(1, &nbo);
	glDeleteBuffers(1, &ibo);

	//Tear down the shader program in reverse of building it
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(shaderProgram);
}

void keypress(unsigned char key, int x, int y) {
	switch(key) {
	case 'q':
		cleanup();
		exit(0);
		break;
	case 's':
		myScene->pressS();
		glutPostRedisplay();
		break;
	case 'n':
		myScene->goToNextNode();
		glutPostRedisplay();
		break;
	case 'a':
		myScene->pressA();
		glutPostRedisplay();
		break;
	case 'd':
		myScene->pressD();
		glutPostRedisplay();
		break;
	case 'w':
		myScene->pressW();
		glutPostRedisplay();
		break;
	case 'x':
		myScene->pressLittleX();
		glutPostRedisplay();
		break;
	case 'X':
		myScene->pressBigX();
		glutPostRedisplay();
		break;
	case 'y':
		myScene->pressLittleY();
		glutPostRedisplay();
		break;
	case 'Y':
		myScene->pressBigY();
		glutPostRedisplay();
		break;
	case 'z':
		myScene->pressLittleZ();
		glutPostRedisplay();
		break;
	case 'Z':
		myScene->pressBigZ();
		glutPostRedisplay();
		break;
	case 'r':
		myScene->pressLittleR();
		glutPostRedisplay();
		break;
	case 'R':
		myScene->pressBigR();
		glutPostRedisplay();
		break;
	case 'e':
		myScene->pressE();
		glutPostRedisplay();
		break;
	case '7':
		lightPos = lightPos + glm::vec3(0.f,0.5,0.f);
		glUniform3fv(lightLocation,1,&lightPos[0]);
		cout << lightPos.y << endl;
		break;
	case '8':
		lightPos = lightPos + glm::vec3(0.f,-0.5,0.f);
		glUniform3fv(lightLocation,1,&lightPos[0]);
		cout << lightPos.y << endl;
		break;
	case '9':
		lightPos = lightPos + glm::vec3(0.5f,0.f,0.f);
		glUniform3fv(lightLocation,1,&lightPos[0]);
		break;
	case '0':
		lightPos = lightPos + glm::vec3(-0.5f,0.f,0.f);
		glUniform3fv(lightLocation,1,&lightPos[0]);
		break;
	case '1':
		lightPos = lightPos + glm::vec3(0.f,0.f,0.5f);
		glUniform3fv(lightLocation,1,&lightPos[0]);
		break;
	case '2':
		lightPos = lightPos + glm::vec3(0.f,0.f,-0.5f);
		glUniform3fv(lightLocation,1,&lightPos[0]);
		break;
	case 'p':
		performRaytrace();
		break;
	}
	glutPostRedisplay();
}

void display() {
	//Always and only do this at the start of a frame, it wipes the slate clean
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	clock_t newTime = clock();

	//part of the animation
	rotation -= 50 * (static_cast<float>(newTime - old) / static_cast<float>(CLOCKS_PER_SEC));

	//create an identity matrix for the modelview matrix
	glm::mat4 modelView = glm::mat4(1.0);

	//Draw the two components of our scene separately, for your scenegraphs it will help
	//your sanity to do separate draws for each type of primitive geometry, otherwise your VBOs will
	//get very, very complicated fast
	//createRedSquare(modelView);	//PASS MODELVIEW BY COPY to get the nice scoping for hierarchical (i'm sure i spelled that wrong) transformations
								//like Norm mentioned in his Scene Graph lecture
	//createBlueSquare(modelView);
	modelView = glm::rotate(modelView, rotation, glm::vec3(0, 1, 0));

	myScene->root->traverse(vbo, cbo, ibo, nbo, positionLocation,colorLocation,normalLocation, u_modelMatrixLocation, modelView);
	glutSwapBuffers();
	old = newTime;
}

void resize(int width, int height) {
	//set the viewport, more boilerplate
	glViewport(0, 0, width, height);

	//
	glm::mat4 projection = glm::perspective(60.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 30.0f);
	glm::mat4 camera2 = glm::lookAt(eyeVector, viewVector, upVector);
	projection = projection * camera2;

	//set the projection matrix here, it only needs to be changed if the screen is resized otherwise it can stay the same
	glUniformMatrix4fv(u_projMatrixLocation, 1, GL_FALSE, &projection[0][0]);
	glutPostRedisplay();
}

//from swiftless.com
char* textFileRead(const char* fileName) {
    char* text;
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

void printLinkInfoLog(int prog) 
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetProgramInfoLog(prog,infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}
}

void printShaderInfoLog(int shader)
{
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	// should additionally check for OpenGL errors here

	if (infoLogLen > 0)
	{
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
		std::cout << "InfoLog:" << std::endl << infoLog << std::endl;
		delete [] infoLog;
	}

	// should additionally check for OpenGL errors here
}

void performRaytrace() {
	
	eyeVector = raytrace1->EYEP;
	upVector = raytrace1->UVEC;
	viewVector = raytrace1->VDIR;
	glUniform3fv(eyePositionShaderIndex,1,&eyeVector[0]);
	glUniform3fv(lightLocation,1,&raytrace1->LPOS[0]);
	glUniform3fv(lightColorShaderIndex,1,&raytrace1->LCOL[0]);
	glUniform3fv(ambientColor,1,&raytrace1->ACOL[0]);
	resize(1280, 720);

	raytrace1->runRaytrace(myScene->root);
}

void displayText() {
	glColor3f(0.0, 1.0, 0.0); // Green

	glRasterPos2i(10, 10);

	string s = "Respect mah authoritah!";
	void * font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(font, c);
	}
}