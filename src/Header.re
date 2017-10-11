
open Utils;

let name auth => {
  [%guard let Some(user) = Firebase.Auth.currentUser auth |> Js.Nullable.to_opt][@else None];
  let name = switch (Firebase.Auth.displayName user |> Js.Nullable.to_opt) {
  | Some name => name
  | None => (Firebase.Auth.email user)
  };
  Some name;
};

let component = ReasonReact.reducerComponent "Header";

let make ::auth _children => ReasonReact.{
  ...component,
  initialState: fun () => (name auth),
  reducer: fun () _ => ReasonReact.NoUpdate,
  render: fun {state, reduce} => {
    <div className=Glamor.(css[
      flexDirection "row",
      alignItems "center",
      lineHeight "1",
      backgroundColor "#333",
      color "white",
      marginBottom "16px",
    ])>
      (spacer 16)
      <a href="#/" className=Glamor.(css[
        fontSize "32px",
        padding "8px", 
        textDecoration "none",
        color "currentColor",
      ])>
        (str "Foood")
      </a>
      spring
      <a href="#/add" className=Glamor.(css[
        cursor "pointer",
        padding "16px",
        textDecoration "none",
        color "currentColor",
      ])>
        (str "Add Recipe")
      </a>
      (switch state {
      | Some name =>
      <div className=Glamor.(css[
        padding "16px",
        cursor "pointer",
        Selector ":hover" [
          backgroundColor "#555"
        ]
      ])>
        (str name)
      </div>
      | None => ReasonReact.nullElement
      })
    </div>
  }
}