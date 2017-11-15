open BaseUtils;
open Utils;

module MadeItsFetcher =
  FirebaseFetcher.Stream(
    {
      include Models.MadeIt;
      let getId = (ing) => ing##id;
    }
  );

let bestScore = items => List.fold_left(
  (best, madeit) => max(best, madeit##rating |> Js.Null.to_opt |> optOr(0)),
  0,
  items
);

let module Styles = {
  open Glamor;
  let base = [
    fontFamily("sans-serif"),
    /* backgroundColor("#aaa"), */
    width("20px"),
    height("20px"),
    alignSelf("center"),
    lineHeight("12px"),
    justifyContent("center"),
    textAlign("center"),
    fontSize("12px"),
    borderRadius("9px"),
    /* border("1px solid #aaa"), */
  ];
  /** old "full circle" colors */
  let colors = [|
    "hsl(0, 100%, 92%)",
    "hsl(30, 100%, 92%)",
    "hsl(60, 100%, 92%)",
    "hsl(211, 100%, 92%)",
    "hsl(120, 100%, 92%)",
  |];
  let colors = [|
    "hsl(0, 100%, 62%)",
    "hsl(30, 100%, 62%)",
    /* "hsl(60, 100%, 62%)", */
    "hsl(211, 100%, 62%)",
    /* "hsl(120, 100%, 62%)", */
    "hsl(173, 65%, 44%)",
    "hsl(120, 58%, 47%)",
  |];
  let mine = Array.map((c) => css(base @ [
    borderColor(c),
    color(c),
    /* backgroundColor(c) */
  ]), colors);
  let theirs = Array.map((c) => css(base @ [
    color(c),
    borderColor(c),
  ]), colors);
  let empty = css(base @ [
    visibility("hidden")
  ]);
};

let stringOfFloat = x => {
  let s = string_of_float(x);
  if (Js.String.endsWith(".", s)) {
    s ++ "0"
  } else {
    s
  }
};

let ringStars = (num) => {
  let r = 10.;
  <div style=ReactDOMRe.Style.(make(
    ~position="relative",
    ~height="16px",
    ~width="16px",
    ~zIndex="-1",
    ~fontSize="8px",
    ~lineHeight="8px",
    ()
  ))>
    (range(5)
    |> List.map((i) => {
      let theta = float_of_int(i) /. 5. *. Js.Math._PI *. 2. +. Js.Math._PI;
      let x = cos(theta) *. r +. r -. 4.;
      let y = sin(theta) *. r +. r -. 4.;
      <div style=ReactDOMRe.Style.(make(
        ~position="absolute",
        /* ~color=(i < num ? "black" : "#aaa"), */
        ~opacity=(i < num ? "1.0" : "0.2"),
        ~top=stringOfFloat(y) ++ "px",
        ~left=stringOfFloat(x) ++ "px",
        ()
      ))>
        (str({j|★|j}))
      </div>
    }) |> Array.of_list |> ReasonReact.arrayToElement)
  </div>
};

let showSummary = (~mine, ~theirs) => {
  if (mine == [] && theirs === []) {
    <div className=Styles.empty />
  } else {
    if (mine !== []) {
      /* best score I've given it */
      let best = bestScore(mine);
      <div className=Styles.mine[best]>
        /* (str(string_of_int(best + 1))) */
        (ringStars(best + 1))
      </div>
    } else {
      let best = bestScore(theirs);
      <div className=Styles.theirs[best]>
        /* (str(string_of_int(best + 1))) */
        (ringStars(best + 1))
      </div>
    }
  }
};

let isMine = (uid, madeit) => {
  madeit##authorId === uid
  || Js.Array.includes(uid, (BaseUtils.magicDefault([||], madeit##ateWithPeople)))
};

let isNotMine = (uid, madeit) => !isMine(uid, madeit);

let make =
  ViewMadeIts.madeItForRecipe(
    (~state, ~fb) =>
      switch state {
      | `Initial => <div className=Styles.empty />
      | `Loaded(madeits) =>
        let uid = Firebase.Auth.fsUid(fb);
        let mine = uid
          |> optMap((uid) => List.filter(isMine(uid), madeits))
          |> optOr([]);
        let theirs = switch uid {
        | None => madeits
        | Some(uid) => List.filter(isNotMine(uid), madeits)
        };
        showSummary(~mine, ~theirs)
      | `Errored(err) =>
        Js.log2("Failed to load madeits", err);
        <div> (str("")) </div>
      }
  );