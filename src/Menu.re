open Utils;

let component = ReasonReact.reducerComponent "Menu";

let make ::menu ::className children => ReasonReact.{
  ...component,
  initialState: fun () => false,
  reducer: fun action _ => ReasonReact.Update action,
  render: fun {state, reduce} => {
    <div
      onMouseEnter=(reduce (fun _ => true))
      onMouseLeave=(reduce (fun _ => false))
      className=Glamor.(css[
        position "relative",
      ])
    >
      (ReasonReact.arrayToElement children)
      (state
      ? <div className=Glamor.(className ^ " " ^ css[
          position "absolute",
          top "100%",
          right "0",
          zIndex "1000",
        ])>menu</div>
      : ReasonReact.nullElement)
    </div>
  }
}
