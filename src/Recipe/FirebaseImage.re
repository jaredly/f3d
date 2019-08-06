open Utils;
let component = ReasonReact.reducerComponent("FirebaseImage");
[@react.component] let make = (~id, ~fb, ~render) => ReactCompat.useRecordApi(ReasonReact.{
  ...component,
  initialState: () => None,
  reducer: (action, _) => ReasonReact.Update(action),
  didMount: ({send}) => {
    Firebase.Storage.get(Firebase.app(fb))
    |> Firebase.Storage.ref
    |> Firebase.Storage.child(id)
    |> Firebase.Storage.getDownloadURL
    |> Js.Promise.then_(url => {
      send(Some(url));
      Js.Promise.resolve(())
    }) |> ignore;
  },
  render: ({state}) => {
    switch state {
    | None => <div>(str("Loading..."))</div>
    | Some(url) => render(url)
    }
  }
})
