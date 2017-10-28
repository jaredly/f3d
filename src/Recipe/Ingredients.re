open Utils;

open BaseUtils;

let ingredientsMap = (list) => {
  let map = Js.Dict.empty();
  List.iter((ing) => Js.Dict.set(map, ing##id, ing), list);
  map
};

module Styles = {
  open Glamor;
  let name = css([]);
  let unknownName = css([fontStyle("italic")]);
  let comment = css([fontStyle("italic"), flex("1"), color("#777")]);
};

let orr = (default, value) =>
  switch value {
  | None => default
  | Some(value) => value
  };

let render = (~batches, ~ingredients, ~allIngredients, ~making) => {
  let map = ingredientsMap(allIngredients);
  <table className=Glamor.(css([maxWidth("100%")]))>
    <tbody>
      (
        Array.mapi(
          (i, ingredient) => {
            let ing = Js.Dict.get(map, ingredient##ingredient);
            let name =
              switch ing {
              | None => <div className=Styles.unknownName> (str("Unknown ingredient")) </div>
              | Some(ing) => <div> (str(ing##name)) </div>
              };
            <tr
              key=(string_of_int(i))
              className=Glamor.(css([verticalAlign("top")]))
              onClick=?(
                switch making {
                | None => None
                | Some((set, onChange)) =>
                  let checked = StringSet.mem(ingredient##id, set);
                  Some(
                    (
                      (_) =>
                        onChange(
                          checked ?
                            StringSet.remove(ingredient##id, set) :
                            StringSet.add(ingredient##id, set)
                        )
                    )
                  )
                }
              )>
              (
                switch making {
                | None => ReasonReact.nullElement
                | Some((set, onChange)) =>
                  let checked = StringSet.mem(ingredient##id, set);
                  <td
                    onClick=(
                      (_) =>
                        onChange(
                          checked ?
                            StringSet.remove(ingredient##id, set) :
                            StringSet.add(ingredient##id, set)
                        )
                    )>
                    <input _type="checkbox" checked=(Js.Boolean.to_js_boolean(checked)) />
                  </td>
                }
              )
              <td
                className=Glamor.(
                            css([
                              textAlign("right"),
                              fontFamily("Source Sans Pro"),
                              whiteSpace("nowrap"),
                              fontWeight("200")
                            ])
                          )>
                (
                  maybe(ingredient##amount, (amount) => str @@ fractionify(amount *. batches))
                  |> orr(ReasonReact.nullElement)
                )
              </td>
              <td>
                (
                  maybe(ingredient##unit, (unit) => str @@ smallUnit(unit))
                  |> orr(ReasonReact.nullElement)
                )
              </td>
              <td className=Glamor.(css([width("8px")])) />
              <td>
                <div className=Glamor.(css([flexDirection("row")]))>
                  name
                  (spacer(16))
                  (
                    maybe(
                      ingredient##comments,
                      (comment) => <div className=Styles.comment> (str(comment)) </div>
                    )
                    |> orr(ReasonReact.nullElement)
                  )
                </div>
              </td>
            </tr>
          },
          ingredients
        )
        /* |> spacedArray (fun i => <tr
             key=(string_of_int i ^ "s")
             className=Glamor.(css[height "8px"])
           />) */
        |> ReasonReact.arrayToElement
      )
    </tbody>
  </table>
};
