open Utils;

module Styles = {
  open Glamor;
  let boxAttrs = [
    width("300px"),
    height("200px"),
    margin("8px"),
    padding("16px"),
    paddingTop("8px"), /*** This makes it so the text is 16px from top & left corner */
    fontSize("24px"),
    lineHeight("36px"),
    overflow("hidden"),
    border("1px solid #aaa")
  ];
  let box = css @@ boxAttrs @ [cursor("pointer")];
  let emptyBox = css @@ [backgroundColor("#fafafa"), ...boxAttrs] @ [border("1px solid #fafafa")];
  let container = css([flexDirection("row"), flexWrap("wrap"), justifyContent("center")]);
};

let empty =
  <div className=Styles.container>
    <div className=Styles.emptyBox key="1" />
    <div className=Styles.emptyBox key="2" />
    <div className=Styles.emptyBox key="3" />
    <div className=Styles.emptyBox key="4" />
    <div className=Styles.emptyBox key="5" />
    <div className=Styles.emptyBox key="6" />
    <div className=Styles.emptyBox key="7" />
    <div className=Styles.emptyBox key="8" />
    <div className=Styles.emptyBox key="9" />
  </div>;

let showRecipes = (~navigate, ~recipes, ~loadingMore, ~fetchMore) =>
  <div className=Styles.container>
    (
      ReasonReact.arrayToElement(
        Array.map(
          (recipe) =>
            <div
              onClick=((_) => navigate("/recipe/" ++ recipe##id))
              key=recipe##id
              className=Styles.box>
              (str(recipe##title))
            </div>,
          recipes
        )
      )
    )
    (
      loadingMore ?
        <button onClick=((_) => fetchMore())> (str("More")) </button> : ReasonReact.nullElement
    )
  </div>;
