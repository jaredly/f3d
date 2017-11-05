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
  "flickrToken": nu(Flickr.token), "following": array(string), "homepageLists": array(string)
  /*** TODO what else here? */
};

module UserData = {
  let name = "userDatas";
  type t = userData;
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
  "collaborators": Js.Dict.t(Js.boolean),
  "isPrivate": Js.boolean,
  "title": string,
  "titleSearch": Js.Dict.t(Js.boolean),
  "created": float,
  "updated": float,
  "imageUrl": nu(string),
  "tags": Js.Dict.t(Js.boolean),
  "source": nu(string),
  "instructions": array(instruction),
  "instructionHeaders": array(header),
  "ingredients": array(recipeIngredient),
  "ingredientHeaders": array(header),
  "ingredientsUsed": Js.Dict.t(Js.boolean),
  "description": nu(string),
  "meta": meta,
  "comments":
    array(
      {. "id": string, "authorId": string, "created": float, "replyTo": nu(string), "text": string}
    )
};

module Recipe = {
  let name = "recipes";
  type t = recipe;
};

type madeIt = {
  .
  "id": string,
  "authorId": string,
  "recipeId": string,
  "notes": string,
  "ingredients": array(recipeIngredient),
  "ingredientHeaders": array(header),
  "images": array(string),
  "created": float,
  "rating": int,
  "meta": meta
};

module MadeIt = {
  let name = "madeIts";
  type t = madeIt;
};

type list = {
  .
  "id": string,
  "authorId": string,
  "collaborators": Js.Dict.t(Js.boolean),
  "isPrivate": Js.boolean,
  "title": string,
  "created": float,
  "updated": float,
  "recipes": Js.Dict.t(Js.boolean)
};

module List = {
  let name = "lists";
  type t = list;
};
