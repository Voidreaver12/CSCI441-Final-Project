/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 vPos;
in vec3 vNorm;

uniform int numLights;
uniform vec3 lPos[10];
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec4 vertexNormal;
out vec4 lightVector[10];
out vec4 cameraVector;

void main() {
	mat4 mvMatrix = vMatrix * mMatrix;
	mat4 mvpMatrix = pMatrix * mvMatrix;
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************
    
	// get all variables into eye space	
	vec4 vertexPosition = mvMatrix * vec4(vPos, 1.0);
	vertexNormal = normalize(mvMatrix * vec4(vNorm, 0.0));
	
	for (int i = 0; i < numLights; i++) {
		vec4 lightPosition = vMatrix * vec4(lPos[i], 1.0);
		lightVector[i] = normalize(lightPosition - vertexPosition);
	}
	
	cameraVector = normalize(-vertexPosition);
	
    gl_Position = mvpMatrix * vec4(vPos, 1.0);
}
