
#ifndef __LION_HPP__
#define __LION_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <CSCI441/objects3.hpp>
#include <math.h>
#include <stdio.h>

class Lion {
public:
	Lion(glm::vec3 p, float s, float platformSize);
	void moveForward(int d);
	void moveBackward();
	void eat(float alpacaSize);
	void rotate(int d);
	void moveLegs();
	void moveLegsBack();
	glm::vec3 headPosition();
	void draw(glm::mat4 modelMtx, GLint uniform_modelMtx_loc, 
				GLint uniform_ambient_loc, GLint uniform_diffuse_loc, 
				GLint uniform_specular_loc, GLint uniform_shininess_loc);

	float size;
	glm::vec3 position;
	glm::vec3 direction;
	float speed;
	float platformSize;
private:
	float rotation;
	float rotationLeg;
	float rotationTail;
	float rotationHead;
	float rotationLegAmount;
	float rotationTailAmount;
};

Lion::Lion(glm::vec3 p, float s, float platformSize) {
	position = p;
	direction = glm::vec3(-1,0,0);
	size = s;
	rotation = 0.0f;
	rotationLeg = 0.0f;
	rotationLegAmount = 0.05f;
	rotationTail = 13.0f;
	rotationTailAmount = 0.01f;
	rotationHead = 12.1f;
	speed = 0.03f;
	this->platformSize = platformSize;
}

void Lion::moveForward(int d) {

	glm::vec3 newPosition = position + direction * speed * (float)d;
	if (newPosition.x >= platformSize) { newPosition.x = platformSize; }
	if (newPosition.x <= -platformSize) { newPosition.x = -platformSize; }
	if (newPosition.z >= platformSize) { newPosition.z = platformSize; }
	if (newPosition.z <= -platformSize) { newPosition.z = -platformSize; }
	position = newPosition;
	if (d > 0) {
		moveLegs();
	}
	else {
		moveLegsBack();
	}
}

void Lion::moveBackward() {
	position -= direction * 0.05f;
}

void Lion::eat(float alpacaSize) {
	size += alpacaSize * 0.05f;
	if (size >= 0.5f) {
		size = 0.5f;
	}
	speed = 0.1f * (size/0.5f);
	if (speed <= 0.03f) { speed = 0.03f; }
}

void Lion::rotate(int d) {
	rotation += (float)d * 0.03f;
	direction.z = -sin(rotation);
	direction.x = cos(rotation);
}

void Lion::moveLegs() {
	rotationLeg += rotationLegAmount;
	if (rotationLeg > 0.5f) {
		rotationLegAmount = -rotationLegAmount;
	}
	if (rotationLeg < -0.5f) {
		rotationLegAmount = -rotationLegAmount;
	}

	rotationTail += rotationTailAmount;
	if (rotationTail > 13.2f) {
		rotationTailAmount = -rotationTailAmount;
	}
	if (rotationTail < 12.8f) {
		rotationTailAmount = -rotationTailAmount;
	}
}

void Lion::moveLegsBack() {
	if (rotationLeg > 0.0f) {
		rotationLeg -= 0.1f;
		if (rotationLeg < 0.0f) { rotationLeg = 0.0f; }
	}
	if (rotationLeg < 0.0f) {
		rotationLeg += 0.1f;
		if (rotationLeg > 0.0f) { rotationLeg = 0.0f; }
	}
}

glm::vec3 Lion::headPosition() {
	return position + direction * 2.3f * size;
}

void Lion::draw(glm::mat4 modelMtx, GLint uniform_modelMtx_loc,
				 GLint uniform_ambient_loc, GLint uniform_diffuse_loc, 
				 GLint uniform_specular_loc, GLint uniform_shininess_loc) {

	//glm::vec3 rotAxis = glm::cross(glm::normalize(direction), glm::vec3(0,1,0));
	modelMtx = glm::translate(modelMtx, position);
	modelMtx = glm::translate(modelMtx, glm::vec3(0, 2.5*size,0));
	modelMtx = glm::scale(modelMtx, glm::vec3(size, size, size));
    modelMtx = glm::rotate(modelMtx, rotation, glm::vec3(0,1,0));
	// body
	glm::mat4 modelMtxBody = glm::scale(modelMtx, glm::vec3(2.4,1.3,1.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxBody[0][0]);
	glUniform4f(uniform_ambient_loc, 0.329412f, 0.223529f, 0.027451f, 1.0f);
	glUniform4f(uniform_diffuse_loc, 0.780392f, 0.568627f, 0.113725f, 1.0f);
	glUniform4f(uniform_specular_loc, 0.992157f, 0.941176f, 0.807843f, 1.0f);
	glUniform1f(uniform_shininess_loc, 27.8974f);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	// head

	glm::mat4 modelMtxHead = glm::translate(modelMtx, glm::vec3(2.3, 1.0, 0));
	modelMtxHead = glm::scale(modelMtxHead, glm::vec3(1.2,1.2,1.2));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxHead[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	// front legs
	glm::mat4 modelMtxLeg = glm::translate(modelMtx, glm::vec3(1.2,0,0.75));
	modelMtxLeg = glm::rotate(modelMtxLeg, -rotationLeg+3.14f, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.5,1.5,0.5));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	modelMtxLeg = glm::translate(modelMtx, glm::vec3(1.2,0,-0.75));
	modelMtxLeg = glm::rotate(modelMtxLeg, -rotationLeg+3.14f, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.5,1.5,0.5));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	// back legs
	modelMtxLeg = glm::translate(modelMtx, glm::vec3(-1.5,0,0.75));
	modelMtxLeg = glm::rotate(modelMtxLeg, rotationLeg, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,-1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.5,1.5,0.5));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	modelMtxLeg = glm::translate(modelMtx, glm::vec3(-1.5,0,-0.75));
	modelMtxLeg = glm::rotate(modelMtxLeg, rotationLeg, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,-1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.5,1.5,0.5));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	// tail
	glm::mat4 modelMtxTail = glm::translate(modelMtx, glm::vec3(-2.3,0,0));
	//modelMtxTail = glm::scale(modelMtxTail, glm::vec3(0.1, 1.0, 0.1));
	modelMtxTail = glm::rotate(modelMtxTail, 13.0f, glm::vec3(0,0,-1));
	modelMtxTail = glm::rotate(modelMtxTail, rotationTail-13.0f, glm::vec3(1,0,0));
	modelMtxTail = glm::translate(modelMtxTail, glm::vec3(0,-1,0));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxTail[0][0]);
	CSCI441::drawSolidCylinder(0.1, 0.1, 1.0, 10, 10);
	//modelMtxTail = glm::translate(modelMtxTail, glm::vec3(0, -0.5, 0));
	modelMtxTail = glm::scale(modelMtxTail, glm::vec3(2.0,-2.0,2.0));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxTail[0][0]);
	glUniform4f(uniform_ambient_loc, 0.2125f, 0.1275f, 0.054f, 1.0f);
	glUniform4f(uniform_diffuse_loc, 0.714f, 0.4284f, 0.18144f, 1.0f);
	glUniform4f(uniform_specular_loc, 0.393548f, 0.271906f, 0.166721f, 1.0f);
	glUniform1f(uniform_shininess_loc, 25.6f);
	CSCI441::drawSolidCone(0.1, 0.2, 10, 10);


	// mane
	glm::mat4 modelMtxMane = glm::translate(modelMtx, glm::vec3(2.2,0.8,0));
	modelMtxMane = glm::rotate(modelMtxMane, rotationHead, glm::vec3(0,0,-1));
	for (int i = 0; i < 20; i++) {
		modelMtxMane = glm::rotate(modelMtxMane, 3.14f/10.0f, glm::vec3(-1,0,0)); 
		glm::mat4 modelMtxMane2 = glm::translate(modelMtxMane, glm::vec3(0, 0, 0.5));
		modelMtxMane2 = glm::scale(modelMtxMane2, glm::vec3(8,8,10));
		modelMtxMane2 = glm::rotate(modelMtxMane2, 3.14f/2.0f, glm::vec3(1,0,0));
		glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxMane2[0][0]);
		CSCI441::drawSolidCone(0.1, 0.2, 10, 10);
	}
	modelMtxMane = glm::translate(modelMtx, glm::vec3(2.0,0.8,0));
	modelMtxMane = glm::rotate(modelMtxMane, rotationHead*0.95f, glm::vec3(0,0,-1));
	for (int i = 0; i < 10; i++) {
		modelMtxMane = glm::rotate(modelMtxMane, 3.14f/10.0f, glm::vec3(-1,0,0)); 
		glm::mat4 modelMtxMane2 = glm::translate(modelMtxMane, glm::vec3(0, 0, 0.5));
		modelMtxMane2 = glm::scale(modelMtxMane2, glm::vec3(6,6,8));
		modelMtxMane2 = glm::rotate(modelMtxMane2, 3.14f/2.0f, glm::vec3(1,0,0));
		glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxMane2[0][0]);
		CSCI441::drawSolidCone(0.1, 0.2, 10, 10);
	}

	// eyes
	glUniform4f(uniform_ambient_loc, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(uniform_diffuse_loc, 0.01f, 0.01f, 0.01f, 1.0f);
	glUniform4f(uniform_specular_loc, 0.50f, 0.50f, 0.50f, 1.0f);
	glUniform1f(uniform_shininess_loc, 32.0f);

	glm::mat4 modelMtxEyes = glm::rotate(modelMtxHead, 3.14f/2.0f + 0.5f, glm::vec3(0,1,0));
	modelMtxEyes = glm::rotate(modelMtxEyes, -0.5f, glm::vec3(1,0,0));
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,1));
	modelMtxEyes = glm::scale(modelMtxEyes, glm::vec3(0.15, 0.15, 0.15));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);

	modelMtxEyes = glm::rotate(modelMtxHead, 3.14f/2.0f - 0.5f, glm::vec3(0,1,0));
	modelMtxEyes = glm::rotate(modelMtxEyes, -0.5f, glm::vec3(1,0,0));
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,1));
	modelMtxEyes = glm::scale(modelMtxEyes, glm::vec3(0.15, 0.15, 0.15));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);

	modelMtxEyes = glm::rotate(modelMtxHead, 3.14f/2.0f, glm::vec3(0,1,0));
	modelMtxEyes = glm::rotate(modelMtxEyes, -0.1f, glm::vec3(1,0,0));
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,1));
	modelMtxEyes = glm::scale(modelMtxEyes, glm::vec3(0.08, 0.08, 0.08));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	//CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);
	//modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,0.1));
	//glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	//CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);

}
#endif // __LION_HPP__

