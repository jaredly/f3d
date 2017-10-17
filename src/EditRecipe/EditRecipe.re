
open Utils;

let module Styles = {
  open Glamor;
  include RecipeStyles;
  /** TODO extend header, change top padding to be different */
  let title = css [
    fontSize "24px",
    fontWeight "inherit",
    fontFamily "inherit",
    flex "1",
    padding "0",
    margin "0",
    border "none",
    borderBottom "1px solid #aaa",
    outline "none",
  ];
  let saveButton = css [
    color "green",
    fontWeight "400",
  ];
  let disabledbutton = css [
    fontWeight "200",
    color "#aaa",
    cursor "default",
    Selector ":hover" [
      color "#aaa",
    ],
  ];
};

type state = {
  title: string,
  description: string,
  meta: Models.meta,
  ingredients: array Models.maybeRecipeIngredient,
  instructions: array Models.instruction,
};

type action =
  | SetDescription string
  | SetIngredients (array Models.maybeRecipeIngredient)
  | SetInstructions (array Models.instruction)
  | SetTitle string;

let clone: Js.t 'a => Js.t 'a = fun obj => Js.Obj.assign (Js.Obj.empty ()) obj;

let updateRecipe recipe {title, description, meta, ingredients, instructions} => {
  let recipe = clone recipe |> Obj.magic;
  recipe##title #= title;
  recipe##description #= description;
  recipe##meta #= meta;
  recipe##ingredients #= (ingredients |> Array.map Models.reallyRecipeIngredient);
  recipe##instructions #= instructions;
  let recipe: Models.recipe = recipe;
  recipe
};

let component = ReasonReact.reducerComponent "EditRecipe";

let make ::saving ::recipe ::allIngredients ::fb ::id ::onSave ::onCancel _children => ReasonReact.{
  ...component,
  initialState: fun () => {
    title: recipe##title,
    meta: recipe##meta,
    description: recipe##description |> Js.Null.to_opt |> orr "",
    ingredients: recipe##ingredients |> Array.map Models.maybeRecipeIngredient,
    instructions: recipe##instructions,
  },
  reducer: fun action state => ReasonReact.Update (switch action {
    | SetTitle title => {...state, title}
    | SetIngredients ingredients => {...state, ingredients}
    | SetInstructions instructions => {...state, instructions}
    | SetDescription description => {...state, description}
  }),
  render: fun {state: {title, description, ingredients, instructions} as state, reduce} => {
    let allIngredientsValid = Js.Array.every
    (fun ing => switch ing##ingredient {
    | Models.Text _ => false
    | Models.Id _ => true
    })
    ingredients;
    /* Js.log2 "Instructions" instructions; */
    let canSave = title !== "" && allIngredientsValid && (not saving);
    <div className=Styles.container>
      <div className=Styles.header>
        <input
          disabled=(Js.Boolean.to_js_boolean saving)
          className=Styles.title
          value=title
          onChange=(reduce (fun evt => SetTitle (evtValue evt)))
        />
        <Tooltip
          message="Cannot save with invalid ingredients"
          enabled=(not allIngredientsValid)
          render=(fun () => {
            <button
              className=(Styles.button ^ " " ^ Styles.saveButton ^ " " ^ (canSave ? "" : Styles.disabledbutton))
              /* disabled=(Js.Boolean.to_js_boolean (not canSave)) */
              onClick=(fun _ => canSave ? onSave (updateRecipe recipe state) : ())
            >
              (str (saving ? "Saving" : "Save"))
            </button>
          })
        />
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
        className=Glamor.(css[
          border "none",
          borderBottom "1px solid #aaa",
          outline "none",
          fontStyle "italic",
          padding "0"
        ])
      />
      (spacer (32 - 2 /* correct for border */))
      <div className=Styles.subHeader>
        (str "ingredients")
        (spacer 32)
        /** TODO num input */
      </div>
      (spacer 16)
      (EditIngredients.render
        ::ingredients
        ::allIngredients
        onChange::(reduce (fun ingredients => SetIngredients ingredients))
      )
      (spacer 32)
      <div className=Styles.subHeader>
        (str "instructions")
      </div>
      (spacer 16)
      (EditInstructions.render
        instructions::instructions
        onChange::(reduce (fun instructions => SetInstructions instructions))
      )
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
