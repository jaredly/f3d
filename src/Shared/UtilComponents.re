
let toggleComponent = ReasonReact.reducerComponent("Toggle");
let toggle = (~initial, ~render) => {
  ...toggleComponent,
  initialState: () => initial,
  reducer: (newState, _state) => ReasonReact.Update(newState),
  render: ({state, reduce}) => {
    render(~on=state, ~toggle=reduce(() => !state))
  }
};

/* let stateless = (name, render) => {
  let component = ReasonReact.statelessComponent(name);
  render(~awesome, ~things,)
}; */

/* let module Stateless (Config: {let }) */
