open Glamor;

let container =
  css
    ([padding("0 16px"), width("800px"), maxWidth("100%"), alignSelf("center"), fontSize("20px")]);
    /* letterSpacing "1px", */

let loading = css([alignItems("center")]);

let title = css([fontSize("44px"), flex("1"), fontFamily("Abril Fatface, sans-serif")]);

let header =
  css([
    backgroundColor("white"),
    flexDirection("row"),
    alignItems("center"),
    position("sticky"),
    zIndex("10"),
    top("0px"),
    paddingBottom("8px"),
    paddingTop("8px")
  ]);

let buttonStyles = [
    fontSize("inherit"),
    border("none"),
    backgroundColor("transparent"),
    padding("0"),
    margin("0"),
    fontWeight("inherit"),
    color(Shared.action),
    fontWeight("200"),
    cursor("pointer"),
    padding("8px 16px"),
    Selector(":hover", [color("black")]),
    outline("none"),
    transition(".1s ease box-shadow, .1s ease transform"),
    Selector(":hover", [
      boxShadow("0 1px 5px #aaa"),
      transform("translateY(-1px)"),
    ]),
  ];

let button =
  css(buttonStyles);

let primaryButton = css( buttonStyles @ [
  backgroundColor(Shared.action),
  color(Shared.light),
]);

let editButton =
  css([
    textDecoration("none"),
    color("currentColor"),
    padding("8px 16px"),
    cursor("pointer"),
    color("#777"),
    Selector(":hover", [color("black")])
  ]);

let subHeader =
  css([
    alignItems("center"),
    flexDirection("row"),
    fontSize("36px"),
    fontWeight("400"),
    /* letterSpacing "1px", */
    fontVariant("small-caps")
  ]);
