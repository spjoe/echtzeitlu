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
uniform sampler2D colorMap;

// fragment-shader input variables
//in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;
in vec2 TexCoord0;


// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;

bool isShadow(vec4 proj_shadow, sampler2DShadow shadowMap, vec3 position, vec3 normal)
{
	vec3 coordPos  = proj_shadow.xyz / proj_shadow.w;
	if(coordPos.x >= 0.0 && coordPos.y >= 0.0 && coordPos.x <= 1.0 && coordPos.y <= 1.0 ){
		if( texture(shadowMap, coordPos) < coordPos.z)
			return true;
		else
			return false;
	}else{
		return false;
	}
	return false;
}

void main()
{
	// renormalize and homogenize input variables
	vec3 normal = normalize(world_normal);
    vec3 position = world_position.xyz / world_position.w;
    
    vec4 ambient = ambient_color * texture(colorMap, TexCoord0);
    
    // calculate lighting + shadows
    vec4 diffuseFinal = vec4(0,0,0,0);
	bool shadowLight[4];
	vec3 light_dir[4];
	vec4 diffuse[4];

	shadowLight[0] = false;
	shadowLight[1] = false; 
	shadowLight[2] = false; 
	shadowLight[3] = false; 

    
    if(num_lights > 0){
    	vec3 light_dir0 = (position - light_position0);
    	if(dot(light_dir0,normal)>0.0)
    		shadowLight[0] = true;
    	else
			shadowLight[0] = isShadow(proj_shadow0, shadowMap0, position, normal);
		light_dir[0] = normalize(light_position0 - position);
		diffuse[0]= texture(colorMap, TexCoord0) * light_color0 * max(0.0, dot(normal, light_dir[0]));
		
    }
	if(num_lights > 1){
		vec3 light_dir1 = (position - light_position1);
    	if(dot(light_dir1,normal)>0.0)
    		shadowLight[1] = true;
    	else
    		shadowLight[1] = isShadow(proj_shadow1, shadowMap1, position, normal);
		light_dir[1] = normalize(light_position1 - position);
		diffuse[1] = texture(colorMap, TexCoord0) * light_color1 * max(0.0, dot(normal, light_dir[1]));
	}

	for(int i = 0; i < num_lights; i++){
		if(shadowLight[i])
			diffuseFinal +=  diffuse[i] * 0.2;
		else
			diffuseFinal +=  diffuse[i];
	}

	// write color to output
	fragColor = ambient + diffuseFinal / num_lights;
}
