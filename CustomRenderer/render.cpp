#include "render.h"
#include <stddef.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "tinyobj/tiny_obj_loader.h"
#include "stb/stb_image.h"

geometry makeGeometry(vertex* verts, size_t vertCount, unsigned int* indices, size_t indxCount)
{
    // Make an instance of geometry
    geometry newGeo = {};
    newGeo.size = indxCount;

    // Generate buffers and VAO
    glGenBuffers(1, &newGeo.vbo);       // VBO
    glGenBuffers(1, &newGeo.ibo);       // IBO
    glGenVertexArrays(1, &newGeo.vao);  // VAO

    // Bind the VAO and buffers
    // NOTE: ORDER MATTERS!!
    glBindVertexArray(newGeo.vao);                      // Bind VAO [FIRST]
    glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);          // Bind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);  // Bind IBO

    // Populate the buffers (aka send the data over)
    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Describe the data contained within the buffers
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                // Position.
                          4,                // How many things?
                          GL_FLOAT,         // What types of things are in that thing?
                          GL_FALSE,         // Normalize the data? (T = yes F = no).
                          sizeof(vertex),   // Byte offset between verticies.
                          (void*)offsetof(vertex, pos));        // Byte offset within a vertex to get to this data.

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,                // Color.
                          4,                // How many things?
                          GL_FLOAT,         // What types of things are in that thing?
                          GL_FALSE,         // Normalize the data? (T = yes F = no).
                          sizeof(vertex),   // Byte offset between verticies.
                          (void*)offsetof(vertex, col));        // Byte offset within a vertex to get to this data.

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,                                    // UVs.
                          2,                                    // How many things?
                          GL_FLOAT,                             // What types of things are in that thing?
                          GL_FALSE,                             // Normalize the data? (T = yes F = no).
                          sizeof(vertex),                       // Byte offset between verticies.
                          (void*)offsetof(vertex, uv));        // Byte offset within a vertex to get to this data.

    // Unbind the butters (VAO then the buffers)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Return the geometry
    return newGeo;
}

geometry loadGeometry(const char* filepath)
{
    // Load all the data from the file.
    tinyobj::attrib_t vertexAttributes;

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string error;

    bool success = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &error, filepath);

    // Double hceck everythings okay.
    if (!success)
    {
        fprintf(stderr, error.c_str());
        return {}; // Empty geoplaceholder for failure on loading.
    }

    // Actually get the mesh data.
    std::vector<vertex> verticies;
    std::vector<unsigned int> indices;

    // Form geometry data from the mesh data provided by tiny-obj.
    size_t offset = 0;
    for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); i++)
    {
        unsigned char faceVerticies = shapes[0].mesh.num_face_vertices[i];

        assert(faceVerticies == 3 && "unsupported number of face verticies");

        for (unsigned char j = 0; j < faceVerticies; j++)
        {
            tinyobj::index_t idx = shapes[0].mesh.indices[offset + j];

            tinyobj::real_t posX = vertexAttributes.vertices[3 * idx.vertex_index + 0];
            tinyobj::real_t posY = vertexAttributes.vertices[3 * idx.vertex_index + 1];
            tinyobj::real_t posZ = vertexAttributes.vertices[3 * idx.vertex_index + 2];

            tinyobj::real_t colR = 1.0f;
            tinyobj::real_t colG = 1.0f;
            tinyobj::real_t colB = 1.0f;

            tinyobj::real_t texU = vertexAttributes.texcoords[2 * idx.texcoord_index + 0];
            tinyobj::real_t texV = vertexAttributes.texcoords[2 * idx.texcoord_index + 1];

            verticies.push_back({ { posX, posY, posZ, 1.0f }, { colR, colG, colB, 1.0f }, {texU, texV} });
            indices.push_back(faceVerticies * i + j);
        }

        offset += faceVerticies;
    }

    // Return makeGeometry using the data from tiny-obj.
    return makeGeometry(verticies.data(), verticies.size(), indices.data(), indices.size());
}

void freeGeometry(geometry& geo)
{
    glDeleteBuffers(1, &geo.vbo);
    glDeleteBuffers(1, &geo.ibo);
    glDeleteVertexArrays(1, &geo.vao);

    // This geometry...
    geo = { /*empty*/ };
    // Yeet.
}

texture loadTexture(const char* filepath)
{
    assert(filepath != nullptr && "File path was invalid");

    // Create variables to hold onto some data.
    int imageWidth, imageHeight, imageFormat;
    unsigned char* rawPixelData = nullptr; // Use nullptr over NULL.

    // Use stb to load the image (and set some settings where necessary).
    stbi_set_flip_vertically_on_load(true);
    rawPixelData = stbi_load(filepath, &imageWidth, &imageHeight, &imageFormat, STBI_default);
    assert(rawPixelData != nullptr && "Image failed to load.");

    // Pass the image data over to makeTexture.
    texture newTexture = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
    assert(newTexture.handle != 0 && "Failed to create texture");

    // Free the image data (it's already in the GPU so we don't need it on the CPU side anymore).
    stbi_image_free(rawPixelData);

    // Return the texture.
    return newTexture;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels)
{
    // Figure out OpenGL texture format to use.
    assert(channels > 0 && channels < 5); // Only supports 1-4 channels.
    GLenum oglFormat = GL_RED;
    switch (channels)
    {
    case 1:
        oglFormat = GL_RED;
        break;
    case 2:
        oglFormat = GL_RG;
        break;
    case 3:
        oglFormat = GL_RGB;
        break;
    case 4:
        oglFormat = GL_RGBA;
        break;
    }

    // Generate a texture.
    texture returnVal = { 0, width, height, channels };

    glGenTextures(1, &returnVal.handle);

    // Bind an buffer data to it.
    glBindTexture(GL_TEXTURE_2D, returnVal.handle);

    glTexImage2D(GL_TEXTURE_2D, 0, oglFormat, width, height, 0, oglFormat, GL_UNSIGNED_BYTE, pixels);

    // Set some filtering settings.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // Filtering when texel density is greater than display (interpolate.)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  // Filtering when texel density is less than display (extrapolate.)

    // Return the texture.
    glBindTexture(GL_TEXTURE_2D, 0);

    return returnVal;
}

void freeTexture(texture& tex)
{
    glDeleteTextures(1, &tex.handle);
    tex = {};
}

char* loadShaderFromTxt(std::string path)
{
    std::string returnValueStr;

    std::ifstream file(path);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            returnValueStr += line + "\n";
        }
    }
    file.close();

    char * returnValue = new char[returnValueStr.size()];
    returnValueStr.copy(returnValue, returnValueStr.size());

    return returnValue;
}

shader makeShader(const char* vertSource, const char* fragSource)
{
    // Make a shader program.
    shader newShader = {};
    newShader.program = glCreateProgram();  // No params!!

    // Create the shaders (not the same as the shader program).
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);     // Vertext shader setup.
    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);   // Fragment shader setup.

    // Compile the shaders.
    glShaderSource(vert, 1, &vertSource, 0);    // Send source code for specified shader.
    glShaderSource(frag, 1, &fragSource, 0);
    glCompileShader(vert);                      // Actually compile the shader.
    glCompileShader(frag);

    // Validate shaders. (Error handling step).
    GLint shaderCompileStatus = 0;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];
        glGetShaderInfoLog(vert, 1024, &logLength, message);
        printf("[ERROR Vertext Shader]: %s", message);

        // Return an empty shader if it fails.
        return newShader;
    }

    glGetShaderiv(frag, GL_COMPILE_STATUS, &shaderCompileStatus);
    if (shaderCompileStatus != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];
        glGetShaderInfoLog(frag, 1024, &logLength, message);
        printf("[ERROR Vertext Shader]: %s", message);

        // Return an empty shader if it fails.
        return newShader;
    }


    // Attatch the shaders.
    glAttachShader(newShader.program, vert); // Associate the vertex shader with the shader program.
    glAttachShader(newShader.program, frag); // Associate the fragment shader with the shader program.

    // Link the shader program.
    glLinkProgram(newShader.program); // Link the shader program

    // Delete the shaders.
    glDeleteShader(vert); // Delete the vertex shader.
    glDeleteShader(frag); // Delete the fragment shader.

    // Return the shader.
    return newShader;
}

void freeShader(shader& shader)
{
    glDeleteProgram(shader.program);

    // I won't do the same joke twice I promise.
    shader = { };
}

void draw(const shader& shader, const geometry& geometry)
{
    // Specify which shader to use.
    glUseProgram(shader.program);

    // Specify which VAO.
    glBindVertexArray(geometry.vao);


    // Draw!
    glDrawElements(GL_TRIANGLES,        // Primitive type.
                   geometry.size,       // Indices.
                   GL_UNSIGNED_INT,     // Index type
                   0);                  
}

void setUniform(const shader& shad, GLuint location, const glm::mat4& value)
{
    // glUniform would effect the last bound shader program.
    // HOWEVER we're going for a little more flexability so we're going to specify which shader program to modify.

    glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader& shad, GLuint location, const texture& tex, int textureSlot)
{
    // Set this texture up in a slot.
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, tex.handle);

    // Pass the slot number over to the uniform.
    glProgramUniform1i(shad.program, location, textureSlot);
}
