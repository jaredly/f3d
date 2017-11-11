open Utils;

let component = ReasonReact.reducerComponent("Menu");

let make = (~menu, ~className, children) =>
  ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, reduce}) =>
      <div
        onMouseEnter=(reduce((_) => true))
        onMouseLeave=(reduce((_) => false))
        className=Glamor.(css([position("relative")]))>
        (ReasonReact.arrayToElement(children))
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
            ReasonReact.nullElement
        )
      </div>
  };
