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
  (7. /. 8., {j|7/8|j}),
];

let fractionify = n => {
  let frac = n -. floor(n);
  let whole = int_of_float(n);

  /*** I want a [%bail_if frac < 0.001][@with x] */
  if (frac < 0.0001) {
    string_of_int(whole);
  } else {
    let rec loop = fractions =>
      switch (fractions) {
      | [] => string_of_float(n)
      | [(num, str), ..._] when frac -. 0.01 < num && frac +. 0.02 > num =>
        (whole > 0 ? string_of_int(whole) ++ " " : "") ++ str
      | [_, ...rest] => loop(rest)
      };
    loop(fractions);
  };
};

let fractionText = text => {
  let fractParts = Js.String.split("/", text);
  switch (fractParts) {
  | [|one, two|] =>
    try (Some(float_of_string(one) /. float_of_string(two))) {
    | _ => None
    }
  | _ => None
  };
};

let makeFloat = text =>
  if (text == "") {
    None;
  } else {
    let parts = Js.String.split(" ", Js.String.trim(text));
    switch (parts) {
    | [|single|] =>
      switch (fractionText(single)) {
      | Some(fract) => Some(fract)
      | None =>
        try (Some(float_of_string(text))) {
        | _ => None
        }
      }
    | [|one, two|] =>
      try (
        {
          let whole = float_of_string(one);
          switch (fractionText(two)) {
          | None => Some(whole)
          | Some(frac) => Some(whole +. frac)
          };
        }
      ) {
      | _ => None
      }
    | _ => None
    };
  };

let makeText = num =>
  num |> BaseUtils.optMap(fractionify) |> BaseUtils.optOr("");

[@react.component]
let make = (~value, ~onChange, ~className=?, ~onPaste=?, ~placeholder=?, _) => {
  let prevValue = Hooks.useLastValue(value);
  let (text, setText) = Hooks.useState(makeText(value));
  if (prevValue != value && value != makeFloat(text)) {
    setText(makeText(value));
  };
  let process = text => {
    setText(text);
    let num = makeFloat(text);
    if (num != value) {
      onChange(num);
    };
  };
  <input
    value=text
    ?className
    ?placeholder
    ?onPaste
    onKeyDown={evt =>
      switch (ReactEvent.Keyboard.key(evt)) {
      | "ArrowUp" =>
        ReactEvent.Keyboard.preventDefault(evt);
        let shift = ReactEvent.Keyboard.shiftKey(evt);
        let off = shift ? 0.5 : 1.;
        process(makeText(Some((makeFloat(text) |> optOr(0.)) +. off)));
      | "ArrowDown" =>
        ReactEvent.Keyboard.preventDefault(evt);
        let shift = ReactEvent.Keyboard.shiftKey(evt);
        let off = shift ? (-0.5) : (-1.);
        process(makeText(Some((makeFloat(text) |> optOr(0.)) +. off)));
      | _ => ()
      }
    }
    onChange={evt => process(Utils.evtValue(evt))}
    onBlur={_ => setText(makeText(value))}
  />;
};