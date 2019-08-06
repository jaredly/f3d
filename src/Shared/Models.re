type nu('t) = Js.null('t);


/*** Sub-objects */
type recipeIngredient = {
  .
  "id": string,
  "ingredient": string,
  "amount": nu(float),
  "unit": nu(string),
  "comments": nu(string)
};

type idOrText =
  | Id(string)
  | Text(string);

type maybeRecipeIngredient = {
  .
  "id": string,
  "ingredient": idOrText,
  "amount": nu(float),
  "unit": nu(string),
  "comments": nu(string)
};

let maybeRecipeIngredient = (~guess=?, ing) => {
  "id": ing##id,
  "ingredient":
    switch guess {
    | Some(text) => Text(text)
    | None => ing##ingredient === "" ? Text("") : Id(ing##ingredient)
    },
  "amount": ing##amount,
  "unit": ing##unit,
  "comments": ing##comments
};

let reallyRecipeIngredient = (ing) => {
  "id": ing##id,
  "ingredient":
    switch ing##ingredient {
    | Id(id) => id
    | Text(text) => failwith("No id")
    },
  "amount": ing##amount,
  "unit": ing##unit,
  "comments": ing##comments
};

/* let module RecipeIngredient = {
     type record = {
       id: string,
       ingredient: string,
       amount: float,
       unit: string,
       comments: string,
     };
   }; */
type instruction = {. "text": string, "ingredientsUsed": Js.Dict.t(string)};

type meta = {
  .
  "cookTime": nu(int),
  "prepTime": nu(int),
  "totalTime": nu(int),
  "ovenTemp": nu(int), /*** Farenheit */
  "yield": nu(float),
  "yieldUnit": nu(string)
};

/*** Database types */
type userData = {
  .
  "flickrToken": nu(Flickr.token),
  "following": array(string),
  "homepageLists": array(string)
  /*** TODO what else here? */
};

module UserData = {
  let name = "userDatas";
  type t = userData;
};

type publicUserData = {
  .
  "name": string
  /* TODO profile pic or something? */
};

module PublicUserData = {
  let name = "publicUserData";
  type t = publicUserData;
};

type ingredient = {
  .
  "id": string,
  "authorId": string,
  "name": string,
  "plural": nu(string),
  "defaultUnit": nu(string),
  "alternativeNames": array(string),
  "created": float,
  "calories": nu(float),
  "diets": array(string),
  "aisle": nu(string)
};

module Ingredient = {
  let name = "ingredients";
  type t = ingredient;
  let getId = (ing) => ing##id;
};

type tag = {
  .
  "id": string, "authorId": string, "name": string, "created": float, "recipeCount": int
};

module Tag = {
  let name = "tags";
  type t = tag;
};

type header = {. "pos": int, "text": string};


/*** The main types */
type recipe = {
  .
  "id": string,
  "authorId": string,
  "collaborators": Js.Dict.t(bool),
  "isPrivate": bool,
  "title": string,
  "titleSearch": Js.Dict.t(bool),
  "created": float,
  "updated": float,
  "imageUrl": nu(string),
  "tags": Js.Dict.t(bool),
  "source": nu(string),
  "instructions": array(instruction),
  "instructionHeaders": array(header),
  "ingredients": array(recipeIngredient),
  "ingredientHeaders": array(header),
  "ingredientsUsed": Js.Dict.t(bool),
  "images": nu(array(string)),
  "notes": nu(string), /* TODO update models */
  "description": nu(string),
  "meta": meta,
  "comments": array({.
    "id": string, "authorId": string,
    "created": float, "replyTo": nu(string), "text": string
  })
};

module Recipe = {
  let name = "recipes";
  type t = recipe;

  let blank: (string, string) => recipe = (id, authorId) => {
    let now = Js.Date.now();
    let empty = Js.Dict.empty();
    {
      "id": id,
      "title": "",
      "titleSearch": empty,
      "created": now,
      "updated": now,
      "imageUrl": Js.null,
      "tags": empty,
      "source": Js.null,
      "instructions": [||],
      "instructionHeaders": [||],
      "ingredientHeaders": [||],
      "ingredients": [||],
      "ingredientsUsed": empty,
      "description": Js.null,
      "notes": Js.null,
      "images": Js.null,
      /* "rating": Js.null, */
      "comments": [||],
      "meta": {
        "cookTime": Js.null,
        "prepTime": Js.null,
        "totalTime": Js.null,
        "ovenTemp": Js.null,
        "yield": Js.null,
        "yieldUnit": Js.null
      },
      "authorId": authorId,
      "collaborators": empty,
      "isPrivate": (false)
    }
  };
};

type madeIt = {.
  "id": string,
  "recipeId": string,
  "authorId": string,

  "collaborators": Js.Dict.t(bool), /* gets copied from main recipe? if its private */
  "isPrivate": bool,

  "created": float,
  "updated": float,

  "imageUrl": nu(string), /* Will just be the first one prolly */
  "images": array(string),

  /* first pass: not allow editing */
  "instructions": array(instruction),
  "instructionHeaders": array(header),

  "ingredients": array(recipeIngredient),
  "ingredientHeaders": array(header),

  "batches": float,
  "ateWithRecipes": array(string), /* recipe ids */
  "ateWithPeople": array(string), /* TODO: update current models */
  /** I like this :D */

  /* first pass: just these */
  "notes": string,
  "rating": nu(int),
  "meta": meta,
};

module MadeIt = {
  let name = "madeIts";
  type t = madeIt;
  let fromRecipe = (recipe: recipe, authorId: string) => {
    "id": BaseUtils.uuid(),
    "recipeId": recipe##id,
    "authorId": authorId,
    "collaborators": recipe##collaborators,
    "isPrivate": false,

    "created": Js.Date.now(),
    "updated": Js.Date.now(),

    "imageUrl": Js.null,
    "images": [||],

    /* first pass: not allow editing */
    "instructions": recipe##instructions,
    "instructionHeaders": recipe##instructionHeaders,

    "ingredients": recipe##ingredients,
    "ingredientHeaders": recipe##ingredientHeaders,

    "batches": 1.,
    "ateWithRecipes": [||],
    "ateWithPeople": [||],
    /** I like this :D */

    /* first pass: just these */
    "notes": "",
    "rating": Js.null,
    "meta": recipe##meta
  };
};

type list = {
  .
  "id": string,
  "authorId": string,
  "collaborators": Js.Dict.t(bool),
  "isPrivate": bool,
  "title": string,
  "created": float,
  "updated": float,
  "recipes": Js.Dict.t(bool)
};

module List = {
  let name = "lists";
  type t = list;
  let getId = (list) => list##id;
};

// type amountWithUnit = {
//   .
//   "amount": float,
//   "unit": string,
// }

// Open questions:
// - how do we account for ingredients being in multiple units?
// - say for tomato sauce, we have 5 16-oz cans, and 1 32-oz can.
// - also if those things are separately required by different recipes, what do I put in the shopping cart?
//    - I guess one way to do it would be to have a "primary unit" for an ingredient, and allow the inputting of "secondary units" along with conversions
//    - but I could probably automatically handle standard units of measure. But then "cups, bunches, etc." would need a conversion ratio

/// oooohk, much simpler solution: Allow multiple units. So we merge things we can convert (straight units of measure), and maybe do best effort on other things
/// but in general we just leave it. So you can have 2 bunches of asparagus and 1 bag frozen asparagus and 2 cups asparagus.

type amountDict = Js.Dict.t(float);

type pantry = {
  .
  // ingredient id -> amount
  "ingredients": Js.Dict.t(amountDict),
  // ingredient id -> as a set
  "staples": Js.Dict.t(bool),
}

type shoppingList = {
  .
  // id -> # batches, ingredients to buy w/ amounts
  "recipes": Js.Dict.t((float, Js.Dict.t(amountDict))),
  // soo when adding a recipe, you should be able to specify that you're only getting a subset of the ingredients. By default, the ingredients that you have marked as "staples" are unchecked, but you can check them.
  // you should also be able to go back and modify the number of batches.
  // Should I let you muck with the amounts/units of ingredients? Maybe not. But it would be nice. And then I can just do the math when you change batch sizes, it's fine.
  
  // ingredient id -> amount
  "extraIngredients": Js.Dict.t(amountDict),
  // ingredient id -> amount
  "boughtIngredients": Js.Dict.t(amountDict)
}