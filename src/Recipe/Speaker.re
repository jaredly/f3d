open BaseUtils;

let component = ReasonReact.reducerComponent("Speaker");

let make = (~instructions, ~allIngredients, ~ingredients, _) =>
  ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state: speaking, reduce}) => {
      let map = Ingredients.ingredientsMap(allIngredients);
      <button
        disabled=(bool.to_js_boolean(speaking))
        onClick=(
          (_) => {
            reduce((_) => true, ());
            SpeechSynthesis.speak(
              instructions[0]##text,
              /* (SpeechSynthesis.fullIngredientText map ingredients.(0)) */
              /* (Array.map
                 (SpeechSynthesis.ingredientText map)
                 ingredients) */
              (_) => (reduce((_) => false))()
            )
          }
        )
        className=Glamor.(
                    css([
                      backgroundColor("transparent"),
                      border("none"),
                      cursor("pointer"),
                      fontSize("inherit"),
                      fontFamily("inherit"),
                      fontWeight("inherit"),
                      Selector(":hover", [backgroundColor("#eee")])
                    ])
                  )>
        (Utils.str("Speak"))
      </button>
    }
  };
