open Utils;

type state = {
  text: string,
  isOpen: bool,
  selection: int,
};
type action = 
  | Open
  | Close
  | SetText string
  ;

let component = ReasonReact.reducerComponentWithRetainedProps "AmountInput";

let module Styles = {
  open Glamor;
  let container = css [
    position "relative"
  ];
  
  let input = css [

  ];

  let list = css [
    position "absolute",
    zIndex "1000",
    top "100%",
    marginTop "5px",
    boxShadow "0 1px 8px #aaa",
    borderRadius "4px",
    backgroundColor "white",
    left "0",
    right "0",
  ];

  let result = css [
    cursor "pointer",
    padding "4px 8px",
    Selector ":hover" [
      backgroundColor "#eee",
    ],
  ];

  let chevron = css [
    Property "pointer-events" "none",
    position "absolute",
    right "4px",
    top "4px",
  ];
};

let showResults ::map ::text ::onSelect => {
  let text = Js.String.toLowerCase text;
  let names = Js.Dict.keys map
  |> Array.map (fun id => Js.Dict.unsafeGet map id)
  |> Js.Array.filter
  (fun ing => ing##name
  |> Js.String.toLowerCase
  |> Js.String.includes text);
  <div className=Styles.list>
    (Array.map
    (fun ing => (
      <div className=Styles.result onMouseDown=(fun evt => {
        ReactEventRe.Mouse.preventDefault evt;
        onSelect ing
      })>
        (str ing##name)
      </div>
    ))
    names
    |> ReasonReact.arrayToElement)
  </div>
};

let make ::ingredientsMap ::value ::onChange ::className=? _children => ReasonReact.{
  ...component,
  initialState: fun _ => {
    text: Js.Dict.get ingredientsMap value |> optMap (fun ing => ing##name) |> optOr "",
    isOpen: false,
    selection: 0,
  },
  retainedProps: value,
  reducer: fun action {text, isOpen, selection} => switch action {
  | Open => ReasonReact.Update {text, isOpen: true, selection}
  | Close => ReasonReact.Update {text, isOpen: false, selection}
  | SetText text => ReasonReact.Update {text, isOpen: true, selection}
  },
  willReceiveProps: fun {retainedProps, reduce, state} => {
    if (retainedProps != value) {
      {
        text: Js.Dict.get ingredientsMap value |> optMap (fun ing => ing##name) |> optOr "",
        isOpen: false,
        selection: 0,
      }
    } else {
      state
    }
  },

  render: fun {state: {text, selection, isOpen}, reduce} => {
    <div className=Styles.container>
      <input
        value=text
        className=Styles.input
        onChange=(reduce (fun evt => SetText (Utils.evtValue evt)))
        onBlur=(reduce (fun _ => Close))
        onFocus=(reduce (fun evt => {
          let obj = (ReactDOMRe.domElementToObj (ReactEventRe.Focus.target evt));
          obj##select() |> ignore;
          Open
        }))
      />
      <div className=Styles.chevron>
        (str {j|▼|j})
      </div>
      (isOpen
      ? showResults map::ingredientsMap ::text onSelect::(fun ing => {
        (reduce (fun _ => Close)) ();
        onChange ing##id
      })
      : ReasonReact.nullElement)
    </div>
  }
}
