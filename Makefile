GCC = gcc
CONSERVATIVE_FLAGS = -std=c99 -Wall -Wextra -pedantic
DEBUGGING_FLAGS = -g -O0
CFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

io.o: io.c io.h
	$(GCC) -c io.c $(CFLAGS)

wave.o: wave.c wave.h io.h
	$(GCC) -c wave.c $(CFLAGS)

render_tone: render_tone.o wave.o io.o
	$(GCC) -o render_tone render_tone.o wave.o io.o -lm

render_tone.o: render_tone.c io.h wave.h
	$(GCC) -c render_tone.c $(CFLAGS)

render_song: io.o wave.o render_song.o 
	$(GCC) -o render_song render_song.o wave.o io.o -lm

render_song.o: render_song.c io.h wave.h
	$(GCC) -c render_song.c $(CFLAGS)

render_echo: render_echo.o wave.o io.o
	$(GCC) -o render_echo render_echo.o wave.o io.o -lm

render_echo.o: render_echo.c io.h wave.h
	$(GCC) -c render_echo.c $(CFLAGS)

clean:
	rm -f *.o render_song render_echo render_tone *~
