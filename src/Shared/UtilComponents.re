open Utils;

let renderComponent = ReasonReact.statelessComponent("Render");
let render = (render) => {
  ...renderComponent,
  render: (_) => render()
};

let toggleComponent = ReasonReact.reducerComponent("Toggle");
let toggle = (~initial, ~render) => {
  ...toggleComponent,
  initialState: () => initial,
  reducer: (newState, _state) => ReasonReact.Update(newState),
  render: ({state, send}) => {
    render(~on=state, ~toggle=(() => send( !state)))
  }
};

let module Backdrop = {
  let module Styles = {
    open Glamor;
    let container = css([
      position("fixed"),
      zIndex("10000"),
      top("0"),
      left("0"),
      right("0"),
      bottom("0"),
      alignItems("center"),
      justifyContent("center"),
      backgroundColor("rgba(0, 0, 0, 0.1)")
    ]);
  };

  let component = ReasonReact.statelessComponent("Backdrop");
  let make = (~onCancel, children) => {
    ...component,
    render: (_) => ReasonReact.createDomElement("div", ~props={
      "onClick": Utils.ignoreArg(onCancel),
      "className": Styles.container
    }, children)
  }
};

let module Loader = (Config: {type t}) => {
  let loaderComponent = ReasonReact.reducerComponent("Loader");
  let make = (~promise, ~loading, ~render) => {
    ...loaderComponent,
    initialState: () => (None: option(Config.t)),
    reducer: (value, state: option(Config.t)) => ReasonReact.Update(Some(value)),
    didMount: ({send}) => {
      Js.Promise.then_((value) => {
        send( value);
        Js.Promise.resolve();
      }, promise) |> ignore;
    },
    render: ({state}) => switch state {
    | None => loading
    | Some(value) => render(value)
    }
  };
};

let module OnVisible = {
  let component = ReasonReact.reducerComponent("OnVisible");
  type unsub = unit => unit;
  let make = (~trigger, children) => {
    ...component,
    initialState: () => ref(None),
    reducer: ((), _) => ReasonReact.NoUpdate,
    willUnmount: ({state}) => switch (state^) {
      | None => ()
      | Some(fn) => fn()
    },
    render: ({state}) => {
      <div ref=((node) => {
        if (state.contents !== None) {
          ()
        } else switch ( Js.Nullable.toOption(node)) {
        | None => ()
        | Some(node) => {
          let fn = ([%bs.raw {|(function(node, cb){
            if (!window.IntersectionObserver) {
              console.warn("No intersection observer. TODO support a slower way")
              return () => {}
            }
            let obs = new window.IntersectionObserver((entries) => {
              if (entries && entries.length == 1 && entries[0].isIntersecting) {
                console.log("triggered!!")
                cb()
              }
            }, {})
            obs.observe(node)
            return () => obs.disconnect()
          }
          )|}]: [@bs] (Dom.element => (unit => unit) => unsub));
          state.contents = Some([@bs]fn(node, trigger));
        }
        }
      })>
        (ReasonReact.array(children))
      </div>
    }
  };
};
