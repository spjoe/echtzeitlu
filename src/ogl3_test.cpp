#include "common.hpp"
#include "shader.hpp"
#include "camera.h"
#include "ModelLoader.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <stdio.h>

#include <AL/alut.h>

const std::string daeModelPath = "../resources/SteamEngine/models/Steam EngineV2_6.dae";
const std::string wavAudioPath = "../resources/music.wav";
// const std::string daeModelPath = "../resources/trianglebox.dae";
glm::mat4 model;
glm::vec3 viewVector(0.0f, 0.0f, 1.0f);

// define our light
glm::vec3 light_position(0.0f, 8.0f, 3.0f);
glm::vec4 light_color(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ambient_color(0.1f, 0.1f, 0.1f, 1.0f);

GLfloat distance = -4.0f;

using namespace echtzeitlu;
Camera m_camera_1;
SceneObject *rootScene;
Shader *defaultShader;

static const glm::vec4 vertices[24] =
{
    glm::vec4(-1, -1, -1, 1),
    glm::vec4( 1, -1, -1, 1),
    glm::vec4( 1,  1, -1, 1),
    glm::vec4(-1,  1, -1, 1),

    glm::vec4(-1, -1, -1, 1),
    glm::vec4( 1, -1, -1, 1),
    glm::vec4( 1, -1,  1, 1),
    glm::vec4(-1, -1,  1, 1),

    glm::vec4( 1, -1, -1, 1),
    glm::vec4( 1,  1, -1, 1),
    glm::vec4( 1,  1,  1, 1),
    glm::vec4( 1, -1,  1, 1),

    glm::vec4( 1,  1, -1, 1),
    glm::vec4(-1,  1, -1, 1),
    glm::vec4(-1,  1,  1, 1),
    glm::vec4( 1,  1,  1, 1),

    glm::vec4(-1,  1, -1, 1),
    glm::vec4(-1, -1, -1, 1),
    glm::vec4(-1, -1,  1, 1),
    glm::vec4(-1,  1,  1, 1),

    glm::vec4(-1, -1,  1, 1),
    glm::vec4( 1, -1,  1, 1),
    glm::vec4( 1,  1,  1, 1),
    glm::vec4(-1,  1,  1, 1)
};

static const glm::vec3 normals[24] =
{
    glm::vec3( 0,  0, -1),
    glm::vec3( 0,  0, -1),
    glm::vec3( 0,  0, -1),
    glm::vec3( 0,  0, -1),

    glm::vec3( 0, -1,  0),
    glm::vec3( 0, -1,  0),
    glm::vec3( 0, -1,  0),
    glm::vec3( 0, -1,  0),

    glm::vec3( 1,  0,  0),
    glm::vec3( 1,  0,  0),
    glm::vec3( 1,  0,  0),
    glm::vec3( 1,  0,  0),

    glm::vec3( 0,  1,  0),
    glm::vec3( 0,  1,  0),
    glm::vec3( 0,  1,  0),
    glm::vec3( 0,  1,  0),

    glm::vec3(-1,  0,  0),
    glm::vec3(-1,  0,  0),
    glm::vec3(-1,  0,  0),
    glm::vec3(-1,  0,  0),

    glm::vec3( 0,  0,  1),
    glm::vec3( 0,  0,  1),
    glm::vec3( 0,  0,  1),
    glm::vec3( 0,  0,  1),
};

static const glm::vec4 colors[24] =
{
    glm::vec4(1, 0, 0.5, 1),
	glm::vec4(1, 0, 0.5, 1),
	glm::vec4(1, 0, 0.5, 1),
	glm::vec4(1, 0, 0.5, 1), 

    glm::vec4(1, 0, 0, 1), 
	glm::vec4(1, 0, 0, 1), 
	glm::vec4(1, 0, 0, 1), 
	glm::vec4(1, 0, 0, 1), 

    glm::vec4(0, 1, 0, 1), 
	glm::vec4(0, 1, 0, 1), 
	glm::vec4(0, 1, 0, 1), 
	glm::vec4(0, 1, 0, 1), 

    glm::vec4(0, 0, 1, 1), 
	glm::vec4(0, 0, 1, 1), 
	glm::vec4(0, 0, 1, 1), 
	glm::vec4(0, 0, 1, 1),

    glm::vec4(1, 1, 0, 1), 
	glm::vec4(1, 1, 0, 1), 
	glm::vec4(1, 1, 0, 1), 
	glm::vec4(1, 1, 0, 1),

    glm::vec4(1, 0.5, 0, 1),
	glm::vec4(1, 0.5, 0, 1),
	glm::vec4(1, 0.5, 0, 1),
	glm::vec4(1, 0.5, 0, 1), 
};

static const int indices[36] =
{
    0, 2, 1, 0, 3, 2,
    4, 5, 6, 4, 6, 7,
    8, 9, 10, 8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23,
};

int width=800,
	height=800;


int oldx,oldy;

static void
reportAlutError (void)
{
  fprintf (stderr, "ALUT error: %s\n",
           alutGetErrorString (alutGetError ()));
  exit (EXIT_FAILURE);
}


void mouseMovementCb (int x, int y)
{
	int dx = oldx - x;
	int dy = oldy - y;

	oldx = x;
	oldy = y;

	GLfloat alpha = 40.0f*(GLfloat)dx/(GLfloat)width;
	GLfloat beta = 40.0f*(GLfloat)dy/(GLfloat)height;
	
	m_camera_1.rotateY(alpha);
	m_camera_1.rotateS(beta);
	
	// rotate camera about origin (0,0,0) and the vector y and s respectively
// 	m_camera_1.orbit(glm::vec3(0,0,0),glm::vec3(0,1,0), alpha);
// 	m_camera_1.orbit(glm::vec3(0,0,0),m_camera_1.s, beta);
}

void init_matrixs()
{
	// calculate the aspect ratio of the window and create a perspective matrix
	// with a 60 degree field of view
	float aspect = float(width)/float(height);
	m_camera_1.perspective(60.0f, aspect, 0.1f, 100.0f);

    // move camera 4 units backward, i.e. movie the scene 4 units forward
	m_camera_1.lookat(glm::vec3(0,0,-4), glm::vec3(0,0,0), glm::vec3(0,1,0));

    // initialize the model matrix to identity
	model = glm::mat4(1.0f);
}
// Test if we got a valid forward compatible context (FCC)
void test_ogl3(void)
{
	GLint profile;

	// retrieve our OpenGL-version
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	// check if we have a core-profile
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
	if (profile == GL_CONTEXT_CORE_PROFILE_BIT) {
		std::cout << "Having a core-profile" << std::endl;
	} else {
		std::cout << "Having a compatibility-profile" << std::endl;
	}

	// Try doing a fixed-function operation.
	// Since this is a FCC, this should create an error.
	cout << "Doing illegal operation (glTranslate) now..." << endl;

	glGetError(); // Clear error state first..
	glTranslatef(1,0,0);

	if (glGetError() != GL_NO_ERROR)
		cout << "Illegal operation caused an error message. :)" << endl;
	else
		cerr << "Illegal operation did NOT cause an error message. :(" << endl;
}

// workaround 2: see explanation before entering main loop
// global variables are a bit ugly, so we could easily encapsulate
// the function pointer in a class...
PFNGLDELETEVERTEXARRAYSPROC my_glDeleteVertexArrays;
void release_vbo_vao(GLuint *vbo_id, GLuint *vao_id)
{
	// on some computers this call fails ...
	// ==> glDeleteVertexArrays(1, vao_id);
	// ... small workaround:
	// NOTE: this call might fail to provide us with the correct address at this point 
	//		 (if window is closed for example)
	//PFNGLDELETEVERTEXARRAYSPROC my_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
	my_glDeleteVertexArrays(1, vao_id);

	glDeleteBuffers(3, vbo_id);

	*vao_id = 0;
	vbo_id[0] = vbo_id[1] = vbo_id[2] = 0;
}


void init_vbo_vao(const Shader &shader, GLuint *vbo_id, GLuint *vao_id)
{
	// Our triangle data
	static GLfloat triangle_positions[] = {	 0.0f,		  1.0f, 0.0f, 1.0f,
											 0.8660254f, -0.5f, 0.0f, 1.0f,
											-0.8660254f, -0.5f, 0.0f, 1.0f	};

	static GLfloat triangle_colors[] = {	1, 0, 0,
											0, 1, 0,
											0, 0, 1	};

	// Allocate and assign a Vertex Array Object to our handle

	// on some computers this call fails ...
	// ==> 	glGenVertexArrays(1, vao_id);
	// ... small workaround:
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, vao_id);

	// Bind our Vertex Array Object as the current used object

	// on some computers this call fails ...
	// ==> glBindVertexArray(*vao_id);
	// ... small workaround:
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(*vao_id);

	// Allocate and assign two Vertex Buffer Objects (VBOs) to our handle
	glGenBuffers(3, vbo_id);
	
	// Bind our first VBO as being the active buffer and storing vertex attributes (coordinates)	
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id[0]);
	glBufferData(GL_ARRAY_BUFFER, 24 * 4 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	GLint vertex_location = shader.get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);

	// Repeat for second VBO storing vertex colors
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id[1]);
	glBufferData(GL_ARRAY_BUFFER, 24 * 4 * sizeof(GLfloat), colors, GL_STATIC_DRAW);

	GLint color_location = shader.get_attrib_location("color");
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(	color_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
	
	// Repeat for second VBO storing normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id[2]);
	glBufferData(GL_ARRAY_BUFFER, 24 * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

	GLint normal_location = shader.get_attrib_location("normal");
	glEnableVertexAttribArray(normal_location);
	glVertexAttribPointer(	normal_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);

	// unbind VAO

	// on some computers this call fails ...
	// ==> glBindVertexArray(0);
	// ... small workaround:
	my_glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


// This draws a shaded 2D triangle on the screen.
void draw (const Shader &shader, GLuint vao_id)
{
//	glm::mat4 rotation_matrix = 
//		glm::gtx::transform::rotate(10.0f*(GLfloat)glfwGetTime(), 
//									0.0f, 0.0f, 1.0f
	// on some computers this call fails ...
	// ==> glBindVertexArray(vao_id);
	// ... small workaround:

	shader.bind();
	get_errors();

	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
	// Set our matrix uniform
//	GLint matrix_location = shader.get_uniform_location("rotation_matrix");
//	glUniformMatrix4fv( matrix_location, 
//						1, 
//						GL_FALSE, 
//						glm::value_ptr(rotation_matrix));

    // set light uniforms
    GLint light_position_uniform = shader.get_uniform_location( "light_position");
    GLint light_color_uniform    = shader.get_uniform_location( "light_color");
    GLint ambient_color_uniform  = shader.get_uniform_location( "ambient_color");

	glUniform3fv(light_position_uniform, 1, glm::value_ptr(light_position));
    glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
    glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
	get_errors();


    // set matrix-uniforms
    GLint perspective_uniform = shader.get_uniform_location( "perspective");
    GLint view_uniform        = shader.get_uniform_location( "view");
    GLint model_uniform       = shader.get_uniform_location( "model");

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));
	get_errors();


	
	/* // set attributes 
	GLint vertex_attrib = shader.get_attrib_location("vertex");
    GLint normal_attrib = shader.get_attrib_location( "normal");
    GLint color_attrib  = shader.get_attrib_location( "color");
	cout << "vertex: " << vertex_attrib << std::endl;
	cout << "normal: " << normal_attrib << std::endl;
	cout << "color: " << color_attrib << std::endl;
	get_errors();
	glEnableVertexAttribArray(vertex_attrib);
    glEnableVertexAttribArray(normal_attrib);
    glEnableVertexAttribArray(color_attrib);
	get_errors();
    glVertexAttribPointer(vertex_attrib, 4, GL_FLOAT, GL_FALSE, 0, vertices);
	get_errors();
    glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, 0, normals);
	get_errors();
    glVertexAttribPointer(color_attrib,  4, GL_FLOAT, GL_FALSE, 0, colors);
	get_errors();*/

	// Draw triangle in VAO
	// draw the geometry
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	get_errors(); //glinvalid
    /*// disable attribute-pointers
    glDisableVertexAttribArray(vertex_attrib);
    glDisableVertexAttribArray(normal_attrib);
    glDisableVertexAttribArray(color_attrib);
	//glDrawArrays(GL_TRIANGLES, 0, 3);*/
	// on some computers this call fails ...
	// ==> glBindVertexArray(0);
	// ... small workaround:
	my_glBindVertexArray(0);

	// Unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.unbind();
	get_errors();
}


int main (int argc, char** argv)
{
	ModelLoader m_loader;
	if (!alutInit (&argc, argv))
    {
      reportAlutError ();
    }

	glfwInit();

	// Set flags so GLFW creates the desired OpenGL context
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (glfwOpenWindow(	width,height,
						0,0,0,0,
						24, 8,
						GLFW_WINDOW) != GL_TRUE) {
			cerr << "Failed to initialize OpenGL window." << endl;
			glfwTerminate();
			return 1;
	}

	glfwSetWindowTitle("Realtime Rendering - OpenGL 3.2 core profile (GLFW)");

	// Load extensions and OGL>1.1 functions with GLEW

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		cerr <<  "Error: " << glewGetErrorString(err) << endl;
	}

	// Enable sRGB gamma correction for framebuffer output.
	glEnable(GL_FRAMEBUFFER_SRGB);

	// Clear errors. GLFW setup seems to cause an opengl error while setup.
	glGetError(); 

	test_ogl3();

	{
// 		glfwGetMousePos(&oldx,&oldy);
		glfwDisable( GLFW_MOUSE_CURSOR );
		glfwSetMousePosCallback( mouseMovementCb );
		bool running = true;

		GLuint vbo_id[3], vao_id;
		rootScene = new Model();
		// Load and compile Shader files
		Shader minimal("../shader/minimal");
		Shader simpleShader("../shader/simple_shader");

		if (!minimal) {
			cerr << "Could not compile minimal shader program." << endl;
			return 1;
		}
		if (!simpleShader) {
			cerr << "Could not compile simple_shader program." << endl;
			return 1;
		}

		simpleShader.bind_frag_data_location("fragColor");
		defaultShader = &simpleShader;
		get_errors();
		//init_vbo_vao(simpleShader, vbo_id, &vao_id);
		get_errors();
		init_matrixs();
		get_errors();

		cout << "Loading Scene" << endl;
		rootScene = m_loader.loadScene(daeModelPath);
		cout << "Done loading Scene" << endl;
		
		
		// workaround 2: 
		// if user closes window, there is no context left to query the proc-address from
		// therefore get the proc address now and use it at the end of the program
		// (the address of the function itself, of course, remains valid)
		my_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
		
		//GEHT!! habs aber auskommentiert da ich wav nicht auf svn spielen will, weil wir erst richtig musik finden müssen"
		/*ALuint musicBuffer, musicSource;
		musicBuffer = alutCreateBufferFromFile (wavAudioPath.data());

		if(AL_NONE == musicBuffer)
			reportAlutError();
		alGenSources (1, &musicSource);
		alSourcei (musicSource, AL_BUFFER, musicBuffer);
		alSourcePlay (musicSource);*/


		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		double time = glfwGetTime( );
		while (running) 
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//draw(simpleShader, vao_id);
			rootScene->draw();
			double tmptime = glfwGetTime();
			rootScene->update(tmptime-time);
			time = tmptime;
			glfwSwapBuffers();

			// Get OGL errors

			get_errors();

			// Check if the window has been closed

			running = running && !glfwGetKey( GLFW_KEY_ESC );
			running = running && !glfwGetKey( 'Q' );
			running = running && glfwGetWindowParam( GLFW_OPENED );

			if(glfwGetKey( 'A' ))
				m_camera_1.translateS(-0.01f);
			if(glfwGetKey( 'D' ))
				m_camera_1.translateS(0.01f);

			if(glfwGetKey( 'W' ))
				m_camera_1.translateF(0.01f);
			if(glfwGetKey( 'S' ))
				m_camera_1.translateF(-0.01f);
			
			// sleep for 1 ms (otherwise the GPU makes nasty sounds) [tm]
			glfwSleep(0.001);

		}

	//	release_vbo_vao(vbo_id, &vao_id);
	//	delete rootScene;
	}

	glfwTerminate();

	if (!alutExit ())
    {
      reportAlutError ();
    }

	return 0;
}
