open Utils;
open BaseUtils;

let component = ReasonReact.statelessComponent("Textarea");

let module Styles = {
  open Glamor;
  let container = css([
    borderTop("2px solid " ++ Shared.action),
    position("relative"),
    paddingTop("8"),
  ])
};

let make = (~madeits, ~fb, _children) => {
  ...component,
  render: (_) => {
    <div>
      (Array.map((madeit) => {
        <div className=Styles.container>
          <div className=RecipeStyles.row>
            <div className=Glamor.(css([fontWeight("600")]))>
            <UserName id=madeit##authorId fb />
            </div>
            (spacer(16))
            (str(madeit##created |> Js.Date.fromFloat |> Js.Date.toDateString))
            (spring)
            (switch (Js.Null.to_opt(madeit##rating)) {
            | None => ReasonReact.nullElement
            | Some(rating) => RatingWidget.showStars(~active=true, ~rating)
            })
            (spacer(8))
            <div className=RecipeStyles.rightSide>
            <button className=RecipeStyles.smallButton>
              (str("edit"))
            </button>
            </div>
          </div>
          (spacer(8))
          <div>
            (str(madeit##notes))
          </div>
        </div>
      }, madeits |> Array.of_list) |> ReasonReact.arrayToElement)
    </div>
  }
};

module MadeItsFetcher = FirebaseFetcher.Stream({
  include Models.MadeIt;
  let getId = (ing) => ing##id;
});

let loading = () => <div>(str("Loading"))</div>;
let failedLoading = (_err) => <div>(str("Failed Loading"))</div>;

let make = (~fb, ~id, children) => {
  MadeItsFetcher.make(
    ~fb,
    ~refetchKey=id,
    ~query=Firebase.Query.whereStr("recipeId", ~op="==", id),
    ~render=((~state) => switch (state) {
      | `Initial => loading()
      | `Loaded(madeits) => make(~madeits, ~fb, [||]) |> ReasonReact.element
      | `Errored(err) => failedLoading(err)
    }),
    children
  )
};
