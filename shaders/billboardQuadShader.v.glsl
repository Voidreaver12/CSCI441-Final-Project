/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

in vec3 vPos;
in float vTime;

uniform mat4 mvMatrix;
out float vTime_g;

void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************
    gl_Position = mvMatrix * vec4(vPos, 1.0);
    vTime_g = vTime;
}
