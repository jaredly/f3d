let component = ReasonReact.statelessComponent "App";

let str = ReasonReact.stringToElement;

let make ::fb _children => {
  ReasonReact.{
    ...component,
    render: fun _ => {
      <Router
        routes=[
          `Prefix ("/recipe/", (fun navigate id => <ViewRecipe fb navigate id />)),
          /* `Exact ("/add", (fun navigate => <AddPage fb navigate />)), */
          `NotFound (fun navigate => <FrontPage fb navigate />),
        ]
        render=(fun child navigate => 
          <div>
            <div className=Glamor.(css[fontSize "32px", padding "16px", flexDirection "row"])>
              (str "Foood")
              <a href="/add">
              </a>
            </div>
            child
          </div>
        )
      />
    }
  }
};
