open Utils;

let component = ReasonReact.reducerComponent("DeleteButton");

module Styles = {
  open Glamor;
  let buttonBase =
    RecipeStyles.buttonStyles
    @ [
      borderBottom("2px solid hsl(0, 100%, 90%)"),
      whiteSpace("nowrap"),
      paddingBottom("6px"),
      /* transition(".1s ease box-shadow, .1s ease transform"), */
      transition(".2s ease border-bottom-color"),
      Selector(
        ":hover",
        [
          borderBottomColor("hsl(0, 100%, 48%)")
          /* boxShadow("0 1px 0px " ++ Shared.action), */
          /* transform("translateY(-1px)"), */
        ]
      )
      /* backgroundColor("#cfc"),
         cursor("pointer"),
         border("none"),
         fontFamily("inherit"),
         fontSize("inherit"),
         fontWeight("inherit"),
         Selector(":hover", [backgroundColor("#afa")]) */
    ];
  let button = Glamor.css(buttonBase);
  let redButton =
    css(
      buttonBase
      @ [
        backgroundColor("hsl(0, 100%, 48%)"),
        color(Shared.light),
        transition(".1s ease box-shadow, .1s ease transform"),
        borderBottom("none"),
        Selector(":hover", [boxShadow("0 1px 5px #aaa"), transform("translateY(-1px)")])
      ]
    );
  let safeButton =
    css(
      buttonBase
      @ [
        borderBottomColor("white"),
        Selector(
          ":hover",
          [
            borderBottomColor(Shared.action)
            /* boxShadow("0 1px 0px " ++ Shared.action), */
            /* transform("translateY(-1px)"), */
          ]
        )
      ]
    );
};

let make = (~onDelete, ~title="Delete", _) =>
  ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, send}) =>
      state ?
        <div className=Glamor.(css([flexDirection("row")]))>
          <button className=Styles.safeButton onClick=(((_) => send(false)))>
            (str("Just kidding"))
          </button>
          (spacer(32))
          <button className=Styles.redButton onClick=((_) => onDelete())>
            (str("Really delete"))
          </button>
        </div> :
        <div className=Glamor.(css([flexDirection("row")]))>
          <button className=Styles.button onClick=(((_) => send(true)))> (str(title)) </button>
        </div>
  };