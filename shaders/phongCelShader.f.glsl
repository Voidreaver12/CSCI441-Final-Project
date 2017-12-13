/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

uniform int numLights;

uniform vec4 lightColor[10];
uniform vec4 diffColor;
uniform vec4 specColor;
uniform vec4 ambiColor;
uniform float shininess;

in vec4 lightVector[10];
in vec4 vertexNormal;
in vec4 cameraVector;

out vec4 fragColorOut;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
	vec4 color = vec4(0,0,0,0);
	
	for (int i = 0; i < numLights; i++) {
		vec4 diffI = diffColor * lightColor[i] * max(dot(vertexNormal, lightVector[i]), 0.0);
		vec4 halfwayVector = normalize(lightVector[i] + cameraVector);
		vec4 specI = specColor * lightColor[i] * pow(max(dot(vertexNormal, halfwayVector), 0.0), shininess);
		vec4 ambiI = ambiColor * lightColor[i];
		
		color += diffI + specI + ambiI;
	}
	
	float edgeTolerance = 0.5;
	if (dot(vertexNormal, cameraVector) < edgeTolerance) {
		// create cartoon edges
		color = vec4(0,0,0,1);
	}
	else {
		// cell shader
		float colorThreshholds[5] = float[](0.3, 0.4, 0.7, 0.8, 1.0);
		for (int i = 0; i < 3; i++) {
			float roundColor = 1.0;
			for (int j = 0; j < colorThreshholds.length(); j++) {
				if (color[i] > colorThreshholds[j]) {
					roundColor = colorThreshholds[j];
				}
			}
			if (color[i] <= colorThreshholds[0]) {
				color[i] = 0.1;
			} else {
				color[i] = roundColor;
			}
		}
	}
	
	fragColorOut = color;
}
