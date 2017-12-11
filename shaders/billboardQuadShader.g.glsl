/*
 *   Geometry Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO #A: set primitive input
layout (points) in;
// TODO #B: set primitive output
layout (triangle_strip, max_vertices=4) out;

uniform mat4 projMatrix;

in float vTime_g[];
// TODO I: add varying output
out vec2 texCoords;
out float vTime;

void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************

    float t = 0.0;
	if (vTime <= 1 && vTime >= 0) {
        //t = vTime;
    }
	vec4 vector = vec4(-1+t, -1+t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(0, 0);
    vTime = vTime_g[0];
    EmitVertex();
    
	vector = vec4(-1+t, 1-t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(1, 0);
    vTime = vTime_g[0];
    EmitVertex();
    
	vector = vec4(1-t, -1+t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(0, 1);
    vTime = vTime_g[0];
    EmitVertex();
    
    vector = vec4(1-t, 1-t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(1, 1);
    vTime = vTime_g[0];
    EmitVertex();

    EndPrimitive();
    
}
