LDIR=./lib
LIBS= -L${LDIR} -lAntTweakBar -lglfw3 -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor

all:
	mkdir obj
	gcc -Wall -fexceptions -g  -c src/glad.c -o obj/glad.o
	g++ -Wall -fexceptions -g  -c src/main.cpp -o obj/main.o
	g++ -Wall -fexceptions -g  -c src/Shader.cpp -o obj/Shader.o
	g++ -Wall -fexceptions -g  -c src/stb_image.cpp -o obj/stb_image.o
	g++ -Wall -fexceptions -g  -c src/Navigation.cpp -o obj/Navigation.o
	g++ -Wall -fexceptions -g  -c src/objLoader.cpp -o obj/objLoader.o
	g++ -Wall -fexceptions -g  -c src/UserInterface.cpp -o obj/UserInterface.o
	
	g++  -o tarea1 obj/glad.o obj/main.o obj/Shader.o obj/stb_image.o obj/Navigation.o obj/objLoader.o obj/UserInterface.o $(LIBS)

exec:
	./tarea1

clean:
	rm -r obj
