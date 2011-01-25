#include "common.hpp"
#include "shader.hpp"
#include "camera.h"
#include "ModelLoader.h"
#include "Model.h"
#include "ParticleManager.h"
#include "ParticleSystem.h"
#include "Lighting.h"
#include "CameraMotion.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <stdio.h>

#include <AL/alut.h>

//const std::string daeModelPath = "../resources/SteamEngine/models/Steam EngineV2_6.dae";
//const std::string daeModelPath = "../resources/SteamEngine/models/Steam EngineV2_6.dae";
// const std::string daeModelPath = "../resources/tmSteamEnginecm.dae";
const std::string daeModelPath = "../resources/tmSteamEngine.dae";
//const std::string daeModelPath = "../resources/sphere.dae";

const std::string wavAudioPath = "../resources/music2.wav";
//const std::string daeModelPath = "../resources/trianglebox.dae";
glm::mat4 model;
glm::vec3 viewVector(0.0f, 0.0f, 1.0f);

// define our light
glm::vec3 light_position(0.0f, 8.0f, 3.0f);
glm::vec4 light_color(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 ambient_color(0.1f, 0.1f, 0.1f, 1.0f);

GLfloat distance = -4.0f;
bool stop = false;

using namespace echtzeitlu;
Camera m_camera_1;
Camera m_camera_ortho;
Lighting *m_lighting;
SceneObject *rootScene;
Shader *defaultShader;
Shader *defaultColorShader;
Shader *defaultBumpShader;
Shader *simpleShader;
Shader* lightShader;
Shader* gaussShader;
Shader* additionShader;

GLuint scene_fbo;
GLuint scene_map;
GLuint scene_depth;

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
 	if(!stop)
 		return;
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
	m_camera_ortho.orthogonal(width, height, 0.01f, 2.0f);

    // move camera 4 units backward, i.e. movie the scene 4 units forward
	m_camera_1.lookat(glm::vec3(-2.2,2.2,0.6), glm::vec3(-1.8,0,0), glm::vec3(0,0,1));
	m_camera_ortho.lookat(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));

    // initialize the model matrix to identity
	model = glm::mat4(1.0f);
}

void init_fbo()
{
	PFNGLGENFRAMEBUFFERSPROC my_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	PFNGLBINDFRAMEBUFFERPROC my_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	PFNGLFRAMEBUFFERTEXTURE2DPROC my_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D");
	PFNGLDELETEFRAMEBUFFERSPROC my_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

	glGenTextures(1, &scene_map);
	glBindTexture(GL_TEXTURE_2D, scene_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	get_errors("init_fbo() A");
	
	glGenTextures(1, &scene_depth);
	glBindTexture(GL_TEXTURE_2D, scene_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	get_errors("init_fbo() A");
	
	my_glGenFramebuffers(1,&scene_fbo);
	my_glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_map, 0);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene_depth, 0);
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	get_errors("init_fbo() B");
}

void delete_fbo()
{
	PFNGLISFRAMEBUFFERPROC my_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebuffer");
	PFNGLDELETEFRAMEBUFFERSPROC my_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffer");
	
	if(my_glIsFramebuffer(scene_fbo))
		my_glDeleteFramebuffers(1, &scene_fbo);
	
	if(glIsTexture(scene_map))
		glDeleteTextures(1,&scene_map);

	get_errors("Lighting::~Lighting()");
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
	CameraMotion cm(&m_camera_1);
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
		Shader BumpShader("../shader/BumpShader");
		Shader LightShader("../shader/LightShader");
		Shader GaussShader("../shader/ipGaussShader");
		Shader AdditionShader("../shader/ipAddition");

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
		if (!BumpShader){
			cerr << "Could not compile BumpShader program." << endl;
			return 1;
		}
		if (!LightShader){
			cerr << "Could not compile LightShader program." << endl;
			return 1;
		}
		if (!GaussShader){
			cerr << "Could not compile GaussShader program." << endl;
			return 1;
		}
		if (!AdditionShader){
			cerr << "Could not compile AdditionShader program." << endl;
			return 1;
		}

		TextureShader.bind_frag_data_location("fragColor");
		ColorShader.bind_frag_data_location("fragColor");
		SimpleShader.bind_frag_data_location("fragColor");
		BumpShader.bind_frag_data_location("fragColor");
		LightShader.bind_frag_data_location("fragColor");
		GaussShader.bind_frag_data_location("fragColor");
		AdditionShader.bind_frag_data_location("fragColor");
		defaultShader = &TextureShader;
		defaultColorShader = &ColorShader;
		simpleShader = &SimpleShader;
		defaultBumpShader = &BumpShader;
		lightShader = &LightShader;
		gaussShader = &GaussShader;
		additionShader = &AdditionShader;
		
		get_errors();
		//init_vbo_vao(simpleShader, vbo_id, &vao_id);
		get_errors();
		init_matrixs();
		get_errors();
		pm.AddSystem(new SteamParticleSystem("Smokie uno",2000,glm::vec4(1.2,3.1,-1.55,1),0.5));
		pm.AddSystem(new SparkParticleSystem("Sparkie uno",100,glm::vec4(2.0,-0.7,-1.5,1)));
		// init lighting
		m_lighting = new Lighting;
		
		init_fbo();

		cout << "loading scene: '" << daeModelPath.c_str() << "'"<< endl;
		rootScene = m_loader.loadScene(daeModelPath);
		cout << "... done loading scene" << endl;
		
		// workaround 2: 
		// if user closes window, there is no context left to query the proc-address from
		// therefore get the proc address now and use it at the end of the program
		// (the address of the function itself, of course, remains valid)
// 		my_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
		
		//GEHT!! habs aber auskommentiert da ich wav nicht auf svn spielen will, weil wir erst richtig musik finden müssen"
		ALuint musicBuffer, musicSource;
		musicBuffer = alutCreateBufferFromFile (wavAudioPath.data());

		if(AL_NONE == musicBuffer)
			reportAlutError();
		alGenSources (1, &musicSource);
		alSourcei (musicSource, AL_BUFFER, musicBuffer);
		

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		
		// Create shadow map (static, because this is outside of running loop)
// 		m_lighting = new Lighting;
// 		m_lighting->addLight(glm::vec3(0.0f,-10.0f,7.0f),glm::vec4(1.0f,1.0f,1.0f,1.0f));
// 		m_lighting->addLight(glm::vec3(10.0f, 5.0f, 7.0f), glm::vec4(1.0f,1.0f,1.0f,1.0f));
// 		get_errors();
		
		
		cm.flyaround(glm::vec3(0,5,5),glm::vec3(0,0,5),glm::vec3(0,0,0),0.5, false, 2.0,glm::vec3(-1.8,0,0));
		std::vector<Shader*> shaders;
		shaders.push_back(defaultShader);
		shaders.push_back(defaultColorShader);
		shaders.push_back(defaultBumpShader);
		
// 		running  = false;
		double totaltime = 0.0;
		int zoom = 0;
		//rootScene->draw();
		if(stop == false) {
			alSourcePlay (musicSource);
		}else{
			cm.save();
		}
		double time = glfwGetTime( );
		while (running) 
		{
			// Preprocessing
			m_camera_1.apply(defaultShader);
			m_camera_1.apply(defaultColorShader);
			m_camera_1.apply(simpleShader);
			m_camera_1.apply(defaultBumpShader);
			m_camera_1.apply(lightShader);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_lighting->createShadow(rootScene, shaders);
			
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_lighting->createLightMap(rootScene);
			
			//draw(simpleShader, vao_id);
			
			// Render
			PFNGLBINDFRAMEBUFFERPROC my_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
			my_glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			rootScene->draw();
			pm.Render();
// 			glBindTexture(GL_TEXTURE_2D, scene_map);
// 			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
			my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
			get_errors("rootScene->draw()");
			
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_lighting->addLightMap(scene_map);
			
			
			// Update
			if(!stop){
				double tmptime = glfwGetTime();
				totaltime += tmptime-time; //versuch einer kamara fahrt
				if(totaltime < 5){
					; // freeze
				}else if(totaltime > 24 && totaltime < 29){
					float factor = std::min(pow((totaltime-23),2),4.0);
					rootScene->update((tmptime-time)/factor);
					pm.Update((tmptime-time)/factor);
	  				cm.update(tmptime-time); //Move Camera
					m_lighting->update((tmptime-time)/factor);
				}else if(totaltime > 29 && zoom == 0){
					zoom++;
					cm.moveto(glm::vec3(0,-2,3),2);
				}else if (totaltime > 29 && totaltime < 45){
					cm.update(tmptime-time);
					rootScene->update((tmptime-time)/4.0);
					pm.Update((tmptime-time)/4.0);
					m_lighting->update((tmptime-time)/4.0);

				}else if (totaltime > 45 && zoom == 1){
					zoom++;
					cm.moveto(glm::vec3(0,10,10),2.0f);
				}else if(totaltime > 45 && totaltime < 60){
					cm.update(tmptime-time);
					rootScene->update((tmptime-time)/4.0);
					pm.Update((tmptime-time)/4.0);
					m_lighting->update((tmptime-time)/4.0);
				}else if (totaltime > 60 && totaltime < 65){
					float factor = std::min(pow((66-totaltime),2),4.0);
					rootScene->update((tmptime-time)/factor);
					pm.Update((tmptime-time)/factor);
	  				cm.update(tmptime-time); //Move Camera
					m_lighting->update((tmptime-time)/factor);
				}
				else if(totaltime > 65 && zoom == 2){
					cm.flyaround(m_camera_1.p, glm::vec3(0,0,5),glm::vec3(0,0,0),0.5, true,2.0);
					zoom++;
				}
				else if(totaltime > 250 ){
					running = false;
				}else{
					rootScene->update(tmptime-time);
					pm.Update(tmptime-time);
	  				cm.update(tmptime-time); //Move Camera
 					m_lighting->update(tmptime-time);
				}
				time = tmptime;;
			}else{
				double tmptime = glfwGetTime();
				rootScene->update(tmptime-time);
				pm.Update(tmptime-time);
// 				cm.update(tmptime-time); //Move Camera
				m_lighting->update(tmptime-time);
				time = tmptime;
			}
			
			glfwSwapBuffers();

			// Get OGL errors

			get_errors();

			// Check if the window has been closed

			running = running && !glfwGetKey( GLFW_KEY_ESC );
// 			running = running && !glfwGetKey( 'Q' );
			running = running && glfwGetWindowParam( GLFW_OPENED );

 			if(stop){
				float kb_speed = 0.05f;
				float ms_speed = 0.03f;
				if(glfwGetKey( 'A' ))
					m_camera_1.translateS(-kb_speed);
				if(glfwGetKey( 'D' ))
					m_camera_1.translateS(kb_speed);
				if(glfwGetKey( 'W' ))
					m_camera_1.translateF(kb_speed);
				if(glfwGetKey( 'S' ))
					m_camera_1.translateF(-kb_speed);
				if(glfwGetKey( 'F' ))
					m_camera_1.orbit(glm::vec3(0,0,0),glm::vec3(0,0,1), -ms_speed);
				if(glfwGetKey( 'H' ))
					m_camera_1.orbit(glm::vec3(0,0,0),glm::vec3(0,0,1), ms_speed);
				if(glfwGetKey( 'T' ))
					m_camera_1.translateF(kb_speed);
				if(glfwGetKey( 'G' ))
					m_camera_1.translateF(-kb_speed);
				if(glfwGetKey( 'P' ))
					m_camera_1.print();
 			}
			if(glfwGetKey( 'M' )){
				glfwSleep(0.1);
				alSourcePlay(musicSource);
			}
			if(glfwGetKey( 'I' )){
				printf("totaltime: %f",totaltime);
			}
			if(glfwGetKey( 'C' )){
				stop = !stop;
				if(stop == true){
 					alSourcePause(musicSource);
 					cm.save();

				}else{
					glfwSleep(0.1);
 					alSourcePlay(musicSource);
 					cm.restore();
				}
				glfwSleep(0.2);
				time = glfwGetTime( ); //zeitrechnung neu beginnen
			}
			
			// sleep for 1 ms (otherwise the GPU makes nasty sounds) [tm]
			glfwSleep(0.001);

		}

		delete_fbo();
		//	release_vbo_vao(vbo_id, &vao_id);
		delete rootScene;
		delete m_lighting;
		pm.Shutdown();
	}

	glfwTerminate();

	if (!alutExit ())
    {
      reportAlutError ();
    }

	printf("quit\n");
	return 0;
}
