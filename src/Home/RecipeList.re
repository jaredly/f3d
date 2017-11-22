open Utils;

module Styles = {
  open Glamor;
  let base = [
    padding("8px 16px"),
    flexDirection("row"),
    fontSize("24px"),
    lineHeight("36px"),
  ];
  let recipe = css @@ base @ [
    cursor("pointer"),
    paddingLeft("14px"),
    borderLeft("2px solid white"),
    transition(".3s ease border-left-color"),
    Selector(":hover", [
    borderLeftColor(Shared.actionLight),
      /* backgroundColor("#eee"), */
    ]),
  ];
  let blankRecipe = css @@ base @ [
    backgroundColor("#fafafa"),
    margin("8px"),
    padding("8px"),
    height("44px"),
    border("1px solid #fafafa")];
  let container = css([flexDirection("column"), flexWrap("wrap"), justifyContent("flex-start")]);
  let time = css([
    fontSize("16px"),
    fontFamily("sans-serif"),
  ]);
};

let empty =
  <div className=Styles.container>
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
    <div className=Styles.blankRecipe />
  </div>;

let showRecipe = (fb, navigate, recipe) => <div
  key=recipe##id
  className=Styles.recipe
  onClick=((_) => navigate("/recipe/" ++ recipe##id))
>
  <MadeItSummary
    fb
    id=recipe##id
    uid=(Firebase.Auth.fsUid(fb))
  />
  (spacer(16))
  <div>
    (str(recipe##title))
  </div>
  (spring)
  (spacer(16))
  <div className=Styles.time>
    (str(recipe##meta##totalTime |> Js.Null.to_opt |> BaseUtils.optMap(BaseUtils.hoursMinutes) |> BaseUtils.optOr("")))
  </div>
  /* <div>
    (str(recipe##created |> Js.Date.fromFloat |> Js.Date.toLocaleDateString))
  </div> */
</div>;

let showRecipes = (~fb, ~navigate, ~recipes, ~loadingMore, ~fetchMore) =>
  <div className=Styles.container>
    (
      ReasonReact.arrayToElement(
        Array.map(
          showRecipe(fb, navigate),
          recipes
        )
      )
    )
    (loadingMore
    ? <UtilComponents.OnVisible trigger=fetchMore>
      (str("Loading..."))
    </UtilComponents.OnVisible>
    : ReasonReact.nullElement)
    (spacer(128))
  </div>;