
type history;
external history: history = "" [@@bs.val];
external pushState: history => Js.t {..} => Js.t {..} => string => unit = "" [@@bs.send];

type window;
external window: window = "" [@@bs.val];
external onhashchange: window => (unit => unit) => unit = "" [@@bs.set];

type location;
external location: location = "" [@@bs.val];
external pathname: location => string = "" [@@bs.get];
external hash: location => string = "" [@@bs.get];
external setHash: location => string => unit = "hash" [@@bs.set];

let str = ReasonReact.stringToElement;

let match path routes navigate => {
  let rec loop routes => {
    switch routes {
    | [] => <div>(str "Not found")</div>
    | [`NotFound fn, ..._] => fn navigate
    | [`Exact (route, fn), ...rest] => if (route === path) {
        fn navigate
      } else {
        loop rest
      }
    | [`Prefix (prefix, fn), ...rest] => if (Js.String.startsWith prefix path) {
        fn navigate (Js.String.slice from::(Js.String.length prefix) to_::(Js.String.length path) path)
      } else {
        loop rest
      }
    }
  };
  loop routes;
};

let currentHash () => {
  hash location |> Js.String.sliceToEnd from::1
};

let component = ReasonReact.reducerComponent "Router";
let make ::render ::routes _children => ReasonReact.{
  ...component,
  initialState: fun () => currentHash(),
  didMount: fun {state, reduce} => {
    onhashchange window (fun _ => {
      (reduce currentHash) ()
    });
    ReasonReact.NoUpdate
  },
  reducer: fun action state => {
    ReasonReact.Update action
  },
  render: fun {state, reduce} => {
    let navigate path => {
      setHash location ("#" ^ path);
      /* pushState history {"um": "ok"} {"a": "b"}  */
    };
    /* (reduce (fun path => path)); */
    render (match state routes navigate) navigate;
  }
};
