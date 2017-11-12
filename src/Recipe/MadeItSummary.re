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
    border("5px solid #aaa"),
  ];
  let colors = [|
    "hsl(0, 100%, 92%)",
    "hsl(30, 100%, 92%)",
    "hsl(60, 100%, 92%)",
    "hsl(211, 100%, 92%)",
    "hsl(120, 100%, 92%)",
  |];
  let mine = Array.map((color) => css(base @ [
    borderColor(color),
    backgroundColor(color)
  ]), colors);
  let theirs = Array.map((color) => css(base @ [
    borderColor(color),
  ]), colors);
  let empty = css(base @ [
    visibility("hidden")
  ]);
};

let showSummary = (~mine, ~theirs) => {
  if (mine == [] && theirs === []) {
    <div className=Styles.empty />
  } else {
    if (mine !== []) {
      /* best score I've given it */
      let best = bestScore(mine);
      <div className=Styles.mine[best]>
        (str(string_of_int(best + 1)))
      </div>
    } else {
      let best = bestScore(theirs);
      <div className=Styles.theirs[best]>
        (str(string_of_int(best + 1)))
      </div>
    }
  }
};

let isMine = (uid, madeit) => {
  madeit##authorId === uid
  || Js.Array.includes(uid, madeit##ateWithPeople)
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