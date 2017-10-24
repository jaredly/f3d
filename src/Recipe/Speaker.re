
open BaseUtils;

let component = ReasonReact.reducerComponent "Speaker";

let make ::instructions ::allIngredients ::ingredients _ => ReasonReact.{
  ...component,
  initialState: fun () => false,
  reducer: fun action _ => ReasonReact.Update action,
  render: fun {state: speaking, reduce} => {
    let map = Ingredients.ingredientsMap allIngredients;
    <button
      disabled=(Js.Boolean.to_js_boolean speaking)
      onClick=(fun _ => {
        reduce (fun _ => true) ();
        SpeechSynthesis.speak
        instructions.(0)##text
        /* (SpeechSynthesis.fullIngredientText map ingredients.(0)) */
        /* (Array.map
        (SpeechSynthesis.ingredientText map)
        ingredients) */
        (fun _ => (reduce (fun _ => false))())
      })
      className=Glamor.(css[
        backgroundColor "transparent",
        border "none",
        cursor "pointer",
        fontSize "inherit",
        fontFamily "inherit",
        fontWeight "inherit",
        Selector ":hover" [
          backgroundColor "#eee",
        ]
      ])
    >
      (Utils.str "Speak")
    </button>
  }
};
