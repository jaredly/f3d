open Utils;

let module Styles = {
  open Glamor;
  let buttonStyles = [
    fontSize("16px"),
    border("none"),
    backgroundColor("#fff"),
    padding("0"),
    margin("0"),
    fontWeight("inherit"),
    /* color(Shared.action), */
    fontWeight("200"),
    cursor("pointer"),
    padding("4px 8px"),
    Selector(":hover", [color("black")]),
    outline("none"),
    borderLeft("2px solid transparent"),
    transition(".1s ease border-left-color"),
    Selector(":hover", [
      borderLeft("2px solid #ccc")
    ]),
    /* flexBasis("0"),
    textAlign("left"),
    padding("16px"),
    flex("1"),
    justifyContent("flex-end"), */
    flexDirection("row"),


    /* flex-basis: 0;
    text-align: left;
    padding: 16px; */
  ];

  let activeStar = css([
    transition(".3s ease color"),
    color("hsla(220, 100%, 48%, 1)"),
  ]);

  let activeLightStar = css([
    transition(".3s ease color"),
    color("hsla(220, 100%, 48%, 0.3)"),
  ]);

  let inactiveStar = css([
    transition(".3s ease color"),
    color("hsla(220, 0%, 68%, 1)"),
  ]);

  let inactiveLightStar = css([
    transition(".3s ease color"),
    color("hsla(220, 0%, 68%, 0.3)"),
  ]);

  let button = css(buttonStyles @ [
    Selector(":hover ." ++ inactiveLightStar, [
      color("hsla(220, 100%, 48%, 0.3)"),
    ]),
    Selector(":hover ." ++ inactiveStar, [
      color("hsla(220, 100%, 48%, 1)"),
    ])
   ]);

  let activeButton = css(buttonStyles @ [
    borderLeft("2px solid " ++ Shared.action),
    fontWeight("600"),
    Selector(":hover", [
      borderLeft("2px solid " ++ Shared.action)
    ]),
  ]);
};

let rec range = (~at=0, i) => at === i ? [] : [at, ...range(~at=at + 1, i)];

let showStars = (~active, ~rating) => {
  (Array.map((n) => {
    <div key=string_of_int(n) className=(switch (n <= rating, active) {
    | (true, true) => Styles.activeStar
    | (false, true) => Styles.activeLightStar
    | (true, false) => Styles.inactiveStar
    | (false, false) => Styles.inactiveLightStar
    })>
      (str({j|★|j}))
    </div>
  }, range(5) |> Array.of_list) |> ReasonReact.arrayToElement)
};

let component = ReasonReact.statelessComponent("RatingWidget");
let ratings = [|
  "Won't make again",
  "Might try again, but needs adjustments",
  "Want to make again",
  "Will definitely make again",
  "Will make regularly"
|];
let make = (~rating, ~onChange, _children) => {
  ...component,
  render: (_) => {
    <div>
      (Array.mapi((i, text) => {
        <div
          key=(string_of_int(i))
          className=(Some(i) == rating ? Styles.activeButton : Styles.button)
          onClick=((_) => Some(i) == rating ? onChange(None) : onChange(Some(i)))
        >
          (showStars(~active=Some(i) == rating, ~rating=i))
          (spacer(16))
          (str(text))
        </div>
      }, ratings) |> ReasonReact.arrayToElement)
    </div>
  }
};
