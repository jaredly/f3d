open Utils;

let component = ReasonReact.statelessComponent("IntInput");

[@react.component] let make = (~value, ~onChange, ~className=?) =>
  ReactCompat.useRecordApi(ReasonReact.{
    ...component,
    render: (_) =>
      <input
        value=(
          switch value {
          | Some(num) => string_of_int(num)
          | None => ""
          }
        )
        ?className
        onChange=(
          (evt) =>
            switch (evtValue(evt)) {
            | "" => onChange(None)
            | text => onChange(Some(int_of_string(text)))
            }
        )
      />
  });
