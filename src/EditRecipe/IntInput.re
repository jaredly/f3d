
open Utils;

let component = ReasonReact.statelessComponent "IntInput";

let make ::value ::onChange ::className=? _children => ReasonReact.{
  ...component,
  render: fun _ => {
    <input
      value=(switch value { | Some num => string_of_int num | None => "" })
      className=?className
      onChange=(fun evt => switch (evtValue evt) {
      | "" => onChange None
      | text => onChange (Some (int_of_string text))
      })
    />
  }
}
