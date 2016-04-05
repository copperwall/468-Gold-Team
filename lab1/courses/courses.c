#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../jansson-2.7/build/include/jansson.h"

#define NUM_DEPARTMENTS 20
#define NUM_PROFESSORS 20

static char *department[] = {"AERO", "AG", "BIO", "BUS", "CSC", "CPE", "DATA", "EE", "ECON", "ENGL", "ENGR", "GEOG", "HIST", "IME", "KINE", "MATH", "MU", "POLS", "PHYS", "STAT"};
static char *professors[] = {"Mammen", "Staley", "Woodruff", "Khosmood", "Oliver", "Dekhtyar", "Braun", "Lund", "McMahan", "Peterson", "Smith", "Jones", "Sorensen", "Retsek", "Stanchev", "Bellardo", "Nico", "Kearns", "Seng", "Lupo"};

int randomNdx(int min, int max) {
   return rand() % (max-min) + min;
}

void createPrereqs(json_t *course, int num) {
   int ndx = 0;
   json_t *prereq = json_array();

   for (ndx; ndx < num; ndx++) {
      json_array_append(prereq, json_integer(randomNdx(100, 600)));
   }

   json_object_set(course, "prereqs", prereq);
}

int main(int argc, char *argv[]) {
   int numObjects = 0;
   int depNdx = 0;
   int ndx = 0;

   if (argc != 2) {
   	printf("Usage: ./Courses numObjects\n");
   	return 0;
   }

   srand(time(0));
   numObjects = atoi(argv[1]);
   json_t *arr = json_array();

   for (ndx; ndx < numObjects; ndx++) {
      json_t *course = json_object(); 
      //id
      json_object_set(course, "id", json_integer(ndx + 1));
      //title
      json_object_set(course, "title", json_string(department[randomNdx(0, NUM_DEPARTMENTS)]));
      //num
      json_object_set(course, "num", json_integer(randomNdx(100, 600)));
      //professor
      json_object_set(course, "professor", json_string(professors[randomNdx(0, NUM_PROFESSORS)]));
      //prereq
      createPrereqs(course, randomNdx(1,4));

      json_array_append(arr, course);
      
   }

   printf("%s\n", json_dumps(arr, JSON_INDENT(3)));
	
   return 0;
}