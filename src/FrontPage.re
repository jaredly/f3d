
/* type state = Loading | Loaded (array Models.recipe); */

let component = ReasonReact.reducerComponent "App";
let str = ReasonReact.stringToElement;

type action = (array Models.recipe);

/* let module Recipes = Firebase.Collection {let name = "recipes"; type t = Models.recipe; }; */

type state = (list string, list string, string);

let make ::fb _children => {
  /* let recipes = Recipes.get fb; */
  ReasonReact.{
    ...component,
    reducer: fun action state => {
      ReasonReact.Update action
    },
    initialState: fun () => [],
    /* , ["hello"], ""), */
    /* didMount: fun {reduce} => {
      let module Q = Firebase.Query;
      Firebase.asQuery recipes
      /* |> Q.limit 10 */
      |> Q.whereBool "isPrivate" op::"==" Js.false_
      |> Q.get
      |> Js.Promise.then_
      (fun snap => {
        let recipes = Array.map Firebase.data (Q.docs snap);
        reduce (fun () => recipes) ();
        Js.Promise.resolve ();
      })
      |> Js.Promise.catch (fun err => {
        Js.log2 "Failed to get recipes" err;
        Js.Promise.resolve ();
      })
      |> ignore;
      ReasonReact.NoUpdate
    }, */
    render: fun {state: ingredients, reduce} => {
      <div>
      (str "Home")
      <SearchingRecipeList
        fb
        ingredients
      />
      </div>
    }
  }

};
