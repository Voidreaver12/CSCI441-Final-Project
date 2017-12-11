
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

struct VertexTimed {
	GLfloat x, y, z, t;
};

class ParticleSystem {
	public:
		ParticleSystem();
		void initialize(vector<string> fields);
		void setupBuffers(GLint vpos_particle_attrib_location, GLint vtime_particle_attrib_location, GLuint texHandle);
		void update();
		void draw(glm::vec3 lookAtPoint, glm::vec3 eyePoint, glm::mat4 modelMtx);
		glm::vec3 position;
	private:
		char psType;
		float fountainAngle;
		float minVelocity;
		float maxVelocity;
		float minLifeTime;
		float maxLifeTime;
		float spawnRate;
		float timePerSpawn;
		float lastUpdatedTime;
		int maxParticles;
		const int MAX_P = 500;
		vector<Particle*> particles;

		GLuint particlesVAO, particlesVBO;
		GLint vpos_attrib_location, vtime_attrib_location;
		GLuint textureHandle;
		VertexTimed points[500];

};

ParticleSystem::ParticleSystem() {}

void ParticleSystem::setupBuffers(GLint vpos_particle_attrib_location, GLint vtime_particle_attrib_location, GLuint texHandle) {
	textureHandle = texHandle;
	//VertexTimed* points = new VertexTimed[maxParticles];

	glGenVertexArrays( 1, &particlesVAO );
	glBindVertexArray( particlesVAO );

	glGenBuffers( 1, &particlesVBO );
	glBindBuffer( GL_ARRAY_BUFFER, particlesVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
	glEnableVertexAttribArray( vpos_particle_attrib_location );
	glVertexAttribPointer( vpos_particle_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTimed), (void*)0 );
	glEnableVertexAttribArray( vtime_particle_attrib_location );
	glVertexAttribPointer( vtime_particle_attrib_location, 1, GL_FLOAT, GL_FALSE, sizeof(VertexTimed), (void*)((sizeof(float))*3) );
}

void ParticleSystem::initialize(vector<string> fields) {
	psType = fields.at(0).at(0);
	if (psType == 'F' && fields.size() == 10) {
		position.x = 	atof(fields.at(1).c_str());
		position.y = 	atof(fields.at(2).c_str());
		position.z = 	atof(fields.at(3).c_str());
		fountainAngle = atof(fields.at(4).c_str());
		minVelocity = 	atof(fields.at(5).c_str());
		maxVelocity = 	atof(fields.at(6).c_str());
		minLifeTime = 	atof(fields.at(7).c_str());
		maxLifeTime = 	atof(fields.at(8).c_str());
		spawnRate =		atof(fields.at(9).c_str());
		timePerSpawn = 1.0/spawnRate;
		lastUpdatedTime = 0.0;
		maxParticles = (int) spawnRate * maxLifeTime;
		printf("making a ps with values: (%f), (%f), (%f), (%f), (%f), (%f), (%f), (%f), (%f)\n", position.x, position.y, position.z, fountainAngle, minVelocity, maxVelocity, minLifeTime, maxLifeTime, spawnRate);
	}
}

void ParticleSystem::update() {

	//printf("updating ps\n");
	//printf("updating (%i) particles\n", particles.size());
	// update all particles
	for (int i = 0; i < particles.size(); i++) {
		particles.at(i)->update();
	}
	
	// remove dead particles
	int size = particles.size();
	for (int i = 0; i < size; i++) {
		if (particles.at(i)->isParticleDead()) {
			particles.erase(particles.begin() + i);
			i--;
			size--;
		}
	}
	
	// If its time to add a new particle, do that
	float currentTime = ((float)clock())/CLOCKS_PER_SEC;
	//printf("currenttime is (%f)\n", currentTime);
	//printf("previous time is (%f)\n", lastUpdatedTime);
	if (currentTime >= lastUpdatedTime + timePerSpawn) {
		lastUpdatedTime = currentTime;									// update time
		float v = ((float)rand()/(float)RAND_MAX)*(maxVelocity - minVelocity) + minVelocity; // get random velocity
		float theta = ((float)rand()/(float)RAND_MAX)*(360.0);			// get random angles
		float phi = ((float)rand()/(float)RAND_MAX)*(fountainAngle);
		float vx = v * sinf(theta*PI/180.0) * sinf(phi*PI/180.0);		// convert spherical to cartesian
		float vy = v * cosf(phi*PI/180.0);
		float vz = v * -cosf(theta*PI/180.0) * sinf(phi*PI/180.0);
		float life = ((float)rand()/(float)RAND_MAX)*(maxLifeTime - minLifeTime) + minLifeTime; // get random life
		Particle *p = new Particle(position, glm::vec3(vx,vy,vz), life);	// create new particle
		particles.push_back(p);
		//printf("added a particle with ttl (%f), size is now  (%i)\n",life, particles.size());
	}
	
	//printf("done updating ps\n");
}

void ParticleSystem::draw(glm::vec3 lookAtPoint, glm::vec3 eyePoint, glm::mat4 modelMtx) {
	//printf("drawing system\n");
	for (int i = 0; i < particles.size(); i++) {
		glm::vec3 pos = particles.at(i)->getPosition();
		points[i].x = pos.x;	points[i].y = pos.y;	points[i].z = pos.z;
		points[i].t = particles.at(i)->getTimeRatio();
	}
	//sort
	
	//printf("computing distances\n");
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
	VertexTimed* orderedPoints = new VertexTimed[size];
	for (int i = 0; i < size; i++) {
		orderedPoints[i] = points[orderedIndices[size-1-i]];
	}
	
	//printf("binding vao and vbo\n");
	glBindVertexArray( particlesVAO );
	glBindBuffer(GL_ARRAY_BUFFER, particlesVBO);
	//printf("buffering data\n");
	glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(VertexTimed), orderedPoints);
	glBindTexture( GL_TEXTURE_2D, textureHandle );
	//printf("drawing (%i) particles\n", particles.size());
	glDrawArrays( GL_POINTS, 0, particles.size() );
	//printf("DONE DRAWING\n");
}


#endif // __PARTICLE_SYSTEM_HPP__
