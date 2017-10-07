
let str = ReasonReact.stringToElement;
let evtValue event => (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;

/**
 * When doing tags, would be good to keep track of how many times a tag is used.
 * Will need transactions to  do that right I think.
 */

let module SearchBar = {
  let component = ReasonReact.reducerComponent "SearchBar";
  let empty = ("", []: list string, []: list string);
  let make ::onChange ::current ::ingredients ::tags ::enabled _children => ReasonReact.{
    ...component,
    initialState: fun () => "",
    reducer: fun action state => ReasonReact.Update action,
    render: fun {state, reduce} => {
      <div className=Glamor.(css[
        backgroundColor "white",
        /* border "1px solid #777", */
        boxShadow "0px 2px 10px #aaa",
        borderRadius "4px",
        position "sticky",
        top "16px"
      ])>
        <div className=Glamor.(css[
          fontSize "32px",
          padding "16px",
        ])>
          <input
            value=state
            onChange=(reduce (fun evt => evtValue evt))
            placeholder="Search by ingredient, tag, or title"
            disabled=(not enabled |> Js.Boolean.to_js_boolean)
            className=Glamor.(css[
              border "none",
              outline "none",
              alignSelf "stretch",
              fontSize "24px",
              padding "4px"
            ])
          />
        </div>
      </div>
    }
  };
};

let module IngredientsLoader = FirebaseFetcher.Static {
  let name = "ingredients";
  type t = Models.ingredient;
  let query q => q;
};

let module TagsLoader = FirebaseFetcher.Static {
  let name = "tags";
  type t = Models.tag;
  let query q => q;
};

/** TODO maybe have the searchbar w/ a loading spinner, and have it only
 * load the data upon focus, so that I don't load all this stuff without
 * need. */
let loadedSearchBar fb state onChange => {
  <IngredientsLoader
    fb pageSize=1000
    render=(fun ::state ::fetchMore => {
      let (ingredientsLoaded, ingredients) = switch state {
      | IngredientsLoader.Inner.Loaded _ items => (true, items)
      | Errored err => {
        Js.log2 "Failed to load" err;
        (false, [||])
      }
      | _ => (false, [|
          /* {"id": "47386447a54b9", "name": "awesome", "plural": Js.Nullable.null, "aisle": Js.Nullable.null, "alternativeNames": [||], "authorId": "", "calories": Js.Nullable.null, "created": 0.0, "defaultUnit": Js.Nullable.null, "diets": [||]},
          {"id": "6adf4409f55c8", "name": "sauces", "plural": Js.Nullable.null, "aisle": Js.Nullable.null, "alternativeNames": [||], "authorId": "", "calories": Js.Nullable.null, "created": 0.0, "defaultUnit": Js.Nullable.null, "diets": [||]},
          {"id": "f1068ba53c45a", "name": "garlic", "plural": Js.Nullable.null, "aisle": Js.Nullable.null, "alternativeNames": [||], "authorId": "", "calories": Js.Nullable.null, "created": 0.0, "defaultUnit": Js.Nullable.null, "diets": [||]}, */
      |])
      };
      <TagsLoader
        fb pageSize=1000
        render=(fun ::state ::fetchMore => {
          let (tagsLoaded, tags) = switch state {
          | TagsLoader.Inner.Loaded _ items => (true, items)
          | Errored err => {
            Js.log2 "Failed to load" err;
            (false, [||])
          }
          | _ => (false, [||])
          };
          <SearchBar
            tags
            ingredients
            enabled=(ingredientsLoaded && tagsLoaded)
            onChange
            current=state
          />
        })
      />
    })
  />
};
