
let str = ReasonReact.stringToElement;
let spring = <div style=ReactDOMRe.Style.(make flexGrow::"1" ()) />;
let spacer ::key=? num => <div key=?key style=ReactDOMRe.Style.(make flexBasis::(string_of_int num ^ "px") ()) />;

let evtValue: ReactEventRe.Form.t => string = fun event =>
  (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;

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

let unitSpeakableNames = [
  ("cup", "cups", ["cup", "cups"]),
  ("teaspoon", "teaspoons", ["teaspoon", "tablespoons", "tsp", "tsps"]),
  ("tablespoon", "tablespoons", ["tablespoon", "tablespoons", "tbs"]),
  ("ounce", "ounces", ["ounce", "ounces"]),
];

let speakableUnit unit isPlural => {
  let canon = Js.String.toLowerCase unit;
  let rec loop names => switch names {
  | [] => unit
  | [(singular, plural, longs), ...rest] when List.mem canon longs => isPlural ? plural : singular
  | [_, ...rest] => loop rest
  };
  loop unitSpeakableNames;
};