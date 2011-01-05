#include "common.hpp"
#include "shader.hpp"
#include "camera.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ParticleManager.h"
#include "ParticleSystem.h"
#include "Lighting.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <stdio.h>

#include <AL/alut.h>

//const std::string daeModelPath = "../resources/SteamEngine/models/Steam EngineV2_6.dae";
//const std::string daeModelPath = "../resources/SteamEngine/models/Steam EngineV2_6.dae";
const std::string daeModelPath = "../resources/tmSteamEngine.dae";

const std::string wavAudioPath = "../resources/music.wav";
//const std::string daeModelPath = "../resources/trianglebox.dae";
glm::mat4 model;
glm::vec3 viewVector(0.0f, 0.0f, 1.0f);

// define our light
glm::vec3 light_position(0.0f, 8.0f, 3.0f);
glm::vec4 light_color(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ambient_color(0.1f, 0.1f, 0.1f, 1.0f);

GLfloat distance = -4.0f;

using namespace echtzeitlu;
Camera m_camera_1;
Lighting m_lighting;
SceneObject *rootScene;
Shader *defaultShader;
Shader *defaultColorShader;
Shader *simpleShader;

int width=800;
int height=600;


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
	
	m_camera_1.rotateZ(alpha);
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
	m_camera_1.lookat(glm::vec3(0,20,20), glm::vec3(0,0,0), glm::vec3(0,0,1));

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
#ifdef DEBUG
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
#endif
}

int main (int argc, char** argv)
{
	ModelLoader m_loader;
	ParticleManager pm;
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
		Shader SimpleShader("../shader/simple_shader");
		Shader TextureShader("../shader/TextureShader");
		Shader ColorShader("../shader/ColorShader");

		if (!minimal) {
			cerr << "Could not compile minimal shader program." << endl;
			return 1;
		}
		if (!TextureShader) {
			cerr << "Could not compile TextureShader program." << endl;
			return 1;
		}
		if (!ColorShader) {
			cerr << "Could not compile ColorShader program." << endl;
			return 1;
		}
		if (!SimpleShader){
			cerr << "Could not compile simple_shader program." << endl;
			return 1;
		}

		TextureShader.bind_frag_data_location("fragColor");
		ColorShader.bind_frag_data_location("fragColor");
		SimpleShader.bind_frag_data_location("fragColor");
		defaultShader = &TextureShader;
		defaultColorShader = &ColorShader;
		simpleShader = &SimpleShader;

		get_errors();
		//init_vbo_vao(simpleShader, vbo_id, &vao_id);
		get_errors();
		init_matrixs();
		get_errors();
		pm.AddSystem(new SmokeParticleSystem("Smokie uno",1,glm::vec4(1,1,1,1)));

		cout << "loading scene: '" << daeModelPath.c_str() << "'"<< endl;
		rootScene = m_loader.loadScene(daeModelPath);
		cout << "... done loading scene" << endl;
		
		// workaround 2: 
		// if user closes window, there is no context left to query the proc-address from
		// therefore get the proc address now and use it at the end of the program
		// (the address of the function itself, of course, remains valid)
// 		my_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
		
		//GEHT!! habs aber auskommentiert da ich wav nicht auf svn spielen will, weil wir erst richtig musik finden müssen"
		/*ALuint musicBuffer, musicSource;
		musicBuffer = alutCreateBufferFromFile (wavAudioPath.data());

		if(AL_NONE == musicBuffer)
			reportAlutError();
		alGenSources (1, &musicSource);
		alSourcei (musicSource, AL_BUFFER, musicBuffer);
		alSourcePlay (musicSource);*/

		m_lighting.addLight(glm::vec3(0.0f,-10.0f,7.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f));

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		double time = glfwGetTime( );
		while (running) 
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//draw(simpleShader, vao_id);
			rootScene->drawSimple();
			pm.Render();
			double tmptime = glfwGetTime();
			rootScene->update(tmptime-time);
			pm.Update(tmptime-time);
			time = tmptime;
			
// 			m_lighting.createShadowMaps(rootScene);
			
			glfwSwapBuffers();

			// Get OGL errors

			get_errors();

			// Check if the window has been closed

			running = running && !glfwGetKey( GLFW_KEY_ESC );
			running = running && !glfwGetKey( 'Q' );
			running = running && glfwGetWindowParam( GLFW_OPENED );

			if(glfwGetKey( 'A' ))
				m_camera_1.translateS(-0.1f);
			if(glfwGetKey( 'D' ))
				m_camera_1.translateS(0.1f);
			if(glfwGetKey( 'W' ))
				m_camera_1.translateF(0.1f);
			if(glfwGetKey( 'S' ))
				m_camera_1.translateF(-0.1f);
			
			// sleep for 1 ms (otherwise the GPU makes nasty sounds) [tm]
			glfwSleep(0.001);

		}

		//	release_vbo_vao(vbo_id, &vao_id);
		delete rootScene;
	}

	glfwTerminate();

	if (!alutExit ())
    {
      reportAlutError ();
    }

	printf("quit\n");
	return 0;
}
