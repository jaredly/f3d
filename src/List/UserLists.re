open Utils;

let module Styles = {
  open Glamor;
  let container = css([maxWidth("100%"), width("800px"), alignSelf("center")]);
  let item = css(RecipeStyles.leftBorderItem @ [
    color("currentColor"),
    textDecoration("none"),
  ])
};

let showLists = (~lists, ~uid, ~navigate) => {
  <div className=Styles.container>
    <div>
      <button className=RecipeStyles.primaryButton>
        (str("Add list"))
      </button>
      (spacer(32))
      (lists |> Array.map(
        (list) => {
          <Link className=Styles.item navigate dest=("/list/" ++ list##id) text=list##title />
        }
      ) |> ReasonReact.arrayToElement)
    </div>
  </div>
};

module Fetcher = FirebaseFetcher.Dynamic(Models.List);
let component = ReasonReact.statelessComponent("UserLists");
let make = (~fb, ~uid, ~navigate, _children) => {
  let query = Firebase.Query.((q) => q |> whereStr("authorId", ~op="==", uid));
  ReasonReact.{
    ...component,
    render: (_self) =>
      <Fetcher
        fb
        pageSize=1000
        query
        render=(
          (~state, ~fetchMore as _) =>
            switch state {
            | `Initial => <div/>
            | `Loaded(_snap, lists) => showLists(~lists, ~uid, ~navigate)
            | `Errored(err) => <div>(str("Failed to load"))</div>
            }
        )
      />
  }
};