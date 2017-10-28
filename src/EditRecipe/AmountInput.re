open Utils;

open BaseUtils;

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
  (7. /. 8., {j|7/8|j})
];

let fractionify = (n) => {
  let frac = n -. floor(n);
  let whole = int_of_float(n);

  /*** I want a [%bail_if frac < 0.001][@with x] */
  if (frac < 0.0001) {
    string_of_int(whole)
  } else {
    let rec loop = (fractions) =>
      switch fractions {
      | [] => string_of_float(n)
      | [(num, str), ..._] when frac -. 0.01 < num && frac +. 0.02 > num =>
        (whole > 0 ? string_of_int(whole) ++ " " : "") ++ str
      | [_, ...rest] => loop(rest)
      };
    loop(fractions)
  }
};

let fractionText = (text) => {
  let fractParts = Js.String.split("/", text);
  switch fractParts {
  | [|one, two|] =>
    try (Some(float_of_string(one) /. float_of_string(two))) {
    | _ => None
    }
  | _ => None
  }
};

let makeFloat = (text) =>
  if (text == "") {
    None
  } else {
    let parts = Js.String.split(" ", Js.String.trim(text));
    switch parts {
    | [|single|] =>
      switch (fractionText(single)) {
      | Some(fract) => Some(fract)
      | None =>
        try (Some(float_of_string(text))) {
        | _ => None
        }
      }
    | [|one, two|] =>
      try {
        let whole = float_of_string(one);
        switch (fractionText(two)) {
        | None => Some(whole)
        | Some(frac) => Some(whole +. frac)
        }
      } {
      | _ => None
      }
    | _ => None
    }
  };

let makeText = (num) => num |> BaseUtils.optMap(fractionify) |> BaseUtils.optOr("");

type action =
  | Set(string)
  | Reset;

let component = ReasonReact.reducerComponentWithRetainedProps("AmountInput");

let make = (~value, ~onChange, ~className=?, ~onPaste=?, ~placeholder=?, _) =>
  ReasonReact.{
    ...component,
    initialState: (_) => makeText(value),
    retainedProps: value,
    reducer: (action, _) =>
      switch action {
      | Set(text) =>
        ReasonReact.UpdateWithSideEffects(
          text,
          (
            ({state, reduce}) => {
              let num = makeFloat(state);
              if (num != value) {
                onChange(num)
              }
            }
          )
        )
      | Reset => ReasonReact.Update(makeText(value))
      },
    willReceiveProps: ({retainedProps, reduce, state}) =>
      if (retainedProps != value && value != makeFloat(state)) {
        /* Js.log2 retainedProps (makeFloat state); */
        makeText(value)
      } else {
        state
      },
    render: ({state, reduce}) => {
      let process = (text) => reduce((_) => Set(text), ());
      <input
        value=state
        ?className
        ?placeholder
        ?onPaste
        onKeyDown=(
          (evt) =>
            switch (ReactEventRe.Keyboard.key(evt)) {
            | "ArrowUp" =>
              ReactEventRe.Keyboard.preventDefault(evt);
              let shift = ReactEventRe.Keyboard.shiftKey(evt);
              let off = shift ? 0.5 : 1.;
              (reduce((_) => Set(makeText(Some((makeFloat(state) |> optOr(0.)) +. off)))))()
            | "ArrowDown" =>
              ReactEventRe.Keyboard.preventDefault(evt);
              let shift = ReactEventRe.Keyboard.shiftKey(evt);
              let off = shift ? (-0.5) : (-1.);
              (reduce((_) => Set(makeText(Some((makeFloat(state) |> optOr(0.)) +. off)))))()
            | _ => ()
            }
        )
        onChange=((evt) => process(Utils.evtValue(evt)))
        onBlur=(reduce((_) => Reset))
      />
    }
  };
