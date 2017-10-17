open Utils;

let module Styles = {
  open Glamor;
  let container = css [
    position "relative"
  ];

  let input = css [
  ];

  let badInput = css [
    backgroundColor "#fee",
    Selector ":focus" [
      backgroundColor "white",
    ]
  ];

  let list = css [
    position "absolute",
    maxHeight "400px",
    overflow "auto",
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

  let selectedResult = css [
    backgroundColor "#eee",
  ];

  let chevron = css [
    Property "pointerEvents" "none",
    position "absolute",
    right "5px",
    top "6px",
    color "#aaa",
    fontSize "10px",
  ];
};

let getResults ::map ::text => {
  let text = Js.String.toLowerCase text;
  Js.Dict.keys map
  |> Array.map (fun id => Js.Dict.unsafeGet map id)
  |> Js.Array.filter
  (fun ing => ing##name
  |> Js.String.toLowerCase
  |> Js.String.includes text);
};

let showResults ::results ::selection ::onSelect => {
  <div className=Styles.list>
    (Array.mapi
    (fun i ing => (
      <div
        key=(string_of_int i)
        className=(Styles.result ^ " " ^ (selection == i ? Styles.selectedResult : ""))
        onMouseDown=(fun evt => {
          ReactEventRe.Mouse.preventDefault evt;
          onSelect ing
        })
      >
        (str ing##name)
      </div>
    ))
    results
    |> ReasonReact.arrayToElement)
  </div>
};

type state = {
  text: string,
  isOpen: bool,
  results: array Models.ingredient,
  selection: int,
};
type action = 
  | Open
  | Close
  | SetText string
  | GoUp
  | GoDown
  ;

let selectUp selection len => selection <= 0 ? len - 1 : selection - 1;
let selectDown selection len => selection + 1 >= len ? 0 : selection + 1;

let component = ReasonReact.reducerComponentWithRetainedProps "AmountInput";

let getText value ingredientsMap => {
  switch value {
    | Models.Text text => text
    | Models.Id id => Js.Dict.get ingredientsMap id |> optMap (fun ing => ing##name) |> optOr "";
  }
};

let make ::ingredientsMap ::value ::onChange ::className=? _children => ReasonReact.{
  ...component,
  initialState: fun _ => {
    let text = switch value {
    | Models.Text text => text
    | Models.Id id => Js.Dict.get ingredientsMap id |> optMap (fun ing => ing##name) |> optOr "";
    };
    {
      text,
      results: getResults map::ingredientsMap ::text,
      isOpen: false,
      selection: 0,
    }
  },
  retainedProps: value,
  reducer: fun action ({text, isOpen, selection, results} as state) => switch action {
  | Open => ReasonReact.Update {...state, isOpen: true}
  | Close => ReasonReact.Update {...state, isOpen: false}
  | SetText text => ReasonReact.Update {...state, isOpen: true, text, results: getResults map::ingredientsMap ::text}
  | GoUp => ReasonReact.Update {...state, isOpen: true, selection: isOpen ? (selectUp selection (Array.length results)) : Array.length results - 1}
  | GoDown => ReasonReact.Update {...state, isOpen: true, selection: isOpen ? (selectDown selection (Array.length results)) : 0}
  },
  willReceiveProps: fun {retainedProps, reduce, state} => {
    if (retainedProps != value) {
      /* let text = Js.Dict.get ingredientsMap value |> optMap (fun ing => ing##name) |> optOr ""; */
      let text = getText value ingredientsMap;
      {
        text,
        isOpen: false,
        selection: 0,
        results: getResults map::ingredientsMap ::text,
      }
    } else {
      state
    }
  },

  render: fun {state: {text, selection, isOpen, results}, reduce} => {
    /* let results = isOpen ? Some (getResults map::ingredientsMap ::text) : None; */
    <div className=Styles.container>
      <input
        value=text
        className=(Styles.input ^ " " ^ (switch value { | Models.Text _ => true | _ => false } ? Styles.badInput : ""))
        onChange=(reduce (fun evt => SetText (Utils.evtValue evt)))
        onBlur=(fun _ => {
          switch value {
            | Models.Text oldText => {
              let found = Js.Array.some
              (fun ing => {
                if (ing##name == text) {
                  onChange (Models.Id ing##id);
                  true
                } else {
                  false
                }
              })
              results;
              if (not found && text != oldText) {
                onChange (Models.Text text)
              }
            }
            | Models.Id id => {
              let change = switch (Js.Dict.get ingredientsMap id) {
              | None => true
              | Some ing => ing##name !== text
              };
              if (change) {
                onChange (Models.Text text)
              }
            }
          };
          (reduce (fun _ => Close)) ()
        })

        onKeyDown=(fun evt => {
          let key = ReactEventRe.Keyboard.key evt;
          let cmd = switch key {
          | "ArrowUp" => (Some GoUp)
          | "ArrowDown" => (Some GoDown)
          | "Escape" => (Some Close)
          | "Enter" => {
            onChange (Models.Id results.(selection)##id);
            None
          }
          | _ => None
          };
          [%guard let Some cmd = cmd][@else ()];
          ReactEventRe.Keyboard.preventDefault evt;
          (reduce (fun _ => cmd)) ();
        })
        onFocus=(reduce (fun evt => {
          let obj = (ReactDOMRe.domElementToObj (ReactEventRe.Focus.target evt));
          obj##select() |> ignore;
          Open
        }))
      />
      <div className=Styles.chevron>
        (str {j|▼|j})
      </div>
      (isOpen ?
      showResults ::results ::selection onSelect::(fun ing => {
        (reduce (fun _ => Close)) ();
        onChange (Models.Id ing##id)
      })
      : ReasonReact.nullElement
      )
    </div>
  }
}
