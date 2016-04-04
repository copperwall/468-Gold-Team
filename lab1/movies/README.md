# Movie Generator

__To Build__

Run `make dep` if you haven't already.
Run `make make-movies`.

__Running__

`./makeMovies <num_movies>`

__Example Output__

```json
[
   {
      "id": 1,
      "title": "The Hateful Eight",
      "genre": "Family",
      "director": {
         "name": "Zack Snyder",
         "oscarsWon": 4
      },
      "year": 1994,
      "actors": [
         {
            "name": "Tommy Wiseau",
            "oscarsWon": 1,
            "oscarsNominated": 6
         },
         {
            "name": "Tom Hanks",
            "oscarsWon": 1,
            "oscarsNominated": 3
         }
      ]
   }
]
```
