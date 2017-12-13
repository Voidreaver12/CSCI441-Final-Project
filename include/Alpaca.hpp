
#ifndef __ALPACA_HPP__
#define __ALPACA_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <CSCI441/objects3.hpp>
#include <math.h>
#include <stdio.h>

class Alpaca {
public:
	Alpaca(glm::vec3 p, float r, float platformSize);
	void moveForward(glm::vec3 lionP);
	void moveBackward();
	void updateHeading(glm::vec3 lionP);
	void moveLegs();
	void moveLegsBack();
	void update();
	void draw(glm::mat4 modelMtx, GLint uniform_modelMtx_loc, 
				GLint uniform_ambient_loc, GLint uniform_diffuse_loc, 
				GLint uniform_specular_loc, GLint uniform_shininess_loc);

	float size;
	glm::vec3 position;
	glm::vec3 direction;
	float scareDistance;
	int age;
	int birthTimer;
	float platformSize;
private:
	float rotation;
	float rotationLeg;
	float rotationTail;
	float rotationHead;
	float rotationLegAmount;
	float rotationTailAmount;
	float adultSpeed;
	float speed = 0.01f;
};

Alpaca::Alpaca(glm::vec3 p, float r, float platformSize) {
	position = p;
	rotation = r;
	direction.x = cos(rotation);
	direction.z = -sin(rotation);
	size = 0.05f;
	rotationLeg = 0.0f;
	rotationLegAmount = 0.025f;
	scareDistance = 7.0f;
	age = 0;
	birthTimer = 100;
	adultSpeed = .01 + ((float)rand()/(float)RAND_MAX)*.06;
	this->platformSize = platformSize;
}

void Alpaca::moveForward(glm::vec3 lionP) {
	glm::vec3 newPosition = position + direction * speed;
	if (newPosition.x >= platformSize) { newPosition.x = platformSize; }
	if (newPosition.x <= -platformSize) { newPosition.x = -platformSize; }
	if (newPosition.z >= platformSize) { newPosition.z = platformSize; }
	if (newPosition.z <= -platformSize) { newPosition.z = -platformSize; }
	position = newPosition;
	moveLegs();
}

void Alpaca::moveBackward() {
	position -= direction * speed;
}

void Alpaca::update() {
	birthTimer -= 1;
	age += 1;
	size = 0.05f + ((float)age/500.0f)*0.1f;
	if (size >= 0.15f) {
		size = 0.15f;
	}
	speed = speed + ((float)age)*0.002f*adultSpeed;
	if (speed >= adultSpeed) {
		speed = adultSpeed;
	}
}
void Alpaca::updateHeading(glm::vec3 lionP) {
	// If lion is too close, move in an arc away from him
	if (glm::length(lionP - position) <= scareDistance) {
		glm::vec3 heading = glm::normalize(direction);
		glm::vec3 towardsAlpaca = glm::normalize(position - lionP);
		glm::vec3 cross = glm::cross(towardsAlpaca, glm::vec3(0,1,0));
		glm::vec3 negcross = -cross;
		glm::mat4 m;
		if (acos(glm::dot(heading, cross)) < acos(glm::dot(heading, negcross))) {
			float angle = acos(glm::dot(heading, towardsAlpaca));
			m = glm::rotate(m, angle*0.01f, glm::vec3(0,1,0));
		}
		else {
			float angle = acos(glm::dot(heading, towardsAlpaca));
			m = glm::rotate(m, angle*0.01f, glm::vec3(0,-1,0));
		}
		glm::vec4 newHeading = m * glm::vec4(heading.x, heading.y, heading.z, 1.0);
		
		direction.x = newHeading.x;
		direction.z = newHeading.z;
		direction = glm::normalize(direction);
		rotation = atan(-direction.z/direction.x);
		if (direction.x < 0.0) { rotation += 3.14f; }
	}
	// If the lion is far away
	else {
		// If alpaca is on the middle portion of platform, move randomly
		if (position.x <= platformSize*0.75 && position.x >= -platformSize*0.75 && position.z <= platformSize*0.75 && position.z >= -platformSize*0.75) {
			rotation += (float)rand()/(float)RAND_MAX * 0.1f - 0.05f;
			direction.x = cos(rotation);
			direction.z = -sin(rotation);
		}
		// If alpaca is on/near the edge of platform move in an arc towards the center
		else {
			glm::vec3 heading = glm::normalize(direction);
			glm::vec3 towardsCenter = glm::normalize(glm::vec3(0,0,0) - position);
			glm::mat4 m;
			float angle = acos(glm::dot(heading, towardsCenter));
			m = glm::rotate(m, angle*0.01f, glm::vec3(0,1,0));
			glm::vec4 newHeading = m * glm::vec4(heading.x, heading.y, heading.z, 1.0);
			direction.x = newHeading.x;
			direction.z = newHeading.z;
			direction = glm::normalize(direction);
			rotation = atan(-direction.z/direction.x);
			if (direction.x < 0.0) { rotation += 3.14f; }
		}
	}
}

void Alpaca::moveLegs() {
	rotationLeg += rotationLegAmount;
	if (rotationLeg > 0.2f) {
		rotationLegAmount = -rotationLegAmount;
	}
	if (rotationLeg < -0.2f) {
		rotationLegAmount = -rotationLegAmount;
	}
}

void Alpaca::moveLegsBack() {
	if (rotationLeg > 0.0f) {
		rotationLeg -= 0.1f;
		if (rotationLeg < 0.0f) { rotationLeg = 0.0f; }
	}
	if (rotationLeg < 0.0f) {
		rotationLeg += 0.1f;
		if (rotationLeg > 0.0f) { rotationLeg = 0.0f; }
	}
}

void Alpaca::draw(glm::mat4 modelMtx, GLint uniform_modelMtx_loc,
				 GLint uniform_ambient_loc, GLint uniform_diffuse_loc, 
				 GLint uniform_specular_loc, GLint uniform_shininess_loc) {

	modelMtx = glm::translate(modelMtx, position);
	modelMtx = glm::translate(modelMtx, glm::vec3(0, 1.5*size,0));
	modelMtx = glm::scale(modelMtx, glm::vec3(size, size, size));
    modelMtx = glm::rotate(modelMtx, rotation, glm::vec3(0,1,0));
	//color
	glUniform4f(uniform_ambient_loc, 0.25f, 0.20725f, 0.20725f, 1.0f);
	glUniform4f(uniform_diffuse_loc, 1.0f, 0.829f, 0.829f, 1.0f);
	glUniform4f(uniform_specular_loc, 0.296648f, 0.296648f, 0.296648f, 1.0f);
	glUniform1f(uniform_shininess_loc, 11.264f);
	// body
	glm::mat4 modelMtxBody = modelMtx;
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxBody[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	modelMtxBody = glm::translate(modelMtx, glm::vec3(1,0,0));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxBody[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	modelMtxBody = glm::translate(modelMtx, glm::vec3(-1,0,0));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxBody[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	// neck
	glm::mat4 modelMtxNeck = glm::translate(modelMtx, glm::vec3(1,1,0));
	modelMtxNeck = glm::scale(modelMtxNeck, glm::vec3(0.7, 0.7, 0.7));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxNeck[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	modelMtxNeck = glm::translate(modelMtx, glm::vec3(1,1.7,0));
	modelMtxNeck = glm::scale(modelMtxNeck, glm::vec3(0.7, 0.7, 0.7));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxNeck[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	modelMtxNeck = glm::translate(modelMtx, glm::vec3(1,2.4,0));
	modelMtxNeck = glm::scale(modelMtxNeck, glm::vec3(0.7, 0.7, 0.7));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxNeck[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	// head
	glm::mat4 modelMtxHead = glm::translate(modelMtx, glm::vec3(1,3.1,0));
	modelMtxHead = glm::scale(modelMtxHead, glm::vec3(0.85, 0.85, 0.85));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxHead[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	glm::mat4 modelMtxNose = glm::translate(modelMtx, glm::vec3(1.7,3.0,0));
	modelMtxNose = glm::scale(modelMtxNose, glm::vec3(0.3, 0.3, 0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxNose[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	// ears
	glm::mat4 modelMtxEar = glm::translate(modelMtx, glm::vec3(0.85,3.8,0.5));
	modelMtxEar = glm::scale(modelMtxEar, glm::vec3(0.3, 0.5, 0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEar[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	modelMtxEar = glm::translate(modelMtx, glm::vec3(0.85,3.8,-0.5));
	modelMtxEar = glm::scale(modelMtxEar, glm::vec3(0.3, 0.5, 0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEar[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	// front legs
	glm::mat4 modelMtxLeg = glm::translate(modelMtx, glm::vec3(1,0,0.5));
	modelMtxLeg = glm::rotate(modelMtxLeg, -rotationLeg+3.14f, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.35,0.5,0.35));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	modelMtxLeg = glm::translate(modelMtx, glm::vec3(1,0,-0.5));
	modelMtxLeg = glm::rotate(modelMtxLeg, -rotationLeg+3.14f, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.35,0.5,0.35));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);
	// back legs
	modelMtxLeg = glm::translate(modelMtx, glm::vec3(-1,0,0.5));
	modelMtxLeg = glm::rotate(modelMtxLeg, rotationLeg, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,-1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.35,0.5,0.35));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	modelMtxLeg = glm::translate(modelMtx, glm::vec3(-1,0,-0.5));
	modelMtxLeg = glm::rotate(modelMtxLeg, rotationLeg, glm::vec3(0,0,-1));
	modelMtxLeg = glm::translate(modelMtxLeg, glm::vec3(0,-1,0));
	modelMtxLeg = glm::scale(modelMtxLeg, glm::vec3(0.35,0.5,0.35));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxLeg[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

	// tail
	glm::mat4 modelMtxTail = glm::translate(modelMtx, glm::vec3(-2,0.3,0));
	modelMtxTail = glm::scale(modelMtxTail, glm::vec3(0.3, 0.3, 0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxTail[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);



	// eyes
	glUniform4f(uniform_ambient_loc, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(uniform_diffuse_loc, 0.01f, 0.01f, 0.01f, 1.0f);
	glUniform4f(uniform_specular_loc, 0.50f, 0.50f, 0.50f, 1.0f);
	glUniform1f(uniform_shininess_loc, 32.0f);

	glm::mat4 modelMtxEyes = glm::rotate(modelMtxHead, 3.14f/2.0f + 0.5f, glm::vec3(0,1,0));
	modelMtxEyes = glm::rotate(modelMtxEyes, -0.3f, glm::vec3(1,0,0));
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,1));
	modelMtxEyes = glm::scale(modelMtxEyes, glm::vec3(0.1, 0.1, 0.1));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);

	modelMtxEyes = glm::rotate(modelMtxHead, 3.14f/2.0f - 0.5f, glm::vec3(0,1,0));
	modelMtxEyes = glm::rotate(modelMtxEyes, -0.3f, glm::vec3(1,0,0));
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,1));
	modelMtxEyes = glm::scale(modelMtxEyes, glm::vec3(0.1, 0.1, 0.1));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,0.3));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);
	
	// nose
	modelMtxEyes = glm::rotate(modelMtxNose, 3.14f/2.0f, glm::vec3(0,1,0));
	modelMtxEyes = glm::rotate(modelMtxEyes, -0.1f, glm::vec3(1,0,0));
	modelMtxEyes = glm::translate(modelMtxEyes, glm::vec3(0,0,0.9));
	modelMtxEyes = glm::scale(modelMtxEyes, glm::vec3(0.2, 0.2, 0.2));
	glUniformMatrix4fv(uniform_modelMtx_loc, 1, GL_FALSE, &modelMtxEyes[0][0]);
	CSCI441::drawSolidSphere(1.0, 16, 16);

}
#endif // __Alpaca_HPP__

