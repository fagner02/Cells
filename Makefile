all: compile link open

compile:
	g++ -IC:\SFML-2.5.1\include -c main.cpp Collision.cpp

link:
	g++ -LC:\SFML-2.5.1\lib -o main.exe main.o Collision.o -lsfml-graphics -lsfml-window  -lsfml-system

open:
	.\main.exe
