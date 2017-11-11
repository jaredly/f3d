/* type state = Loading | Loaded (array Models.recipe); */
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

let emptyBoxes = () =>
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
  ReasonReact.{
    ...component,
    render: (_self) =>
      <Fetcher
        fb
        pageSize=20
        refetchKey
        query=Firebase.Query.(
                (q) => {

                  /*** Only recipes that have the specified ingredients */
                  let q =
                    List.fold_left(
                      (q, ing) =>
                        q |> whereBool("ingredientsUsed." ++ ing##id, ~op="==", Js.true_),
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
                  q
                  /*** Only public ones (TODO also fetch private ones I can see) */
                  |> whereBool("isPrivate", ~op="==", Js.false_)
                }
              )
        render=(
          (~state, ~fetchMore) =>
            switch state {
            | `Initial => emptyBoxes()
            | `Loaded(snap, recipes) =>
              showRecipes(~navigate, ~recipes, ~loadingMore=snap !== None, ~fetchMore)
            | `Errored(err) => showError(err)
            }
        )
      />
  }
};
/* let component = ReasonReact.reducerComponent "App";
   let str = ReasonReact.stringToElement;

   type action = (array Models.recipe);

   let module Recipes = Firebase.Collection {let name = "recipes"; type t = Models.recipe; };

   let orr orr fn opt => switch opt { | None => orr | Some x => fn x };

   let make ::fb ::ingredients _children => {
     let recipes = Recipes.get fb;

     let search ::start ::ingredients => {
       let module Q = Firebase.Query;
       let q = Firebase.asQuery recipes
         |> Q.whereBool "isPrivate" op::"==" Js.false_
         |> Q.limit 10;
       let q = switch start { | None => q | Some start => Q.startAfter start q };
       let q = List.fold_left
       (fun q id => Q.whereBool ("ingredientsUsed." ^ id) op::"==" Js.true_ q)
       q
       ingredients;
       q
       /** TODO find ones I own that are private, and ones I collaborate on that are private */
     };

     let handleResult reduce prom => {
       prom
       |> Js.Promise.then_
       (fun snap => {
         let recipes = Array.map Firebase.data (Firebase.Query.docs snap);
         reduce (fun () => (snap, recipes)) ();
         Js.Promise.resolve ();
       })
       |> Js.Promise.catch (fun err => {
         Js.log2 "Failed to get recipes" err;
         Js.Promise.resolve ();
       })
       |> ignore;
     };

     ReasonReact.{
       ...component,
       /* willReceiveProps: fun self => {
         ReasonReact.NoUpdate
       }, */
       reducer: fun action state => {
         ReasonReact.Update (Some action)
       },
       didMount: fun {reduce} => {
         let module Q = Firebase.Query;
         search start::None ingredients::[]
         |> Q.get
         |> handleResult reduce;
         ReasonReact.NoUpdate
       },
       initialState: fun () => None,
       render: fun {state, reduce} => {
         <div>
         (str "Home")
         (RecipeList.make (switch state { | None => None | Some (_, recipes) => Some recipes }))
         </div>
       }
     }

   }; */
