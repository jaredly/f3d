
open Utils;

let module Styles = {
  open Glamor;
  include RecipeStyles;
};

let component = ReasonReact.reducerComponent "EditRecipe";

type state = {
  meta: {.},
  ingredients: array Models.recipeIngredient,
  instructions: array Models.instruction,
};

let make ::recipe ::ingredients ::fb ::id ::onSave ::onCancel _children => ReasonReact.{
  ...component,
  initialState: fun () => (),
  reducer: fun () () => ReasonReact.NoUpdate ,
  render: fun {state, reduce} => {
    <div className=Styles.container>
      <div className=Styles.header>
        <div className=Styles.title>
          (str recipe##title)
        </div>
        spring
        <button className=Styles.button onClick=onSave>
          (str "Save")
        </button>
        <button className=Styles.button onClick=onCancel>
          (str "Cancel")
        </button>
      </div>
      (spacer 8)
      (Meta.metaLine meta::recipe##meta source::recipe##source)
      (spacer 16)
      <div className=Glamor.(css[fontStyle "italic"])>
        (orr "" (Js.Nullable.to_opt recipe##description) |> ifEmpty "No description" |> str)
      </div>
      (spacer 32)
      <div className=Styles.subHeader>
        (str "ingredients")
        (spacer 32)
        /** TODO num input */
      </div>
      (spacer 16)
      /* (EditIngredients.render ingredients::recipe##ingredients allIngredients::ingredients) */
      (spacer 32)
      <div className=Styles.subHeader>
        (str "instructions")
      </div>
      (spacer 16)
      /* (Instructions.render instructions::recipe##instructions) */
      (spacer 64)
    </div>
  }
};


/* 
/** Data loading part */
let module RecipeFetcher = FirebaseFetcher.Single Models.Recipe;
let module IngredientsFetcher = FirebaseFetcher.Static { include Models.Ingredient; let query q => q; };
let make ::fb ::navigate ::id children => {
  Js.log id;
  RecipeFetcher.make ::fb ::id
  render::(fun ::state => {
    <IngredientsFetcher
      fb
      pageSize=1000
      render=(fun state::ingredients fetchMore::_ => {
        switch (doubleState state ingredients) {
        | `Initial => loadingRecipe ()
        | `Loaded (recipe, (_, ingredients)) => make ::recipe ::ingredients ::fb ::id [||] |> ReasonReact.element
        | `Errored err => failedLoading err
        }
      })
    />
  }) children
} */
