/* type state = Loading | Loaded (array Models.recipe); */
open Utils;

let showError = (_err) => <div> (str("Failed to fetch")) </div>;

module Fetcher = FirebaseFetcher.Dynamic(Models.Recipe);

let component = ReasonReact.statelessComponent("SearchingRecipeList");

let make = (~fb, ~search, ~navigate, _children) => {
  let (text: string, ingredients, tags) = search;
  let refetchKey =
    text
    ++ (
      "%%"
      ++ (
        Js.Array.joinWith(":", Array.map((i) => i##id, Array.of_list(ingredients)))
        ++ ("%%" ++ Js.Array.joinWith(":", Array.map((i) => i##id, Array.of_list(tags))))
      )
    );
  let query =
    Firebase.Query.(
      (q) => {
        /*** Only recipes that have the specified ingredients */
        let q =
          List.fold_left(
            (q, ing) => q |> whereBool("ingredientsUsed." ++ ing##id, ~op="==", Js.true_),
            q,
            ingredients
          );
        let q =
          List.fold_left(
            (q, tag) => q |> whereBool("tags." ++ tag##id, ~op="==", Js.true_),
            q,
            tags
          );

        /*** TODO search bar should add texts to things */
        let q = q
          /*** Only public ones (TODO also fetch private ones I can see) */
          |> whereBool("isPrivate", ~op="==", Js.false_);
        if (ingredients === [] && tags == []) {
          q |> orderBy(~fieldPath="updated", ~direction="desc")
        } else {
          q
        }
      }
    );
  ReasonReact.{
    ...component,
    render: (_self) =>
      <Fetcher
        fb
        pageSize=20
        refetchKey
        query
        render=(
          (~state, ~fetchMore) =>
            switch state {
            | `Initial => RecipeList.empty
            | `Loaded(snap, recipes) =>
              RecipeList.showRecipes(~fb, ~navigate, ~recipes, ~loadingMore=snap !== None, ~fetchMore)
            | `Errored(err) => showError(err)
            }
        )
      />
  }
};