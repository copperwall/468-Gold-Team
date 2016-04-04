#include <string.h>
#include <time.h>
#include "../jansson-2.7/build/include/jansson.h"

json_t *generateMovieJSON(int id);

json_t *getRandomTitle() {
   char *titles[] = {"The Hateful Eight", "Django Unchained", "Pulp Fiction", "Resevior Dogs", "Hackers", "The Room"};

   return json_string(titles[rand() % 6]);
}

json_t *getRandomGenre() {
   char *genres[] = {"Comedy", "Animated", "Drama", "Thriller", "Western", "Family", "Romantic Comedy", "Horror", "Mystery"};

   return json_string(genres[rand() % 9]);
}

json_t *getRandomDirector() {
   char *names[] = {"Quentin Tarantino", "Guillermo Del Toro", "Christopher Nolan", "Zack Snyder", "Ben Affleck", "Tommy Wiseau"};
   char *name = names[rand() % 6];
   int oscarsWon = rand() % 5;
   json_t *director = json_object();

   json_object_set_new(director, "name", json_string(name));
   json_object_set_new(director, "oscarsWon", json_integer(oscarsWon));

   return director;
}

json_t *getRandomActor() {
   char *names[] = {"Jamie Foxx", "Christian Bale", "Tom Hanks", "Samuel L Jackson", "Robert Di Nero", "Anne Hathaway", "Tommy Wiseau"};
   char *name = names[rand() % 7];
   int oscarsNominated = rand() % 10;
   int oscarsWon = rand() % 5;
   json_t *actor = json_object();

   json_object_set_new(actor, "name", json_string(name));
   json_object_set_new(actor, "oscarsNominated", json_integer(oscarsNominated));
   json_object_set_new(actor, "oscarsWon", json_integer(oscarsWon));

   return actor;
}

json_t *getRandomYear() {
   int years[] = {1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 2005, 2010, 2015, 2016};

   return json_integer(years[rand() % 12]);
}

int main(int argc, char *argv[]) {
   if (argc != 2) {
      printf("Usage: movies_generator num_movies\n");
      exit(1);
   }

   int num_movies = atoi(argv[1]);
   int i;

   json_t *root_list, *movie;

   root_list = json_array();

   srand(time(0));
   for (i = 1; i <= num_movies; i++) {
      movie = generateMovieJSON(i);
      json_array_append_new(root_list, movie);
   }

   printf("%s\n", json_dumps(root_list, JSON_INDENT(3)));
}

json_t *generateMovieJSON(int id) {
   json_t *root, *title, *genre, *year, *director, *actors, *actor;
   int i, num_actors;

   num_actors = rand() % 5;

   root = json_object();
   title = getRandomTitle();
   genre = getRandomGenre();
   year = getRandomYear();
   director = getRandomDirector();

   actors = json_array();

   for (i = 0; i < num_actors; i++) {
      actor = getRandomActor();
      // Add actor to actors, decref if needed
      json_array_append_new(actors, actor);
      ++i;
   }

   json_object_set_new(root, "id", json_integer(id));
   if (id == 1 || (rand() % 10) <= 7) {
      json_object_set_new(root, "title", title);
   }
   if (id == 1 || (rand() % 10) <= 7) {
      json_object_set_new(root, "genre", genre);
   }
   if (id == 1 || (rand() % 10) <= 7) {
      json_object_set_new(root, "year", year);
   }
   if (id == 1 || (rand() % 10) <= 7) {
      json_object_set_new(root, "director", director);
   }
   if (id == 1 || (rand() % 10) <= 7) {
      json_object_set_new(root, "actors", actors);
   }

   return root;
}
