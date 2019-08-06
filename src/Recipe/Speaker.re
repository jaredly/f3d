open BaseUtils;

let component = ReasonReact.reducerComponent("Speaker");

[@react.component] let make = (~instructions, ~allIngredients, ~ingredients, _) =>
  ReactCompat.useRecordApi(ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state: speaking, send}) => {
      let map = Ingredients.ingredientsMap(allIngredients);
      <button
        disabled=((speaking))
        onClick=(
          (_) => {
            send(true);
            SpeechSynthesis.speak(
              instructions[0]##text,
              /* (SpeechSynthesis.fullIngredientText map ingredients.(0)) */
              /* (Array.map
                 (SpeechSynthesis.ingredientText map)
                 ingredients) */
              (_) => (send(false))
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
  });
