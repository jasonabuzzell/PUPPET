#ifndef SHADERS_H
#define SHADERS_H
#pragma once

#include "glad.h"
#include <cerrno>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string get_file_contents(const char *filename);

class Shader {
	public:
		// Reference ID of the Shader Program
		GLuint ID;
		// Constructor that build the Shader Program from 2 different shaders
		Shader(const char *vertexFile, const char *fragmentFile);

		void Activate();
		void Delete();

	private:
		// Checks if the different Shaders have compiled properly
		void compileErrors(unsigned int shader, const char *type);
};

#endif