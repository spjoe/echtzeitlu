#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include "common.hpp"
#include <sstream>

bool file_exists(const string &filename);

string read_file(const string &filename);

// Query OpenGL errors and print error messages to STDERR.
void get_errors(void);
void get_errors(std::string msg);
GLuint * GenerateVBO(size_t count);
GLuint GenerateVBO();
void bindVBO(GLuint id, void * datapointer, size_t size);

template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}
#endif //#ifndef _UTILITY_HPP_
