open Utils;

module Styles = {
  open Glamor;
  let container =
    css([borderTop("1px solid " ++ Shared.actionLight),
    position("relative"),
    marginTop("32"),
    paddingTop("16")]);
  let notes = css([whiteSpace("pre-wrap")]);
  let image = css([width("200px"), height("200px"), objectFit("contain")]);
};

let view = (~uid, ~fb, ~madeit, ~onEdit) =>
  <div className=Styles.container>
    <div className=RecipeStyles.wrappingRow>
      <div className=Glamor.(css([fontWeight("600")]))> <UserName id=madeit##authorId fb /> </div>
      (spacer(16))
      (str(madeit##created |> Js.Date.fromFloat |> Js.Date.toDateString))
      spring
      <div className=RecipeStyles.row>
      (
        switch (Js.Null.to_opt(madeit##rating)) {
        | None => ReasonReact.nullElement
        | Some(rating) => RatingWidget.showStars(~active=true, ~rating)
        }
      )
      </div>
      (spacer(8))
      (
        switch uid {
        | Some(uid) when uid === madeit##authorId =>
          <div className=RecipeStyles.rightSide>
            <button
              onClick=(ignoreArg(onEdit))
              className=(RecipeStyles.smallButton ++ " " ++ Glamor.(css([marginTop("8px")])))>
              (str("edit"))
            </button>
          </div>
        | _ => ReasonReact.nullElement
        }
      )
    </div>
    (spacer(8))
    <div className=Styles.notes> (str(madeit##notes)) </div>
    (madeit##images != [||] ? spacer(32) : ReasonReact.nullElement)
    <div className=RecipeStyles.row>
    (madeit##images |>
    Array.map(id => <FirebaseImage key=id fb id render=(url => <img src=url
        className=Styles.image
    />) />)
    |> ReasonReact.arrayToElement)
    </div>
  </div>;

let component = ReasonReact.statelessComponent("MadeIt");

let make = (~madeit, ~uid, ~fb, _children) =>
  UtilComponents.toggle(
    ~initial=false,
    ~render=
      (~on, ~toggle) =>
        switch (uid, on) {
        | (Some(uid), true) =>
          <EditMadeIt
            fb
            uid
            title="Edit experience"
            action="Save"
            initial=madeit
            onCancel=toggle
            onSave=toggle
          />
        | _ => view(~fb, ~uid, ~madeit, ~onEdit=toggle)
        }
  );
/*
 ERROR
 because I want to conditionally return differently typed components. Which sholdn't be a problem :/

 let make = (~madeit, ~uid, ~fb, _children) =>
   switch uid {
   | None => UtilComponents.render(() => view(~fb, ~uid, ~madeit, ~onEdit=() => ()))
   | Some(uid) =>
       UtilComponents.toggle(
         ~initial=true,
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
               view(~fb, ~uid=Some(uid), ~madeit, ~onEdit=toggle)
             }
       )
     }; */