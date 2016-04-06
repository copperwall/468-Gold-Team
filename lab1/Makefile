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

make-beers:
	gcc -o makeBeers beers/beers.c -L jansson-2.7/build/lib -ljansson

clean-beers:
	rm -rf makeBeers

make-courses:
	gcc -o makeCourses courses/courses.c -L jansson-2.7/build/lib -ljansson

clean-courses:
	rm -rf makeCourses

make-JSON2CSV:
	gcc -g -o makeJSON2CSV JSON2CSV.c JSON2CSV.h -L jansson-2.7/build/lib -ljansson

clean-JSON2CSV:
	rm -rf makeJSON2CSV


make-all:
	gcc -o makeBeers beers/beers.c -L jansson-2.7/build/lib -ljansson
	gcc -o makeCocktails cocktails/cocktails.c -L jansson-2.7/build/lib -ljansson
	gcc -o makeCourses courses/courses.c -L jansson-2.7/build/lib -ljansson
	gcc -o makeMovies movies/movies.c -L jansson-2.7/build/lib -ljansson	
	gcc -o makeJSON2CSV JSON2CSV.c JSON2CSV.h -L jansson-2.7/build/lib -ljansson

clean-all:
	rm -rf makeBeers
	rm -rf makeCocktails
	rm -rf makeCourses
	rm -rf makeMovies
	rm -rf makeJSON2CSV
