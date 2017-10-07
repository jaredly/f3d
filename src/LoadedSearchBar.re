
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
let render fb outerState onChange => {
  <IngredientsLoader
    fb pageSize=1000
    render=(fun ::state ::fetchMore => {
      open IngredientsLoader.Inner;
      let (ingredientsLoaded, ingredients) = switch state {
      | Loaded _ items => (true, items)
      | Errored err => { Js.log2 "Failed to load ingredients" err; (false, [||]) }
      | _ => (false, [||])
      };

      <TagsLoader
        fb pageSize=1000
        render=(fun ::state ::fetchMore => {
          open TagsLoader.Inner;
          let (tagsLoaded, tags) = switch state {
          | Loaded _ items => (true, items)
          | Errored err => { Js.log2 "Failed to load tags" err; (false, [||]) }
          | _ => (false, [||])
          };

          <SearchBar
            tags
            ingredients
            enabled=(ingredientsLoaded && tagsLoaded)
            onChange
            current=outerState
          />
        })
      />
    })
  />
};
