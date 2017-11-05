open Utils;

let module Form = {
  type state = {
    notes: string,
    ingredients: array(Models.maybeRecipeIngredient),
    ingredientHeaders: array(Models.header),
    images: array(string),
    meta: Models.meta,
    rating: option(int),
  };
  type action =
    | SetText(string)
    ;
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let make = (~recipe: Models.recipe, ~onCancel, _children) => {
    ...component,
    initialState: (_) => {
      notes: "",
      ingredients: Array.map(Models.maybeRecipeIngredient,recipe##ingredients),
      ingredientHeaders: recipe##ingredientHeaders,
      images: [||],
      rating: None,
      meta: recipe##meta
    },
    reducer: (action, state) => switch(action) {
    | SetText(notes) => ReasonReact.Update({...state, notes})
    },
    render: ({reduce, state}) => {
      <div>
        <Textarea
          value=state.notes
          onChange=(reduce ((text) => SetText(text)))
        />
        <button
          onClick=((_) => onCancel())
          className=RecipeStyles.button
        >
          (str("Cancel"))
        </button>
      </div>
    }
  };

};

let module Adder = {
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let make = (~recipe, _children) => {
    ...component,
    initialState: (_) => false,
    reducer: ((), state) => ReasonReact.Update(!state),
    render: ({state, reduce}) => {
      <div className=Glamor.(css([
        marginBottom("64px"),
      ]))>
      (state
      ?
        <Form
          recipe
          onCancel=(reduce (() => ()))
        />
      :
        <div className=Glamor.(css([
          flexDirection("row"), alignItems("center"),
          justifyContent("center"),
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
