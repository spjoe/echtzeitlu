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

uniform sampler2DShadow shadowMap;

// uniform shader-parameters
uniform vec3 light_position;
uniform vec4 light_color;
uniform vec4 ambient_color;
//uniform sampler2D texture;


// fragment-shader input variables
in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;
in vec4 proj_shadow;

// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;


void main()
{
	// renormalize and homogenize input variables
	vec3 normal = normalize(world_normal);
    vec3 position = world_position.xyz / world_position.w;
    
    // calculate the light-direction
    vec3 light_dir = normalize(light_position - position);

	// calculate lighting
    vec4 ambient = ambient_color * frag_color;
    vec4 diffuse = frag_color * light_color * max(0.0, dot(normal, light_dir));
    
    float shadow = 1.0;
	vec3 coordPos  = proj_shadow.xyz / proj_shadow.w;
	if(coordPos.x >= 0.0 && coordPos.y >= 0.0 && coordPos.x <= 1.0 && coordPos.y <= 1.0 ){
		shadow = texture(shadowMap, coordPos) < coordPos.z - 0.001 ? 0.2 : 1.0;
	}
	
    // write color to output
    fragColor = shadow * (ambient + diffuse);

	//fragColor = texture2D( texture, gl_TexCoord[0].st);

}
