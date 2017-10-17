open Utils;

let module Styles = {
  open Glamor;
  let container = css [
    maxWidth "100%",
    width "400px",
    alignSelf "center",
  ];
  let input = css [
    padding "8px 16px",
    fontSize "20px",
  ];
  let button = css [
    cursor "pointer",
    backgroundColor "#eee",
    border "none",
    alignSelf "center",
    padding "8px 16px",
    fontWeight "400",
  ];
};

let component = ReasonReact.reducerComponent "LogInPage";

let make ::auth ::navigate _ => ReasonReact.{
  ...component,
  initialState: fun () => ("", "", false, ""),
  reducer: fun action state => ReasonReact.Update action,
  render: fun {state: (username, password, loading, error), reduce} => {
    <div className=Styles.container>
      <input
        _type="text"
        className=Styles.input
        placeholder="Email Address"
        value=username
        disabled=(Js.Boolean.to_js_boolean loading)
        onChange=(reduce (fun evt => (evtValue evt, password, false, error)))
      />
      (spacer 16)
      <input
        _type="password"
        className=Styles.input
        placeholder="Password"
        value=password
        disabled=(Js.Boolean.to_js_boolean loading)
        onChange=(reduce (fun evt => (username, evtValue evt, false, error)))
      />
      (spacer 16)
      <button
        className=Styles.button
        disabled=(Js.Boolean.to_js_boolean loading)
        onClick=(fun _ => {
          (reduce (fun _ => (username, password, true, ""))) ();
          Firebase.Auth.signInWithEmailAndPassword auth username password
          |> Js.Promise.then_ (fun user => {navigate "/"; Js.Promise.resolve ()})
          |> Js.Promise.catch (fun err => {
            (reduce (fun _ => (username, password, false, "Failed to login"))) ();
            Js.Promise.resolve ();
          })
          |> ignore;
        })
      >
        (str (loading ? "Loading..." : "Sign In"))
      </button>
    </div>
  }
}
