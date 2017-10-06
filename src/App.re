let component = ReasonReact.statelessComponent "App";

let str = ReasonReact.stringToElement;

let make ::fb _children => {
  ReasonReact.{
    ...component,
    render: fun {state} => {
      <div>
        (str "Hi")
        <FrontPage fb /> 
      </div>
    }
  }
};
