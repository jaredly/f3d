
let component = ReasonReact.reducerComponent "Recipe";
let str = ReasonReact.stringToElement;

let module Styles = {
  open Glamor;
  let container = css [
    padding "16px",
    width "1000px",
    maxWidth "100%",
    alignSelf "center",
  ];
  let loading = css [
    alignItems "center",
  ];
  let title = css [
    fontSize "24px"
  ];
};

let make ::recipe ::fb ::id _children => ReasonReact.{
  ...component,
  initialState: fun () => (),
  reducer: fun () () => ReasonReact.NoUpdate,
  render: fun {state, reduce} => {
    <div className=Styles.container>
      <div className=Styles.title>
        (str recipe##title)
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
