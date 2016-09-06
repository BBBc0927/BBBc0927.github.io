OBJ=main_demo

$(OBJ): main_demo.o
	gcc -o $(OBJ) main_demo.o -lpthread

main_demo.o: main_demo.c
	gcc -c main_demo.c -lpthread

clean:	
	rm -rf *.o $(OBJ)
