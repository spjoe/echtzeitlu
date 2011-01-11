#include "ModelImage.h"

using namespace echtzeitlu;

ModelImage::ModelImage(domImage* img){
		domImage* imageElement = img;
	
		if ( !imageElement )
			return;

		imageElement->getInit_from()->getValue().str();
			
		const std::string file = cdom::uriToNativePath(imageElement->getInit_from()->getValue().str());
		
		GLFWimage image;
		if (glfwReadImage(file.c_str(), &image, GLFW_ORIGIN_UL_BIT) != GL_TRUE)
			return;
		get_errors();
		glGenTextures(1, &_id);
		get_errors();
		glBindTexture(GL_TEXTURE_2D, _id);
		get_errors();
		// Load texture from file, and build all mipmap levels
		//if( !glfwLoadTexture2D( file.c_str(), GLFW_ORIGIN_UL_BIT | GLFW_BUILD_MIPMAPS_BIT ) )
		//{
		//	return;
		//}
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		get_errors();
		// Use trilinear interpolation for minification
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR );
		get_errors();
		// Use bilinear interpolation for magnification
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR );
		get_errors();

		//if(glfwLoadMemoryTexture2D(image.Data, image.BytesPerPixel * image.Height * image.Width, GLFW_BUILD_MIPMAPS_BIT) == GL_FALSE);
		//	return;
		//image.Format;
		
#ifdef GL_GENERATE_MIPMAP_SEG_AVOIDANCE
#ifdef DEBUG
		printf("using gluBuild2DMipmaps()\n");
#endif
		gluBuild2DMipmaps(GL_TEXTURE_2D, 0x8C40 /*SRGB_EXT*/, image.Width, image.Height,image.Format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(image.Data));
#else
		glTexImage2D(GL_TEXTURE_2D, 0, 0x8C40 /*SRGB_EXT*/, image.Width, image.Height,
						0, image.Format, GL_UNSIGNED_BYTE,
						reinterpret_cast<void*>(image.Data));
		glGenerateMipmap(GL_TEXTURE_2D);
#endif
		get_errors();
		glfwFreeImage(&image);
	}
unsigned ModelImage::getTexId(){
	return _id;
}
ModelImage::ModelImage(string file){
		
		GLFWimage image;
		if (glfwReadImage(file.c_str(), &image, GLFW_ORIGIN_UL_BIT) != GL_TRUE)
			return;
		get_errors();
		glGenTextures(1, &_id);
		get_errors();
		glBindTexture(GL_TEXTURE_2D, _id);
		get_errors();
		// Load texture from file, and build all mipmap levels
		//if( !glfwLoadTexture2D( file.c_str(), GLFW_ORIGIN_UL_BIT | GLFW_BUILD_MIPMAPS_BIT ) )
		//{
		//	return;
		//}
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		get_errors();
		// Use trilinear interpolation for minification
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR );
		get_errors();
		// Use bilinear interpolation for magnification
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_LINEAR );
		get_errors();

		//if(glfwLoadMemoryTexture2D(image.Data, image.BytesPerPixel * image.Height * image.Width, GLFW_BUILD_MIPMAPS_BIT) == GL_FALSE);
		//	return;
		//image.Format;
		
#ifdef GL_GENERATE_MIPMAP_SEG_AVOIDANCE
#ifdef DEBUG
		printf("using gluBuild2DMipmaps()\n");
#endif
		gluBuild2DMipmaps(GL_TEXTURE_2D, 0x8C40 /*SRGB_EXT*/, image.Width, image.Height,image.Format, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(image.Data));
#else
		glTexImage2D(GL_TEXTURE_2D, 0, 0x8C40 /*SRGB_EXT*/, image.Width, image.Height,
						0, image.Format, GL_UNSIGNED_BYTE,
						reinterpret_cast<void*>(image.Data));
		glGenerateMipmap(GL_TEXTURE_2D);
#endif
		get_errors();
		glfwFreeImage(&image);
	}