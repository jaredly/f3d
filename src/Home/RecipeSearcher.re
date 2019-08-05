/* type state = Loading | Loaded (array Models.recipe); */
open Utils;

let showError = (_err) => <div> (str("Failed to search")) </div>;

module Fetcher = FirebaseFetcher.Dynamic(Models.Recipe);

let make = (~initial=RecipeList.empty, ~showError=showError, ~render, ~fb, ~search, ~navigate, _children) => {
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
            (q, ing) => q |> whereBool("ingredientsUsed." ++ ing##id, ~op="==", true),
            q,
            ingredients
          );
        let q =
          List.fold_left(
            (q, tag) => q |> whereBool("tags." ++ tag##id, ~op="==", true),
            q,
            tags
          );

        /*** TODO search bar should add texts to things */
        let q = q
          /*** Only public ones (TODO also fetch private ones I can see) */
          |> whereBool("isPrivate", ~op="==", false);
        if (ingredients === [] && tags == []) {
          q |> orderBy(~fieldPath="updated", ~direction="desc")
        } else {
          q
        }
      }
    );

    Fetcher.make(
      ~fb,
      // ~pageSize=200,
      ~refetchKey,
      ~query,
      ~render=(
        (~state, ~fetchMore) =>
          switch state {
          | `Initial => initial
          | `Errored(err) => showError(err)
          | `Loaded(snap, recipes) => render(~fb, ~navigate, ~recipes, ~loadingMore=snap !== None, ~fetchMore)
          }
      ),
      [||]
    )
};
