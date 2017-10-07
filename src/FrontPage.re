
/* type state = Loading | Loaded (array Models.recipe); */

let component = ReasonReact.reducerComponent "App";
let str = ReasonReact.stringToElement;

type action = (array Models.recipe);

/* let module Recipes = Firebase.Collection {let name = "recipes"; type t = Models.recipe; }; */

type state = (list string, list string, string);

let module SearchBar = {
  let component = ReasonReact.statelessComponent "SearchBar";
  let empty = ("", []: list string, []: list string);
  let make ::onChange ::current _children => ReasonReact.{
    ...component,
    render: fun _ => {
      <div />
    }
  };
};

let id x => x;

let make ::fb _children => {
  /* let recipes = Recipes.get fb; */
  ReasonReact.{
    ...component,
    reducer: fun action state => {
      ReasonReact.Update action
    },
    initialState: fun () => SearchBar.empty,
    render: fun {state, reduce} => {
      <div className=Glamor.(css[width "1000px", alignSelf "center"])>
        <SearchBar
          onChange=(reduce id)
          current=state
        />
        <div className=Glamor.(css[
          backgroundColor "white",
          border "1px solid #777",
          fontSize "32px",
          padding "16px",
          position "sticky",
          top "16px"
        ])>
          (str "Search by ingredient")
        </div>
        <div className=Glamor.(css[flexBasis "16px"])/>
        <SearchingRecipeList fb search=state />
      </div>
    }
  }

};
