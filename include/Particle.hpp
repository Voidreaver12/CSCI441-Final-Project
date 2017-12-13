
#ifndef __PARTICLE_HPP__
#define __PARTICLE_HPP__

#include <glm/glm.hpp>
#include "time.h"
#include <stdio.h>

class Particle {
	public:
		Particle(glm::vec3 p, glm::vec3 v);
		void update();
		glm::vec3 getPosition();
		glm::vec3 getVelocity();
	private:
		glm::vec3 position;
		glm::vec3 velocity;
		float initialTime;
		float lastUpdated;
};


Particle::Particle(glm::vec3 p, glm::vec3 v) {
	position = p;
	velocity = v;
	initialTime = ((float)clock())/CLOCKS_PER_SEC;
	lastUpdated = initialTime;
}

void Particle::update() {
	float currentTime = ((float)clock())/CLOCKS_PER_SEC;
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


glm::vec3 Particle::getPosition() {
	return position;
}

glm::vec3 Particle::getVelocity() {
	return velocity;
}

#endif // __PARTICLE_HPP__

