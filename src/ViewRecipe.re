
let component = ReasonReact.reducerComponent "Recipe";
let str = ReasonReact.stringToElement;

let make ::recipe ::fb ::id _children => ReasonReact.{
  ...component,
  initialState: fun () => (),
  reducer: fun () () => ReasonReact.NoUpdate,
  render: fun {state, reduce} => {
    <div >
      (str "Recipe!")
      (str recipe##title)
    </div>
  }
};

let loadingRecipe () => {
  <div>
    (str "Loading")
  </div>
};

let failedLoading err => {
  Js.log err;
  <div>
    (str "Errored though")
  </div>
};

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
