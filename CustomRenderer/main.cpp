#include "context.h"
#include "render.h"

#include "glm/ext.hpp"

int main()
{
	// GAME SETUP
	// ----------------------------------------------------------
	context game;
	game.init(444, 444, "Cube.exe");
	// ----------------------------------------------------------

	// Vertices
	vertex triVerts[] =
	{
		{{-0.5f, -0.5f, 0, 1}, {0.0f, 1.0f, 0.0f, 1.0f}, {0,   0}, {0.0f, 0.0f, -1.0f, 0.0f}},	// Bottom Left
		{{ 0.5f, -0.5f, 0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}, {1,   0}, {0.0f, 0.0f, -1.0f, 0.0f}},	// Bottom Right
		{{ 0.0f,  0.5f, 0, 1}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.5, 1}, {0.0f, 0.0f, -1.0f, 0.0f}}	// Top Center
	};

	vertex quadVerts[] =
	{
		{{-1.0f, -1.0f, 0, 1}, {1.0f, 0.0f, 0.0f, 1.0f}, {0, 0}, {0.0f, 0.0f, -1.0f, 0.0f} },	// Bottom Left
		{{ 1.0f, -1.0f, 0, 1}, {0.0f, 1.0f, 0.0f, 1.0f}, {1, 0}, {0.0f, 0.0f, -1.0f, 0.0f} },	// Bottom Right
		{{-1.0f,  1.0f, 0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}, {0, 1}, {0.0f, 0.0f, -1.0f, 0.0f} },	// Top Left
		{{ 1.0f,  1.0f, 0, 1}, {0.0f, 0.0f, 0.0f, 1.0f}, {1, 1}, {0.0f, 0.0f, -1.0f, 0.0f} }	// Top Right
	};

	// Indices
	unsigned int triIndices[] = { 2, 0, 1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	// Make the geometry.
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry quad = makeGeometry(quadVerts, 6, quadIndices, 6);
	geometry cube = loadGeometry("res\\cube.obj");

	// Load textures.
	texture him = loadTexture("res\\He.png");

	// Make the shader.
	shader basicColorShader = loadShader("res\\basicVertex.txt", "res\\plainColorFragment.txt");
	shader mvpShader = loadShader("res\\mvpVertex.txt", "res\\basicFragment.txt");
	shader lightShader = loadShader("res\\litVertex.txt", "res\\litFragment.txt");

	light sun = { {-1.0f, 0.0f, 0.0f}, {1.0f,1.0f,1.0f} };

	// Set up matrices.
	glm::mat4 camProj = glm::perspective(glm::radians(80.0f), 444.0f / 444.0f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 rotatingShape = glm::identity<glm::mat4>();

	//setUniform(mvpShader, 0, camProj);
	//setUniform(mvpShader, 1, camView);
	//setUniform(mvpShader, 2, rotatingShape);
	//setUniform(mvpShader, 3, him, 0);

	setUniform(lightShader, 0, camProj);
	setUniform(lightShader, 1, camView);

	setUniform(lightShader, 2, rotatingShape);

	setUniform(lightShader, 3, him, 0);

	setUniform(lightShader, 5, { 0.1f, 0.1f, 0.1f });
	setUniform(lightShader, 6, sun.color);
	setUniform(lightShader, 7, sun.direction);


	// GAME LOOP
	// ----------------------------------------------------------
	while (!game.shouldClose())
	{
		game.tick();
		// Implement game logic here!

		rotatingShape = glm::rotate(rotatingShape, glm::radians(0.05f), glm::vec3(0, 1, 0));

		game.clear();
		// Implement render logic here!
		draw(basicColorShader, quad);

		setUniform(lightShader, 2, rotatingShape);
		setUniform(lightShader, 4, game.time());
		draw(lightShader, cube);
	}
	// ----------------------------------------------------------

	freeGeometry(triangle);
	freeGeometry(quad);
	freeGeometry(cube);

	freeShader(basicColorShader);
	freeShader(mvpShader);
	freeShader(lightShader);

	freeTexture(him);

	game.term();
	return 0;
}