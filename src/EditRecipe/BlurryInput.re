[@react.component] let make = (~value: string, ~resetOnBlur=false, ~onChange=?, ~render) => {
  let (state, setState) = Hooks.useUpdatingState(value);

  render(~value=state, ~onChange=((text) => setState(text)), ~onBlur=(_) => {
    BaseUtils.optMap((fn) => fn(state), onChange) |> ignore;
    if (resetOnBlur) {
      setState(value)
    };
    })
}