#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include "common.hpp"

bool file_exists(const string &filename);

string read_file(const string &filename);

// Query OpenGL errors and print error messages to STDERR.
void get_errors(void);
GLuint * GenerateVBO(size_t count);
GLuint GenerateVBO();
void bindVBO(GLuint id, void * datapointer, size_t size);
#endif //#ifndef _UTILITY_HPP_
