tools.o : tools.c tools.h
	gcc -c -g tools.c -o tools.o $$(sdl2-config --cflags --libs) -lm
	
canvas.o : canvas.c canvas.h
	gcc -c -g canvas.c -o canvas.o $$(sdl2-config --cflags --libs) -lm
	
terrain.o : terrain.c terrain.h
	gcc -c -g terrain.c -o terrain.o $$(sdl2-config --cflags --libs) -lm
	
display.o : display.c display.h
	gcc -c -g display.c -o display.o $$(sdl2-config --cflags --libs) -lm 
	
dispo.o : dispo.c dispo.h
	gcc -c -g dispo.c -o dispo.o $$(sdl2-config --cflags --libs) -lm 
	
gen-algo.o : gen-algo.c gen-algo.h
	gcc -c -g gen-algo.c -o gen-algo.o $$(sdl2-config --cflags --libs) -lm 
	
interactions.o : interactions.c interactions.h
	gcc -c -g interactions.c -o interactions.o $$(sdl2-config --cflags --libs) -lm 
	
linked_list.o : linked_list.c linked_list.h
	gcc -c -g linked_list.c -o linked_list.o
	
	
	
	

clean :
	rm -f *.o
	
clean_all :
	rm -f *.o
	rm main

main : main.c tools.c canvas.c terrain.c dispo.c display.c gen-algo.c interactions.c linked_list.c tools.o canvas.o terrain.o dispo.o display.o gen-algo.o interactions.o linked_list.o
	gcc -g tools.o canvas.o terrain.o dispo.o display.o gen-algo.o interactions.o linked_list.o main.c -o main $$(sdl2-config --cflags --libs) -lm
	rm -f *.o
