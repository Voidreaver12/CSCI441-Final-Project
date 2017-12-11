/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 330 core

// TODO J: add varying input
in vec2 texCoords;
in float vTime;

out vec4 fragColorOut;

// TODO K: add texture uniform
uniform sampler2D textureMap;

void main() {

    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************

    fragColorOut = clamp(texture(textureMap, texCoords) * vec4(3,3,3,1),0,1);

    vec4 blue = vec4(0.388235, 0.784314, 1, 1);
    vec4 gold = vec4(1, 0.85098, 0.262745, 1);
    vec4 white = vec4(1,1,1,1);

    if (vTime >= 0.75) {
        float t = 4*(vTime-0.75);
        fragColorOut = (fragColorOut * t * white) + (fragColorOut * (1-t) * gold);
    }
    else if (vTime >= 0.5) {
        float t = 4*(vTime-0.5);
        fragColorOut = (fragColorOut * t * gold) + (fragColorOut * (1-t) * white);
    }
    else if (vTime >= 0.25) {
        float t = 4*(vTime-0.25);
        fragColorOut = (fragColorOut * (1-t) * blue) + (fragColorOut * t * white);
    }
    else if (vTime >= 0.0) {
        float t = 4*(vTime-0.0);
        fragColorOut = (fragColorOut * (1-t) * white) + (fragColorOut * t * blue);
    }

 
}
