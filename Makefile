CC = GCC

all: clean pipeline pipe

pipeline: obj/image.o obj/utils.o 
	$(CC) src/main.c obj/image.o obj/utils.o -o pipeline -lm -Wall

pipe: obj/image.o obj/utils.o 
	$(CC) src/pipeline.c obj/image.o obj/utils.o -o pipe -lm -Wall

obj/utils.o:
	$(CC) -c -Iinclude src/utils.c -o obj/utils.o -Wall

obj/image.o:
	$(CC) -c -Iinclude src/image.c -o obj/image.o -Wall

clean:
	rm -f obj/*.o
	rm -f pipeline
	rm -f pipe