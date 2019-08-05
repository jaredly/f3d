open Utils;

module Styles = {
  open Glamor;
  let container = css([]);
  let instruction =
    css([
      fontSize("20px"),
      /* lineHeight "32px", */
      letterSpacing("1px"),
      flexDirection("row")
    ]);
  let instructionText =
    css([
      fontSize("20px"),
      /* lineHeight "20px", */
      letterSpacing("1px"),
      padding("4px 8px"),
      border("none"),
      borderBottom("1px solid #aaa")
    ]);
  let textContainer = css([flex("1")]);
};

let clone: Js.t('a) => Js.t('a) = (obj) => Js.Obj.assign(Js.Obj.empty(), obj);

let blankInstruction: Models.instruction = {"text": "", "ingredientsUsed": Js.Dict.empty()};

let render = (~instructions, ~instructionHeaders, ~onChange) => {
  let setText = (i, instruction: Models.instruction, value: string) => {
    if (i < Array.length(instructions) || value !== "") {
      let instruction = Obj.magic(clone(instruction));
      instruction##text#=value;
      let instructions = Array.copy(instructions);
      if (i >= Array.length(instructions)) {
        Js.Array.push(instruction, instructions) |> ignore
      } else {
        instructions[i] = instruction
      };
      onChange((instructionHeaders, instructions))
    }
  };
  let addInstruction = (i, value: string) => {
    let instructions = Array.copy(instructions);
    Js.Array.spliceInPlace(
      ~pos=i,
      ~remove=0,
      ~add=[|{"text": value, "ingredientsUsed": Js.Dict.empty()}|],
      instructions
    )
    |> ignore;
    onChange((instructionHeaders, instructions))
  };
  let removeToPrev = (i, value: string) => {
    let instruction = Obj.magic(clone(instructions[i - 1]));
    instruction##text#=(instruction##text ++ (" " ++ value));
    let instructions = Array.copy(instructions);
    instructions[i - 1] = instruction;
    Js.Array.spliceInPlace(~pos=i, ~remove=1, ~add=[||], instructions) |> ignore;
    onChange((instructionHeaders, instructions))
  };
  let removeFirst = () => {
    let instructions = Js.Array.sliceFrom(1, instructions);
    onChange((instructionHeaders, instructions))
  };
  <div className=Styles.container>
    (
      Array.mapi(
        (i, instruction) =>
          <div key=(string_of_int(i)) className=Styles.instruction>
            (str(string_of_int(i + 1) ++ ". "))
            (spacer(4))
            <BlurryInput
              render=(
                (~value, ~onChange as onChangeInner, ~onBlur) =>
                  <Textarea
                    className=Styles.instructionText
                    containerClassName=Styles.textContainer
                    onChange=onChangeInner
                    onPaste=(
                      PasteUtils.parseInstructions(
                        (instructions) => onChange(([||], instructions))
                      )
                    )
                    onReturn=(
                      (pre, post) => {
                        onChangeInner(Js.String.trim(pre));
                        addInstruction(i + 1, Js.String.trim(post))
                      }
                    )
                    onDelete=(
                      (text) => {
                        if (i > 0) {
                          removeToPrev(i, text)
                        } else {
                          removeFirst();
                        }
                      }
                    )
                    value
                    onBlur
                  />
              )
              value=instruction##text
              onChange=((value) => setText(i, instruction, value))
            />
          </div>,
        Js.Array.concat([|blankInstruction|], instructions)
      )
      |> spacedArray((i) => spacer(~key=string_of_int(i) ++ "s", 16))
      |> ReasonReact.array
    )
  </div>
};
