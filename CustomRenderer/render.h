#pragma once

#include "glew/glew.h" // Glew (GLuint, etc)
#include "glm/glm.hpp" // Glm math types (Vec4, etc.)

// Define vertex structure.
struct vertex
{
	glm::vec4 pos;
};

// An object to represent our mesh.
struct geometry
{
	GLuint vao, vbo, ibo;	// Buffers.
	GLuint size;			// Index cound.
};

// An object to represent our shader.
struct shader
{
	GLuint program;
};

// Functions to make and unmake the above types.
geometry makeGeometry(vertex *verts, size_t vertCount, unsigned int *indices, size_t indxCount);

void freeGeometry(geometry &geo);

shader makeShader(const char *vertSource, const char *fragSource);

void freeShader(shader& shader);

void draw(const shader& shader, const geometry& geometry);