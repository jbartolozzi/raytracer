#ifndef NODE
#define NODE
#include "furniture.h"
#include "material.h"

class node {
public:
	node* parent;
	vector<node*>* children;
	furniture* nodeFurniture;
	float rotation;
	
	material* objMaterial;
	glm::vec3 scale;
	glm::vec3 translation;
	glm::vec3 color;

	glm::mat4 transformation;
	glm::mat4 invertedTransform;
	
	bool selected;

	// parent, children, funiture, rotation, scale, translation
	node(node* _parent, vector<node*>* _children, furniture* _nodeFurniture, float _rotation, glm::vec3 _scale, glm::vec3 _translation, material* _nodeMaterial) {
		objMaterial = _nodeMaterial;
		parent = _parent;
		children = _children;
		nodeFurniture = _nodeFurniture;
		rotation = _rotation;
		scale = _scale;
		translation = _translation;
		color = objMaterial->diffuse;
		selected = false;
		// Set the inverted transformation matrix to the inversion of the translation vector
	}
	node* head;
	node* selectedNode;
	void traverse(unsigned int _vbo, unsigned int _cbo, unsigned int _ibo, unsigned int _nbo, unsigned int positionLocation,
		unsigned int colorLocation, unsigned int normalLocation, unsigned int u_modelMatrixLocation, glm::mat4 matrix);
};
#endif