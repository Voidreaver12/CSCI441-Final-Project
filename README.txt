One Final Hu-Ra / Ian Tobiason, Adrein Perinet, Noah Deibert

Final Project

Description:
A game where you play as a Lion trying to eat Alpacas
	Two Texture Applications: (1) skybox, (2) textured quads in Alpaca death particles
	Two Lights: (1) global light source, (2) "feared" Alpaca emits a red light (dynamic)
	Two Shaders: (1) Cel Shader, (2) "Fear" Shader (distorts the vertices of nearby Alpacas to make them quake in fear)

Usage:
1 (Arcball Camera) 2 (Chase Camera) 3 (Sky Camera)
		W (forward)
A (left) 	D (right)
		S (back)
	SPACE (speed burst)
Zoom by ctrl left click dragging. Move camera with left click dragging.

Change Game Attributes in setup.txt

compile code with make, run with ./Final.exe

File Format:
#Starting number of Alpacas (int)
#Birth Rate (Set it higher for long cooldown between baby making) (int)
#Global Light Color (4 floats seperated by commas)
#Dynamic Light Color (4 floats seperated by commas)
#Max number of Alpacas (int)
#Burst Speed (float)
#Max Burst Time (int)
#Cool Down for Burst (int)

Adrien: Shaders and cameras
Noah: Character/particle models and game logic
Ian: Game logic and setup file

30 hours taken
lab12: 5/10 we did no post processing
Fun: 10/10



