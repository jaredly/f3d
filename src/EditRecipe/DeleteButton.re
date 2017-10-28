open Utils;

let component = ReasonReact.reducerComponent("DeleteButton");

let buttonBase =
  Glamor.[
    backgroundColor("#cfc"),
    cursor("pointer"),
    border("none"),
    fontFamily("inherit"),
    fontSize("inherit"),
    fontWeight("inherit"),
    Selector(":hover", [backgroundColor("#afa")])
  ];

let button = Glamor.css(buttonBase);

let redButton =
  Glamor.(
    css(buttonBase @ [backgroundColor("#fcc"), Selector(":hover", [backgroundColor("#faa")])])
  );

let make = (~onDelete, _) =>
  ReasonReact.{
    ...component,
    initialState: () => false,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, reduce}) =>
      state ?
        <div className=Glamor.(css([flexDirection("row")]))>
          <button className=button onClick=(reduce((_) => false))> (str("Just kidding")) </button>
          (spacer(32))
          <button className=redButton onClick=((_) => onDelete())> (str("Really delete")) </button>
        </div> :
        <div className=Glamor.(css([flexDirection("row")]))>
          <button className=redButton onClick=(reduce((_) => true))> (str("Delete")) </button>
        </div>
  };
