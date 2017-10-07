
/* type state = Loading | Loaded (array Models.recipe); */

let component = ReasonReact.reducerComponent "App";
let str = ReasonReact.stringToElement;

type action = (array Models.recipe);

type state = (list string, list string, string);

let make ::fb _children => {
  ReasonReact.{
    ...component,
    reducer: fun action state => {
      ReasonReact.Update action
    },
    initialState: fun () => SearchBar.SearchBar.empty,
    render: fun {state, reduce} => {
      <div className=Glamor.(css[width "1000px", alignSelf "center"])>
        (SearchBar.loadedSearchBar fb state (reduce (fun x => x)))
        <div className=Glamor.(css[flexBasis "16px"])/>
        <SearchingRecipeList fb search=state />
      </div>
    }
  }
};
