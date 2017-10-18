let component = ReasonReact.reducerComponent "App";

let str = ReasonReact.stringToElement;

let make ::fb ::auth _children => {
  ReasonReact.{
    ...component,
    initialState: fun () => Firebase.Auth.currentUser auth |> Js.Nullable.to_opt,
    reducer: fun action _ => ReasonReact.Update action,
    didMount: fun {reduce} => {
      Firebase.Auth.onAuthStateChanged auth (fun user => {
        (reduce (fun _ => user |> Js.Nullable.to_opt)) ()
      });
      ReasonReact.NoUpdate
    },
    render: fun {state} => {
      let basicRoutes = [
        `Prefix ("/recipe/", (fun navigate id => <ViewRecipe fb navigate id />)),
        `NotFound (fun navigate => <FrontPage fb navigate />),
      ];
      let routes = state === None
        ? [
          `Exact ("/login", (fun navigate => <LogInPage auth navigate />)),
        ]
        : [
          `Exact ("/add", (fun navigate => <AddRecipe fb navigate />)),
        ];
      let routes = routes @ basicRoutes;
      <Router
        routes
        render=(fun child navigate => 
          <div>
            <Header auth navigate />
            child
          </div>
        )
      />
    }
  }
};
