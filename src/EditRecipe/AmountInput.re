
let fractions = [
  (1. /. 2., {j|1/2|j}),

  (1. /. 3., {j|1/3|j}),
  (2. /. 3., {j|2/3|j}),

  (1. /. 4., {j|1/4|j}),
  (3. /. 4., {j|3/4|j}),

  (1. /. 6., {j|1/6|j}),
  (5. /. 6., {j|5/6|j}),

  (1. /. 8., {j|1/8|j}),
  (3. /. 8., {j|3/8|j}),
  (5. /. 8., {j|5/8|j}),
  (7. /. 8., {j|7/8|j}),
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

let makeFloat text => {
  if (text == "") {
    None
  } else {
    let parts = Js.String.split " " (Js.String.trim text);
    switch parts {
    | [|single|] =>
      try (Some (float_of_string text)) {
      | _ => None
      }
    | [|one, two|] => try {
        let whole = float_of_string one;
        let fractParts = Js.String.split "/" two;
        switch fractParts {
        | [|one, two|] => {
          Some (whole +. float_of_string one /. float_of_string two)
        }
        | _ => None
        }
    } {
      | _ => None
    }
    | _ => None
    }
  }
};

let makeText num => {
  num |> Utils.optMap fractionify |> Utils.optOr ""
};

type action = Set string | Reset;

let component = ReasonReact.reducerComponentWithRetainedProps "AmountInput";

let make ::value ::onChange ::className=? _ => ReasonReact.{
  ...component,
  initialState: fun _ => (makeText value),
  retainedProps: value,
  reducer: fun action _ => switch action {
  | Set text => ReasonReact.UpdateWithSideEffects text (fun {state, reduce} => {
      let num = makeFloat state;
      if (num != value) {
        onChange num;
      }
    })
  | Reset => ReasonReact.Update (makeText value)
  },
  willReceiveProps: fun {retainedProps, reduce, state} => {
    if (retainedProps != value && value != makeFloat state) {
      /* Js.log2 retainedProps (makeFloat state); */
      (makeText value)
    } else {
      state
    }
  },

  render: fun {state, reduce} => {
    let process text => {
      reduce (fun _ => Set text) ();
    };
    <input
      value=state
      className=?className
      onChange=(fun evt => process (Utils.evtValue evt))
      onBlur=(reduce (fun _ => Reset))
    />
  }
}
