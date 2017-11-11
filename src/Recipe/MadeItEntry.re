open Utils;

let module RatingWidget = {
  let module Styles = {
    open Glamor;
    let buttonStyles = [
      fontSize("16px"),
      border("none"),
      backgroundColor("#fff"),
      padding("0"),
      margin("0"),
      fontWeight("inherit"),
      color(Shared.action),
      fontWeight("200"),
      cursor("pointer"),
      padding("4px 8px"),
      Selector(":hover", [color("black")]),
      outline("none"),
      borderBottom("2px solid transparent"),
      transition(".1s ease border-bottom-color"),
      Selector(":hover", [
        borderBottom("2px solid #ccc")
      ]),
    ];

    let button = css(buttonStyles @ [

    ]);

    let activeButton = css(buttonStyles @ [
      borderBottom("2px solid " ++ Shared.action),
      Selector(":hover", [
        borderBottom("2px solid " ++ Shared.action)
      ]),
    ]);
  };

  let component = ReasonReact.statelessComponent("RatingWidget");
  let ratings = [|
    "Won't make again",
    "Might try again, but needs adjustments",
    "Want to make again",
    "Will definitely make again",
    "Will make regularly"
  |];
  let make = (~rating, ~onChange, _children) => {
    ...component,
    render: (_) => {
      <div>
        (Array.mapi((i, text) => {
          <div
            key=(string_of_int(i))
            className=(Some(i) == rating ? Styles.activeButton : Styles.button)
            onClick=((_) => Some(i) == rating ? onChange(None) : onChange(Some(i)))
          >
            (str(text))
          </div>
        }, ratings) |> ReasonReact.arrayToElement)
      </div>
    }
  };
};

let module Form = {
  let module Styles = {
    open Glamor;
    let label = css([
      fontSize("20px"),
      fontWeight("400"),
    ]);
    let title = css([
      fontSize("32px"),
      borderBottom("2px solid " ++ Shared.dark),
      fontFamily(Shared.titleFont),
    ]);
    let container = css([
        padding("16px"),
        border("2px solid #eee")
    ]);

  };

  type state = {
    notes: string,
    rating: option(int),
    meta: Models.meta,

    /*
    ingredients: array(Models.maybeRecipeIngredient),
    ingredientHeaders: array(Models.header),

    instructions: array(Models.maybeRecipeIngredient),
    instructionHeaders: array(Models.header),

    images: array(string),
    imageUrl: Js.null(string),

    created: float,
    */
  };

  type action =
    | SetText(string)
    | SetRating(option(int))
    ;
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let makeFullMadeIt = (~recipe, ~state as {notes, rating, meta}): Models.madeIt => {
    "id": BaseUtils.uuid(),
    "recipeId": recipe##id,
    "authorId": recipe##authorId, /** TODO different */
    "collaborators": recipe##collaborators,
    "isPrivate": recipe##isPrivate,
    "created": Js.Date.now(),
    "updated": Js.Date.now(),
    "imageUrl": Js.null,
    "images": [||],
    "instructions": recipe##instructions,
    "instructionHeaders": recipe##instructionHeaders,
    "ingredients": recipe##ingredients,
    "ingredientHeaders": recipe##ingredientHeaders,
    "ateWithRecipes": [||],
    "batches": 1.,
    "meta": meta,
    "notes": notes,
    "rating": Js.Null.from_opt(rating)
  };

  let make = (~recipe: Models.recipe, ~onCancel, ~onSave, _children) => {
    ...component,
    initialState: (_) => {
      notes: "",
      /*
      ingredients: Array.map(Models.maybeRecipeIngredient,recipe##ingredients),
      ingredientHeaders: recipe##ingredientHeaders,
      images: [||],
      */
      rating: None,
      meta: recipe##meta
    },
    reducer: (action, state) => switch(action) {
    | SetText(notes) => ReasonReact.Update({...state, notes})
    | SetRating(rating) => ReasonReact.Update({...state, rating})
    },
    render: ({reduce, state: {notes, rating} as state}) => {
      <div className=Styles.container>
        <input/>
        <div className=Styles.label> (str("Rating")) </div>
        (spacer(16))
        <RatingWidget
          rating
          onChange=(reduce ((rating) => SetRating(rating)))
        />
        (spacer(32))
        <div className=Styles.label> (str("Notes")) </div>
        (spacer(16))
        <Textarea
          value=notes
          onChange=(reduce ((text) => SetText(text)))
        />
        (spacer(32))
        <div className=RecipeStyles.row>
        <button
          onClick=((_) => onSave(makeFullMadeIt(~recipe, ~state)))
          className=RecipeStyles.primaryButton
        >
          (str("Save"))
        </button>
        <button
          onClick=((_) => onCancel())
          className=RecipeStyles.button
        >
          (str("Cancel"))
        </button>
        </div>
      </div>
    }
  };

};

let module Adder = {
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let make = (~recipe, ~onAdd, _children) => {
    ...component,
    initialState: (_) => false,
    reducer: ((), state) => ReasonReact.Update(!state),
    render: ({state, reduce}) => {
      <div className=Glamor.(css([
      ]))>
      (state
      ?
        <Form
          recipe
          onCancel=(reduce (() => ()))
          onSave=(reduce((madeIt) => {
            Js.log("made");
            onAdd(madeIt);
            ()
          }))
        />
      :
        <div className=Glamor.(css([
          flexDirection("row"), alignItems("flex-start"),
          justifyContent("flex-start"),
        ]))>
          <button
            className=RecipeStyles.primaryButton
            onClick=(reduce ((_) => ()))
          >
            (str("Record experience"))
          </button>
        </div>)
      </div>
    }
  }
};
