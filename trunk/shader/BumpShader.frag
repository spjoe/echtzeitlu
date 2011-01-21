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
uniform float ks; // specular coefficient


// fragment-shader input variables
in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;
in vec2 TexCoord0;

in vec3 lightVec[4];
in vec3 eyeVec;
//in vec3 halfVec[4];

// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;

bool isShadow(vec4 proj_shadow, sampler2DShadow shadowMap)
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
	vec3 frag_world_normal = normalize(world_normal);
    vec3 position = world_position.xyz / world_position.w;
    
    vec4 ambient = ambient_color * texture(colorMap, TexCoord0);
    
    vec3 eye = normalize(eyeVec);
    
    // calculate lighting + shadows
    vec4 diffuseFinal = vec4(0,0,0,0);
    vec4 specularFinal = vec4(0,0,0,0);
	bool shadowLight[4];
	vec3 light_dir[4];
	vec4 diffuse[4];
	float specular[4];

	shadowLight[0] = false;
	shadowLight[1] = false; 
	shadowLight[2] = false; 
	shadowLight[3] = false; 


	//light_dir[1] = normalize(light_position1 - position);
	//fragColor = light_color1 *  max(0.0, dot(normal, light_dir[1]));
	//return;


	//vec3 vVec = normalize(eyeVec);
	vec4 base = texture2D(colorMap, TexCoord0);
	vec3 bump;
	//normal Map
	//vec3 bump = normalize( texture2D(bumpMap, TexCoord0).xyz * 2.0 - 1.0);

	//
	// Height Map
	//
	// Neighbourhood;
	//  lo o  ro
	//  l  p  r
	//  lu u  ru

	float r = textureOffset(bumpMap, TexCoord0, ivec2(1, 0)).r;
	float l = textureOffset(bumpMap, TexCoord0, ivec2(-1, 0)).r;
	float o = textureOffset(bumpMap, TexCoord0, ivec2(0, 1)).r;
	float u = textureOffset(bumpMap, TexCoord0, ivec2(0, -1)).r;

	//float p = texture(bumpMap, TexCoord0).r;

	float lo = textureOffset(bumpMap, TexCoord0, ivec2(-1, 1)).r;
	float ro = textureOffset(bumpMap, TexCoord0, ivec2(1, 1)).r;
	float lu = textureOffset(bumpMap, TexCoord0, ivec2(-1, -1)).r;
	float ru = textureOffset(bumpMap, TexCoord0, ivec2(1, -1)).r;


	// Sobel Filter
	float uz = 1*lo + 2*l + 1*lu -1*ro -2*r -1*ru;
	float vz = 1*lo + 2*o + 1*ro -1*lu -2*u -1*ru;
	vec3 diffu =  vec3(1,0,uz);
	vec3 diffv =  vec3(0,1,vz);
	
	bump = normalize(cross(diffu,diffv));

	//float distSqr = dot(lightVec[0], lightVec[0]);
	//vec3 lVec = lightVec[0] * inversesqrt(distSqr);
	//fragColor =  base * light_color1 * max( dot(lVec, bump), 0.0 );
	//fragColor = vec4(bump,1);
	//fragColor = texture2D( colorMap, TexCoord0);
	//return;
    
    if(num_lights > 0){
    	vec3 light_dir0 = (position - light_position0);
    	if(dot(light_dir0,world_normal)>0.0)
    		shadowLight[0] = true;
    	else
			shadowLight[0] = isShadow(proj_shadow0, shadowMap0);
			
		if(shadowLight[0]){
			fragColor = vec4(1,0,0,0);
			//return;
		}
			
		vec3 lVec = normalize(lightVec[0]);
		diffuse[0]= base * light_color0 * max( dot(lVec, bump), 0.0 );
		
		vec3 refl = normalize(eye - 2 * bump * dot(bump, eye));
		specular[0] = max(dot(refl,lVec),0.0);
		specular[0] = pow(specular[0],2);
		//fragColor = light_color0 * specular[0]; //max(dot(vec3(0,0,1),bump),0.0); //specular[0];
		//return;
		//fragColor = light_color0 * max( dot(lVec, vec3(0,0,1)), 0.0 );
		//return;
    }
	if(num_lights > 1){
		vec3 light_dir1 = (position - light_position1);
    	if(dot(light_dir1,world_normal)>0.0)
    		shadowLight[1] = true;
    	else
    		shadowLight[1] = isShadow(proj_shadow1, shadowMap1);
    	
    	if(shadowLight[1]){
			fragColor = vec4(1,0,0,0);
			//return;
		}
		
		vec3 lVec = normalize(lightVec[1]);
		diffuse[1]= base * light_color1 * max( dot(lVec, bump), 0.0 );
		
		vec3 refl = normalize(eye - 2 * bump * dot(bump, eye));
		specular[1] = max(dot(refl,lVec),0.0);
		specular[1] = pow(specular[1],2);
	}

	for(int i = 0; i < num_lights; i++){
		if(shadowLight[i]){
			diffuseFinal +=  diffuse[i] * 0.2;
		}else{
			diffuseFinal +=  diffuse[i];
			specularFinal += vec4(1,1,1,1) * specular[i];
		}
	}

	// write color to output
	fragColor = ambient + (1.0-ks) * diffuseFinal / num_lights + ks * specularFinal;
	//fragColor = specularFinal;


	//fragColor = texture2D( colorMap, TexCoord0);

}
