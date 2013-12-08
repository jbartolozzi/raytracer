#ifndef RAYTRACE
#define RAYTRACE

#include "camera.h"
#include "EasyBMP.h"
#include "stubs.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "material.h"
#include "sceneGraph.h"


using namespace glm;

class raytrace {
public:
	// Attribute values
	camera *mainCamera;
	std::string FILE;
	int RESOX;
	int RESOY;
	vec3 EYEP;
	vec3 VDIR;
	vec3 UVEC;
	float FOVY;
	vec3 LPOS;
	vec3 LCOL;
	vec3 ACOL;
	// materials
	material MAT1;
	material MAT2;
	material MAT3;
	std::vector<material*> materials;
	void readAttributes(char* line);
	char* readNextCharToken();
	int readNextIntToken();
	float readNextFloatToken();
	vec3 readNextVecToken();
	material readNextMaterial();
	void printVector(vec3 a);

	raytrace(char* filename);
	void readFile(char* filename);
	void printStats();

	void runRaytrace(node* root);
	vec3 recursiveTrace();

};

#endif