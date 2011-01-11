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

vec4 getShadow(vec3 light_position, vec4 light_color, vec4 proj_shadow, sampler2DShadow shadowMap,
				vec3 position, vec3 normal)
{
    vec3 light_dir = normalize(light_position - position);
	vec4 diffuse = frag_color * light_color * max(0.0, dot(normal, light_dir));
	vec3 coordPos  = proj_shadow.xyz / proj_shadow.w;
	if(coordPos.x >= 0.0 && coordPos.y >= 0.0 && coordPos.x <= 1.0 && coordPos.y <= 1.0 ){
		if( texture(shadowMap, coordPos) < coordPos.z)
			diffuse = 0.5 * diffuse;
		else
			diffuse = diffuse;
	}else{
		diffuse = diffuse;
	}
	return diffuse;
}

void main()
{
	// renormalize and homogenize input variables
	vec3 normal = normalize(world_normal);
    vec3 position = world_position.xyz / world_position.w;
    
    vec4 ambient = ambient_color * frag_color;
    
    // calculate lighting + shadows
    vec4 diffuse = vec4(0,0,0,0);
    
    if(num_lights > 0)
    	diffuse = getShadow(light_position0, light_color0, proj_shadow0, shadowMap0, position, normal);
	
	if(num_lights > 1)
		diffuse = diffuse + getShadow(light_position1, light_color1, proj_shadow1, shadowMap1, position, normal);
	
	if(num_lights > 2)
		diffuse = diffuse + getShadow(light_position2, light_color2, proj_shadow2, shadowMap2, position, normal);
	
	if(num_lights > 3)
		diffuse = diffuse + getShadow(light_position3, light_color3, proj_shadow3, shadowMap3, position, normal);
	

    // write color to output
    fragColor = ambient + diffuse / num_lights;

	//fragColor = texture2D( texture, gl_TexCoord[0].st);

}
