
open Utils;
open BaseUtils;

let module Styles = {
  open Glamor;
  let container = css [];
  let instruction = css [
    fontSize "20px",
    lineHeight "32px",
    whiteSpace "pre-wrap",
    flexDirection "row",
    /* letterSpacing "1px", */
  ];
};

let render ::instructions ::making => {
  <div className=Styles.container>
    (Array.mapi
      (fun i instruction =>
        <div
          key=(string_of_int i)
          className=Styles.instruction
          onClick=?(
            switch making {
            | None => None
            | Some (set, onChange) => {
                let checked = IntSet.mem i set;
                Some (
                  fun _ => onChange (checked ? (IntSet.remove i set) : (IntSet.add i set))
                )
              }
            }
          )
        >
          (switch making {
          | None => ReasonReact.nullElement
          | Some (set, onChange) => {
            let checked = IntSet.mem i set;
            <div
              style=(ReactDOMRe.Style.make height::"1.5em" alignItems::"center" justifyContent::"center" ())
            >
            <input
              _type="checkbox"
              style=(ReactDOMRe.Style.make width::"1em" ())
              checked=(Js.Boolean.to_js_boolean checked)
            />
            </div>
          }})
          (str (string_of_int (i + 1) ^ ". "))
          (str instruction##text)
        </div>
      )
    instructions
    |> spacedArray (fun i => spacer key::(string_of_int i ^ "s") 16)
    |> ReasonReact.arrayToElement)
  </div>
};