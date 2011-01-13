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
uniform sampler2D bumpMap;


// fragment-shader input variables
in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;
in vec2 TexCoord0;

in vec3 lightVec[4];
in vec3 eyeVec;
in vec3 halfVec[4];

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

	vec3 vVec = normalize(eyeVec);
	vec4 base = texture2D(colorMap, TexCoord0);
	vec3 bump = normalize( texture2D(bumpMap, TexCoord0).xyz * 2.0 - 1.0);
	//float distSqr = dot(lightVec[1], lightVec[1]);
	//vec3 lVec = lightVec[1] * inversesqrt(distSqr);
	//fragColor = light_color1 * max( dot(lVec, bump), 0.0 );
	//return;
    
    if(num_lights > 0){
		shadowLight[0] = isShadow(proj_shadow0, shadowMap0, position, normal);
		float distSqr = dot(lightVec[0], lightVec[0]);
		vec3 lVec = lightVec[0] * inversesqrt(distSqr);
		diffuse[0]= base * light_color0 * max( dot(lVec, bump), 0.0 );
    }
	if(num_lights > 1){
		shadowLight[1] = isShadow(proj_shadow1, shadowMap1, position, normal);
		float distSqr = dot(lightVec[1], lightVec[1]);
		vec3 lVec = lightVec[1] * inversesqrt(distSqr);
		light_dir[1] = normalize(light_position1 - position);
		diffuse[1]= base * light_color1 * max( dot(lVec, bump), 0.0 );
	}
	//if(num_lights > 2){
	//	shadowLight[2] = isShadow(proj_shadow2, shadowMap2, position, normal);
	//	light_dir[2] = normalize(light_position1 - position);
	//	diffuse[2] = texture(colorMap, TexCoord0) * light_color2 * max(0.0, dot(normal, light_dir[2]));
	//  diffuse[2]= texture(colorMap, TexCoord0) * light_color2 * max(0.0, dot(BumpNorm, LightDirTangentSpace2));
	//}
	//if(num_lights > 3){
	//	shadowLight[3] = isShadow(proj_shadow3, shadowMap3, position, normal);
	//	light_dir[3] = normalize(light_position1 - position);
	//	diffuse[3] = texture(colorMap, TexCoord0) * light_color3 * max(0.0, dot(normal, light_dir[3]));
	//  diffuse[3]= texture(colorMap, TexCoord0) * light_color3 * max(0.0, dot(BumpNorm, LightDirTangentSpace3));
	//}
	for(int i = 0; i < num_lights; i++){
		if(shadowLight[i])
			diffuseFinal +=  diffuse[i] * 0.5;
		else
			diffuseFinal +=  diffuse[i];
	}

	// write color to output
	fragColor = ambient + diffuseFinal / num_lights;


	//fragColor = texture2D( colorMap, TexCoord0);

}
