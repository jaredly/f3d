
type nu 't = Js.null 't;

/** Sub-objects */

type recipeIngredient = Js.t {.
  id: string,
  ingredient: string,
  amount: nu float,
  unit: nu string,
  comments: nu string
};

type idOrText =
  | Id string
  | Text string;

type maybeRecipeIngredient = Js.t {.
  id: string,
  ingredient: idOrText,
  amount: nu float,
  unit: nu string,
  comments: nu string
};

let maybeRecipeIngredient ing => {
  "id": ing##id,
  "ingredient": Id (ing##ingredient),
  "amount": ing##amount,
  "unit": ing##unit,
  "comments": ing##comments,
};

let reallyRecipeIngredient ing => {
  "id": ing##id,
  "ingredient": switch (ing##ingredient) {
  | Id id => id
  | Text text => failwith "No id"
  },
  "amount": ing##amount,
  "unit": ing##unit,
  "comments": ing##comments,
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

type instruction = (Js.t {.
  text: string,
  ingredientsUsed: Js.Dict.t string
});

type meta = Js.t {.
  cookTime: nu int,
  prepTime: nu int,
  totalTime: nu int,
  ovenTemp: nu int, /** Farenheit */

  yield: nu float,
  yieldUnit: nu string
};

/** Database types */

type userData = Js.t {.
  flickrToken: nu Flickr.token,
  following: array string,
  homepageLists: array string
  /** TODO what else here? */
};

let module UserData = { let name = "userDatas"; type t = userData };

type ingredient = Js.t {.
  id: string,
  authorId: string,

  name: string,
  plural: nu string,
  defaultUnit: nu string,
  alternativeNames: array string,
  created: float,
  calories: nu float,
  diets: array string,
  aisle: nu string
};

let module Ingredient = { let name = "ingredients"; type t = ingredient };

type tag = Js.t {.
  id: string,
  authorId: string,
  name: string,
  created: float,
  recipeCount: int
};

let module Tag = { let name = "tags"; type t = tag };

/** The main types */

type recipe = Js.t {.
  id: string,
  authorId: string,
  collaborators: Js.Dict.t Js.boolean,
  isPrivate: Js.boolean,

  title: string,
  titleSearch: Js.Dict.t Js.boolean,
  created: float,
  updated: float,
  imageUrl: nu string,
  tags: Js.Dict.t string,

  source: nu string,
  instructions: array instruction,
  ingredients: array recipeIngredient,
  ingredientsUsed: Js.Dict.t Js.boolean,
  description: nu string,

  meta: meta,

  comments: nu (array (Js.t {.
    id: string,
    authorId: string,
    created: float,
    replyTo: nu string,
    text: string
  }))
};

let module Recipe = { let name = "recipes"; type t = recipe };

type madeIt = Js.t {.
  id: string,
  authorId: string,

  recipeId: string,
  notes: string,
  addedIngredients: array recipeIngredient,
  removedIngredients: array string,
  images: array string,
  created: float,

  meta: meta
};

let module MadeIt = { let name = "madeIts"; type t = madeIt };

type list = Js.t {.
  id: string,
  authorId: string,
  collaborators: Js.Dict.t Js.boolean,
  isPrivate: Js.boolean,

  title: string,
  created: float,
  updated: float,
  recipes: Js.Dict.t Js.boolean
};

let module List = { let name = "lists"; type t = list };
