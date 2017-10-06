let component = ReasonReact.statelessComponent "App";

let str = ReasonReact.stringToElement;

let make ::fb _children => {
  ReasonReact.{
    ...component,
    render: fun {state} => {
      <div>
        <div className=Glamor.(css[fontSize "32px", padding "16px", flexDirection "row"])>
          (str "Foood")
        </div>
        <FrontPage fb /> 
      </div>
    }
  }
};
