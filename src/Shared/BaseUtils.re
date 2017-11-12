module StringSet =
  Set.Make(
    {
      type t = string;
      let compare = compare;
    }
  );

module IntSet =
  Set.Make(
    {
      type t = int;
      let compare = compare;
    }
  );

let uuid: unit => string = [%bs.raw {|function() {return Math.random().toString(16).slice(2)}|}];

let magicDefault = (default, v) => {
  if (Obj.magic(v) |> Js.Null_undefined.test) {
    default
  } else {
    v
  }
};

let optMap = (fn, value) =>
  switch value {
  | None => None
  | Some(v) => Some(fn(v))
  };

let optBind = (fn, value) =>
  switch value {
  | None => None
  | Some(v) => fn(v)
  };

let optOr = (default, value) =>
  switch value {
  | None => default
  | Some(v) => v
  };

let orr = (default, value) =>
  switch value {
  | None => default
  | Some(value) => value
  };

let ifEmpty = (default, value) =>
  if (value == "") {
    default
  } else {
    value
  };

let maybe = (item, render) =>
  switch (Js.Null.to_opt(item)) {
  | None => None
  | Some(value) => Some(render(value))
  };

let rec filterMaybe = (items) =>
  switch items {
  | [] => []
  | [Some(x), ...rest] => [x, ...filterMaybe(rest)]
  | [None, ...rest] => filterMaybe(rest)
  };
