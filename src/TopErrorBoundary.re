[@bs.module] external myJSReactClass : ReasonReact.reactClass = "../../../src/TopErrorBoundary.js";

let make = (children) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=myJSReactClass,
    ~props=Js.Obj.empty(),
    children
  );