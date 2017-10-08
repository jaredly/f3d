
open Utils;
let component = ReasonReact.reducerComponent "Recipe";

let module Styles = {
  open Glamor;
  let container = css [
    padding "16px",
    width "800px",
    maxWidth "100%",
    alignSelf "center",
  ];
  let loading = css [
    alignItems "center",
  ];
  let title = css [
    fontSize "24px"
  ];
  let header = css [
    flexDirection "row",
    alignItems "center",
  ];
  let button = css [
    fontSize "inherit",
    border "none",
    backgroundColor "transparent",
    padding "0",
    margin "0",
    fontWeight "inherit",

    color "#777",
    cursor "pointer",
    padding "8px 16px",
    Selector ":hover" [
      color "black",
    ]
  ];
  let editButton = css [
    textDecoration "none",
    color "currentColor",
    padding "8px 16px",
    cursor "pointer",
    color "#777",
    Selector ":hover" [
      color "black",
    ]
  ];
};

let make ::recipe ::fb ::id _children => ReasonReact.{
  ...component,
  initialState: fun () => false,
  reducer: fun action _state => ReasonReact.Update action,
  render: fun {state, reduce} => {
    <div className=Styles.container>
      <div className=Styles.header>
      <div className=Styles.title>
        (str recipe##title)
      </div>
      spring
      <button className=Styles.button onClick=(reduce (fun _ => not state))>
        (str (state ? "Stop making" : "Make"))
      </button>
      <a className=Styles.editButton href=("#/edit/recipe/" ^ id)>
        (str "Edit")
      </a>
      </div>
    </div>
  }
};

let loadingRecipe () => {
  <div className=Styles.container>
    <div className=Styles.loading>
    (str "Loading")
    </div>
  </div>
};

let failedLoading err => {
  Js.log err;
  <div>
    (str "Errored though")
  </div>
};

/** Data loading part */
let module Fetcher = FirebaseFetcher.Single Models.Recipe;
let make ::fb ::navigate ::id children => {
  Js.log id;
  Fetcher.make ::fb ::id
  render::(fun ::state => {
    switch state {
    | `Initial => loadingRecipe ()
    | `Loaded recipe => make ::recipe ::fb ::id [||] |> ReasonReact.element
    | `Errored err => failedLoading err
    }
  }) children
}
