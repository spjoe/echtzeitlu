CXX=g++
CXXFLAGS = -I./external/glm/include
GLFWFLAGS=`pkg-config --cflags libglfw`
GLFWLIBS=`pkg-config --libs libglfw`
GLEWFLAGS=`pkg-config --cflags glew`
GLEWLIBS=`pkg-config --libs glew`
FREEALUTFLAGS=`pkg-config --cflags freealut`
FREEALUTLIBS=`pkg-config --libs freealut`

all: gl32release gl32debug

gl32debug: src/ogl3_test.o src/shader.o src/utility.o
	g++ ${GLFWFLAGS} ${GLEWFLAGS} ${FREEALUTFLAGS} src/ogl3_test.o src/shader.o src/utility.o ${GLFWLIBS} ${GLEWLIBS} ${FREEALUTLIBS} -o gl32debug 
	mv gl32debug ./bin

gl32release: src/ogl3_test.o src/shader.o src/utility.o
	g++ ${GLFWFLAGS} ${GLEWFLAGS} ${FREEALUTFLAGS} src/ogl3_test.o src/shader.o src/utility.o ${GLFWLIBS} ${GLEWLIBS} ${FREEALUTLIBS} -o gl32release
	mv gl32release ./bin

clean:
	rm -f src/*.o bin/gl32*
