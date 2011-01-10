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

// light & shadows
uniform int num_lights;
uniform sampler2DShadow shadowMap0;		// bound to GL_TEXTURE1
uniform sampler2DShadow shadowMap1;		// bound to GL_TEXTURE2
uniform sampler2DShadow shadowMap2;		// bound to GL_TEXTURE3
uniform sampler2DShadow shadowMap3;		// bound to GL_TEXTURE4
uniform vec3 light_position0;
uniform vec3 light_position1;
uniform vec3 light_position2;
uniform vec3 light_position3;
uniform vec4 light_color0;
uniform vec4 light_color1;
uniform vec4 light_color2;
uniform vec4 light_color3;
in vec4 proj_shadow0;
in vec4 proj_shadow1;
in vec4 proj_shadow2;
in vec4 proj_shadow3;

// uniform shader-parameters
uniform vec4 ambient_color;
//uniform sampler2D texture;


// fragment-shader input variables
in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;

// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;

const float dark = 0.5;

void main()
{
	
	// renormalize and homogenize input variables
	vec3 normal = normalize(world_normal);
    vec3 position = world_position.xyz / world_position.w;
    
    vec4 ambient = ambient_color * frag_color;
    
    // calculate lighting + shadows
    vec4 diffuse = vec4(0,0,0,0);
    
    if(num_lights > 0){
    	vec3 light_dir0 = normalize(light_position0 - position);
		vec4 diffuse0 = frag_color * light_color0 * max(0.0, dot(normal, light_dir0));
		float shadow0 = 1.0;
		vec3 coordPos0  = proj_shadow0.xyz / proj_shadow0.w;
		if(coordPos0.x >= 0.0 && coordPos0.y >= 0.0 && coordPos0.x <= 1.0 && coordPos0.y <= 1.0 ){
			if( texture(shadowMap0, coordPos0) < coordPos0.z)
				diffuse = 0.5 * diffuse0;
			else
				diffuse = diffuse0;
		}
	}
	if(num_lights > 1){
		vec3 light_dir1 = normalize(light_position1 - position);
		vec4 diffuse1 = frag_color * light_color1 * max(0.0, dot(normal, light_dir1));
		float shadow1 = 1.0;
		vec3 coordPos1  = proj_shadow1.xyz / proj_shadow1.w;
		if(coordPos1.x >= 0.0 && coordPos1.y >= 0.0 && coordPos1.x <= 1.0 && coordPos1.y <= 1.0 ){
			if( texture(shadowMap1, coordPos1) < coordPos1.z)
				diffuse = diffuse + 0.5 * diffuse1;
			else
				diffuse = diffuse + diffuse1;
		}
	}
	if(num_lights > 2){
		vec3 light_dir2 = normalize(light_position2 - position);
		vec4 diffuse2 = frag_color * light_color2 * max(0.0, dot(normal, light_dir2));
		float shadow2 = 1.0;
		vec3 coordPos2  = proj_shadow2.xyz / proj_shadow2.w;
		if(coordPos2.x >= 0.0 && coordPos2.y >= 0.0 && coordPos2.x <= 1.0 && coordPos2.y <= 1.0 ){
			if( texture(shadowMap2, coordPos2) < coordPos2.z)
				diffuse = diffuse + 0.5 * diffuse2;
			else
				diffuse = diffuse + diffuse2;
		}
	}
	if(num_lights > 3){
		vec3 light_dir3 = normalize(light_position3 - position);
		vec4 diffuse3 = frag_color * light_color3 * max(0.0, dot(normal, light_dir3));
		float shadow3 = 1.0;
		vec3 coordPos3  = proj_shadow3.xyz / proj_shadow3.w;
		if(coordPos3.x >= 0.0 && coordPos3.y >= 0.0 && coordPos3.x <= 1.0 && coordPos3.y <= 1.0 ){
			if( texture(shadowMap3, coordPos3) < coordPos3.z)
				diffuse = diffuse + 0.5 * diffuse3;
			else
				diffuse = diffuse + diffuse3;
		}
	}

    // write color to output
    fragColor = ambient + diffuse / num_lights;

	//fragColor = texture2D( texture, gl_TexCoord[0].st);

}
