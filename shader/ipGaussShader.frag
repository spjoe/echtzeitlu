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
uniform vec4 color;
uniform sampler2D colorMap;



// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;
in vec2 texCoord;

void main()
{
    vec4 gauss_sum = vec4(0,0,0,0);
    
    float kernel[5];
    kernel[0] = 1.0f;
    kernel[1] = 4.0f;
    kernel[2] = 6.0f;
    kernel[3] = 4.0f;
    kernel[4] = 1.0f;
    float divisor = 1.0f/(kernel[0]+kernel[1]+kernel[2]+kernel[3]+kernel[4]);
    
	gauss_sum += textureOffset(colorMap, texCoord, ivec2( 2, 0)) * kernel[0];
	gauss_sum += textureOffset(colorMap, texCoord, ivec2( 1, 0)) * kernel[1];
	gauss_sum += textureOffset(colorMap, texCoord, ivec2( 0, 0)) * kernel[2];
	gauss_sum += textureOffset(colorMap, texCoord, ivec2( 1, 0)) * kernel[3];
	gauss_sum += textureOffset(colorMap, texCoord, ivec2( 2, 0)) * kernel[4];
	gauss_sum = gauss_sum * divisor;

		
    
    // write color to output
    fragColor = gauss_sum;
}
