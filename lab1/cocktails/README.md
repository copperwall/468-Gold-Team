Hey! This is a cocktail creator!

It randomly creates a cocktail name and selects its ingredients.

To build and run:
* In the parent directory run make:
 * `make dep` if you haven't already
 * `make make-cocktails`
* Run the thing with the number of elements you want
 * `./makeCocktails n`

Love,
Scott

Here's some example output:

```json
[
   {
      "glass": "cocktail",
      "id": 1,
      "ingredients": [
         {
            "parts": 1,
            "type": "rum"
         }
      ],
      "name": "pineapple daiquiri",
      "perparation": "Blend with ice."
   },
   {
      "glass": "cocktail",
      "id": 2,
      "ingredients": [
         {
            "parts": 2,
            "type": "coffee liquer"
         },
         {
            "parts": 1,
            "type": "gin"
         }
      ],
      "name": "basil margarita",
      "perparation": "Blend with ice."
   }
]
```
