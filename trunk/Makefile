CXX=g++
CXXFLAGS = -I./external/glm/include -I./external/collada/include/1.4/dom/ -I./external/collada/include/1.4/ -I./external/collada/include/ -DGL_GENERATE_MIPMAP_SEG_AVOIDANCE
FLAGS=`pkg-config --cflags libglfw glew freealut`
LIBS=`pkg-config --libs libglfw glew freealut`
COLLADALIBS=-L./external/collada/lib -lcollada14dom -lboost_filesystem -lminizip -lxml2 -lpcrecpp
SRC = src/ogl3_test.o src/shader.o src/utility.o src/camera.o src/ModelLoader.o src/SceneObject.o src/Model.o src/Effect.o src/Timer.o src/ModelObject.o src/ModelImage.o src/ModelEffect.o src/ModelMaterial.o

all: gl32release gl32debug

gl32debug: ${SRC}
	g++ ${FLAGS} ${SRC} ${LIBS} ${COLLADALIBS} -o gl32debug 
	mv gl32debug ./bin

gl32release: ${SRC}
	g++ ${FLAGS} ${SRC} ${LIBS} ${COLLADALIBS} -o gl32release
	mv gl32release ./bin

clean:
	rm -f src/*.o bin/gl32*
