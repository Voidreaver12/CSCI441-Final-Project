
#ifndef __PARTICLE_HPP__
#define __PARTICLE_HPP__

#include <glm/glm.hpp>
#include "time.h"
#include <stdio.h>

class Particle {
	public:
		Particle(glm::vec3 p, glm::vec3 v, float t);
		void update();
		void draw();
		glm::vec3 getPosition();
		glm::vec3 getVelocity();
		bool isParticleDead();
		float getTimeRatio();
	private:
		glm::vec3 position;
		glm::vec3 velocity;
		float timeToLive;
		float initialTime;
		float lastUpdated;
		bool isDead = false;
};


Particle::Particle(glm::vec3 p, glm::vec3 v, float t) {
	position = p;
	velocity = v;
	timeToLive = t;
	initialTime = ((float)clock())/CLOCKS_PER_SEC;
	lastUpdated = initialTime;
}

void Particle::update() {
	//printf("getting updated");
	float currentTime = ((float)clock())/CLOCKS_PER_SEC;
	// if the particles time has run out, mark it for deletion and dont update
	if (initialTime + timeToLive <= currentTime) {
		isDead = true;
		return;
	}
	// update
	float deltaTime = currentTime - lastUpdated;
	velocity += glm::vec3(0, -1, 0) * deltaTime;
	position += velocity * deltaTime;
	// if hit the ground, bounce
	if (position.y <= 0 && velocity.y <= 0) {
		position.y = 0.0;
		velocity.y = -velocity.y/2;
	}
	lastUpdated = currentTime;
}

void Particle::draw() {


}

glm::vec3 Particle::getPosition() {
	return position;
}

glm::vec3 Particle::getVelocity() {
	return velocity;
}

bool Particle::isParticleDead() {
	return isDead;
}

float Particle::getTimeRatio() {
	//printf("time ratio: %f\n", ((lastUpdated - initialTime) / timeToLive));
	return (lastUpdated - initialTime) / timeToLive;
}
#endif // __PARTICLE_HPP__

