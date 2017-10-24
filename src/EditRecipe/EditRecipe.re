
open Utils;

let module Styles = {
  open Glamor;
  include RecipeStyles;
  /** TODO extend header, change top padding to be different */
  let title = css [
    fontSize "44px",
    fontWeight "inherit",
    fontFamily "Abril Fatface",
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
  source: (Js.null string),
  ingredients: array Models.maybeRecipeIngredient,
  ingredientHeaders: array Models.header,
  instructions: array Models.instruction,
  instructionHeaders: array Models.header,
};

type action =
  | SetDescription string
  | SetSource (Js.null string)
  | SetIngredients (array Models.header, array Models.maybeRecipeIngredient)
  | SetInstructions (array Models.header, array Models.instruction)
  | SetBoth (array Models.maybeRecipeIngredient, array Models.instruction)
  | SetMeta Models.meta
  | Import state
  | SetTitle string;

let clone: Js.t 'a => Js.t 'a = fun obj => Js.Obj.assign (Js.Obj.empty ()) obj;

let makeTitleSearch title => {
  let tokens = SearchIndex.tokens title;
  let obj = Js.Dict.empty ();
  Array.iter (fun tok => Js.Dict.set obj tok Js.true_) tokens;
  obj;
};

let makeIngredientsUsed ingredients => {
  let obj = Js.Dict.empty ();
  Array.iter (fun ing => Js.Dict.set obj ing##ingredient Js.true_) ingredients;
  obj;
};

let updateRecipe recipe {title, description, source, meta, ingredients, ingredientHeaders, instructions, instructionHeaders} => {
  let recipe = clone recipe |> Obj.magic;
  recipe##title #= title;
  recipe##description #= description;
  recipe##meta #= meta;
  recipe##ingredients #= (ingredients |> Array.map Models.reallyRecipeIngredient);
  recipe##instructions #= instructions;
  recipe##ingredientHeaders #= ingredientHeaders;
  recipe##instructionHeaders #= instructionHeaders;
  recipe##source #= source;
  recipe##updated #= (Js.Date.now ());
  recipe##titleSearch #= (makeTitleSearch title);
  recipe##ingredientsUsed #= (makeIngredientsUsed recipe##ingredients);
  let recipe: Models.recipe = recipe;
  recipe
};

let component = ReasonReact.reducerComponent "EditRecipe";

let make ::saving ::recipe ::allIngredients ::fb ::id ::onSave ::onCancel ::onDelete=? _children => ReasonReact.{
  ...component,
  initialState: fun () => {
    title: recipe##title,
    meta: recipe##meta,
    source: recipe##source,
    description: recipe##description |> Js.Null.to_opt |> BaseUtils.optOr "",
    ingredients: recipe##ingredients |> Array.map Models.maybeRecipeIngredient,
    ingredientHeaders: recipe##ingredientHeaders,
    instructions: recipe##instructions,
    instructionHeaders: recipe##instructionHeaders,
  },
  reducer: fun action state => ReasonReact.Update (switch action {
    | SetTitle title => {...state, title}
    | SetMeta meta => {...state, meta}
    | SetSource source => {...state, source}
    | SetBoth (ingredients, instructions) => {...state, ingredients, instructions}
    | SetIngredients (ingredientHeaders, ingredients) => {...state, ingredients, ingredientHeaders}
    | SetInstructions (instructionHeaders, instructions) => {...state, instructions, instructionHeaders}
    | Import state => state
    | SetDescription description => {...state, description}
  }),

  render: fun {state: {title, description, ingredients, ingredientHeaders, instructions, instructionHeaders, meta, source} as state, reduce} => {
    let allIngredientsValid = Js.Array.every
    (fun ing => switch ing##ingredient {
    | Models.Text _ => false
    | Models.Id _ => true
    })
    ingredients;
    let canSave = title !== "" && allIngredientsValid && (not saving);
    <div className=Styles.container>
      <div className=Styles.header>
        <input
          disabled=(Js.Boolean.to_js_boolean saving)
          className=Styles.title
          placeholder="Recipe title"
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
      (EditMeta.render
        onChange::(reduce (fun meta => SetMeta meta))
        meta::meta
        source::source
        onChangeSource::(reduce (fun source => SetSource source)))
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
        ::ingredientHeaders
        ::allIngredients
        ::fb
        onPaste::(fun (ingredients, instructions) => {
          switch (instructions) {
          | None => (reduce (fun _ => SetIngredients ([||], ingredients))) ()
          | Some inst => (reduce (fun _ => SetBoth (ingredients, inst))) ()
          }
        })
        onChange::(reduce (fun (headers, ingredients) => SetIngredients (headers, ingredients)))
      )
      (spacer 32)
      <div className=Styles.subHeader>
        (str "instructions")
      </div>
      (spacer 16)
      (EditInstructions.render
        instructions::instructions
        instructionHeaders::instructionHeaders
        onChange::(reduce (fun (headers, instructions) => SetInstructions (headers, instructions)))
      )
      (spacer 64)
      <ImportInput
        fb
        allIngredients
        onImport=(reduce (fun recipe => Import {
          title: recipe##title,
          instructions: recipe##instructions,
          instructionHeaders: [||], /** TODO import headers too. maybe? */
          source: Js.Null.return recipe##source,
          meta: recipe##meta,
          ingredients: recipe##ingredients,
          description: recipe##description,
          ingredientHeaders: [||], /** TODO import headers too */
        }))
      />
      (spacer 32)
      {[%guard let Some onDelete = onDelete][@else ReasonReact.nullElement];
        <DeleteButton
          onDelete
        />
      }
      (spacer 64)
    </div>
  }
};
