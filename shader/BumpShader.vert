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
uniform mat4 invTransModel;

// light & shadows
uniform mat4 shadowProjView0;
uniform mat4 shadowProjView1;
uniform mat4 shadowProjView2;
uniform mat4 shadowProjView3;
uniform vec3 light_position0;
uniform vec3 light_position1;
uniform vec3 light_position2;
uniform vec3 light_position3;
out vec4 proj_shadow0;
out vec4 proj_shadow1;
out vec4 proj_shadow2;
out vec4 proj_shadow3;

// vertex-shader input variables
in vec4 vertex;
//in vec4 color;
in vec3 normal;
in vec2 texkoord;
//bumpMapping
in vec3 InvNormal;
in vec3 InvBinormal;
in vec3 InvTangent;

// vertex-shader output variables (passed to fragment-shader)
//out vec4 frag_color;
out vec3 world_normal;
out vec4 world_position;
out vec2 TexCoord0;

//bumpMappin
out vec3 lightVec[4];
out vec3 eyeVec;
//out vec3 halfVec[4];


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
    //frag_color = color;
    TexCoord0 = texkoord;
	//gl_TexCoord[0].st = texkoord;

    // transform vertex down the pipeline
    gl_Position = perspective * view * model * vertex;

	//BumpMap
	vec3 pos = world_position.xyz / world_position.w;
	vec3 light_dir0 = normalize(pos - light_position0); //light direction in the world space
	vec3 light_dir1 = normalize(pos - light_position1);
	vec3 light_dir2 = normalize(pos - light_position2);
	vec3 light_dir3 = normalize(pos - light_position3);

//	light_dir0 = normalize(light_dir0 * -mat3(invTransModel));// light direction im model space
//	light_dir1 = normalize(light_dir1 * -mat3(invTransModel));
//	light_dir2 = normalize(light_dir2 * -mat3(invTransModel));
//	light_dir3 = normalize(light_dir3 * -mat3(invTransModel));
	
	light_dir0 = normalize(mat3(invTransModel) * light_dir0);// light direction im model space
	light_dir1 = normalize(mat3(invTransModel) * light_dir1);
	light_dir2 = normalize(mat3(invTransModel) * light_dir2);
	light_dir3 = normalize(mat3(invTransModel) * light_dir3);

	mat3 rotmat = mat3(InvTangent,InvBinormal,InvNormal);
    //Rotate the light into tangent space
	//For 4 lights max
    lightVec[0] = rotmat * light_dir0; //light direction in the Tangent Space
	lightVec[1] = rotmat * light_dir1;
	lightVec[2] = rotmat * light_dir2;
	lightVec[3] = rotmat * light_dir3;
	
	eyeVec = rotmat * mat3(invTransModel) * normalize(transpose(view) * vec4(0,0,1,1)).xyz;
	
	// For later use (phong lightning model...
	//vec3 eyeVec = normalize(-pos *  mat3(invTransModel)); 
	//eyeVec = rotmat * normalize(eyeVec); //eye Vektor in tangent Space

	//for( int i = 0;  i < 4; i++){
	//	halfVec[0] = (eyeVec + lightVec[0]) / length(eyeVec + lightVec[0]);
	//}


}
