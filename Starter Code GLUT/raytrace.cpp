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
	mat4 rot = rotate(45.f,vec3(0,1,0));
	mat4 trans = translate(vec3(0,1,0));
	mat4 scal = scale(vec3(1,1,1));
	mat4 out = scal;
	mat4 invOut = inverse(out);

	mesh *mesh1 = new mesh("column.dat",vec3(0.5,0.5,0.5));
	int NUMBOUNCES = 1;
	for (int i = 0; i < RESOX; i++) {
		for (int j = 0; j < RESOY; j++) {
			int xPixel = i;
			int yPixel = (RESOY -1) - j;
			// Perform raytrace per pixel
			vec3 ray = mainCamera->getDirectionFromCoordinate(i,j);
			
			// get shaded value
			intersectionPoint outputIntersection = recursiveTrace(root,mainCamera->eye,ray);
			if (outputIntersection.tValue > 0) {
				vec3 color;
				float reflec = outputIntersection.node->objMaterial->reflectivityCoefficient;
				vec3 mater = outputIntersection.node->objMaterial->getColor(outputIntersection.point,outputIntersection.normal,mainCamera->eye,LPOS,LCOL);
				//get shadow
				// cast trace from point to light, if there is an inerseciton, return ambient
				intersectionPoint shadowIntersection = recursiveTrace(root,vec3(0.03) + outputIntersection.point, normalize(LPOS - outputIntersection.point));

				if (shadowIntersection.tValue > 0.0001) {
					color = ACOL;
					output(xPixel,yPixel)->Red = color.x * 255;
					output(xPixel,yPixel)->Green = color.y * 255;
					output(xPixel,yPixel)->Blue = color.z * 255;
				}

				else {
					if (outputIntersection.node->objMaterial->reflectivityCoefficient == 0) {
						color = mater;
						output(xPixel,yPixel)->Red = color.x * 255;
						output(xPixel,yPixel)->Green = color.y * 255;
						output(xPixel,yPixel)->Blue = color.z * 255;
					}
					else {
						color = (1-reflec) * mater + (reflec *  reflectionTrace(root,outputIntersection.point, normalize(reflect(outputIntersection.point - mainCamera->eye, outputIntersection.normal)),NUMBOUNCES));
						output(xPixel,yPixel)->Red = color.x * 255;
						output(xPixel,yPixel)->Green = color.y * 255;
						output(xPixel,yPixel)->Blue = color.z * 255;
					}
				}
			}

			//test plane
			
			/*
			intersectionPoint plane1 = Test_RayPlaneIntersectInverse(mainCamera->eye,ray,out,invOut,2,2,root);
			if (plane1.tValue > 0) {
				vec3 color = m1->getColor(plane1.point,plane1.normal,mainCamera,LPOS,LCOL);
				output(xPixel,yPixel)->Red = color.x * 255;
				output(xPixel,yPixel)->Green = color.y * 255;
				output(xPixel,yPixel)->Blue = color.z * 255;
			}*/

			/*
			intersectionPoint cyl = Test_RayCylinderIntersectInverse(mainCamera->eye,ray,out,invOut,0.5,0.5,root);
			if (cyl.tValue > 0) {
				vec3 color = m1->getColor(cyl.point,cyl.normal,mainCamera,LPOS,LCOL);
				output(xPixel,yPixel)->Red = color.x * 255;
				output(xPixel,yPixel)->Green = color.y * 255;
				output(xPixel,yPixel)->Blue = color.z * 255;
			}*/
			
			// For meshes, transform the values before testing the intersections, DO NOT PASS IN THE TRANSFORMATION MATRICES
			/*intersectionPoint poly = Test_RayPolyIntersectInverse(mainCamera->eye,ray,out,invOut,root,mesh1->faces);
			if (poly.tValue > 0) {
				vec3 color = m1->getColor(poly.point,poly.normal,mainCamera,LPOS,LCOL);
				output(i,j)->Red = color.x * 255;
				output(i,j)->Green = color.y * 255;
				output(i,j)->Blue = color.z * 255;
			}*/
			
			/*
			intersectionPoint cubeinter = Test_RayCubeIntersectInverse(mainCamera->eye, ray, out, invOut,root);
			if (cubeinter.tValue >= 0) {
				vec3 color = m1->getColor(cubeinter.point,cubeinter.normal,mainCamera,LPOS,LCOL);
				output(i,j)->Red = color.x * 255;
				output(i,j)->Green = color.y * 255;
				output(i,j)->Blue = color.z * 255;
			}*/
			
			/*
			intersectionPoint is1 = Test_RaySphereIntersectInverse(mainCamera->eye,ray,out,invOut,root);
			if (is1.tValue > 0) {
				vec3 color = m1->getColor(is1.point,is1.normal,mainCamera,LPOS,LCOL);
				output(xPixel,yPixel)->Red = color.x * 255;
				output(xPixel,yPixel)->Green = color.y * 255;
				output(xPixel,yPixel)->Blue = color.z * 255;
			}*/
			
		}
		if (i%100 == 0) {
			cout << ((float)i/RESOX) * 100.f << "% Complete" << endl;
		}
	}
	cout << "FILEWRITE COMPLETE" << endl;
	output.WriteToFile("test1.bmp");
}

vec3 raytrace::reflectionTrace(node* node, vec3 p0, vec3 v0, int iteration) {
	intersectionPoint outputIntersection = recursiveTrace(node,p0,v0);
	if (outputIntersection.tValue > 0 && iteration > 0) {
		float reflec = outputIntersection.node->objMaterial->reflectivityCoefficient;
		vec3 mater = outputIntersection.node->objMaterial->getColor(outputIntersection.point,outputIntersection.normal,p0,LPOS,LCOL);
		vec3 color;
		//get shadow
		// cast trace from point to light, if there is an inerseciton, return ambient
		intersectionPoint shadowIntersection = recursiveTrace(node,vec3(0.03) + outputIntersection.point, normalize(LPOS - outputIntersection.point));

		if (shadowIntersection.tValue > 0.0001) {
			// return shadow
			return ACOL;
		}
		else {
			if (outputIntersection.node->objMaterial->reflectivityCoefficient == 0) {
				color = mater;
			}
			else {
				color = (1-reflec) * mater +  (reflec *  reflectionTrace(node,outputIntersection.point, normalize(reflect(outputIntersection.point - p0, outputIntersection.normal)),iteration--));
			}
		}
		return color;
	}
	else {
		if (outputIntersection.tValue > 0) {
			return outputIntersection.node->objMaterial->getColor(outputIntersection.point,outputIntersection.normal,p0,LPOS,LCOL);
		}
		else {
			return vec3(0,0,0);
		}
	}
}

intersectionPoint raytrace::recursiveTrace(node* sceneNode, vec3 eye, vec3 ray) {
	vector<intersectionPoint> intersectionPoints;
	for (int i = 0; i < sceneNode->nodeFurniture->primatives.size(); i++) {
		intersectionPoint is;
		// check if the poly is a sphere
		if (sceneNode->nodeFurniture->primatives[i]->polyType == primative::primativeTypes::SPHERE) {
			is = Test_RaySphereIntersectInverse(eye,ray,sceneNode->transformation,sceneNode->invertedTransform,sceneNode);
			if (is.tValue > 0) {
				intersectionPoints.push_back(is);
			}
		}
		//cylinder
		else if (sceneNode->nodeFurniture->primatives[i]->polyType == primative::primativeTypes::CYLINDER) {
			is = Test_RayCylinderIntersectInverse(eye,ray,sceneNode->transformation,sceneNode->invertedTransform,sceneNode->nodeFurniture->primatives[i]->WIDTH,sceneNode->nodeFurniture->primatives[i]->HEIGHT,sceneNode);
			if (is.tValue > 0) {
				intersectionPoints.push_back(is);
			}
		}
		//cube
		else if (sceneNode->nodeFurniture->primatives[i]->polyType == primative::primativeTypes::CUBE) {
			is = Test_RayCubeIntersectInverse(eye,ray,sceneNode->transformation,sceneNode->invertedTransform,sceneNode);
			if (is.tValue > 0) {
				intersectionPoints.push_back(is);
			}
		}
		//plane
		else if (sceneNode->nodeFurniture->primatives[i]->polyType == primative::primativeTypes::PLANE) {
			is = Test_RayPlaneIntersectInverse(eye,ray,sceneNode->transformation,sceneNode->invertedTransform,sceneNode->nodeFurniture->primatives[i]->WIDTH,sceneNode->nodeFurniture->primatives[i]->HEIGHT,sceneNode);
			if (is.tValue > 0) {
				intersectionPoints.push_back(is);
			}
			if (is.tValue > 0) {
				int hey = 0;
			}
		}
		// or mesh
		else {
			is = Test_RayPolyIntersectInverse(eye,ray,sceneNode->transformation,sceneNode->invertedTransform,sceneNode,sceneNode->nodeFurniture->primatives[0]->faces);
			if (is.tValue > 0) {
				intersectionPoints.push_back(is);
			}
			if (is.tValue > 0) {
				int hey = 0;
			}
		}
	}

	if (sceneNode->children != NULL) {
		for (int j = 0; j < sceneNode->children->size(); j++) {
			intersectionPoints.push_back(recursiveTrace(sceneNode->children->at(j),eye,ray));
		}
	}

	//if there were any intersections, go through them and find the smallest
	if (intersectionPoints.size() > 0) {
		intersectionPoint minT = intersectionPoints[0];
		//for the size of the array set minT to the smallest t value intersection
		for (int i = 0; i < intersectionPoints.size(); i++) {
			if (intersectionPoints[i].tValue < minT.tValue && intersectionPoints[i].tValue > 0) {
				minT = intersectionPoints[i];
			}
		}
		return minT;
	}
	else {
		intersectionPoint failed;
		failed.tValue = -1;
	}
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