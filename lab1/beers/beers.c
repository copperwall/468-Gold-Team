//Nick Verbos
//Generates random beer json objects for CSC468 lab1

#include <stdlib.h>
#include <stdio.h>
#include "../jansson-2.7/build/include/jansson.h"

#define MAX_STRING_LEN 50
#define MAX_LOCATIONS 10
#define MAX_SCORE 100.0
#define NUM_BREWERIES 5


//returns a random double between low and high inclusivesly
double randomDouble(double low, double high) {
   return ((double)rand() * (high - low )) / (double)RAND_MAX + low;
}

//returns a random int between low and high inclusively
int randomInt(int low, int high) {
   return (rand() % (high + 1 - low)) + low;
}

json_t *getRandomType() {
   char *breweries[] = {"christmas ale", "IPA", "oatmeal stout", "hefenweizen", "imperial stout", "ice beer", "dark lager", "pilsner", "porter", "DBA"};

   return json_string(breweries[randomInt(0, 9)]);
}

json_t *getRandomBreweryName() {
   char *breweries[] = {"Great Lakes", "Lagunitas", "Central Coast", "Firestone", "Weihenstephaner"};

   return json_string(breweries[randomInt(0, 4)]);
}

json_t *getRandomLocations(){
   char *locations[] = {"San Luis Obispo", "Los Angeles", "Cleveland", "Munich", "San Diego", "Barcelona", "Copenhagen", "Stockholm", "Orlando", "Seattle"};
   json_t *chosenLocations;
   int chosen[MAX_LOCATIONS] = {};
   int next;
   int numLocations = randomInt(1, MAX_LOCATIONS);
   chosenLocations = json_array();

   while(numLocations){
      next = randomInt(0, 9);
      if(!chosen[next]){
         chosen[next] = 1;
         json_array_append_new(chosenLocations, json_string(locations[next]));
         numLocations--;
      }
   }
   return chosenLocations;
}

json_t *generateBreweries(){
   int numLocations, i;
   json_t *brewery;

   brewery = json_object();
   
      json_object_set_new(brewery, "name", getRandomBreweryName());
      json_object_set_new(brewery, "locations", getRandomLocations());
   
   return brewery;
}

/*json_t *getRandomBrewery() {
   return &breweries[randomInt(0, NUM_BREWERIES - 1)];
}*/

json_t *generateRandomBeerJSON(int id){
   json_t *root, *brewery, *type, *score;
   
   root = json_object();   
   if(id == 1 || randomInt(0, 10) <= 10){
      json_object_set_new(root, "id", json_integer(id));
   }
   if(id == 1 || randomInt(0, 10) <= 10){
      json_object_set_new(root, "score", json_integer(randomInt(0, 100)));
   }
   if(id == 1 || randomInt(0, 10) <= 10){
      json_object_set_new(root, "brewery", generateBreweries());
   }

   if(id == 1 || randomInt(0, 10) <= 10){
      json_object_set_new(root, "type", getRandomType());
   }

   return root;
   
}


int main(int argc, char *argv[]){
   int numBeers, i = 0;
   json_t *root_list, *beer;
   
   if(argc != 2 || (argc == 2 && !(numBeers = atoi(argv[1])))){
      puts("invalid arguments.  Exiting.");
      exit(1);
   }

   srand(time(NULL));
   
   generateBreweries();
   root_list = json_array();
   
   while (i++ < numBeers) {
      beer = generateRandomBeerJSON(i);

      json_array_append_new(root_list, beer);
   }

   printf("%s\n", json_dumps(root_list, JSON_INDENT(3)));
   return 0;
}
