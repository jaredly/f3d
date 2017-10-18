
open Utils;

let ingredientsMap list => {
  let map = Js.Dict.empty ();
  List.iter
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
    maxWidth "50vw",
    color "#777",
  ];
};

let orr default value => switch value { | None => default | Some value => value };

let render ::batches ::ingredients ::allIngredients => {
  let map = ingredientsMap allIngredients;
  <table className=Glamor.(css[
    maxWidth "100%",
  ])>
  <tbody>
  (Array.mapi
    (fun i ingredient => {
      let ing = Js.Dict.get map ingredient##ingredient;
      let name = switch ing {
      | None => <div className=Styles.unknownName>(str "Unknown ingredient")</div>
      | Some ing => <div>(str ing##name)</div>
      };
      <tr key=(string_of_int i) className=Glamor.(css[ ])>
        <td className=Glamor.(css[textAlign "right"])>
          (maybe ingredient##amount (fun amount => {
            (str @@ fractionify (amount *. batches))
          }) |> orr ReasonReact.nullElement)
        </td>
        <td>
          (maybe ingredient##unit (fun unit => {
            (str @@ smallUnit unit)
          }) |> orr ReasonReact.nullElement)
        </td>
        <td className=Glamor.(css[width "8px"]) />
        <td>
          <div className=Glamor.(css[flexDirection "row"])>
            (name)
            (spacer 16)
            (maybe ingredient##comments (fun comment => {
              <div className=Styles.comment>
                (str comment)
              </div>
            }) |> orr ReasonReact.nullElement)
          </div>
        </td>
      </tr>
    })
  ingredients
  /* |> Array.mapi (fun i row => <tr key={string_of_int i}>row</tr>) */
  |> spacedArray (fun i => <tr
    key=(string_of_int i ^ "s") 
    className=Glamor.(css[height "8px"])
  />)
  |> ReasonReact.arrayToElement)
  </tbody>
  </table>
};


