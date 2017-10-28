let str = ReasonReact.stringToElement;

let boxAttrs =
  Glamor.[
    width("200px"),
    height("200px"),
    margin("8px"),
    padding("16px"),
    fontSize("24px"),
    lineHeight("36px"),
    overflow("hidden"),
    border("1px solid #aaa")
  ];

let box = Glamor.css(boxAttrs);

let emptyBox = Glamor.(css([backgroundColor("#fafafa"), ...boxAttrs]));

let make = (~recipes) =>
  <div className=Glamor.(css([flexDirection("row"), flexWrap("wrap")]))>
    (
      ReasonReact.arrayToElement(
        switch recipes {
        | None => [|
            <div className=emptyBox key="1" />,
            <div className=emptyBox key="2" />,
            <div className=emptyBox key="3" />,
            <div className=emptyBox key="4" />,
            <div className=emptyBox key="5" />,
            <div className=emptyBox key="6" />,
            <div className=emptyBox key="7" />,
            <div className=emptyBox key="8" />
          |]
        | Some(recipes) =>
          Array.map(
            (recipe) => <div key=recipe##id className=box> (str(recipe##title)) </div>,
            recipes
          )
        }
      )
    )
  </div>;
