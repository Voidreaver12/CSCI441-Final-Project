/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: lab10
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with GLSL Geometry shader.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2017
 *
 */

//*************************************************************************************

#include <GL/glew.h>        // include GLEW to get our OpenGL 3.0+ bindings
#include <GLFW/glfw3.h>			// include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// include c++ libraries
#include <stdio.h>				// for printf functionality
#include <stdlib.h>				// for exit functionality
#include <time.h>					// for time functionality
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// note that all of these headers end in *3.hpp
// these class library files will only work with OpenGL 3.0+
#include <CSCI441/modelLoader3.hpp> // to load in OBJ models
#include <CSCI441/objects3.hpp>     // to render our 3D primitives
#include <CSCI441/OpenGLUtils3.hpp> // to print info about OpenGL
#include <CSCI441/ShaderProgram3.hpp>   // our shader helper functions
#include <CSCI441/TextureUtils.hpp>   // our texture helper functions

// Particle systems
#include "include/Particle.hpp"
#include "include/ParticleSystem.hpp"

// Lion
#include "include/Lion.hpp"
#include "include/Alpaca.hpp"

//*************************************************************************************
//
// Global Parameters

int windowWidth, windowHeight;

bool controlDown = false;
bool leftMouseDown = false;
glm::vec2 mousePosition( -9999.0f, -9999.0f );
glm::vec3 eyePoint;
glm::vec3 lookAtPoint;
glm::vec3 cameraAngles( 1.82f, 2.01f, 15.0f );
glm::vec3 eyePointOffset(   10.0f, 10.0f, 10.0f );
glm::vec3 upVector(    0.0f,  1.0f,  0.0f );

int cameraCenterResponsiveness = 15;
float cameraScale = 10.0;
bool chaseCamera = false;

// Skybox stuff
struct VertexTextured { GLfloat x, y, z, s, t; };

GLuint skyboxTextureHandles[6];
GLuint skyboxVAODs[6];

CSCI441::ShaderProgram *textureShaderProgram = NULL;
GLint m_uniform_location, v_uniform_location, p_uniform_location;
GLint vpos_texture_attrib_location, vtex_texture_attrib_loaction;

// Platform
GLuint platformTextureHandle;
GLuint platformVAO;
float platformSize = 10.0;

// Particles
GLuint particleTextureHandle;
vector<ParticleSystem*> particleSystems;
CSCI441::ShaderProgram *particleShaderProgram = NULL;
GLint attrib_particle_vpos_loc, uniform_particle_time_loc;
GLint uniform_particle_modelview_loc, uniform_particle_projection_loc;


// phong
CSCI441::ShaderProgram *phongShaderProgram = NULL;
GLint uniform_phong_m_loc, uniform_phong_v_loc, uniform_phong_p_loc, uniform_phong_norm_loc; // matrices
GLint uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_ma_loc, uniform_phong_s_loc; // material properties
GLint attrib_phong_vpos_loc, attrib_phong_vnorm_loc; // vertex attributes

CSCI441::ShaderProgram *celShaderProgram = NULL;
GLint cel_m_uniform_location;
GLint cel_v_uniform_location;
GLint cel_p_uniform_location;
GLint cel_numlights_uniform_location;
GLint cel_light_uniform_location;
GLint cel_diff_uniform_location;
GLint cel_spec_uniform_location;
GLint cel_ambi_uniform_location;
GLint cel_shiny_uniform_location;
GLint cel_vpos_attrib_location;
GLint cel_normal_atrrib_location;
GLint cel_lpos_uniform_location;

CSCI441::ShaderProgram *fearShaderProgram = NULL;
GLint fear_m_uniform_location;
GLint fear_v_uniform_location;
GLint fear_p_uniform_location;
GLint fear_random_uniform_location;
GLint fear_vpos_attrib_location;
GLint fear_normal_atrrib_location;

// lights
glm::vec3 globalLightPosition ( -200.0f, 1000.0f, -100.0f );
glm::vec3 dynamicLightPosition ( 1000.0f, 0.0f, -500.0f );

//Are set from setup file
glm::vec4 globalLightColor;
glm::vec4 dynamicLightColor;


// lion and alpacas
Lion* lion = NULL;
vector<Alpaca*> alpacas;
float babyDistance = 1.0f;

float eatDistance = 1.0f;
int isLionRotating = 0;
int	isLionMoving = 0;
int lionMoveSign = 0;

//Are set from setup file
int startingNumAlpacas;
int birthRate;
int maxAlpacas;

//******************************************************************************
//
// Helper Functions

float randNumber( int max ) {
	return rand() / (float)RAND_MAX * max * 2.0 - max;
}

// convertSphericalToCartesian() ///////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraAngles is updated.
//
////////////////////////////////////////////////////////////////////////////////
void convertSphericalToCartesian() {
	eyePointOffset.x = cameraAngles.z * sinf( cameraAngles.x ) * sinf( cameraAngles.y );
	eyePointOffset.y = cameraAngles.z * -cosf( cameraAngles.y );
	eyePointOffset.z = cameraAngles.z * -cosf( cameraAngles.x ) * sinf( cameraAngles.y );
}

void recomputeOrientation() {
	if (lionMoveSign > 0 && !leftMouseDown) {
		glm::vec3 desiredVector = glm::normalize(lion->direction);
		
		float angle = glm::acos(glm::dot(desiredVector, glm::vec3(0,0,1)));
		if (desiredVector.x >= 0) {
			angle *= -1;
		}
		
		if (fabs(cameraAngles.x - angle) > M_PI - 0.1f) {
			if (angle > cameraAngles.x) {
				angle -= M_PI * 2;
			}
			else {
				angle += M_PI * 2;
			}
		}
		cameraAngles.x = (cameraAngles.x * (cameraCenterResponsiveness - 1) + angle) / cameraCenterResponsiveness;
		
		if (cameraAngles.x > 2 * M_PI) {
			cameraAngles.x = cameraAngles.x - 2 * M_PI;
		} else if (cameraAngles.x < 0) {
			cameraAngles.x = 2 * M_PI + cameraAngles.x;
		}
	}
	if (!controlDown) {
		cameraAngles.z = lion->size * cameraScale;
	}
	
	convertSphericalToCartesian();	
}

//******************************************************************************
//
// Event Callbacks

// error_callback() ////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
////////////////////////////////////////////////////////////////////////////////
static void error_callback(int error, const char* description) {
	fprintf(stderr, "[ERROR]: %s\n", description);
}

// key_callback() //////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's keypress callback.
//	Responds to key presses and key releases
//
////////////////////////////////////////////////////////////////////////////////
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE || key == 'Q') && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

  if( action == GLFW_PRESS ) {
    switch( key ) {
		case GLFW_KEY_1:
			chaseCamera = !chaseCamera;
			break;
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
        	glfwSetWindowShouldClose( window, GLFW_TRUE );
			break;
		case GLFW_KEY_A:
			isLionRotating += 1;
			break;
		case GLFW_KEY_D:
			isLionRotating -= 1;
			break;
		case GLFW_KEY_W:
			isLionMoving += 1;
			lionMoveSign = 1;
			break;
		case GLFW_KEY_S:
			isLionMoving -= 1;
			lionMoveSign = -1;
			break;
    }
  }
  else if (action == GLFW_RELEASE) {
	  switch(key) {
		case GLFW_KEY_A:
			isLionRotating -= 1;
			break;
		case GLFW_KEY_D:
			isLionRotating += 1;
			break;
		case GLFW_KEY_W:
			isLionMoving -= 1;
			lionMoveSign = 0;
			break;
		case GLFW_KEY_S:
			isLionMoving += 1;
			lionMoveSign = 0;
			break;
	  }
  }
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//		We will register this function as GLFW's mouse button callback.
//	Responds to mouse button presses and mouse button releases.  Keeps track if
//	the control key was pressed when a left mouse click occurs to allow
//	zooming of our arcball camera.
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS ) {
		leftMouseDown = true;
	} else {
		leftMouseDown = false;
		mousePosition.x = -9999.0f;
		mousePosition.y = -9999.0f;
	}
  controlDown = mods & GLFW_MOD_CONTROL;
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's cursor movement callback.
//	Responds to mouse movement.  When active motion is used with the left
//	mouse button an arcball camera model is followed.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	// make sure movement is in bounds of the window
	// glfw captures mouse movement on entire screen
	if( xpos > 0 && xpos < windowWidth ) {
		if( ypos > 0 && ypos < windowHeight ) {
			// active motion
			if( leftMouseDown ) {
				if( (mousePosition.x - -9999.0f) < 0.001f ) {
					mousePosition.x = xpos;
					mousePosition.y = ypos;
				} else {
					if( !controlDown ) {
						cameraAngles.x += (xpos - mousePosition.x)*0.005f;
						cameraAngles.y += (ypos - mousePosition.y)*0.005f;

						if( cameraAngles.y < M_PI/2 ) cameraAngles.y = M_PI/2 + 0.001f;
						if( cameraAngles.y >= M_PI ) cameraAngles.y = M_PI - 0.001f;
					} else {
						double totChgSq = (xpos - mousePosition.x) + (ypos - mousePosition.y);
						cameraAngles.z += totChgSq*0.01f;

						if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
						if( cameraAngles.z >= 30.0f ) cameraAngles.z = 30.0f;
					}
					convertSphericalToCartesian();

					mousePosition.x = xpos;
					mousePosition.y = ypos;
				}
			}
			// passive motion
			else {

			}
		}
	}
}

// scroll_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's scroll wheel callback.
//	Responds to movement of the scroll where.  Allows zooming of the arcball
//	camera.
//
////////////////////////////////////////////////////////////////////////////////
static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset ) {
	double totChgSq = yOffset;
	cameraAngles.z += totChgSq*0.01f;

	if( cameraAngles.z <= 2.0f ) cameraAngles.z = 2.0f;
	if( cameraAngles.z >= 50.0f ) cameraAngles.z = 50.0f;

	convertSphericalToCartesian();
}

//******************************************************************************
//
// Setup Functions

// setupGLFW() /////////////////////////////////////////////////////////////////
//
//		Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* setupGLFW() {
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback(error_callback);

	// initialize GLFW
	if (!glfwInit()) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit(EXIT_FAILURE);
	} else {
		fprintf( stdout, "[INFO]: GLFW initialized\n" );
	}

	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );						// request forward compatible OpenGL context
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );	// request OpenGL Core Profile context
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );		// request OpenGL 3.x context
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );		// request OpenGL 3.3 context

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow(640, 480, "Final Project", NULL, NULL);
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(	window );	// make the created window the current window
	glfwSwapInterval( 1 );				    // update our screen after at least 1 screen refresh

	glfwSetKeyCallback( 			  window, key_callback				  );	// set our keyboard callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function
	glfwSetCursorPosCallback(	  window, cursor_callback  			);	// set our cursor position callback function
	glfwSetScrollCallback(			window, scroll_callback			  );	// set our scroll wheel callback function

	return window;										// return the window that was created
}

// setupOpenGL() ///////////////////////////////////////////////////////////////
//
//      Used to setup everything OpenGL related.
//
////////////////////////////////////////////////////////////////////////////////
void setupOpenGL() {
	glEnable( GL_DEPTH_TEST );					// enable depth testing
	glDepthFunc( GL_LESS );							// use less than depth test

	glEnable(GL_BLEND);									// enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// use one minus blending equation

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// clear the frame buffer to black

	glEnable( GL_TEXTURE_2D );					// enable 2D texturing

	glPointSize( 4.0 );									// make our points bigger
}

// setupGLEW() /////////////////////////////////////////////////////////////////
//
//      Used to initialize GLEW
//
////////////////////////////////////////////////////////////////////////////////
void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum glewResult = glewInit();

	/* check for an error */
	if( glewResult != GLEW_OK ) {
		printf( "[ERROR]: Error initalizing GLEW\n");
		/* Problem: glewInit failed, something is seriously wrong. */
  	fprintf( stderr, "[ERROR]: %s\n", glewGetErrorString(glewResult) );
		exit(EXIT_FAILURE);
	} else {
		 fprintf( stdout, "[INFO]: GLEW initialized\n" );
		 fprintf( stdout, "[INFO]: Status: Using GLEW %s\n", glewGetString(GLEW_VERSION) );
	}

	if( !glewIsSupported( "GL_VERSION_2_0" ) ) {
		printf( "[ERROR]: OpenGL not version 2.0+.  GLSL not supported\n" );
		exit(EXIT_FAILURE);
	}
}

// setupShaders() //////////////////////////////////////////////////////////////
//
//      Create our shaders.  Send GLSL code to GPU to be compiled.  Also get
//  handles to our uniform and attribute locations.
//
////////////////////////////////////////////////////////////////////////////////
void setupShaders() {

	// skybox shader
	textureShaderProgram = new CSCI441::ShaderProgram( "shaders/textureShader.v.glsl",
													   "shaders/textureShader.f.glsl" );
	m_uniform_location 		 = textureShaderProgram->getUniformLocation( "mMatrix" );
	v_uniform_location 		 = textureShaderProgram->getUniformLocation( "vMatrix" );
	p_uniform_location 		 = textureShaderProgram->getUniformLocation( "pMatrix" );
	vpos_texture_attrib_location = textureShaderProgram->getAttributeLocation( "vPos" );
	vtex_texture_attrib_loaction = textureShaderProgram->getAttributeLocation( "vTex" );

	// particle shader
	
	particleShaderProgram = new CSCI441::ShaderProgram( "shaders/billboardQuadShader.v.glsl",
														"shaders/billboardQuadShader.g.glsl",
														"shaders/billboardQuadShader.f.glsl" );
	uniform_particle_modelview_loc = particleShaderProgram->getUniformLocation( "mvMatrix" );
	uniform_particle_projection_loc = particleShaderProgram->getUniformLocation( "projMatrix" );
	attrib_particle_vpos_loc = particleShaderProgram->getAttributeLocation( "vPos" );
	uniform_particle_time_loc = particleShaderProgram->getUniformLocation( "time" );
	

	// phong shader
	phongShaderProgram = new CSCI441::ShaderProgram( "shaders/Phong.v.glsl", "shaders/Phong.f.glsl" );
	uniform_phong_m_loc 		= phongShaderProgram->getUniformLocation( "modelMtx" );
	uniform_phong_v_loc 		= phongShaderProgram->getUniformLocation( "viewMtx" );
	uniform_phong_p_loc 		= phongShaderProgram->getUniformLocation( "projectionMtx" );
	uniform_phong_norm_loc   	= phongShaderProgram->getUniformLocation( "normalMtx" );
	uniform_phong_md_loc 		= phongShaderProgram->getUniformLocation( "materialDiffuse" );
	uniform_phong_ms_loc 		= phongShaderProgram->getUniformLocation( "materialSpecular" );
	uniform_phong_ma_loc 		= phongShaderProgram->getUniformLocation( "materialAmbient" );
	uniform_phong_s_loc			= phongShaderProgram->getUniformLocation( "materialShininess" );
	attrib_phong_vpos_loc 		= phongShaderProgram->getAttributeLocation( "vPos" );
	attrib_phong_vnorm_loc	 	= phongShaderProgram->getAttributeLocation( "vNormal" );
	
	// ********** CEL SHADER ***********
	celShaderProgram = new CSCI441::ShaderProgram( "shaders/phongCelShader.v.glsl",
		"shaders/phongCelShader.f.glsl" );
	
	cel_numlights_uniform_location = celShaderProgram->getUniformLocation( "numLights" );
	cel_light_uniform_location = celShaderProgram->getUniformLocation( "lightColor" );
	cel_diff_uniform_location = celShaderProgram->getUniformLocation( "diffColor" );
	cel_spec_uniform_location = celShaderProgram->getUniformLocation( "specColor" );
	cel_ambi_uniform_location = celShaderProgram->getUniformLocation( "ambiColor" );
	cel_shiny_uniform_location = celShaderProgram->getUniformLocation( "shininess" );
	
	cel_m_uniform_location = celShaderProgram->getUniformLocation( "mMatrix" );
	cel_v_uniform_location = celShaderProgram->getUniformLocation( "vMatrix" );
	cel_p_uniform_location = celShaderProgram->getUniformLocation( "pMatrix" );
	
	cel_vpos_attrib_location = celShaderProgram->getAttributeLocation( "vPos" );
	cel_normal_atrrib_location = celShaderProgram->getAttributeLocation( "vNorm" );
	cel_lpos_uniform_location = celShaderProgram->getUniformLocation( "lPos" );
	
	// ********** FEAR SHADER ***********
	
	fearShaderProgram = new CSCI441::ShaderProgram( "shaders/fearShader.v.glsl",
		"shaders/fearShader.f.glsl" );
	
	fear_random_uniform_location = fearShaderProgram->getUniformLocation( "random" );	
	fear_m_uniform_location = fearShaderProgram->getUniformLocation( "mMatrix" );
	fear_v_uniform_location = fearShaderProgram->getUniformLocation( "vMatrix" );
	fear_p_uniform_location = fearShaderProgram->getUniformLocation( "pMatrix" );
	
	fear_vpos_attrib_location = fearShaderProgram->getAttributeLocation( "vPos" );
	fear_normal_atrrib_location = fearShaderProgram->getAttributeLocation( "vNorm" );
}

// setupBuffers() //////////////////////////////////////////////////////////////
//
//      Create our VAOs & VBOs. Send vertex data to the GPU for future rendering
//
////////////////////////////////////////////////////////////////////////////////
void setupBuffers() {

	// lion
	lion = new Lion(glm::vec3(-7, 0, -7), 0.1, platformSize);

	// alpacas
	for (int i = 0; i < startingNumAlpacas; i++) {
		float alpacaX = ((float)rand()/(float)RAND_MAX)*platformSize*2 - platformSize;
		float alpacaZ = ((float)rand()/(float)RAND_MAX)*platformSize*2 - platformSize;
		Alpaca* alpaca = new Alpaca(glm::vec3(alpacaX, 0.0f, alpacaZ), ((float)rand()/(float)RAND_MAX)*6.283f, platformSize);
		alpacas.push_back(alpaca);
	}

	//Skybox
	int skyboxsize = 40;
	float padding = 0.01;
	int index = 0;
	// Create vertex arrays
	VertexTextured skybox[6][4];
	// bottom
	skybox[index][0].s = 0; skybox[index][0].t = 0; skybox[index][0].x = -skyboxsize; skybox[index][0].y = -skyboxsize+padding; skybox[index][0].z = -skyboxsize;
	skybox[index][1].s = 1; skybox[index][1].t = 0; skybox[index][1].x = skyboxsize; skybox[index][1].y = -skyboxsize+padding; skybox[index][1].z = -skyboxsize;
	skybox[index][2].s = 1; skybox[index][2].t = 1; skybox[index][2].x = skyboxsize; skybox[index][2].y = -skyboxsize+padding; skybox[index][2].z = skyboxsize;
	skybox[index][3].s = 0; skybox[index][3].t = 1; skybox[index][3].x = -skyboxsize; skybox[index][3].y = -skyboxsize+padding; skybox[index][3].z = skyboxsize;
	// top
	index++;
	skybox[index][0].s = 0; skybox[index][0].t = 1; skybox[index][0].x = -skyboxsize; skybox[index][0].y = skyboxsize-padding; skybox[index][0].z = -skyboxsize;
	skybox[index][1].s = 1; skybox[index][1].t = 1; skybox[index][1].x = skyboxsize; skybox[index][1].y = skyboxsize-padding; skybox[index][1].z = -skyboxsize;
	skybox[index][2].s = 1; skybox[index][2].t = 0; skybox[index][2].x = skyboxsize; skybox[index][2].y = skyboxsize-padding; skybox[index][2].z = skyboxsize;
	skybox[index][3].s = 0; skybox[index][3].t = 0; skybox[index][3].x = -skyboxsize; skybox[index][3].y = skyboxsize-padding; skybox[index][3].z = skyboxsize;
	// sides
	index++;
	skybox[index][0].s = 0; skybox[index][0].t = 0; skybox[index][0].x = -skyboxsize+padding; skybox[index][0].y = -skyboxsize; skybox[index][0].z = -skyboxsize;
	skybox[index][1].s = 0; skybox[index][1].t = 1; skybox[index][1].x = -skyboxsize+padding; skybox[index][1].y = skyboxsize; skybox[index][1].z = -skyboxsize;
	skybox[index][2].s = 1; skybox[index][2].t = 1; skybox[index][2].x = -skyboxsize+padding; skybox[index][2].y = skyboxsize; skybox[index][2].z = skyboxsize;
	skybox[index][3].s = 1; skybox[index][3].t = 0; skybox[index][3].x = -skyboxsize+padding; skybox[index][3].y = -skyboxsize; skybox[index][3].z = skyboxsize;
	index++;
	skybox[index][0].s = 1; skybox[index][0].t = 0; skybox[index][0].x = skyboxsize-padding; skybox[index][0].y = -skyboxsize; skybox[index][0].z = -skyboxsize;
	skybox[index][1].s = 1; skybox[index][1].t = 1; skybox[index][1].x = skyboxsize-padding; skybox[index][1].y = skyboxsize; skybox[index][1].z = -skyboxsize;
	skybox[index][2].s = 0; skybox[index][2].t = 1; skybox[index][2].x = skyboxsize-padding; skybox[index][2].y = skyboxsize; skybox[index][2].z = skyboxsize;
	skybox[index][3].s = 0; skybox[index][3].t = 0; skybox[index][3].x = skyboxsize-padding; skybox[index][3].y = -skyboxsize; skybox[index][3].z = skyboxsize;
	index++;
	skybox[index][0].s = 1; skybox[index][0].t = 0; skybox[index][0].x = -skyboxsize; skybox[index][0].y = -skyboxsize; skybox[index][0].z = -skyboxsize+padding;
	skybox[index][1].s = 0; skybox[index][1].t = 0; skybox[index][1].x = skyboxsize; skybox[index][1].y = -skyboxsize; skybox[index][1].z = -skyboxsize+padding;
	skybox[index][2].s = 0; skybox[index][2].t = 1; skybox[index][2].x = skyboxsize; skybox[index][2].y = skyboxsize; skybox[index][2].z = -skyboxsize+padding;
	skybox[index][3].s = 1; skybox[index][3].t = 1; skybox[index][3].x = -skyboxsize; skybox[index][3].y = skyboxsize; skybox[index][3].z = -skyboxsize+padding;
	index++;
	skybox[index][0].s = 0; skybox[index][0].t = 0; skybox[index][0].x = -skyboxsize; skybox[index][0].y = -skyboxsize; skybox[index][0].z = skyboxsize-padding;
	skybox[index][1].s = 1; skybox[index][1].t = 0; skybox[index][1].x = skyboxsize; skybox[index][1].y = -skyboxsize; skybox[index][1].z = skyboxsize-padding;
	skybox[index][2].s = 1; skybox[index][2].t = 1; skybox[index][2].x = skyboxsize; skybox[index][2].y = skyboxsize; skybox[index][2].z = skyboxsize-padding;
	skybox[index][3].s = 0; skybox[index][3].t = 1; skybox[index][3].x = -skyboxsize; skybox[index][3].y = skyboxsize; skybox[index][3].z = skyboxsize-padding;

	// Index array
	unsigned short * index_array = new unsigned short[4];
	index_array[0] = 0; index_array[1] = 1; index_array[2] = 3; index_array[3] = 2;
	GLuint indexVBOD;
	glGenBuffers(1, &indexVBOD);
	
	for (int i = 0; i < 6; i++) {
		glGenVertexArrays(1, &skyboxVAODs[i]);
		glBindVertexArray(skyboxVAODs[i]);
		GLuint quadVBOD;
		glGenBuffers(1, &quadVBOD);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBOD);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTextured) * 4, skybox[i], GL_STATIC_DRAW);
		glEnableVertexAttribArray(vpos_texture_attrib_location);
		glVertexAttribPointer(vpos_texture_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*)0);
		glEnableVertexAttribArray(vtex_texture_attrib_loaction);
		glVertexAttribPointer(vtex_texture_attrib_loaction, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*)((sizeof(float))*3));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOD);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 4, index_array, GL_STATIC_DRAW);
	}

	// platform
	VertexTextured platform[4] = {
		{ -platformSize, 0.0f, -platformSize,   0.0f,  0.0f },
		{  platformSize, 0.0f, -platformSize,   1.0f,  0.0f },
		{ -platformSize, 0.0f,  platformSize,   0.0f,  1.0f },
		{  platformSize, 0.0f,  platformSize,   1.0f,  1.0f }
	};
	
	glGenVertexArrays(1, &platformVAO);
	glBindVertexArray(platformVAO);

	unsigned short platform_index_array[4] = {0,1,2,3};
	GLuint platformIndexVBO;
	glGenBuffers(1, &platformIndexVBO);

	GLuint platformVBO;
	glGenBuffers(1, &platformVBO);
	glBindBuffer(GL_ARRAY_BUFFER, platformVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTextured) * 9, platform, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vpos_texture_attrib_location);
	glVertexAttribPointer(vpos_texture_attrib_location, 3, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*)0);
	glEnableVertexAttribArray(vtex_texture_attrib_loaction);
	glVertexAttribPointer(vtex_texture_attrib_loaction, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTextured), (void*)((sizeof(float))*3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, platformIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * 10, platform_index_array, GL_STATIC_DRAW);
}

void setupTextures() {
	skyboxTextureHandles[0] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/negy.jpg" );
	skyboxTextureHandles[1] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/posy.jpg" );
	skyboxTextureHandles[2] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/negx.jpg" );
	skyboxTextureHandles[3] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/posx.jpg" );
	skyboxTextureHandles[4] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/negz.jpg" );
	skyboxTextureHandles[5] = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/posz.jpg" );
	platformTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/platform.jpg" );
	particleTextureHandle = CSCI441::TextureUtils::loadAndRegisterTexture( "textures/puff.png" );
}

// Used to split file contents into fields
vector<string> split(const std::string &s, char delim) {
	vector<string> fields;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        fields.push_back(item);
    }
	return fields;
}

void readSetupFile(){
	string line;
	ifstream myfile ("setup.txt");
	if (myfile.is_open()) {
		getline (myfile,line);
		getline (myfile,line);
		startingNumAlpacas = int(atof(line.c_str()));
		getline (myfile,line);
		getline (myfile,line);
		birthRate = int(atof(line.c_str()));
		getline (myfile,line);
		getline (myfile,line);
		vector<string> color = split(line, ',');
		globalLightColor = glm::vec4(float(atof(color.at(0).c_str())),float(atof(color.at(1).c_str())),float(atof(color.at(2).c_str())),float(atof(color.at(3).c_str())));
		getline (myfile,line);
		getline (myfile,line);
		color = split(line, ',');
		dynamicLightColor = glm::vec4(float(atof(color.at(0).c_str())),float(atof(color.at(1).c_str())),float(atof(color.at(2).c_str())),float(atof(color.at(3).c_str())));
		getline (myfile,line);
		getline (myfile,line);
		maxAlpacas = int(atof(line.c_str()));
		myfile.close();
	}
	else cout << "Unable to open file";
}

void makeBabies() {
	for (unsigned int i = 0; i < alpacas.size(); i++) {
		for (unsigned int j = 0; j < alpacas.size(); j++) {
			if (i != j) {
				Alpaca* a1 = alpacas.at(i);
				Alpaca* a2 = alpacas.at(j);
				if (a1->birthTimer <= 0 && a2->birthTimer <= 0)
					if (glm::length(a1->position - a2->position) <= babyDistance) {
						a1->birthTimer = birthRate;
						a2->birthTimer = birthRate;
						glm::vec3 halfwayPosition = a1->position + 0.5f * (a2->position - a1->position);
						Alpaca* baby = new Alpaca(halfwayPosition, ((float)rand()/(float)RAND_MAX)*6.283f, platformSize);
						alpacas.push_back(baby);
					}
			}
		}
	}
	while(alpacas.size() > maxAlpacas) {
		alpacas.pop_back();
	}
}

void eatAlpacas() {
	int numAlpacas = alpacas.size();
	for (int i = 0; i < numAlpacas; i++) {
		if (glm::length(lion->headPosition() - alpacas.at(i)->position) <= eatDistance*(lion->size/0.5f)) {
			lion->eat(alpacas.at(i)->size);
			glm::vec3 eatPosition = alpacas.at(i)->position;
			delete alpacas.at(i);
			alpacas.erase(alpacas.begin() + i);
			numAlpacas -= 1;

			bool isPsNearby = false;
			// create particle system
			for (unsigned int i = 0; i < particleSystems.size(); i++) {
				if (glm::length(eatPosition - particleSystems.at(i)->position) <= 0.75f) {
					isPsNearby = true;
				}
			}
			if (!isPsNearby) {
				ParticleSystem* ps = new ParticleSystem();
				ps->initialize(eatPosition, 60.0f, 0.1f, 0.3f, 2.0f);
				ps->setupBuffers(attrib_particle_vpos_loc, particleTextureHandle);
				ps->createParticles();
				particleSystems.push_back(ps);
			}
		}
	}
}

void updateScene() {

	lion->rotate(isLionRotating);
	lion->moveForward(isLionMoving);

	for (unsigned int i = 0; i < alpacas.size(); i++) {
		alpacas.at(i)->update();
		alpacas.at(i)->updateHeading(lion->position);
		alpacas.at(i)->moveForward(lion->position);
	}
	if (alpacas.size() <= 100) { makeBabies(); }

	eatAlpacas();
	
	int numPS = particleSystems.size();
	for (int i = 0; i < numPS; i++) {
		particleSystems.at(i)->update();
		if (particleSystems.at(i)->dead) {
			delete particleSystems.at(i);
			particleSystems.erase(particleSystems.begin() + i);
			numPS -= 1;
		}
	}
}

//******************************************************************************
//
// Rendering / Drawing Functions - this is where the magic happens!

int findClosest() {
	glm::vec3 lionPosition = lion->position;
	int closest = -1;
	int minDistance = -1;
	for (unsigned int i = 0; i < alpacas.size(); i++) {
		float currentDistance = glm::length(lionPosition - alpacas.at(i)->position);
		if (closest == -1 || currentDistance < minDistance) {
			closest = i;
			minDistance = currentDistance;
		}
	}
	return closest;
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//		This method will contain all of the objects to be drawn.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene( glm::mat4 viewMtx, glm::mat4 projMtx ) {

  	// stores our model matrix
  	glm::mat4 modelMtx;

	// Use texture shader program
	textureShaderProgram->useProgram();
	// send MVP to GPU
	glUniformMatrix4fv(m_uniform_location, 1, GL_FALSE, &modelMtx[0][0]);
	glUniformMatrix4fv(v_uniform_location, 1, GL_FALSE, &viewMtx[0][0]);
	glUniformMatrix4fv(p_uniform_location, 1, GL_FALSE, &projMtx[0][0]);
	// skybox
	for (int i = 0; i < 6; i++) {
		glBindTexture( GL_TEXTURE_2D, skyboxTextureHandles[i] );
		glBindVertexArray(skyboxVAODs[i]);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
	}
	// platform
	glBindTexture(GL_TEXTURE_2D, platformTextureHandle);
	glBindVertexArray(platformVAO);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);
	


	/*
	// Phong shader
	phongShaderProgram->useProgram();

	glm::mat4 nMtx = glm::transpose(glm::inverse(viewMtx * modelMtx));
	glUniformMatrix4fv( uniform_phong_v_loc, 1, GL_FALSE, &viewMtx[0][0] );
	glUniformMatrix4fv( uniform_phong_p_loc, 1, GL_FALSE, &projMtx[0][0] );
	glUniformMatrix4fv( uniform_phong_norm_loc, 1, GL_FALSE, &nMtx[0][0] );

	// Lion
	lion->draw(modelMtx, uniform_phong_m_loc, uniform_phong_ma_loc, uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc);
	// Alpacas
	for (unsigned int i = 0; i < alpacas.size(); i++) {
		alpacas.at(i)->draw(modelMtx, uniform_phong_m_loc, uniform_phong_ma_loc, uniform_phong_md_loc, uniform_phong_ms_loc, uniform_phong_s_loc);
	}*/
	
	int closestAlpaca = findClosest();
	if (closestAlpaca >= 0) {
		dynamicLightPosition = alpacas.at(closestAlpaca)->position;
		
		// fear shader
		fearShaderProgram->useProgram();
		
		glUniform1f(fear_random_uniform_location, glfwGetTime());
		glUniformMatrix4fv(fear_v_uniform_location, 1, GL_FALSE, &viewMtx[0][0]);
		glUniformMatrix4fv(fear_p_uniform_location, 1, GL_FALSE, &projMtx[0][0]);
		alpacas.at(closestAlpaca)->draw(modelMtx, fear_m_uniform_location, cel_ambi_uniform_location, cel_diff_uniform_location, cel_spec_uniform_location, cel_shiny_uniform_location);
	}

	celShaderProgram->useProgram();
	
	glUniform1i(cel_numlights_uniform_location, 2);
	glm::vec4 lightColors[2];
	lightColors[0] = globalLightColor;
	lightColors[1] = dynamicLightColor;
	glUniform4fv(cel_light_uniform_location, 2, &lightColors[0][0]);

	glUniformMatrix4fv(cel_v_uniform_location, 1, GL_FALSE, &viewMtx[0][0]);
	glUniformMatrix4fv(cel_p_uniform_location, 1, GL_FALSE, &projMtx[0][0]);
	glm::vec3 lightPositions[2];
	lightPositions[0] = globalLightPosition;
	lightPositions[1] = dynamicLightPosition;
	glUniform3fv(cel_lpos_uniform_location, 2, &lightPositions[0][0]);
	
	// Lion
	lion->draw(modelMtx, cel_m_uniform_location, cel_ambi_uniform_location, cel_diff_uniform_location, cel_spec_uniform_location, cel_shiny_uniform_location);
	
	// Alpacas
	for (int i = 0; i < (int) alpacas.size(); i++) {
		if (i != closestAlpaca) {
			alpacas.at(i)->draw(modelMtx, cel_m_uniform_location, cel_ambi_uniform_location, cel_diff_uniform_location, cel_spec_uniform_location, cel_shiny_uniform_location);
		}
	}
	
	
	// particle shader
	particleShaderProgram->useProgram();
	glm::mat4 mvMtx = viewMtx * modelMtx;
	glUniformMatrix4fv(uniform_particle_modelview_loc, 1, GL_FALSE, &mvMtx[0][0]);
	glUniformMatrix4fv(uniform_particle_projection_loc, 1, GL_FALSE, &projMtx[0][0]);
	// sort particle systems
	glm::vec4 v = glm::normalize(glm::vec4(lookAtPoint, 1) - glm::vec4(eyePoint, 1));
	int size = particleSystems.size();
	int* orderedIndices = new int[size];
	float* distances = new float[size];
	for (int i = 0; i < size; i++) {
		glm::vec4 p = glm::vec4(particleSystems.at(i)->position, 1);
		p = modelMtx * p;
		glm::vec4 ep = (p - glm::vec4(eyePoint, 1));
		float distance = glm::dot(v, ep);
		orderedIndices[i] = i;
		distances[i] = distance;
	}
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (distances[j] < distances[i]) {
				double tempd = distances[i];
				distances[i] = distances[j];
				distances[j] = tempd;
				int tempi = orderedIndices[i];
				orderedIndices[i] = orderedIndices[j];
				orderedIndices[j] = tempi;
			}
		}
	}
	for (int i = 0; i < particleSystems.size(); i++) {
		particleSystems.at(orderedIndices[i])->draw(lookAtPoint, eyePoint, modelMtx, uniform_particle_time_loc);
	}
	
}


///*****************************************************************************
//
// Our main function
// main() ///////////////////////////////////////////////////////////////
//
//		Really you should know what this is by now.
//
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] ) {
	srand( time(0) );		// seed our RNG
	readSetupFile();

	/*
	if (argc != 2) {
		fprintf( stderr, "Usage: ./a6.exe <controlFile.txt>\n");
		exit(EXIT_FAILURE);
	}	
	*/

  // GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();										// initialize all of the OpenGL specific information
	setupGLEW();											// initialize all of the GLEW specific information

  CSCI441::OpenGLUtils::printOpenGLInfo();

	setupShaders(); 									// load our shader program into memory
	setupBuffers();										// load all our VAOs and VBOs into memory
	setupTextures();									// load all our textures into memory

  // needed to connect our 3D Object Library to our shader
  CSCI441::setVertexAttributeLocations( attrib_phong_vpos_loc, attrib_phong_vnorm_loc, -1);

	convertSphericalToCartesian();		// set up our camera position
	CSCI441::drawSolidSphere( 1, 16, 16);
  //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.
	while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
	
    	glDrawBuffer( GL_BACK );				// work with our back frame buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

		// Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.
		glfwGetFramebufferSize( window, &windowWidth, &windowHeight );

		// update the viewport - tell OpenGL we want to render to the whole window
		glViewport( 0, 0, windowWidth, windowHeight );

		// set the projection matrix based on the window size
		// use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projectionMatrix = glm::perspective( 45.0f, windowWidth / (float) windowHeight, 0.001f, 100.0f );

		// set up our look at matrix to position our camera
		eyePoint = eyePointOffset + lion->position;
		lookAtPoint = lion->position;
		if (chaseCamera) {
			lookAtPoint += lion->direction;
		}
		
		glm::mat4 viewMatrix = glm::lookAt( eyePoint, lookAtPoint, upVector );

		// draw everything to the window
		// pass our view and projection matrices as well as deltaTime between frames
		renderScene( viewMatrix, projectionMatrix );

		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen

		updateScene();
		
		recomputeOrientation();
	}

  glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;
}
