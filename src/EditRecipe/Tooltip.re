open Utils;

let component = ReasonReact.reducerComponent("Tooltip");

[@react.component] let make = (~render, ~message, ~enabled) =>
  ReactCompat.useRecordApi( ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, state) => ReasonReact.Update(action),
    render: ({state, send}) =>
      <div
        className=Glamor.(css([position("relative")]))
        onMouseEnter=(((_) => send(true)))
        onMouseLeave=(((_) => send(false)))>
        (render())
        (
          state && enabled ?
            <div
              className=Glamor.(
                          css([
                            position("absolute"),
                            zIndex("100000"),
                            padding("4px 8px"),
                            borderRadius("4px"),
                            fontSize("12px"),
                            top("100%"),
                            marginTop("4px"),
                            whiteSpace("pre"),
                            right("0"),
                            backgroundColor("#555"),
                            color("white"),
                            Property("pointerEvents", "none")
                          ])
                        )>
              (str(message))
            </div> :
            ReasonReact.null
        )
      </div>
  });
