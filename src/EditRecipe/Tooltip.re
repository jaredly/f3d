
open Utils;

let component = ReasonReact.reducerComponent "Tooltip";

let make ::render ::message ::enabled _children => ReasonReact.{
  ...component,
  initialState: fun () => false,
  reducer: fun action state => ReasonReact.Update action,
  render: fun {state, reduce} => {
    <div
      className=Glamor.(css[
        position "relative",
      ])
      onMouseEnter=(reduce (fun _ => true))
      onMouseLeave=(reduce (fun _ => false))
    >
      (render ())
      (state && enabled
      ? <div className=Glamor.(css[
          position "absolute",
          zIndex "100000",
          padding "4px 8px",
          borderRadius "4px",
          fontSize "12px",
          top "100%",
          marginTop "4px",
          whiteSpace "pre",
          right "0",          
          backgroundColor "#555",
          color "white",
          Property "pointerEvents" "none",
        ])>
          (str message)
        </div>
      : ReasonReact.nullElement)
    </div>
  }
};

