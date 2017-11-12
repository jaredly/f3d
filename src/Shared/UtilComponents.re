open Utils;

let toggleComponent = ReasonReact.reducerComponent("Toggle");
let toggle = (~initial, ~render) => {
  ...toggleComponent,
  initialState: () => initial,
  reducer: (newState, _state) => ReasonReact.Update(newState),
  render: ({state, reduce}) => {
    render(~on=state, ~toggle=reduce(() => !state))
  }
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
        } else switch (Js.Null.to_opt(node)) {
        | None => ()
        | Some(node) => {
          let fn = ([%bs.raw {|(function(node, cb){
            let obs = new IntersectionObserver((entries) => {
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
        (ReasonReact.arrayToElement(children))
      </div>
    }
  };
};
