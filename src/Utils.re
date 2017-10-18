
let module StringSet = Set.Make {
  type t = string;
  let compare = compare;
};

let module IntSet = Set.Make {
  type t = int;
  let compare = compare;
};

let uuid: unit => string = [%bs.raw{|function() {return Math.random().toString(16).slice(2)}|}];

let str = ReasonReact.stringToElement;
let spring = <div style=ReactDOMRe.Style.(make flexGrow::"1" ()) />;
let spacer ::key=? num => <div key=?key style=ReactDOMRe.Style.(make flexBasis::(string_of_int num ^ "px") ()) />;

let evtValue: ReactEventRe.Form.t => string = fun event =>
  (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;

let optMap fn value => switch value { | None => None | Some v => Some (fn v) };
let optBind fn value => switch value { | None => None | Some v => fn v };
let optOr default value => switch value { | None => default | Some v => v };

let orr default value => switch value { | None => default | Some value => value };
let ifEmpty default value => if (value == "") { default } else {value};

let maybe item render => switch (Js.Null.to_opt item) {
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



let nonnull v => not @@ Js.Null.test v;

let fractions = [
  (1. /. 2., {j|½|j}),

  (1. /. 3., {j|⅓|j}),
  (2. /. 3., {j|⅔|j}),

  (1. /. 4., {j|¼|j}),
  (3. /. 4., {j|¾|j}),

  (1. /. 6., {j|⅙|j}),
  (5. /. 6., {j|⅚|j}),

  (1. /. 8., {j|⅛|j}),
  (3. /. 8., {j|⅜|j}),
  (5. /. 8., {j|⅝|j}),
  (7. /. 8., {j|⅞|j}),
];

let fractionify n => {
  let frac = n -. floor n;
  let whole = (int_of_float n);
  /** I want a [%bail_if frac < 0.001][@with x] */
  if (frac < 0.0001) {
    string_of_int whole
  } else {
    let rec loop fractions => switch fractions {
      | [] => (string_of_float n)
      | [(num, str), ..._] when frac -. 0.01 < num && frac +. 0.02 > num =>
          ((whole > 0 ? string_of_int whole ^ " " : "") ^ str)
      | [_, ...rest] => loop rest
    };
    loop fractions
  }
};

let unitShortNames = [
  ("c", ["cup", "cups"]),
  ("t", ["teaspoon", "tablespoons", "tsp", "tsps"]),
  ("T", ["tablespoon", "tablespoons", "tbs"]),
  ("oz", ["ounce", "ounces"]),
];

let smallUnit unit => {
  let canon = Js.String.toLowerCase unit;
  let rec loop names => switch names {
  | [] => unit
  | [(short, longs), ...rest] when List.mem canon longs => short
  | [_, ...rest] => loop rest
  };
  loop unitShortNames;
};