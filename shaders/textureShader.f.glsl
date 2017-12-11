/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core


in vec2 texCoords;

out vec4 fragColorOut;

uniform sampler2D textureMap;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
    
	fragColorOut = texture(textureMap, texCoords);
	
}
