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
uniform vec3 light_position;
uniform vec4 light_color;
uniform vec4 ambient_color;
uniform sampler2D colorMap;
uniform sampler2D bumpMap;

in vec4 proj_shadow0;
in vec4 proj_shadow1;
in vec4 proj_shadow2;
in vec4 proj_shadow3;
in vec3 LightDirTangentSpace0;
in vec3 LightDirTangentSpace1;
in vec3 LightDirTangentSpace2;
in vec3 LightDirTangentSpace3;

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
uniform int num_lights;



// fragment-shader input variables
//in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;
in vec2 TexCoord0;

// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;

vec4 getShadow(vec3 light_position, vec4 light_color, vec4 proj_shadow, sampler2DShadow shadowMap,
				vec3 position, vec3 normal)
{
    vec3 light_dir = normalize(light_position - position);
	vec4 diffuse = texture2D( colorMap, TexCoord0.st) * light_color * max(0.0, dot(normal, light_dir));
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

	vec4 ambient = ambient_color * texture2D( colorMap, TexCoord0.st);
    
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
	


	// renormalize and homogenize input variables
	//vec3 normal = normalize(world_normal);
    //vec3 position = world_position.xyz / world_position.w;
    
    // calculate the light-direction
    //vec3 light_dir = normalize(light_position - position);


	//Get the color of the bump-map
    //vec3 BumpNorm = vec3(texture2D(bumpMap, TexCoord0.xy));
    //Get the color of the texture
    //vec3 DecalCol = vec3(texture2D(colorMap, TexCoord0.xy));
    //Expand the bump-map into a normalized signed vector
    //BumpNorm = (BumpNorm -0.5) * 2.0;
	//float NdotL = max(dot(BumpNorm, LightDirTangentSpace0), 0.0);
    //Calculate the final color gl_FragColor
    //vec3 diffuse = NdotL * DecalCol;
    //Set the color of the fragment...  If you want specular lighting or other types add it here
    //fragColor = vec4(diffuse,1);

	// calculate lighting
	//vec4 ctmp = texture2D( texture, gl_TexCoord[0].st);
	//vec4 ctmp = texture2D( texture, TexCoord0);
    //vec4 ambient = ambient_color * ctmp;
    //vec4 diffuse = ctmp * light_color * max(0.0, dot(normal, light_dir));
    
    // write color to output
    //fragColor = ambient + diffuse;

	//fragColor = texture2D( colorMap, TexCoord0.st);

}
