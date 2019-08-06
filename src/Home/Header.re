open Utils;

let name = (auth) => {
  let%Lets.Opt (user) = Firebase.Auth.currentUser(auth) |>  Js.Nullable.toOption;
  let name =
    switch (Firebase.Auth.displayName(user) |>  Js.Nullable.toOption) {
    | Some(name) => name
    | None => Firebase.Auth.email(user)
    };
  Some(name)
};

let component = ReasonReact.statelessComponent("Header");

module Styles = {
  open Glamor;
  let container =
    css([
      flexDirection("row"),
      alignItems("center"),
      lineHeight("1"),
      backgroundColor(Shared.dark),
      color("white"),
      marginBottom("16px")
    ]);
  let logo =
    css([fontSize("32px"), padding("8px"), textDecoration("none"), color("currentColor")]);
  let topMenu =
    css([cursor("pointer"), padding("16px"), textDecoration("none"), color("currentColor")]);
  let menuItem =
    css([padding("16px 24px"), cursor("pointer"), Selector(":hover", [backgroundColor("#555")])]);
  let loginButton = css([
    padding("16px"),
    cursor("pointer"),
    textDecoration("none"),
    color("currentColor"),
    Selector(":hover", [backgroundColor("#555")])
  ]);
  let username = css([
    padding("16px"),
    cursor("pointer"),
    Selector(":hover", [backgroundColor("#555")])
  ]);
};

[@react.component] let make = (~auth, ~navigate) =>
  ReactCompat.useRecordApi(ReasonReact.{
    ...component,
    render: (_) => {
      let uid = Firebase.Auth.currentUser(auth) |>  Js.Nullable.toOption;
      <div className=Styles.container>
        (spacer(16))
        <a href="#/" className=Styles.logo> (str("Foood")) </a>
        (uid |> BaseUtils.optFoldReact(
          (_) => <a href="#/lists" className=Styles.topMenu>(str("Lists"))</a>
        ))
        spring
        (uid |> BaseUtils.optFoldReact(
          (_) => <a href="#/add" className=Styles.topMenu> (str("Add Recipe")) </a>
        ))
        (
          switch (name(auth)) {
          | Some(name) =>
            <Menu
              className=Glamor.(css([backgroundColor("#333")]))
              menu=(
                ReasonReact.array([|
                  <div
                    className=Styles.menuItem
                    key="Logout"
                    onClick=((_) => Firebase.Auth.signOut(auth))>
                    (str("Logout"))
                  </div>
                |])
              )>
              <div
                key="name"
                className=Styles.username>
                (str(name))
              </div>
            </Menu>
          | None =>
            <Link
              dest="/login"
              navigate
              text="Login"
              className=Styles.loginButton
            />
          }
        )
      </div>
    }
  });
