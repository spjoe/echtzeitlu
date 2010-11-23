#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include "common.hpp"

// This is a very simple shader class

class Shader
{
public:

	// Loads shaders from files and compiles them.
	// When path is "hello", the files "hello.frag" & "hello.vert"
	// will be loaded.
	Shader(const string &path);
	Shader(const string vert, const string frag);
	~Shader();

	// Bind the shader to the OGL state-machine
	void bind() const
	{
		glUseProgram(_program);
	}

	// Unbind the shader
	void unbind() const
	{
		glUseProgram(0);
	}

	// Query the location of a vertex attribute inside the shader.
	GLint get_attrib_location(const std::string &name) const
	{
		return glGetAttribLocation(_program, name.c_str());
	}

	// Query the location of a uniform variable inside the shader.
	GLint get_uniform_location(const std::string &name) const
	{
		return glGetUniformLocation(_program, name.c_str());
	}

	// Define the name of the variable inside the shader which represents the final color for each fragment.
	void bind_frag_data_location(const std::string &name)
	{
		if(_program > 0)
		{
			glBindFragDataLocation(_program, 0, name.c_str() );
			link();
		}
	}

	// A little cast helper.
	// With this you can simply do "if (shader) {...}" to test if a
	// shader has been compiled successfully.
	operator bool ()
	{
		return _success;
	}

private:

	bool _success;

	GLuint _vertex_shader;
	GLuint _fragment_shader;
	GLuint _program;

	GLuint compile(GLenum type, const string &source);
	void link (void);

	void shader_log(GLuint shader);
	void program_log(GLuint program);
};

#endif //#ifndef _SHADER_HPP_

