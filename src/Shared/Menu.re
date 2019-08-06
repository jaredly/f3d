open Utils;

let component = ReasonReact.reducerComponent("Menu");

[@react.component] let make = (~menu, ~className, ~children) =>
  ReactCompat.useRecordApi( ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, send}) =>
      <div
        onMouseEnter=(((_) => send(true)))
        onMouseLeave=(((_) => send(false)))
        className=Glamor.(css([position("relative")]))>
        children
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
  });
