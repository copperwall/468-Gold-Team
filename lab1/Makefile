dep:
	cd jansson-2.7 && \
		mkdir build && \
		cd build && \
		cmake .. && \
		make && \
		sed -i -e 's/<jansson_config.h>/"jansson_config.h"/g' include/jansson.h

clean-dep:
	rm -rf jansson-2.7/build

make-cocktails:
	gcc -o makeCocktails cocktails/cocktails.c -L jansson-2.7/build/lib -ljansson

clean-cocktails:
	rm -rf makeCocktails

make-movies:
	gcc -o makeMovies movies/movies.c -L jansson-2.7/build/lib -ljansson

clean-movies:
	rm -rf makeMovies 
