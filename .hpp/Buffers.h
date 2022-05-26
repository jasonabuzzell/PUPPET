#ifndef BUFFERS_H
#define BUFFERS_H

#pragma once

#include "glad.h"

class EBO {
    public:
        // ID reference of Elements Buffer Object
        GLuint ID;
        // Constructor that generates a Elements Buffer Object and links it to indices
        EBO(GLuint* indices, GLsizeiptr size);

        void Bind();
        void Unbind();
        void Delete();
};

class VBO {
    public:
        // ID reference of the Vertex Buffer Object
        GLuint ID;
        // Constructor that generates a Vertex Buffer Object and links it to vertices
        VBO(GLfloat* vertices, GLsizeiptr size);

        void Bind();
        void Unbind();
        void Delete();
};

class VAO {
    public:
        // ID reference for the Vertex Array Object
        GLuint ID;
        // Constructor that generates a VAO ID
        VAO();
        // Links a VBO to the VAO using a certain layout
        void LinkVBO(VBO& VBO, GLuint layout);
        // Links a VBO Attribute such as a position or color to the VAO
        void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

        void Bind();
        void Unbind();
        void Delete();
};

#endif