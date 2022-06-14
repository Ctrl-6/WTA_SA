all: exec

exec: main.o 
	g++ -Wall -o exec main.o 

main.o: main.cpp
	g++ -Wall -c main.cpp

clean:
	rm -rf *.o exec
	







