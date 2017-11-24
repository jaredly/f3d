open Utils;
let component = ReasonReact.reducerComponent("FirebaseImage");
let make = (~id, ~fb, ~render, _children) => ReasonReact.{
  ...component,
  initialState: () => None,
  reducer: (action, _) => ReasonReact.Update(action),
  didMount: ({reduce}) => {
    Firebase.Storage.get(Firebase.app(fb))
    |> Firebase.Storage.ref
    |> Firebase.Storage.child(id)
    |> Firebase.Storage.getDownloadURL
    |> Js.Promise.then_(url => {
      reduce(() => Some(url))();
      Js.Promise.resolve(())
    }) |> ignore;
    ReasonReact.NoUpdate
  },
  render: ({state}) => {
    switch state {
    | None => <div>(str("Loading..."))</div>
    | Some(url) => render(url)
    }
  }
}
