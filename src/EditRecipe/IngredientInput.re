open Utils;
open BaseUtils;

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

  let addResult = css [
    color "#666",
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

let showResults ::results ::selection ::onSelect ::onNew => {
  let numResults = Array.length results;
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
      <div
        className=(Styles.result ^ " " ^ Styles.addResult ^ " " ^ (selection == numResults ? Styles.selectedResult : ""))
        onMouseDown=(fun evt => {
          ReactEventRe.Mouse.preventDefault evt;
          onNew ();
        })
      >
        (str "New ingredient")
      </div>
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

let selectUp selection len => selection <= 0 ? len : selection - 1;
let selectDown selection len => selection + 1 > len ? 0 : selection + 1;

let component = ReasonReact.reducerComponentWithRetainedProps "AmountInput";

let getText value ingredientsMap => {
  switch value {
    | Models.Text text => text
    | Models.Id id => Js.Dict.get ingredientsMap id |> optMap (fun ing => ing##name) |> optOr "";
  }
};

let make ::ingredientsMap ::value ::onChange ::addIngredient ::className=? _children => ReasonReact.{
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
    let findByName text => {
      let result = ref None;
      Js.Array.some
      (fun ing => {
        if (ing##name == text) {
          result := Some (ing##id);
          /* onChange (Models.Id ing##id); */
          true
        } else {
          false
        }
      })
      results |> ignore;
      !result
    };
    let isTextDifferent = switch value { | Models.Text _ => true | _ => false };
    <div className=Styles.container>
      <input
        value=text
        placeholder="Select ingredient"
        className=(Styles.input ^ " " ^ (isTextDifferent ? Styles.badInput : ""))
        onChange=(reduce (fun evt => SetText (Utils.evtValue evt)))
        onBlur=(fun _ => {
          switch value {
            | Models.Text oldText => {
              let found = findByName text;
              switch found {
              | None => if (text != oldText) {
                onChange (Models.Text text)
              }
              | Some id => onChange (Models.Id id)
              }
            }
            | Models.Id id => {
              let change = switch (Js.Dict.get ingredientsMap id) {
              | None => true
              | Some ing => ing##name !== text
              };
              if (change) {
                switch (findByName text) {
                | None => onChange (Models.Text text)
                | Some id => onChange (Models.Id id)
                }
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
            if (selection === Array.length results) {
              addIngredient text
              |> Js.Promise.then_ (fun id => {
                onChange (Models.Id id);
                Js.Promise.resolve ();
              }) |> ignore;
            } else {
              onChange (Models.Id results.(selection)##id);
            };
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
        if (Models.Id ing##id == value) {
          (reduce (fun _ => SetText ing##name)) ();
          (reduce (fun _ => Close)) ();
        } else {
          onChange (Models.Id ing##id);
        }
      }) onNew::(fun () => {
        addIngredient text
        |> Js.Promise.then_ (fun id => {
          onChange (Models.Id id);
          Js.Promise.resolve ();
        }) |> ignore;
      })
      : ReasonReact.nullElement
      )
    </div>
  }
}
