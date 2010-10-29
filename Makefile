CXX=g++
CXXFLAGS = -I./external/glm/include -I./external/collada/include
GLFWFLAGS=`pkg-config --cflags libglfw`
GLFWLIBS=`pkg-config --libs libglfw`
GLEWFLAGS=`pkg-config --cflags glew`
GLEWLIBS=`pkg-config --libs glew`
FREEALUTFLAGS=`pkg-config --cflags freealut`
FREEALUTLIBS=`pkg-config --libs freealut`
COLLADALIBS=-L./external/collada/lib -lcollada14dom -lboost_filesystem -lminizip -lxml2 -lpcrecpp
SRC = src/ogl3_test.o src/shader.o src/utility.o src/camera.o src/ModelLoader.o

all: gl32release gl32debug

gl32debug: ${SRC}
	g++ ${GLFWFLAGS} ${GLEWFLAGS} ${FREEALUTFLAGS} ${SRC} ${GLFWLIBS} ${GLEWLIBS} ${FREEALUTLIBS} ${COLLADALIBS} -o gl32debug 
	mv gl32debug ./bin

gl32release: ${SRC}
	g++ ${GLFWFLAGS} ${GLEWFLAGS} ${FREEALUTFLAGS} ${SRC} ${GLFWLIBS} ${GLEWLIBS} ${FREEALUTLIBS} ${COLLADALIBS} -o gl32release
	mv gl32release ./bin

clean:
	rm -f src/*.o bin/gl32*
