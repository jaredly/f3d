
/* type state = Loading | Loaded (array Models.recipe); */

let component = ReasonReact.reducerComponent "App";
let str = ReasonReact.stringToElement;

type action = (array Models.recipe);

type state = (list string, list string, string);

let make ::fb ::navigate _children => {
  ReasonReact.{
    ...component,
    reducer: fun action state => {
      ReasonReact.Update action
    },
    initialState: fun () => SearchBar.empty,
    render: fun {state, reduce} => {
      <div className=Glamor.(css[maxWidth "100%", width "1000px", alignSelf "center"])>
        (LoadedSearchBar.render fb state (reduce (fun x => x)))
        <div className=Glamor.(css[flexBasis "16px"])/>
        <SearchingRecipeList fb search=state navigate />
      </div>
    }
  }
};
