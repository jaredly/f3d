let component = ReasonReact.reducerComponent("App");

[%%bs.raw {|
  if (window.Bugsnag) {
    Bugsnag.notifyReleaseStages = ["production"];
    Bugsnag.enableNotifyUnhandledRejections();
    Bugsnag.releaseStage = __DEV__ ? 'development' : 'production'
  }
|}];

let str = ReasonReact.string;

module MyRouter = Router;
let make = (~fb, ~auth, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => Firebase.Auth.currentUser(auth) |>  Js.Nullable.toOption,
    reducer: (action, _) => ReasonReact.Update(action),
    didMount: ({send}) => {
      Firebase.Auth.onAuthStateChanged(
        auth,
        (user) => {
          [%bs.raw {|window.Bugsnag && (Bugsnag.user = user)|}];
          (send(user |>  Js.Nullable.toOption))
        }
      );
    },
    render: ({state}) => {
      let basicRoutes = [
        `Prefix(("/recipe/", (navigate, id) => <ViewRecipe fb navigate id user=state />)),
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
                //  `Exact(("/shopping", (navigate) => <ShoppingList fb uid navigate />)),
                //  `Exact(("/pantry", (navigate) => <Pantry fb uid navigate />)),
               ]
             },
             [`Exact(("/login", (navigate) => <LogInPage auth navigate />))]
           );
      let routes = routes @ basicRoutes;
      <MyRouter
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