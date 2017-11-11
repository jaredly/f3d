open Utils;

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
    created: MomentRe.Moment.t,

    /*
    ingredients: array(Models.maybeRecipeIngredient),
    ingredientHeaders: array(Models.header),

    instructions: array(Models.maybeRecipeIngredient),
    instructionHeaders: array(Models.header),

    images: array(string),
    imageUrl: Js.null(string),

    */
  };

  type action =
    | SetText(string)
    | SetRating(option(int))
    | SetCreated(MomentRe.Moment.t)
    ;
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let makeFullMadeIt = (~recipe, ~state as {notes, rating, meta, created}): Models.madeIt => {
    "id": BaseUtils.uuid(),
    "recipeId": recipe##id,
    "authorId": recipe##authorId, /** TODO different */
    "collaborators": recipe##collaborators,
    "isPrivate": recipe##isPrivate,
    "created": MomentRe.Moment.valueOf(created),
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
      created: MomentRe.momentNow(),
      rating: None,
      meta: recipe##meta
    },
    reducer: (action, state) => switch(action) {
    | SetText(notes) => ReasonReact.Update({...state, notes})
    | SetRating(rating) => ReasonReact.Update({...state, rating})
    | SetCreated(created) => ReasonReact.Update({...state, created})
    },

    render: ({reduce, state: {notes, rating, created} as state}) => {
      <div className=Styles.container>
        <div className=RecipeStyles.row>
          <div className=Styles.label> (str("Date")) </div>
          (spacer(16))
          <DatePicker
            selected=created
            onChange=reduce((time) => SetCreated(time))
            placeholderText="Awesome"
          />
        </div>
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
          (str("Add"))
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
    initialState: (_) => true,
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
