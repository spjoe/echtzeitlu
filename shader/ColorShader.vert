/*
 *  OpenGL 3.x Demonstration
 *  
 *  for the "Computergraphics 2" lab
 *
 *  Institute of Computer Graphics and Algorithms
 *  Vienna University of Technology
 *  Vienna, Austria
 *
 *  March 2010
 *
 *  ginquo
 */

// Note: If running OpenGL 2.1, you have to do the following to get it to run:
//       1. delete the two marked lines
//       2. rename 'in' to 'attribute'
//       3. rename 'out' to 'varying'

#version 130              // delete this line if using GLSL 1.2
precision mediump float;  // delete this line if using GLSL 1.2

// uniform shader-parameters
uniform mat4 perspective;
uniform mat4 view;
uniform mat4 model;

// light & shadows
uniform mat4 shadowProjView0;
uniform mat4 shadowProjView1;
uniform mat4 shadowProjView2;
uniform mat4 shadowProjView3;
out vec4 proj_shadow0;
out vec4 proj_shadow1;
out vec4 proj_shadow2;
out vec4 proj_shadow3;

// vertex-shader input variables
in vec4 vertex;
in vec4 color;
in vec3 normal;
//in vec2 texkoord;

// vertex-shader output variables (passed to fragment-shader)
out vec4 frag_color;
out vec3 world_normal;
out vec4 world_position;

void main()
{
	// transform normal into world space
	vec4 normal4 = vec4(normal, 0.0f);
    world_normal = (model * normal4).xyz;
    
    // transform vertex to world-space
    world_position = model * vertex;
    
    // calculate shadow projection
    proj_shadow0 = shadowProjView0 * world_position;
    proj_shadow1 = shadowProjView1 * world_position;
    proj_shadow2 = shadowProjView2 * world_position;
    proj_shadow3 = shadowProjView3 * world_position;
    
    // just pass color to fragment-shader
    frag_color = color;
	//gl_TexCoord[0].st = texkoord;

    // transform vertex down the pipeline
    gl_Position = perspective * view * model * vertex;
}
