open Utils;

module Styles = {
  open Glamor;
  include RecipeStyles;

  /*** TODO extend header, change top padding to be different */
  let title =
    css([
      fontSize("44px"),
      fontWeight("inherit"),
      fontFamily("Abril Fatface"),
      flex("1"),
      padding("0"),
      margin("0"),
      border("none"),
      borderBottom("1px solid #aaa"),
      outline("none")
    ]);
  let saveButton = RecipeStyles.primaryButton;
  let disabledbutton =
    css([
      fontWeight("200"),
      color("#aaa"),
      cursor("default"),
      Selector(":hover", [color("#aaa")])
    ]);
};

type state = {
  title: string,
  description: string,
  notes: string,
  meta: Models.meta,
  source: Js.null(string),
  ingredients: array(Models.maybeRecipeIngredient),
  ingredientHeaders: array(Models.header),
  instructions: array(Models.instruction),
  instructionHeaders: array(Models.header)
};

type action =
  | SetDescription(string)
  | SetNotes(string)
  | SetSource(Js.null(string))
  | SetIngredients((array(Models.header), array(Models.maybeRecipeIngredient)))
  | SetInstructions((array(Models.header), array(Models.instruction)))
  | SetBoth((array(Models.maybeRecipeIngredient), array(Models.instruction)))
  | SetMeta(Models.meta)
  | Import(state)
  | SetTitle(string);

let clone: Js.t('a) => Js.t('a) = (obj) => Js.Obj.assign(Js.Obj.empty(), obj);

let makeTitleSearch = (title) => {
  let tokens = SearchIndex.tokens(title);
  let obj = Js.Dict.empty();
  Array.iter((tok) => Js.Dict.set(obj, tok, true), tokens);
  obj
};

let makeIngredientsUsed = (ingredients) => {
  let obj = Js.Dict.empty();
  Array.iter((ing) => Js.Dict.set(obj, ing##ingredient, true), ingredients);
  obj
};

let updateRecipe =
    (
      recipe,
      {
        title,
        description,
        notes,
        source,
        meta,
        ingredients,
        ingredientHeaders,
        instructions,
        instructionHeaders
      }
    ) => {
  let recipe = clone(recipe) |> Obj.magic;
  recipe##title#=title;
  recipe##description#=description;
  recipe##notes#=(notes |> BaseUtils.magicDefault(""));
  /* TODO data upgrade */
  /* recipe##rating#=rating; */
  recipe##meta#=meta;
  recipe##ingredients#=(ingredients |> Array.map(Models.reallyRecipeIngredient));
  recipe##instructions#=instructions;
  recipe##ingredientHeaders#=ingredientHeaders;
  recipe##instructionHeaders#=instructionHeaders;
  recipe##source#=source;
  recipe##updated#=(Js.Date.now());
  recipe##titleSearch#=(makeTitleSearch(title));
  recipe##ingredientsUsed#=(makeIngredientsUsed(recipe##ingredients));
  let recipe: Models.recipe = recipe;
  recipe
};

let component = ReasonReact.reducerComponent("EditRecipe");

let make =
    (~saving, ~recipe, ~allIngredients, ~fb, ~id, ~onSave, ~onCancel, ~onDelete=?, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => {
      title: recipe##title,
      meta: recipe##meta,
      source: recipe##source,
      description: recipe##description |> Js.Null.toOption |> BaseUtils.optOr(""),
      notes: recipe##notes |> Js.Null.toOption |> BaseUtils.optOr(""),
      ingredients: recipe##ingredients |> Array.map(Models.maybeRecipeIngredient),
      ingredientHeaders: recipe##ingredientHeaders,
      instructions: recipe##instructions,
      instructionHeaders: recipe##instructionHeaders
    },
    reducer: (action, state) =>
      ReasonReact.Update(
        switch action {
        | SetTitle(title) => {...state, title}
        | SetMeta(meta) => {...state, meta}
        | SetSource(source) => {...state, source}
        | SetBoth((ingredients, instructions)) => {...state, ingredients, instructions}
        | SetIngredients((ingredientHeaders, ingredients)) => {
            ...state,
            ingredients,
            ingredientHeaders
          }
        | SetInstructions((instructionHeaders, instructions)) => {
            ...state,
            instructions,
            instructionHeaders
          }
        | Import(state) => state
        | SetDescription(description) => {...state, description}
        | SetNotes(notes) => {...state, notes}
        }
      ),
    render:
      (
        {
          state:
            {
              title,
              description,
              notes,
              ingredients,
              ingredientHeaders,
              instructions,
              instructionHeaders,
              meta,
              source
            } as state,
          send
        }
      ) => {
      let allIngredientsValid =
        Js.Array.every(
          (ing) =>
            switch ing##ingredient {
            | Models.Text(_) => false
            | Models.Id(_) => true
            },
          ingredients
        );
      let canSave = title !== "" && allIngredientsValid && ! saving;
      <div className=Styles.container>
        <div className=Styles.header>
          <input
            disabled=((saving))
            className=Styles.title
            placeholder="Recipe title"
            value=title
            onChange=(((evt) => send(SetTitle(evtValue(evt)))))
          />
          <Tooltip
            message="Cannot save with invalid ingredients"
            enabled=(! allIngredientsValid)
            render=(
              () =>
                <button
                  className=(
                    Styles.button
                    ++ (
                      " " ++ (Styles.saveButton ++ (" " ++ (canSave ? "" : Styles.disabledbutton)))
                    )
                  )
                  /* disabled=( (not canSave)) */
                  onClick=((_) => canSave ? onSave(updateRecipe(recipe, state)) : ())>
                  (str(saving ? "Saving" : "Save"))
                </button>
            )
          />
          <button className=Styles.button onClick=onCancel> (str("Cancel")) </button>
        </div>
        (spacer(8))
        (
          EditMeta.render(
            ~onChange=((meta) => send(SetMeta(meta))),
            ~meta,
            ~source,
            ~onChangeSource=((source) => send(SetSource(source)))
          )
        )
        (spacer(16))
        <Textarea
          value=description
          onChange=(((text) => send(SetDescription(text))))
          className=Glamor.(
                      css([
                        border("none"),
                        borderBottom("1px solid #aaa"),
                        outline("none"),
                        fontStyle("italic"),
                        padding("0")
                      ])
                    )
        />
        (spacer(32 - 2 /* correct for border */))
        <div className=Styles.subHeader>
           (str("Ingredients")) (spacer(32)) </div>
        (spacer(16))
        (
          EditIngredients.render(
            ~ingredients,
            ~ingredientHeaders,
            ~allIngredients,
            ~fb,
            ~onPaste=
              ((ingredients, instructions)) =>
                switch instructions {
                | None => (send(SetIngredients(([||], ingredients))))
                | Some(inst) => (send(SetBoth((ingredients, inst))))
                },
            ~onChange=(((headers, ingredients)) => send(SetIngredients((headers, ingredients))))
          )
        )
        (spacer(32))
        <div className=Styles.subHeader> (str("Instructions")) </div>
        (spacer(16))
        (
          EditInstructions.render(
            ~instructions,
            ~instructionHeaders,
            ~onChange=(((headers, instructions)) => send(SetInstructions((headers, instructions))))
          )
        )
        (spacer(32))
        <div className=Styles.subHeader> (str("Notes")) </div>
        (spacer(16))
        <Textarea
          value=notes
          onChange=(((text) => send(SetNotes(text))))
          className=Glamor.(
                      css([
                        border("none"),
                        borderBottom("1px solid #aaa"),
                        outline("none"),
                        fontStyle("italic"),
                        padding("0")
                      ])
                    )
        />
        (spacer(64))
        <ImportInput
          fb
          allIngredients
          onImport=(
            (
              (recipe) =>
                send(Import
                  ({
                    title: recipe##title,
                    instructions: recipe##instructions,
                    instructionHeaders: [||], /*** TODO import headers too. maybe? */
                    source: Js.Null.return(recipe##source),
                    meta: recipe##meta,
                    ingredients: recipe##ingredients,
                    description: recipe##description,
                    notes: "",
                    ingredientHeaders: [||]
                  })) /*** TODO import headers too */
            )
          )
        />
        (spacer(32))
        {
          let%Lets.Opt.React onDelete = onDelete;
          <DeleteButton onDelete />
        }
        (spacer(64))
      </div>
    }
  };
