
let component = ReasonReact.statelessComponent "Link";

let make ::navigate ::dest ::text ::className=? _ => ReasonReact.{
  ...component,
  render: fun _ => (
    <a
      onMouseDown=(fun evt => {
        if (ReactEventRe.Mouse.button evt === 0 &&
            ReactEventRe.Mouse.metaKey evt === false &&
            ReactEventRe.Mouse.ctrlKey evt === false &&
            ReactEventRe.Mouse.shiftKey evt === false
        ) {
          ReactEventRe.Mouse.preventDefault evt;
          navigate dest;
        }
      })
      href=("#" ^ dest)
      className=?className
    >
      (ReasonReact.stringToElement text)
    </a>
  )
}
