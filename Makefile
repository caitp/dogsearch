dogsearch: dogsearch.o
	$(CC) $< -o $@

dogsearch.o: dogsearch.c
	$(CC) -c $< -o $@

clean:
	rm dogsearch.o dogsearch
