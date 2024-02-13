CC = clang

OBJ = main.o

output: $(OBJ)
	$(CC) $^ -o midiapp -framework CoreMIDI -framework CoreFoundation

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ) midiapp