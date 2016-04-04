/**
 * Let's make cocktails!
 * This is a JSON generator for cocktail recipes.
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../jansson-2.7/build/include/jansson.h"

void buildIngredientsArray(json_t* parent, int num);
json_t* getCocktailName();
json_t* getGlass();
json_t* getPreparation();
int range(int low, int high);

int main(int argc, char *argv[]) {
   srand(time(0));

   // The number of drinks we should put in our output array.
   int drinksToCreate = atoi(argv[1]);

   // A container to put our drinks in.
   json_t* container = json_array();

   int i = 0;
   for (i; i < drinksToCreate; i++) {
      // Create a json object for the drink
      json_t* drink = json_object();

      // Add an ID
      json_object_set(drink, "id",
       json_integer(i + 1));

      // Add a name property
      json_object_set(drink, "name",
       getCocktailName());

      // Add a glass
      json_object_set(drink, "glass",
       getGlass());

      // Add a preparation
      json_object_set(drink, "perparation",
       getPreparation());

      // Add ingredients to the drink.
      buildIngredientsArray(drink, range(1, 4));

      // Append the drink to the container.
      json_array_append(container, drink);
   }

   printf("%s\n", json_dumps(container, JSON_INDENT(3) | JSON_SORT_KEYS));

   return 0;
}

/**
 * Add an array of ingredients to the drink object.
 */
void buildIngredientsArray(json_t* parent, int num) {
   json_t* ingredients = json_array();

   int typeCount = 7;
   const char* types[] = {
      "gin", "vodka", "rum", "vermouth", "St. Germain",
      "orange liquer", "coffee liquer"
   };

   int i = 0;
   for (i; i < num; i++) {
      json_t* ingredient = json_object();

      // Set the ingredient name
      json_object_set(ingredient, "type",
       json_string(types[range(0, typeCount)])); 

      // Set the ingredient parts
      json_object_set(ingredient, "parts",
       json_integer(range(1, 3)));

      // Append it to the ingredients list
      json_array_append(ingredients, ingredient);
   }

   json_object_set(parent, "ingredients", ingredients);
}

json_t* getCocktailName() {
   int nameCount = 5;
   const char* names[] = {"martini", "vodka cran", "daiquiri",
    "hurricane", "margarita"};
   
   int fruitCount = 5;
   const char* fruit[] = {"strawberry ", "apple ", "basil ",
    "cranberry ", "pineapple "};

   char name[50];
   strcpy(name, fruit[range(0, fruitCount)]);
   strcat(name, names[range(0, nameCount)]);

   return json_string(name);
}

json_t* getPreparation() {
   int prepCount = 3;
   const char* preps[] = {
      "Stir over ice, strain into glass.",
      "Blend with ice.",
      "Pour iver ice."
   };
   
   char preparation[50];
   strcpy(preparation, preps[range(0, prepCount)]);

   return json_string(preparation);
}

json_t* getGlass() {
   int glassCount = 4;
   const char* glasses[] = {
      "tumbler",
      "hurricane",
      "cocktail",
      "shooter"
   };
   
   char glass[50];
   strcpy(glass, glasses[range(0, glassCount)]);

   return json_string(glass);
}

int range(int low, int max) {
   return rand() % (max - low) + low;
}
