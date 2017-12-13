/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 vPos;
in vec3 vNorm;

uniform float random;
uniform mat4 vMatrix;
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;

out vec4 vertexNormal;
out vec4 cameraVector;

void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************
    
	// get all variables into eye space	
	vec4 vertexPosition = mvMatrix * vec4(vPos, 1.0);
	vertexNormal = normalize(mvMatrix * vec4(vNorm, 0.0));
	cameraVector = normalize(-vertexPosition);
	
	// psueudo random generator
	vec3 newVertex = vPos;
	float avg = 0.0;
	float glitch = 1.0;
	for (int i = 0; i < 3; i++) {
		avg += vPos[i];
	}
	avg /= 3;
	newVertex += vNorm * avg * sin(random * avg);
	
    gl_Position = mvpMatrix * vec4(newVertex, 1);
}
