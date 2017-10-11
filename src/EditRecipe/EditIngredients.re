
open Utils;

let ingredientsMap list => {
  let map = Js.Dict.empty ();
  Array.iter
  (fun ing => Js.Dict.set map ing##id ing)
  list;
  map
};

let module Styles = {
  open Glamor;
  let name = css [

  ];
  let unknownName = css [
    fontStyle "italic",
  ];
  let comment = css [
    fontStyle "italic",
    color "#777",
  ];
};

let clone: Js.t 'a => Js.t 'a = fun obj => {
  Js.Obj.assign (Js.Obj.empty ()) obj
};

let render ::ingredients ::allIngredients ::onChange => {

  let setUnit i (ingredient: Models.recipeIngredient) (value: string) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##unit #= (value === "" ? Js.null : Js.Null.return value);
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let setComments i (ingredient: Models.recipeIngredient) (value: string) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##comments #= (value === "" ? Js.null : Js.Null.return value);
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let setAmount i (ingredient: Models.recipeIngredient) (value: option float) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##amount #= (Js.Null.from_opt value);
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let map = ingredientsMap allIngredients;
  <table className=Glamor.(css[
  ])>
  <tbody>
  (Array.mapi
    (fun i ingredient => {
      <tr key=(string_of_int i) className=Glamor.(css[ ])>
        <td>
          <AmountInput
            value=(ingredient##amount |> Js.Null.to_opt)
            onChange=(fun value => setAmount i ingredient value)
          />
        </td>
        <td>
          <input
            value=(ingredient##unit |> Js.Null.to_opt |> optOr "")
            onChange=(fun evt => setUnit i ingredient (evtValue evt))
          />
        </td>
        <td className=Glamor.(css[width "16px"]) />
          {
            let ing = Js.Dict.get map ingredient##ingredient;
            switch ing {
            | None => <td className=Styles.unknownName>(str "Unknown ingredient")</td>
            | Some ing => <td>(str ing##name)</td>
            }
          }
        <td className=Glamor.(css[width "16px"]) />
        <td>
          <input
            value=(ingredient##comments |> Js.Null.to_opt |> optOr "")
            onChange=(fun evt => setComments i ingredient (evtValue evt))
          />
        </td>
      </tr>
    })
  ingredients
  /* |> Array.mapi (fun i row => <tr key={string_of_int i}>row</tr>) */
  |> spacedArray (fun i => <tr
    key=(string_of_int i ^ "s") 
    className=Glamor.(css[height "16px"])
  />)
  |> ReasonReact.arrayToElement)
  </tbody>
  </table>
};
