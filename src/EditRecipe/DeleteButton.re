
open Utils;

let component = ReasonReact.reducerComponent "DeleteButton";

let button = Glamor.(css[

]);

let make ::onDelete _ => ReasonReact.{
  ...component,
  initialState: fun () => false,
  reducer: fun action _ => ReasonReact.Update action,
  render: fun {state, reduce} => {
    state
    ?
    <div className=Glamor.(css[flexDirection "row"])>
      <button
        className=button
        onClick=(reduce (fun _ => false))
      >
        (str "Just kidding")
      </button>
      <button
        className=button
        onClick=(fun _ => onDelete ())
      >
        (str "Really delete")
      </button>
    </div>
    :
    <div className=Glamor.(css[flexDirection "row"])>
      <button
        className=button
        onClick=(reduce (fun _ => true))
      >
        (str "Delete")
      </button>
    </div>
  }
};
