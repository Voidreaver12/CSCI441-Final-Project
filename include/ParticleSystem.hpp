
#ifndef __PARTICLE_SYSTEM_HPP__
#define __PARTICLE_SYSTEM_HPP__

#include <glm/glm.hpp>
#include "Particle.hpp"
#include "time.h"
#include "math.h"
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265

struct Point {
	GLfloat x, y, z;
};

class ParticleSystem {
	public:
		ParticleSystem();
		void initialize(glm::vec3 p, float angle, float minV, float maxV, float t);
		void setupBuffers(GLint vpos_particle_attrib_location, GLuint texHandle);
		void update();
		void draw(glm::vec3 lookAtPoint, glm::vec3 eyePoint, glm::mat4 modelMtx, GLint uniform_time_loc);
		void createParticles();
		glm::vec3 position;
		bool dead = false;
	private:
		float getTimeRatio();
		float fountainAngle;
		float minVelocity;
		float maxVelocity;
		float minLifeTime;
		float maxLifeTime;
		float timeToLive;
		float initialTime;
		const int MAX_P = 5;

		vector<Particle*> particles;

		GLuint particlesVAO, particlesVBO;
		GLint vpos_attrib_location, vtime_attrib_location;
		GLuint textureHandle;

		Point points[10];

};

ParticleSystem::ParticleSystem() {}

void ParticleSystem::setupBuffers(GLint vpos_particle_attrib_location, GLuint texHandle) {
	textureHandle = texHandle;

	glGenVertexArrays( 1, &particlesVAO );
	glBindVertexArray( particlesVAO );

	glGenBuffers( 1, &particlesVBO );
	glBindBuffer( GL_ARRAY_BUFFER, particlesVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glEnableVertexAttribArray( vpos_particle_attrib_location );
	glVertexAttribPointer( vpos_particle_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0 );
}

void ParticleSystem::initialize(glm::vec3 p, float angle, float minV, float maxV, float t) {
	position = p;
	fountainAngle = angle;
	minVelocity = 	minV;
	maxVelocity = 	maxV;
	timeToLive = 	t;
	initialTime = ((float)clock())/CLOCKS_PER_SEC;
	//printf("making a ps with values: (%f), (%f), (%f), (%f), (%f), (%f), (%f)\n", position.x, position.y, position.z, fountainAngle, minVelocity, maxVelocity, timeToLive);
}

void ParticleSystem::update() {
	for (int i = 0; i < particles.size(); i++) {
		particles.at(i)->update();
	}
	float currentTime = ((float)clock())/CLOCKS_PER_SEC;
	if (currentTime - initialTime > timeToLive) {
		particles.clear();
		dead = true;
	}
}

void ParticleSystem::createParticles() {
	for (int i = 0; i < MAX_P; i++) {
		float v = ((float)rand()/(float)RAND_MAX)*(maxVelocity - minVelocity) + minVelocity; // get random velocity
		float theta = ((float)rand()/(float)RAND_MAX)*(360.0);			// get random angles
		float phi = ((float)rand()/(float)RAND_MAX)*(fountainAngle);
		float vx = v * sinf(theta*PI/180.0) * sinf(phi*PI/180.0);		// convert spherical to cartesian
		float vy = v * cosf(phi*PI/180.0);
		float vz = v * -cosf(theta*PI/180.0) * sinf(phi*PI/180.0);
		Particle *p = new Particle(position, glm::vec3(vx,vy,vz));	// create new particle
		particles.push_back(p);
	}
}

float ParticleSystem::getTimeRatio() {
	float currentTime = ((float)clock())/CLOCKS_PER_SEC;
	return 1.0f - (currentTime - initialTime)/timeToLive;
}

void ParticleSystem::draw(glm::vec3 lookAtPoint, glm::vec3 eyePoint, glm::mat4 modelMtx, GLint uniform_time_loc) {
	//printf("drawing system\n");
	for (int i = 0; i < particles.size(); i++) {
		glm::vec3 pos = particles.at(i)->getPosition();
		points[i].x = pos.x;	points[i].y = pos.y;	points[i].z = pos.z;
	}
	//sort
	glm::vec4 v = glm::normalize(glm::vec4(lookAtPoint, 1) - glm::vec4(eyePoint, 1));
	int size = particles.size();
	int* orderedIndices = new int[size];
	float* distances = new float[size];
	for (int i = 0; i < size; i++) {
		glm::vec4 p = glm::vec4(points[i].x, points[i].y, points[i].z, 1);
		p = modelMtx * p;
		glm::vec4 ep = (p - glm::vec4(eyePoint, 1));
		float distance = glm::dot(v, ep);
		orderedIndices[i] = i;
		distances[i] = distance;
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (distances[i] < distances[j]) {
				double tempd = distances[i];
				distances[i] = distances[j];
				distances[j] = tempd;
				int tempi = orderedIndices[i];
				orderedIndices[i] = orderedIndices[j];
				orderedIndices[j] = tempi;
			}
		}
	}
	Point* orderedPoints = new Point[size];
	for (int i = 0; i < size; i++) {
		orderedPoints[i] = points[orderedIndices[size-1-i]];
	}
	
	//printf("binding vao and vbo\n");
	glBindVertexArray( particlesVAO );
	glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
	//printf("buffering data\n");
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Point), orderedPoints);
	glBindTexture( GL_TEXTURE_2D, textureHandle );
	//printf("drawing (%i) particles\n", particles.size());
	// send time uniform
	glUniform1f(uniform_time_loc, getTimeRatio());
	glDrawArrays( GL_POINTS, 0, particles.size() );
	//printf("DONE DRAWING\n");
}


#endif // __PARTICLE_SYSTEM_HPP__
