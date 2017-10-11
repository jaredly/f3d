
let str = ReasonReact.stringToElement;
let spring = <div style=ReactDOMRe.Style.(make flexGrow::"1" ()) />;
let spacer ::key=? num => <div key=?key style=ReactDOMRe.Style.(make flexBasis::(string_of_int num ^ "px") ()) />;

let optMap fn value => switch value { | None => None | Some v => Some (fn v) };
let optBind fn value => switch value { | None => None | Some v => fn v };
let optOr default value => switch value { | None => default | Some v => v };

let orr default value => switch value { | None => default | Some value => value };
let ifEmpty default value => if (value == "") { default } else {value};

let maybe item render => switch (Js.Nullable.to_opt item) {
| None => None
| Some value => Some (render value)
};

let rec filterMaybe items => switch items {
| [] => []
| [Some x, ...rest] => [x, ...filterMaybe rest]
| [None, ...rest] => filterMaybe rest
};

let rec spacedItems ::i=0 spacer items => switch items {
| [] => []
| [item] => [item]
| [item, ...rest] => [item, spacer i, ...spacedItems i::(i + 1) spacer rest]
};

let spacedArray spacer items => {
  let num = Array.length items;
  [%guard let true = num > 0][@else [||]];
  let result = Array.make (num * 2 - 1) items.(0);
  items |> Array.iteri (fun i item => {
    let at = i == 0 ? 0 : i * 2;
    result.(at) = item;
    if (i < num - 1) {
      result.(at + 1) = spacer i;
    }
  });
  result
};

 let doubleState a b => {
   switch a {
   | `Initial => `Initial
   | `Errored err => `Errored err
   | `Loaded a => switch b {
      | `Initial => `Initial
      | `Errored err => `Errored err
      | `Loaded b => `Loaded (a, b)
   }
   }
 };

let str = ReasonReact.stringToElement;
let evtValue event => (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;