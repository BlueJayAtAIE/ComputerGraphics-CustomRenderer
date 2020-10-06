#include "context.h"
#include "render.h"

#include "glm/ext.hpp"

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
		{{-0.5f, -0.5f, 0, 1}, {0.0f, 1.0f, 0.0f, 1.0f}},	// Bottom Left
		{{ 0.5f, -0.5f, 0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}},	// Bottom Right
		{{ 0.0f,  0.5f, 0, 1}, {1.0f, 0.0f, 0.0f, 1.0f}}	// Top Center
	};

	vertex quadVerts[] =
	{
		{{-1.0f, -1.0f, 0, 1}, {1.0f, 0.0f, 0.0f, 1.0f}},	// Bottom Left
		{{ 1.0f, -1.0f, 0, 1}, {0.0f, 1.0f, 0.0f, 1.0f}},	// Bottom Right
		{{-1.0f,  1.0f, 0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}},	// Top Left
		{{ 1.0f,  1.0f, 0, 1}, {0.0f, 0.0f, 0.0f, 1.0f}}	// Top Right
	};

	// Indices
	unsigned int triIndices[] = { 2, 0, 1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	// Make the geometry.
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry quad = makeGeometry(quadVerts, 6, quadIndices, 6);
	geometry cube = loadGeometry("res\\cube.obj");

	// Source for vertex shader.
	const char* basicVertShader =
		"#version 410\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec4 color;\n"
		"out vec4 vColor;\n"
		"void main()" 
		"{" 
			"gl_Position = position; "
			"vColor = color;"
		"}";

	// Source for fragment shader.
	const char* basicFragShader =
		"#version 410\n"
		"in vec4 vColor;\n"
		"out vec4 outputColor;\n"
		"void main() { outputColor = vColor; }";

	// Source for vertex shader.
	const char* mvpVertShader =
		"#version 430\n"
		"// ------- VERTEX PROVIDED ATTRIBUTES -----------\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 1) in vec4 color;\n"
		"// ------- UNIFORM ATTRIBUTES -----------\n"
		"layout (location = 0) uniform mat4 proj;\n"
		"layout (location = 1) uniform mat4 view;\n"
		"layout (location = 2) uniform mat4 model;\n"
		"out vec4 vColor;\n"
		"void main()"
		"{"
			"gl_Position = proj * view * model * position; "
			"vColor = color;"
		"}";

	// TEST
	//printf(loadShaderFromTxt("basicVertexShader.txt"));

	// Make the shader.
	shader basicShader = makeShader(basicVertShader, basicFragShader);
	shader mvpShader = makeShader(mvpVertShader, basicFragShader);

	// Set up matrices.
	glm::mat4 camProj = glm::perspective(glm::radians(80.0f), 444.0f / 444.0f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 triModel = glm::identity<glm::mat4>();

	setUniform(mvpShader, 0, camProj);
	setUniform(mvpShader, 1, camView);

	setUniform(mvpShader, 2, triModel);

	// GAME LOOP
	// ----------------------------------------------------------
	while (!game.shouldClose())
	{
		game.tick();
		// Implement game logic here!

		triModel = glm::rotate(triModel, glm::radians(0.05f), glm::vec3(0, 1, 0));

		game.clear();
		// Implement render logic here!
		draw(basicShader, quad);

		setUniform(mvpShader, 2, triModel);
		draw(mvpShader, cube);
	}
	// ----------------------------------------------------------

	freeGeometry(triangle);
	freeGeometry(quad);
	freeGeometry(cube);

	freeShader(basicShader);
	freeShader(mvpShader);

	game.term();
	return 0;
}