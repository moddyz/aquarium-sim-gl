CC=g++
FRAMEWORK = -framework GLUT -framework OpenGL -framework Cocoa /opt/local/lib/libGLEW.a
COMPILERFLAGS = -Wall
CFLAGS = $(COMPILERFLAGS) 
INCLUDES += -I/opt/local/include
OBJECTS = gl.o gl_config.o gl_util.o gl_info.o glm.o multiview.o simulation.o
all: gl
gl: $(OBJECTS)
	$(CC) $(CFLAGS) $(FRAMEWORK) $(OBJECTS) -o $@ 
gl.o: gl.cpp
	$(CC) -c $(INCLUDES) gl.cpp 
gl_config.o: gl_config.cpp
	$(CC) -c $(INCLUDES) gl_config.cpp 
gl_util.o: gl_util.cpp
	$(CC) -c $(INCLUDES) gl_util.cpp
gl_info.o: gl_info.cpp
	$(CC) -c $(INCLUDES) gl_info.cpp
glm.o: glm.cpp
	$(CC) -c $(INCLUDES) glm.cpp
simulation.o: simulation.cpp
	$(CC) -c $(INCLUDES) simulation.cpp
multiview.o: multiview.cpp
	$(CC) -c $(INCLUDES) multiview.cpp

clean:
	rm -f *.o gl
.PHONY: all clean
