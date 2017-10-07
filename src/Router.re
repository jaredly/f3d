
type history;
external history: history = "" [@@bs.val];
external pushState: history => string => Js.t {..} => unit = "" [@@bs.send];

type location;
external location: location = "" [@@bs.val];
external pathname: location => string = "" [@@bs.get];

let str = ReasonReact.stringToElement;

let match path routes navigate => {
  let rec loop routes => {
    switch routes {
    | [] => <div>(str "Not found")</div>
    | [`NotFound fn, ..._] => fn navigate
    | [`Prefix (prefix, fn), ...rest] => if (Js.String.startsWith prefix path) {
        fn navigate (Js.String.slice from::(Js.String.length prefix) to_::(Js.String.length path) path)
      } else {
        loop rest
      }
    }
  };
  loop routes;
};

let component = ReasonReact.reducerComponent "Router";
let make ::render ::routes _children => ReasonReact.{
  ...component,
  initialState: fun () => pathname location,
  reducer: fun action state => {
    pushState history action {"um": "ok"};
    ReasonReact.Update action
  },
  render: fun {state, reduce} => {
    let navigate = (reduce (fun path => path));
    render (match state routes navigate) navigate;
  }
};
