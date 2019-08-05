open Utils;

open BaseUtils;

module Styles = {
  open Glamor;
  let container = css([]);
  let instruction =
    css
      ([fontSize("20px"), lineHeight("32px"), whiteSpace("pre-wrap"), flexDirection("row")]);
      /* letterSpacing "1px", */
};

let render = (~instructions, ~making) =>
  <div className=Styles.container>
    (
      Array.mapi(
        (i, instruction) =>
          <div
            key=(string_of_int(i))
            className=Styles.instruction
            onClick=?(
              switch making {
              | None => None
              | Some((set, onChange)) =>
                let checked = IntSet.mem(i, set);
                Some(((_) => onChange(checked ? IntSet.remove(i, set) : IntSet.add(i, set))))
              }
            )>
            (
              switch making {
              | None => ReasonReact.null
              | Some((set, _onChange)) =>
                let checked = IntSet.mem(i, set);
                <div
                  style=(
                    ReactDOMRe.Style.make(
                      ~height="1.5em",
                      ~alignItems="center",
                      ~justifyContent="center",
                      ()
                    )
                  )>
                  <input
                    type_="checkbox"
                    style=(ReactDOMRe.Style.make(~width="1em", ()))
                    checked=((checked))
                  />
                </div>
              }
            )
            (str(string_of_int(i + 1) ++ ". "))
            (str(instruction##text))
          </div>,
        instructions
      )
      |> spacedArray((i) => spacer(~key=string_of_int(i) ++ "s", 16))
      |> ReasonReact.array
    )
  </div>;
