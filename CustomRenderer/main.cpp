#include "context.h"
#include "render.h"

int main()
{
	// GAME SETUP
	// ----------------------------------------------------------
	context game;
	game.init(444, 444, "Triangle.exe");
	// ----------------------------------------------------------

	// Vertices
	vertex triVerts[] =
	{
		{{-0.5f, -0.5f, 0, 1}},	// Bottom Left
		{{0.5f, -0.5f, 0, 1}},	// Bottom Right
		{{0, 0.5f, 0, 1}}		// Top Center
	};

	// Indices
	unsigned int triIndices[] = { 2, 0, 1 };

	// Make the geometry.
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);


	// Source for vertex shader.
	// TODO find a way to change vColor via script here.
	const char* basicVertShader =
		"#version 410\n"
		"layout (location = 0) in vec4 position;\n"
		"out vec4 vColor;\n"
		"void main()" 
		"{" 
			"gl_Position = position; "
			"vColor = position;"
		"}";

	// Source for fragment shader.
	const char* basicFragShader =
		"#version 410\n"
		"in vec4 vColor;\n"
		"out vec4 outputColor;\n"
		"void main() { outputColor = vColor; }";

	// Make the shader.
	shader basicShader = makeShader(basicVertShader, basicFragShader);

	// GAME LOOP
	// ----------------------------------------------------------
	while (!game.shouldClose())
	{
		game.tick();
		// Implement game logic here!

		game.clear();
		// Implement render logic here!
		draw(basicShader, triangle);
	}
	// ----------------------------------------------------------

	freeGeometry(triangle);

	freeShader(basicShader);

	game.term();
	return 0;
}