#pragma once

#include <string>

#include "glew/glew.h" // Glew (GLuint, etc)
#include "glm/glm.hpp" // Glm math types (Vec4, etc.)
#include "glm/ext.hpp" // value_ptr

// Define vertex structure.
struct vertex
{
	glm::vec4 pos; // Vertex position
	glm::vec4 col; // Vertex color
	glm::vec2 uv;
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

struct texture
{
	GLuint handle;
	unsigned int width, height, channels;
};

// Functions to make and unmake the above types.
geometry makeGeometry(vertex *verts, size_t vertCount, unsigned int *indices, size_t indxCount);

geometry loadGeometry(const char *filepath);

void freeGeometry(geometry &geo);


texture loadTexture(const char* filepath);

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels);

void freeTexture(texture &tex);


shader makeShader(const char *vertSource, const char *fragSource);

shader loadShader(const char* vertPath, const char* fragPath);

void freeShader(shader &shader);


void draw(const shader &shader, const geometry &geometry);

void setUniform(const shader &shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader &shad, GLuint location, const texture &tex, int textureSlot);