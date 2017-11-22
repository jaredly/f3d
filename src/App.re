let component = ReasonReact.reducerComponent("App");

let str = ReasonReact.stringToElement;

let make = (~fb, ~auth, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => Firebase.Auth.currentUser(auth) |> Js.Nullable.to_opt,
    reducer: (action, _) => ReasonReact.Update(action),
    didMount: ({reduce}) => {
      Firebase.Auth.onAuthStateChanged(
        auth,
        (user) => (reduce((_) => user |> Js.Nullable.to_opt))()
      );
      ReasonReact.NoUpdate
    },
    render: ({state}) => {
      let basicRoutes = [
        `Prefix(("/recipe/", (navigate, id) => <ViewRecipe fb navigate id />)),
        `Prefix(("/list/", (navigate, id) => <ListView fb navigate id />)),
        `NotFound((navigate) => <FrontPage fb navigate />)
      ];
      let routes =
        state
        |> BaseUtils.optFold(
             (user) => {
               let uid = Firebase.Auth.uid(user);
               [
                 `Exact(("/add", (navigate) => <AddRecipe fb uid navigate />)),
                 `Exact(("/lists", (navigate) => <UserLists fb uid navigate />)),
               ]
             },
             [`Exact(("/login", (navigate) => <LogInPage auth navigate />))]
           );
      let routes = routes @ basicRoutes;
      <Router
        routes
        render=(
          (child, navigate) =>
            <div style=ReactDOMRe.Style.(make(~color=Shared.dark, ()))>
              <Header auth navigate />
              child
            </div>
        )
      />
    }
  };