open Utils;

type state =
  | Saving
  | Editing;

let blankRecipe: (string, string) => Models.recipe =
  (id, authorId) => {
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
      "isPrivate": Js.Boolean.to_js_boolean(false)
    }
  };

let component = ReasonReact.reducerComponent("Recipe");

let make = (~ingredients, ~fb, ~navigate, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => Editing,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, reduce}) => {
      [@else ReasonReact.nullElement] [%guard let Some(uid) = Firebase.Auth.fsUid(fb)];
      let id = BaseUtils.uuid();
      <EditRecipe
        saving=(state === Saving)
        recipe=(blankRecipe(id, uid))
        allIngredients=ingredients
        fb
        id
        onSave=(
          (recipe) => {
            (reduce((_) => Saving))();
            module FB = Firebase.Collection(Models.Recipe);
            let collection = FB.get(fb);
            let doc = Firebase.doc(collection, recipe##id);
            Js.log2("Saving", recipe);
            Firebase.set(doc, recipe)
            |> Js.Promise.then_(
                 () => {
                   navigate("/recipes/" ++ recipe##id);
                   Js.Promise.resolve()
                 }
               )
            |> Js.Promise.catch(
                 (err) => {
                   Js.log(err);
                   [%bs.debugger];
                   Js.Promise.resolve()
                 }
               )
            |> ignore;
            ()
          }
        )
        onCancel=((_) => ())
      />
    }
  };

module Styles = RecipeStyles;

let loadingRecipe = () =>
  <div className=Styles.container> <div className=Styles.loading> (str("Loading")) </div> </div>;

let failedLoading = (err) => {
  Js.log(err);
  <div> (str("Errored though")) </div>
};


/*** Data loading part */
module IngredientsFetcher =
  FirebaseFetcher.StaticStream(
    {
      include Models.Ingredient;
      let query = (q) => q;
      let getId = (ing) => ing##id;
    }
  );

let make = (~fb, ~navigate, _children) =>
  IngredientsFetcher.make(
    ~fb,
    ~render=
      (~state as ingredients) =>
        switch ingredients {
        | `Initial => loadingRecipe()
        | `Loaded(ingredients) => make(~ingredients, ~fb, ~navigate, [||]) |> ReasonReact.element
        | `Errored(err) => failedLoading(err)
        },
    [||]
  );
