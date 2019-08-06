// [@bs.module] external reactDatePicker : ReasonReact.reactClass = "react-datepicker";

[%%bs.raw "require('react-datepicker/dist/react-datepicker.css');"];

[@bs.module "react-datepicker"][@react.component]
external make: (
  ~placeholderText: string,
  ~selected: MomentRe.Moment.t,
  ~onChange: (MomentRe.Moment.t) => unit,
) => React.element = "default";
