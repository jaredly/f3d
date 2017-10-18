
open Utils;

let module BlurryInput = {
  let component = ReasonReact.reducerComponentWithRetainedProps "BlurryInput";
  let make ::value ::onChange ::render _children => ReasonReact.{
    ...component,
    initialState: fun () => value,
    retainedProps: value,
    willReceiveProps: fun {retainedProps, state} => {
      if (retainedProps !== value && value !== state) {
        value
      } else {
        state
      }
    },
    reducer: fun action state => ReasonReact.Update action,
    render: fun {state, reduce} => {
      render
        value::state
        onChange::(reduce (fun text => text))
        onBlur::(fun _ => onChange state)
    }
  }
};

let module Styles = {
  open Glamor;
  let container = css [];
  let instruction = css [
    fontSize "20px",
    /* lineHeight "32px", */
    letterSpacing "1px",
    flexDirection "row",
  ];
  let instructionText = css [
    fontSize "20px",
    /* lineHeight "20px", */
    letterSpacing "1px",
    padding "4px 8px",
    border "none",
    borderBottom "1px solid #aaa",
  ];
  let textContainer = css [
    flex "1",
  ];
};

let clone: Js.t 'a => Js.t 'a = fun obj => {
  Js.Obj.assign (Js.Obj.empty ()) obj
};

let render ::instructions ::onChange => {

  let setText i (value: string) => {
    let instruction = Obj.magic (clone instructions.(i));
    instruction##text #= value;
    let instructions = Array.copy instructions;
    instructions.(i) = instruction;
    onChange instructions;
  };

  let addInstruction i (value: string) => {
    let instructions = Array.copy instructions;
    Js.Array.spliceInPlace pos::i remove::0 add::[|
    {"text": value, "ingredientsUsed": Js.Dict.empty ()}
    |] instructions |> ignore;
    onChange instructions;
  };

  let removeToPrev i (value: string) => {
    let instruction = Obj.magic (clone instructions.(i - 1));
    instruction##text #= (instruction##text ^ " " ^ value);
    let instructions = Array.copy instructions;
    instructions.(i - 1) = instruction;
    Js.Array.spliceInPlace pos::i remove::1 add::[||] instructions |> ignore;
    onChange instructions;
  };

  <div className=Styles.container>
    (Array.mapi
      (fun i instruction =>
        <div key=(string_of_int i) className=Styles.instruction>
          (str (string_of_int (i + 1) ^ ". "))
          (spacer 4)
          <BlurryInput
            render=(fun ::value ::onChange ::onBlur => {
              <Textarea
                className=Styles.instructionText
                containerClassName=Styles.textContainer
                onChange
                onReturn=(fun pre post => {
                  onChange (Js.String.trim pre);
                  addInstruction (i + 1) (Js.String.trim post);
                })
                onDelete=(fun text => {
                  [%guard let true = i > 0][@else ()];
                  removeToPrev i text;
                })
                value
                onBlur
              />
            })
            value=instruction##text
            onChange=(fun value => setText i value)
          />
        </div>
      )
    instructions
    |> spacedArray (fun i => spacer key::(string_of_int i ^ "s") 16)
    |> ReasonReact.arrayToElement)
  </div>
};
