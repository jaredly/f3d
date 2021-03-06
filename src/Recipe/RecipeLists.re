open Utils;

let module Styles = {
  open Glamor;
  let link = css([
    color("currentColor"),
    textDecoration("none"),
  ]);
  let check = css([color(Shared.action)]);
  let noCheck = css([
    color("#eee")
  ]);
  let item = css([
    flexDirection("row"),
    cursor("pointer"),
    Glamor.Selector(":hover", [
      Selector("& > ." ++ noCheck, [
        color(Shared.action)
      ])
    ])
  ]);
  Js.log2(item, noCheck);
  let listName = css([
    flex("1"),
    whiteSpace("nowrap"),
    overflow("hidden"),
    textOverflow("ellipsis"),
  ]);
  let title = css([
    fontWeight("bold"),
  ]);
  let container = css([
  ]);
};

let isMember = (recipes, recipeId) => Js.Dict.get(recipes, recipeId) |> BaseUtils.optFold(x => x, false);

let toggleRecipe = (list: Models.list, recipeId: string) => {
  let newRecipes = Js.Dict.fromArray(Js.Dict.entries(list##recipes));
  Js.Dict.set(newRecipes, recipeId, !isMember(list##recipes, recipeId) );
  let newList = Js.Obj.assign(Js.Obj.empty(), list);
  newList##recipes #= newRecipes;
  newList##updated #= (Js.Date.now());
  newList;
};

let showLists = (~fb, ~recipeId, ~lists, ~uid, ~navigate) => {
  let module Collection = Firebase.Collection(Models.List);
  let collection = Collection.get(fb);
  <div className=Styles.container>
    <div className=Styles.title>
      (str("Your lists"))
    </div>
    (spacer(8))
    (lists |> Array.of_list |> Array.map(
      (list) => {
        <div className=Styles.item key=list##id onClick=((evt) => {
          let updatedList = toggleRecipe(list, recipeId);
          /** TODO loading state */
          Firebase.set(Firebase.doc(collection, list##id), updatedList) |> ignore;
        })>
          <div className={isMember(list##recipes, recipeId)
            ? Styles.check
            : Styles.noCheck}>
            (str({j|✔|j}))
          </div>
          (spacer(8))
          <div className=Styles.listName>
          (str(list##title))
          </div>

          (spacer(8))
          <Link className=Styles.link navigate dest=("/list/" ++ list##id) text={j|🔗|j} />
        </div>
      }
    ) |> ReasonReact.array)
  </div>
};

module Fetcher = FirebaseFetcher.Stream(Models.List);
let component = ReasonReact.statelessComponent("RecipeLists");
[@react.component] let make = (~fb, ~uid, ~recipeId, ~navigate) => {
  let query = Firebase.Query.((q) => q |> whereStr("authorId", ~op="==", uid));
  ReactCompat.useRecordApi(ReasonReact.{
    ...component,
    render: (_self) =>
      <Fetcher
        fb
        query
        render=(
          (~state) =>
            switch state {
            | `Initial => <div/>
            | `Loaded(lists) => showLists(~fb, ~recipeId, ~lists, ~uid, ~navigate)
            | `Errored(err) => <div>(str("Failed to load"))</div>
            }
        )
      />
  })
};
