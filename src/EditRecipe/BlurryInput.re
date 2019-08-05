let component = ReasonReact.reducerComponentWithRetainedProps("BlurryInput");
let make = (~value: string, ~resetOnBlur=false, ~onChange=?, ~render, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => value,
    retainedProps: value,
    willReceiveProps: ({retainedProps, state}) =>
      if (retainedProps !== value && value !== state) {
        value
      } else {
        state
      },
    reducer: (action, state) => ReasonReact.Update(action),
    render: ({state, send}) =>
      render(~value=state, ~onChange=((text) => send(text)), ~onBlur=(_) => {
        BaseUtils.optMap((fn) => fn(state), onChange) |> ignore;
        if (resetOnBlur) {
          send(value)
        };
      })
  };
