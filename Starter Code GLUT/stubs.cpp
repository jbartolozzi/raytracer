#include "stubs.h"

using namespace glm;

intersectionPoint Test_RaySphereIntersect(vec3 const& P0, vec3 const& V0, mat4 const& T) {
	glm::vec4 newP0 = glm::inverse(T) * glm::vec4(P0,1.f);
	glm::vec4 newV0 = glm::inverse(T) * glm::vec4(V0,0.f);
	glm::vec3 P01 = glm::vec3(newP0.x,newP0.y,newP0.z);
	glm::vec3 V01 = glm::vec3(newV0.x,newV0.y,newV0.z);
	return Test_RaySphereIntersectInverse(P01,V01,T);
}

intersectionPoint Test_RaySphereIntersectInverse(vec3 const& P0, vec3 const& V0, mat4 const& T) {
	intersectionPoint output;
	output.tValue = -1;
	output.normal = vec3(0,0,0);
	output.point = vec3(0,0,0);

	vec4 P02 = T * vec4(P0,1);
	vec4 V02 = T * vec4(V0,1);
	vec3 P03 = vec3(P02.x,P02.y,P02.z);
	vec3 V03 = vec3(V02.x,V02.y,V02.z);

	float A = glm::dot(V03,V03);
	float B = 2*(glm::dot(V03,P03));
	float C = glm::dot(P03,P03) - 1;
	float discriminant = B*B - (4*A*C);
	if (discriminant < 0) {
		return output;
	}
	else {
		float t0 = (-B - glm::sqrt(discriminant)) / 2*A;
		float t1 = (-B + glm::sqrt(discriminant)) / 2*A;
		if (t0 < 0 && t1 >= 0) {
			output.tValue = t1;
			output.normal = normalize(P03 + t1 * V03);
			output.point = P03 + t1*V03;
			return output;
		}
		if (t1 < 0 && t0 >= 0) {
			output.tValue = t0;
			output.normal = normalize(P03 + t0 * V03);
			output.point = P03 + t0*V03;
			return output;
		}
		float min = glm::min(t0,t1);
		if (min > 0) {
			output.tValue = min;
			output.normal = normalize(P03 + min * V03);
			output.point = P03 + min*V03; 
			return output;
		}
		else {
			return output;
		}
	}
}

intersectionPoint Test_RayPolyIntersect(vec3 const& P0, vec3 const& V0, vec3 const& p1, vec3 const& p2, vec3 const& p3, mat4 const& T) {
	glm::vec4 newP0 = glm::inverse(T) * glm::vec4(P0,1.f);
	glm::vec4 newV0 = glm::inverse(T) * glm::vec4(V0,0.f);
	glm::vec3 P01 = glm::vec3(newP0.x,newP0.y,newP0.z);
	glm::vec3 V01 = glm::vec3(newV0.x,newV0.y,newV0.z);
	return Test_RayPolyIntersectInverse(P01,V01,p1,p2,p3,T);
}
intersectionPoint Test_RayPolyIntersectInverse(vec3 const& P0, vec3 const& V0, vec3 const& p1, vec3 const& p2, vec3 const& p3, mat4 const& T) {
	intersectionPoint output;
	output.tValue = -1;
	output.normal = vec3(0,0,0);

	vec4 P02 = T * vec4(P0,1);
	vec4 V02 = T * vec4(V0,1);
	vec3 P03 = vec3(P02.x,P02.y,P02.z);
	vec3 V03 = vec3(V02.x,V02.y,V02.z);


	glm::vec3 E1 = p2 - p1;
	glm::vec3 E2 = p3 - p1;
	glm::vec3 T1 = P03 - p1;
	glm::vec3 D = V03;
	glm::vec3 P = glm::cross(D,E2);
	glm::vec3 Q = glm::cross(T1,E1);
	glm::vec3 tuv = (1.f / (glm::dot(P,E1))) * glm::vec3(glm::dot(Q,E2),glm::dot(P,T1),glm::dot(Q,D));
	
	if (tuv.y + tuv.z > 1 || tuv.y < 0 || tuv.z < 0) {
		return output;
	}
	else {
		output.tValue = tuv.x;
		output.normal = normalize(cross(p2-p1,p3-p1));
		output.point = P03 + tuv.x * V03;
		return output;
	}
}


intersectionPoint Test_RayCubeIntersect(vec3 const& P0, vec3 const& V0, mat4 const& T) {
	glm::vec4 newP0 = glm::inverse(T) * glm::vec4(P0,1.f);
	glm::vec4 newV0 = glm::inverse(T) * glm::vec4(V0,0.f);
	glm::vec3 P01 = glm::vec3(newP0.x,newP0.y,newP0.z);
	glm::vec3 V01 = glm::vec3(newV0.x,newV0.y,newV0.z);
	return Test_RayCubeIntersectInverse(P01,V01,T);
}

intersectionPoint Test_RayCubeIntersectInverse(vec3 const& P0, vec3 const& V0, mat4 const& T) {
	intersectionPoint output;
	std::vector<float> tees;
	// top face
	glm::vec3 norm = glm::vec3(0,1,0);
	glm::vec3 p0 = glm::vec3(0,0.5,0);
	float t1 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);
	// bottom face
	norm = glm::vec3(0,-1,0);
	p0 = glm::vec3(0,-0.5,0);
	float t2 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);
	
	// front face
	norm = glm::vec3(0,0,1);
	p0 = glm::vec3(0,0,0.5);
	float t3 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);

	// back face
	norm = glm::vec3(0,0,-1);
	p0 = glm::vec3(0,0,-0.5);
	float t4 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);

	// left face
	norm = glm::vec3(-1,0,0);
	p0 = glm::vec3(-0.5,0,0);
	float t5 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);
	// right face
	norm = glm::vec3(1,0,0);
	p0 = glm::vec3(0.5,0,0);
	float t6 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);
	

	tees.push_back(t1);
	tees.push_back(t2);
	tees.push_back(t3);
	tees.push_back(t4);
	tees.push_back(t5);
	tees.push_back(t6);
	float min = t1;
	for(unsigned int i = 0; i < tees.size(); i++) {
		if (tees[i] > 0 && tees[i] < min) {
			min = tees[i];
		}
	}
	vec3 checkPoint = P0 + min*V0;
	if (checkPoint.x <= 0.5 && checkPoint.y <= 0.5 && checkPoint.z <= 0.5
		&& checkPoint.x >= -0.5 && checkPoint.y >= -0.5 && checkPoint.z >= -0.5 && min >= 0) {
			output.tValue = min;
			// find the largest magnitude of x,y,z position
			return output;
	}
	else {
		return output;
	}
}

intersectionPoint Test_RayCylinderIntersect(vec3 const& P0, vec3 const& V0, mat4 const& T) {
	glm::vec4 newP0 = glm::inverse(T) * glm::vec4(P0,1.f);
	glm::vec4 newV0 = glm::inverse(T) * glm::vec4(V0,0.f);
	glm::vec3 P01 = glm::vec3(newP0.x,newP0.y,newP0.z);
	glm::vec3 V01 = glm::vec3(newV0.x,newV0.y,newV0.z);
	return Test_RayCylinderIntersectInverse(P01,V01,T);
}

intersectionPoint Test_RayCylinderIntersectInverse(vec3 const& P0, vec3 const& V0, mat4 const& T) {
	intersectionPoint output;
	output.tValue = -1;
	output.normal = vec3(0,0,0);

	std::vector<float> tees;
	vec3 AB = vec3(0,0.5,0) - vec3(0,-0.5,0);
	vec3 AO = P0 - vec3(0,-0.5,0);

	float AB_dot_d  = glm::dot(AB,V0);
	float AB_dot_AO = glm::dot(AB,AO);
	float AB_dot_AB = glm::dot(AB,AB);
	float m = AB_dot_d / AB_dot_AB;
	float n = AB_dot_AO / AB_dot_AB;

	vec3 Q = V0 - (AB * m);
	vec3 R = AO - (AB * n);

	float a = glm::dot(Q,Q);
	float b = 2.f * glm::dot(Q,R);
	float c = glm::dot(R,R) - 0.5*0.5;

	float discriminant = b * b - 4.f * a * c;
	if (discriminant >= 0.f) {
		float tmin = (-b - glm::sqrt(discriminant)) / (2.f * a);
		float tmax = (-b + glm::sqrt(discriminant)) / (2.f * a);
		if (tmin < 0 && tmax >= 0) {
			tees.push_back(tmax);
		}
		else if (tmax < 0 && tmin >= 0) {
			tees.push_back(tmin);
		}
		if (tmin > tmax) {
			float temp = tmin;
			tmin = tmax;
			tmax = temp;
		}
		if (tmin >= 0) {
			tees.push_back(tmin);
		}
	}
	
	// top
	vec3 norm = glm::vec3(0,1.f,0);
	vec3 p0 = glm::vec3(0,0.5,0);
	float t3;
	if (glm::dot(V0,norm)) {
		t3 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);
	}
	else {
		t3 = 0;
	}
	vec3 checkt3 = P0 + t3*V0;
	if (checkt3.x*checkt3.x + checkt3.z*checkt3.z <= 0.5*0.5) {
		tees.push_back(t3);
	}
	
	// bottom
	norm = glm::vec3(0,-1,0);
	p0 = glm::vec3(0,-0.5,0);
	float t4;
	if (glm::dot(V0,norm)) {
		t4 = glm::dot((p0 - P0),norm) / glm::dot(V0,norm);
	}
	else {
		t4 = 0;
	}
	vec3 checkT4 = P0 + t4*V0;
	if (checkT4.x*checkT4.x + checkT4.z*checkT4.z <= 0.5*0.5) {
		tees.push_back(t4);
	}

	if (tees.size() < 1) {
		return output;
	}
	else {
		float min = 1000000;
		for (unsigned int i = 0; i < tees.size(); i++) {
			if (tees[i] < min && tees[i] > 0) {
				min = tees[i];
			}
		}
		output.tValue = min;
		// find where the point is,
		// check y values for caps
		// otherwise == distance from centerpoint on y axis
		return output;
	}
}