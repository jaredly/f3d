let component = ReasonReact.statelessComponent("Link");

let make = (~navigate, ~dest, ~text, ~className=?, _) =>
  ReasonReact.{
    ...component,
    render: (_) =>
      <a
        onMouseDown=(
          (evt) => {
            ReactEvent.Mouse.stopPropagation(evt);
            if (ReactEvent.Mouse.button(evt) === 0
                && ReactEvent.Mouse.metaKey(evt) === false
                && ReactEvent.Mouse.ctrlKey(evt) === false
                && ReactEvent.Mouse.shiftKey(evt) === false) {
              ReactEvent.Mouse.preventDefault(evt);
              navigate(dest)
            }
          }
        )
        href=("#" ++ dest)
        ?className>
        (ReasonReact.string(text))
      </a>
  };
