/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec4 vertexNormal;
in vec4 cameraVector;

out vec4 fragColorOut;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
	vec4 color = vec4(0.8,0,0,1);
	
	float edgeTolerance = 0.5;
	if (dot(vertexNormal, cameraVector) < edgeTolerance) {
		// create cartoon edges
		color = vec4(0,0,0,1);
	}
	
	fragColorOut = color;
}
