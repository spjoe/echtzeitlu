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



// fragment-shader input variables
//in vec4 frag_color;
in vec3 world_normal;
in vec4 world_position;
in vec2 TexCoord0;

// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;


void main()
{

	


	// renormalize and homogenize input variables
	vec3 normal = normalize(world_normal);
    vec3 position = world_position.xyz / world_position.w;
    
    // calculate the light-direction
    //vec3 light_dir = normalize(light_position - position);


	//Get the color of the bump-map
    vec3 BumpNorm = vec3(texture2D(bumpMap, TexCoord0.xy));
    //Get the color of the texture
    vec3 DecalCol = vec3(texture2D(colorMap, TexCoord0.xy));
    //Expand the bump-map into a normalized signed vector
    BumpNorm = (BumpNorm -0.5) * 2.0;
	float NdotL = max(dot(BumpNorm, LightDirTangentSpace0), 0.0);
    //Calculate the final color gl_FragColor
    vec3 diffuse = NdotL * DecalCol;
    //Set the color of the fragment...  If you want specular lighting or other types add it here
    fragColor = vec4(diffuse,1);

	// calculate lighting
	//vec4 ctmp = texture2D( texture, gl_TexCoord[0].st);
	//vec4 ctmp = texture2D( texture, TexCoord0);
    //vec4 ambient = ambient_color * ctmp;
    //vec4 diffuse = ctmp * light_color * max(0.0, dot(normal, light_dir));
    
    // write color to output
    //fragColor = ambient + diffuse;

	fragColor = texture2D( colorMap, TexCoord0.st);

}
