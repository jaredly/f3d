
open BaseUtils;

let component = ReasonReact.reducerComponent "Speaker";

let make ::allIngredients ::ingredients _ => ReasonReact.{
  ...component,
  initialState: fun () => false,
  reducer: fun action _ => ReasonReact.Update action,
  render: fun {state: speaking, reduce} => {
    let map = Ingredients.ingredientsMap allIngredients;
    <button
      /* disabled=(Js.Boolean.to_js_boolean speaking) */
      onClick=(fun _ => {
        reduce (fun _ => true) ();
        SpeechSynthesis.speakSeveral
        (Array.map
        (SpeechSynthesis.ingredientText map)
        ingredients)
        (fun _ => (reduce (fun _ => false))())
      })
    >
      (Utils.str "Speak")
    </button>
  }
};
