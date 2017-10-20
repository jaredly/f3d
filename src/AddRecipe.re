
open Utils;

type state =
  | Saving
  | Editing;

let blankRecipe: string => string => Models.recipe = fun id authorId => {
  let now = Js.Date.now ();
  let empty = Js.Dict.empty ();
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
    "comments": [||],

    "meta": {
      "cookTime": Js.null,
      "prepTime": Js.null,
      "totalTime": Js.null,
      "ovenTemp": Js.null,
      "yield": Js.null,
      "yieldUnit": Js.null,
    },
    "authorId": authorId,
    "collaborators": empty,
    "isPrivate": Js.Boolean.to_js_boolean false,
  }
};

let component = ReasonReact.reducerComponent "Recipe";
let make ::ingredients ::fb ::navigate _children => ReasonReact.{
  ...component,
  initialState: fun () => Editing,
  reducer: fun action _ => ReasonReact.Update action,
  render: fun {state, reduce} => {
    [%guard let Some uid = Firebase.Auth.fsUid fb][@else ReasonReact.nullElement];
    let id = BaseUtils.uuid();
    <EditRecipe
      saving=(state === Saving)
      recipe=(blankRecipe id uid)
      allIngredients=ingredients
      fb
      id=id
      onSave=(fun recipe => {
        (reduce (fun _ => Saving)) ();
        let module FB = Firebase.Collection Models.Recipe;
        let collection = FB.get fb;
        let doc = Firebase.doc collection recipe##id;
        Js.log2 "Saving" recipe;
        Firebase.set doc recipe
        |> Js.Promise.then_ (fun () => {
          navigate ("/recipes/" ^ recipe##id);
          Js.Promise.resolve ()
        })
        |> Js.Promise.catch (fun err => {
          Js.log err;
          [%bs.debugger];
          Js.Promise.resolve ()
        })
        |> ignore;
        ()
      })
      onCancel=(fun _ => {
        ()
      })
    />
  }
};

let module Styles = RecipeStyles;

let loadingRecipe () => {
  <div className=Styles.container>
    <div className=Styles.loading>
    (str "Loading")
    </div>
  </div>
};

let failedLoading err => {
  Js.log err;
  <div>
    (str "Errored though")
  </div>
};


/** Data loading part */
let module IngredientsFetcher = FirebaseFetcher.StaticStream {
  include Models.Ingredient;
  let query q => q;
  let getId ing => ing##id;
};

let make ::fb ::navigate children => {
  IngredientsFetcher.make
    ::fb
    render::(fun state::ingredients => {
      switch (ingredients) {
      | `Initial => loadingRecipe ()
      | `Loaded ingredients => make ::ingredients ::fb ::navigate [||] |> ReasonReact.element
      | `Errored err => failedLoading err
      }
    })
    [||]
}