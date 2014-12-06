OBJ = main.o
INC = -I "./"

Crepuscular: $(OBJ)
	g++ $(OBJ) -o Crepuscular.exe
	rm -f $(OBJ)
	
main.o:
	g++ -c main.cpp $(INC)
	
clean:
	rm -f $(OBJ)