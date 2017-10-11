
open Utils;

let module Styles = {
  open Glamor;
  include RecipeStyles;
  let title = css [
    fontSize "24px",
    fontWeight "inherit",
    fontFamily "inherit",
    flex "1",
    padding "4px 0",
    margin "0",
    border "none",
    borderBottom "1px solid #aaa",
    outline "none",
  ];
};

type state = {
  title: string,
  description: string,
  meta: Models.meta,
  ingredients: array Models.recipeIngredient,
  instructions: array Models.instruction,
};

type action =
  | SetDescription string
  | SetTitle string;

let clone: Js.t 'a => Js.t 'a = fun obj => Js.Obj.assign (Js.Obj.empty ()) obj;

let updateRecipe recipe {title, description, meta, ingredients, instructions} => {
  let recipe = clone recipe |> Obj.magic;
  recipe##title #= title;
  recipe##description #= description;
  recipe##meta #= meta;
  recipe##ingredients #= ingredients;
  recipe##instructions #= instructions;
  let recipe: Models.recipe = recipe;
  recipe
};

let component = ReasonReact.reducerComponent "EditRecipe";

let make ::saving ::recipe ::ingredients ::fb ::id ::onSave ::onCancel _children => ReasonReact.{
  ...component,
  initialState: fun () => {
    title: recipe##title,
    meta: recipe##meta,
    description: recipe##description |> Js.Nullable.to_opt |> orr "",
    ingredients: recipe##ingredients,
    instructions: recipe##instructions,
  },
  reducer: fun action state => ReasonReact.Update (switch action {
    | SetTitle title => {...state, title}
    | SetDescription description => {...state, description}
  }),
  render: fun {state: {title, description} as state, reduce} => {
    <div className=Styles.container>
      <div className=Styles.header>
        <input
          disabled=(Js.Boolean.to_js_boolean saving)
          className=Styles.title
          value=title
          onChange=(reduce (fun evt => SetTitle (evtValue evt)))
        />
        <button className=Styles.button onClick=(fun _ => onSave (updateRecipe recipe state))>
          (str "Save")
        </button>
        <button className=Styles.button onClick=onCancel>
          (str "Cancel")
        </button>
      </div>
      (spacer 8)
      (Meta.metaLine meta::recipe##meta source::recipe##source)
      (spacer 16)
      <Textarea
        value=description
        onChange=(reduce (fun text => SetDescription text))
        className=""
      />
      /* <div className=Glamor.(css[fontStyle "italic"])>
        (orr "" (Js.Nullable.to_opt recipe##description) |> ifEmpty "No description" |> str)
      </div> */
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
