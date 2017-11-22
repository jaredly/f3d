let component = ReasonReact.statelessComponent("Link");

let make = (~navigate, ~dest, ~text, ~className=?, _) =>
  ReasonReact.{
    ...component,
    render: (_) =>
      <a
        onMouseDown=(
          (evt) => {
            ReactEventRe.Mouse.stopPropagation(evt);
            if (ReactEventRe.Mouse.button(evt) === 0
                && ReactEventRe.Mouse.metaKey(evt) === false
                && ReactEventRe.Mouse.ctrlKey(evt) === false
                && ReactEventRe.Mouse.shiftKey(evt) === false) {
              ReactEventRe.Mouse.preventDefault(evt);
              navigate(dest)
            }
          }
        )
        href=("#" ++ dest)
        ?className>
        (ReasonReact.stringToElement(text))
      </a>
  };
