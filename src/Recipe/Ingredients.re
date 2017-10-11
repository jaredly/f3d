
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

let render ::batches ::ingredients ::allIngredients => {
  let map = ingredientsMap allIngredients;
  <div className=Glamor.(css[
  ])>
  (Array.mapi
    (fun i ingredient => {
      let ing = Js.Dict.get map ingredient##ingredient;
      let name = switch ing {
      | None => <div className=Styles.unknownName>(str "Unknown ingredient")</div>
      | Some ing => <div>(str ing##name)</div>
      };
      let items = [
        maybe ingredient##amount (fun amount => {
          (str @@ fractionify amount)
        }),
        (nonnull ingredient##amount && nonnull ingredient##unit)
        ? Some (spacer 4)
        : None,
        maybe ingredient##unit (fun unit => {
          (str @@ smallUnit unit)
        }),
        (nonnull ingredient##amount || nonnull ingredient##unit)
        && (nonnull ingredient##comments)
        ? Some (spacer 16)
        : None,
        maybe ingredient##comments (fun comment => {
          <div className=Glamor.(css [fontStyle "italic"])>
            (str comment)
          </div>
        })
      ];
      <div key=(string_of_int i) className=Glamor.(css[
        flexDirection "row",
      ])>
        (name)
        (spacer 16)
        (items
          |> filterMaybe
          |> Array.of_list
          |> ReasonReact.arrayToElement)
      </div>
    })
  ingredients
  |> spacedArray (fun i => spacer key::(string_of_int i ^ "s") 24)
  |> ReasonReact.arrayToElement)
  </div>
};

/** Tried this table, didn't like it as much */
let render' ::batches ::ingredients ::allIngredients => {
  let map = ingredientsMap allIngredients;
  <table className=Glamor.(css[
  ])>
  <tbody>
  (Array.mapi
    (fun i ingredient => {
      let ing = Js.Dict.get map ingredient##ingredient;
      let name = switch ing {
      | None => <td className=Styles.unknownName>(str "Unknown ingredient")</td>
      | Some ing => <td>(str ing##name)</td>
      };
      let items = [
        maybe ingredient##amount (fun amount => {
          (str @@ fractionify amount)
        }),
        (nonnull ingredient##amount && nonnull ingredient##unit)
        ? Some (spacer 4)
        : None,
        maybe ingredient##unit (fun unit => {
          (str @@ smallUnit unit)
        }),

      ];
      <tr key=(string_of_int i) className=Glamor.(css[ ])>
        (name)
        <td className=Glamor.(css[width "16px"]) />
        <td>
          <div className=Glamor.(css[flexDirection "row"])>
          (items
            |> filterMaybe
            |> Array.of_list
            |> ReasonReact.arrayToElement)
          </div>
        </td>
        <td className=Glamor.(css[width "16px"]) />
        <td>
          (maybe ingredient##comments (fun comment => {
            <div className=Glamor.(css [fontStyle "italic"])>
              (str comment)
            </div>
          }) |> (fun item => switch item {
          | None => ReasonReact.nullElement
          | Some item => item
          }))
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

let orr default value => switch value { | None => default | Some value => value };

let render ::batches ::ingredients ::allIngredients => {
  let map = ingredientsMap allIngredients;
  <table className=Glamor.(css[
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
            (str @@ fractionify amount)
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
    className=Glamor.(css[height "16px"])
  />)
  |> ReasonReact.arrayToElement)
  </tbody>
  </table>
};


