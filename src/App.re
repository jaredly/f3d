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
        `NotFound((navigate) => <FrontPage fb navigate />)
      ];
      let routes =
        state === None ?
          [`Exact(("/login", (navigate) => <LogInPage auth navigate />))] :
          [`Exact(("/add", (navigate) => <AddRecipe fb navigate />))];
      let routes = routes @ basicRoutes;
      <Router routes render=((child, navigate) => <div> <Header auth navigate /> child </div>) />
    }
  };
