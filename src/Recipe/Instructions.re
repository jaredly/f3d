
open Utils;

let module Styles = {
  open Glamor;
  let container = css [];
  let instruction = css [
    fontSize "20px",
    lineHeight "32px",
    whiteSpace "pre-wrap",
    letterSpacing "1px",
  ];
};

let render ::instructions => {
  <div className=Styles.container>
    (Array.mapi
      (fun i instruction =>
        <div key=(string_of_int i) className=Styles.instruction>
          (str (string_of_int (i + 1) ^ ". "))
          (str instruction##text)
        </div>
      )
    instructions
    |> spacedArray (fun i => spacer key::(string_of_int i ^ "s") 16)
    |> ReasonReact.arrayToElement)
  </div>
};