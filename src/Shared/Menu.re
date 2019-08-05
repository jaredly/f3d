open Utils;

let component = ReasonReact.reducerComponent("Menu");

let make = (~menu, ~className, children) =>
  ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, send}) =>
      <div
        onMouseEnter=(((_) => send(true)))
        onMouseLeave=(((_) => send(false)))
        className=Glamor.(css([position("relative")]))>
        (ReasonReact.array(children))
        (
          state ?
            <div
              className=Glamor.(
                          className
                          ++ (
                            " "
                            ++ css([position("absolute"), top("100%"), right("0"), zIndex("1000")])
                          )
                        )>
              menu
            </div> :
            ReasonReact.null
        )
      </div>
  };
