/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 vPos;
in vec2 vTex;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec2 texCoords;
void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************
    
    gl_Position = pMatrix * vMatrix * mMatrix * vec4(vPos, 1.0);
	texCoords = vTex;
}
