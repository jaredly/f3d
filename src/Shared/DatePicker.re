[@bs.module] external reactDatePicker : ReasonReact.reactClass = "react-datepicker";

[%%bs.raw "require('react-datepicker/dist/react-datepicker.css');"];

let make = (
  ~placeholderText: string,
  ~selected: MomentRe.Moment.t,
  ~onChange: (MomentRe.Moment.t) => unit,
  children
) =>
  ReasonReact.wrapJsForReason(
    ~reactClass=reactDatePicker,
    ~props={"placeholderText": placeholderText, "selected": selected, "onChange": onChange},
    children
  );