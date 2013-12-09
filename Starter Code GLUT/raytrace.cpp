#include "raytrace.h"

raytrace::raytrace(char* filename) {
	readFile(filename);
	mainCamera = new camera(EYEP, VDIR, UVEC, FOVY, RESOX, RESOY);
	materials.push_back(&MAT1);
	materials.push_back(&MAT2);
	materials.push_back(&MAT3);
	printStats();
	std::cout << "File Read Successfull" << std::endl;
}

void raytrace::runRaytrace(node* root) {
	BMP output;
	output.SetSize(RESOX, RESOY);
	output.SetBitDepth(24);
	vec3 outR;

	material *m1 = new material();
	
	for (int i = 0; i < RESOX; i++) {
		for (int j = 0; j < RESOY; j++) {
			output(i,j)->Red = 0;
			output(i,j)->Green = 0;
			output(i,j)->Blue = 0;
		}
	}
	mat4 rot = rotate(-80.f,vec3(1,0,0));
	mat4 trans = translate(vec3(0.2,0,0));
	mat4 scal = scale(vec3(1,0.5,1));
	mat4 out = rot;
	out = inverse(out);

	mesh *mesh1 = new mesh("column.dat",vec3(0.5,0.5,0.5));

	for (int i = 0; i < RESOX; i++) {
		for (int j = 0; j < RESOY; j++) {
			int xPixel = i;
			int yPixel = (RESOY -1) - j;
			// Perform raytrace per pixel
			vec3 ray = mainCamera->getDirectionFromCoordinate(i,j);
			
			
			intersectionPoint cyl = Test_RayCylinderIntersectInverse(mainCamera->eye,ray,out);
			if (cyl.tValue > 0) {
				vec3 color = m1->getColor(cyl.point,cyl.normal,mainCamera,LPOS,LCOL);
				output(xPixel,yPixel)->Red = color.x * 255;
				output(xPixel,yPixel)->Green = color.y * 255;
				output(xPixel,yPixel)->Blue = color.z * 255;
			}

			/*
			for(int k = 0; k < mesh1->faces.size(); k++) {
				intersectionPoint is3 = Test_RayPolyIntersectInverse(mainCamera->eye, -ray, mesh1->faces[k].ind1,mesh1->faces[k].ind2,mesh1->faces[k].ind3, out);
				if (is3.tValue >= 0) {
					vec3 color = m1->getColor(is3.point,is3.normal,mainCamera,LPOS,LCOL);
					output(i,j)->Red = color.x * 255;
					output(i,j)->Green = color.y * 255;
					output(i,j)->Blue = color.z * 255;
				}
			}
			*/

			/*
			
			intersectionPoint cubeinter = Test_RayCubeIntersectInverse(mainCamera->eye, ray, out);

			if (cubeinter.tValue >= 0) {
				vec3 color = m1->getColor(cubeinter.point,cubeinter.normal,mainCamera,LPOS,LCOL);
					output(i,j)->Red = color.x * 255;
					output(i,j)->Green = color.y * 255;
					output(i,j)->Blue = color.z * 255;
			}
			*/

			/*
			intersectionPoint is1 = Test_RaySphereIntersectInverse(mainCamera->eye,ray,out);
			if (is1.tValue > 0) {
				vec3 color = m1->getColor(is1.point,is1.normal,mainCamera,LPOS,LCOL);
				output(xPixel,yPixel)->Red = color.x * 255;
				output(xPixel,yPixel)->Green = color.y * 255;
				output(xPixel,yPixel)->Blue = color.z * 255;
			}*/
			
		
		}
		if (i%25 == 0) {
			cout << ((float)i/RESOX) * 100.f << "% Complete" << endl;
		}
	}
	cout << "FILEWRITE COMPLETE" << endl;
	output.WriteToFile("test1.bmp");
}

vec3 raytrace::recursiveTrace() {
	return vec3(0,0,0);
}



/////////////////////////////////////////////////////////////////////////////////////////////////
/*                         FILE READER METHODS                                                 */
/////////////////////////////////////////////////////////////////////////////////////////////////

void raytrace::readFile(char* filename) {
	std::string line;
	std::ifstream file(filename);
	while (!file.good()) {
		std::cout << "Invalid File Name Please Try Again: ";
		char* filename = new char[25];
		std::cin >> filename;
		file.open(filename);
	}
	while(!file.eof()) {
		char line[100];
		file.getline(line,100);
		readAttributes(line);
	}
}

void raytrace::readAttributes(char* line) {
	char* attribute = strtok(line, " ");
	if (attribute == nullptr) {
	}
	else if (strcmp(attribute, "FILE")==0) {
		FILE = strtok(NULL, " ");
		//output.WriteToFile((char*)raytraceOutput.c_str());
	}
	else if (strcmp(attribute, "RESO")==0) {
		RESOX = readNextIntToken();
		RESOY = readNextIntToken();
	}
	else if (strcmp(attribute, "EYEP")==0) {
		EYEP = readNextVecToken();
	}
	else if (strcmp(attribute, "VDIR")==0) {
		VDIR = readNextVecToken();
	}
	else if (strcmp(attribute, "UVEC")==0) {
		UVEC = readNextVecToken();
	}
	else if (strcmp(attribute, "FOVY")==0) {
		FOVY = readNextFloatToken();
	}
	else if (strcmp(attribute, "LPOS")==0) {
		LPOS = readNextVecToken();
	}
	else if (strcmp(attribute, "LCOL")==0) {
		LCOL = readNextVecToken();
	}
	else if (strcmp(attribute, "ACOL")==0) {
		ACOL = readNextVecToken();
	}
	else if (strcmp(attribute, "MAT1")==0) {
		MAT1 = readNextMaterial();
	}
	else if (strcmp(attribute, "MAT2")==0) {
		MAT2 = readNextMaterial();
	}
	else if (strcmp(attribute, "MAT3")==0) {
		MAT3 = readNextMaterial();
	}
}

char* raytrace::readNextCharToken(){
	char* token = strtok(NULL, " ");
	return token;
}

int raytrace::readNextIntToken(){
	return atoi(readNextCharToken());
}

float raytrace::readNextFloatToken(){
	return (float) atof(readNextCharToken());
}

vec3 raytrace::readNextVecToken() {
	glm::vec3 newVec;
	newVec.x = readNextFloatToken();
	newVec.y = readNextFloatToken();
	newVec.z = readNextFloatToken();
	return newVec;
}

material raytrace::readNextMaterial() {
	vec3 diffuse = readNextVecToken();
	vec3 amb = ACOL;
	float shine = readNextFloatToken();
	float coeff = readNextFloatToken();

	material output = material(diffuse,amb,shine,coeff);
	return output;
}

void raytrace::printStats() {
	std::cout << "Raytrace file parse stats: " << std::endl << "___________________________" << std::endl;
	std::cout << "Filename is " << (char*)FILE.c_str() << std::endl;
	std::cout << "Width = " << RESOX << std::endl;
	std::cout << "Height = " << RESOY << std::endl;
	std::cout << "Eye = ";
	printVector(EYEP);
	std::cout << "View = ";
	printVector(VDIR);
	std::cout << "Up = ";
	printVector(UVEC);
	std::cout << "Fovy = " << FOVY << std::endl;
	std::cout << "Light Pos = ";
	printVector(LPOS);
	std::cout << "Light Color = ";
	printVector(LCOL);
	std::cout << "Ambient Color = ";
	printVector(ACOL);
	std::cout << std::endl;
	std::cout << "Material 1 = ";
	printVector(MAT1.diffuse);
	std::cout << "Mat1 Specular = " << MAT1.shininess << std::endl;
	std::cout << "Mat1 Reflectivity = " << MAT1.reflectivityCoefficient << std::endl << std::endl;

	std::cout << "Material 2 = ";
	printVector(MAT2.diffuse);
	std::cout << "Mat2 Specular = " << MAT2.shininess << std::endl;
	std::cout << "Mat2 Reflectivity = " << MAT2.reflectivityCoefficient << std::endl << std::endl;

	std::cout << "Material 3 = ";
	printVector(MAT3.diffuse);
	std::cout << "Mat3 Specular = " << MAT3.shininess << std::endl;
	std::cout << "Mat3 Reflectivity = " << MAT3.reflectivityCoefficient << std::endl << std::endl;
}

void raytrace::printVector(vec3 a) {
	std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ")" << std::endl;
}