open Utils;

module Styles = {
  open Glamor;
  let container =
    css([borderTop("2px solid " ++ Shared.action), position("relative"), paddingTop("8")]);
  let notes = css([whiteSpace("pre-wrap")]);
};

let view = (~uid, ~fb, ~madeit, ~onEdit) =>
  <div className=Styles.container>
    <div className=RecipeStyles.row>
      <div className=Glamor.(css([fontWeight("600")]))> <UserName id=madeit##authorId fb /> </div>
      (spacer(16))
      (str(madeit##created |> Js.Date.fromFloat |> Js.Date.toDateString))
      spring
      (
        switch (Js.Null.to_opt(madeit##rating)) {
        | None => ReasonReact.nullElement
        | Some(rating) => RatingWidget.showStars(~active=true, ~rating)
        }
      )
      (spacer(8))
      (
        switch uid {
        | Some(uid) when uid === madeit##authorId =>
          <div className=RecipeStyles.rightSide>
            <button onClick=(ignoreArg(onEdit)) className=(
              RecipeStyles.smallButton ++ " " ++
              Glamor.(css([marginTop("8px")]))
            )>
              (str("edit"))
            </button>
          </div>
        | _ => ReasonReact.nullElement
        }
      )
    </div>
    (spacer(8))
    <div className=Styles.notes> (str(madeit##notes)) </div>
  </div>;

let component = ReasonReact.statelessComponent("MadeIt");

let make = (~madeit, ~uid, ~fb, _children) =>
  UtilComponents.toggle(
    ~initial=false,
    ~render=
      (~on, ~toggle) =>
        if (on) {
          <EditMadeIt
            fb
            uid
            title="Edit experience"
            action="Save"
            initial=madeit
            onCancel=toggle
            onSave=toggle
          />
        } else {
          view(~fb, ~uid, ~madeit, ~onEdit=toggle)
        }
  );