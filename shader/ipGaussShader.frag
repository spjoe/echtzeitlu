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
uniform bool vertical;


// fragment-shader output variable (-> stored in the frame-buffer, i.e. "the pixel you see")
out vec4 fragColor;
in vec2 texCoord;

const float blurSize = 1.0/256.0;
 
void main(void)
{
   vec4 sum = vec4(0.0);
 
   // blur in y (vertical)
   // take nine samples, with the distance blurSize between them
	if(vertical){
		sum += texture2D(colorMap, vec2(texCoord.x - 4.0*blurSize, texCoord.y)) * 0.05;
		sum += texture2D(colorMap, vec2(texCoord.x - 3.0*blurSize, texCoord.y)) * 0.09;
		sum += texture2D(colorMap, vec2(texCoord.x - 2.0*blurSize, texCoord.y)) * 0.12;
		sum += texture2D(colorMap, vec2(texCoord.x - blurSize, texCoord.y)) * 0.15;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y)) * 0.16;
		sum += texture2D(colorMap, vec2(texCoord.x + blurSize, texCoord.y)) * 0.15;
		sum += texture2D(colorMap, vec2(texCoord.x + 2.0*blurSize, texCoord.y)) * 0.12;
		sum += texture2D(colorMap, vec2(texCoord.x + 3.0*blurSize, texCoord.y)) * 0.09;
		sum += texture2D(colorMap, vec2(texCoord.x + 4.0*blurSize, texCoord.y)) * 0.05;
	}else{
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y - 4.0*blurSize)) * 0.05;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y - 3.0*blurSize)) * 0.09;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y - 2.0*blurSize)) * 0.12;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y - blurSize)) * 0.15;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y)) * 0.16;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y + blurSize)) * 0.15;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y + 2.0*blurSize)) * 0.12;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y + 3.0*blurSize)) * 0.09;
		sum += texture2D(colorMap, vec2(texCoord.x, texCoord.y + 4.0*blurSize)) * 0.05;
	}
	
 
   fragColor = sum;
}
