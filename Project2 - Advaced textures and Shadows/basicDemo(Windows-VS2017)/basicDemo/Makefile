tarea1:
	mkdir bin bin/Debug/
	mkdir obj obj/Debug/
	gcc -Wall -fexceptions -g  -c /home/dprog/Escritorio/tarea1/glad.c -o obj/Debug/glad.o
	g++ -Wall -fexceptions -g  -c /home/dprog/Escritorio/tarea1/main.cpp -o obj/Debug/main.o
	g++ -Wall -fexceptions -g  -c /home/dprog/Escritorio/tarea1/Shader.cpp -o obj/Debug/Shader.o
	g++ -Wall -fexceptions -g  -c /home/dprog/Escritorio/tarea1/stb_image.cpp -o obj/Debug/stb_image.o
	
	g++  -o bin/Debug/tarea1 obj/Debug/glad.o obj/Debug/main.o obj/Debug/Shader.o obj/Debug/stb_image.o   -lGL -lGLEW -lSDL2 -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor -lassimp

exec:
	./bin/Debug/tarea1

clean:
	rm -r bin
	rm -r obj
