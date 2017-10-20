
type nu 't = Js.null 't;
type parseResult = Js.t {.
  title: string,
  description: string,
  ingredients: array Models.recipeIngredient,
  instructions: array Models.instruction,
  meta: Models.meta
};

external importRecipe: string => Js.Promise.t parseResult = "" [@@bs.module "js/importRecipe.js"];


let fillOutRecipe url fb allIngredients recipe => {
  {
    "title": recipe##title,
    "source": url,
    "instructions": recipe##instructions,
    "ingredients": recipe##ingredients |> Array.map (PasteUtils.matchIngredient allIngredients),
    "description": recipe##description,
    "meta": recipe##meta,
  }
};


let component = ReasonReact.reducerComponent "ImportInput";

let make ::fb ::allIngredients ::onImport _ => ReasonReact.{
  ...component,
  initialState: fun () => None,
  reducer: fun action _ => ReasonReact.Update action,
  render: fun {state, reduce} => {
    <input
      value=(switch state { | None => "" | Some v => v})
      disabled=(Js.Boolean.to_js_boolean (state !== None))
      placeholder="Paste in URL to import"
      onChange=(fun evt => {
        let url = Utils.evtValue evt;
        (reduce (fun _ => Some url)) ();
        importRecipe url
        |> Js.Promise.then_ (fun recipe => {
          (reduce (fun _ => None)) ();
          onImport (fillOutRecipe url fb (Array.of_list allIngredients) recipe);
          Js.Promise.resolve ();
        })
        |> Js.Promise.catch (fun err => {
          (reduce (fun _ => None)) ();
          Js.log2 "Failed to import" err;
          Js.Promise.resolve ();
        }) |> ignore;
      })
    />
  }
}
