/*
 *   Geometry Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core


layout (points) in;

layout (triangle_strip, max_vertices=4) out;

uniform mat4 projMatrix;
uniform float time;

out vec2 texCoords;


void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************

    float t = clamp(time, 0, 1);
    t = t / 7.0f;

	vec4 vector = vec4(-t, -t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(0, 0);
    EmitVertex();
    
	vector = vec4(-t, t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(1, 0);
    EmitVertex();
    
	vector = vec4(t, -t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(0, 1);
    EmitVertex();
    
    vector = vec4(t, t, 0, 1);
    gl_Position = projMatrix * (gl_in[0].gl_Position + vector);
	texCoords = vec2(1, 1);
    EmitVertex();

    EndPrimitive();
    
}
