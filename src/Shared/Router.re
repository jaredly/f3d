type history;

[@bs.val] external history : history = "";

[@bs.send] external pushState : (history, Js.t({..}), Js.t({..}), string) => unit = "";

type window;

[@bs.val] external window : window = "";

[@bs.set] external onhashchange : (window, unit => unit) => unit = "";

type location;

[@bs.val] external location : location = "";

[@bs.get] external pathname : location => string = "";

[@bs.get] external hash : location => string = "";

[@bs.set] external setHash : (location, string) => unit = "hash";

let str = ReasonReact.stringToElement;

let switch_ = (path, routes, navigate) => {
  let rec loop = (routes) =>
    switch routes {
    | [] => <div> (str("Not found")) </div>
    | [`NotFound(fn), ..._] => fn(navigate)
    | [`Exact(route, fn), ...rest] =>
      if (route === path) {
        fn(navigate)
      } else {
        loop(rest)
      }
    | [`Prefix(prefix, fn), ...rest] =>
      if (Js.String.startsWith(prefix, path)) {
        fn(
          navigate,
          Js.String.slice(~from=Js.String.length(prefix), ~to_=Js.String.length(path), path)
        )
      } else {
        loop(rest)
      }
    };
  loop(routes)
};

let currentHash = () => hash(location) |> Js.String.sliceToEnd(~from=1);

let component = ReasonReact.reducerComponent("Router");

let make = (~render, ~routes, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => currentHash(),
    didMount: ({state, reduce}) => {
      onhashchange(window, (_) => {
        [%bs.raw {|window.Bugsnag && (Bugsnag.context = window.location.hash, Bugsnag.refresh())|}];
        (reduce(currentHash))()
      });
      ReasonReact.NoUpdate
    },
    reducer: (action, state) => ReasonReact.Update(action),
    render: ({state, reduce}) => {
      let navigate = (path) =>
        setHash
          (location, "#" ++ path);
          /* pushState history {"um": "ok"} {"a": "b"}  */
      /* (reduce (fun path => path)); */
      render(switch_(state, routes, navigate), navigate)
    }
  };
