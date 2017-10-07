
let module Styles = {
  let selectedFilter = Glamor.(css [
    fontSize "20px",
    padding "4px 8px",
    cursor "pointer",
    borderRadius "4px",
    Selector ":hover" [
      backgroundColor "#eee"
    ]
  ]);
  let input = Glamor.(css[
    border "none",
    outline "none",
    alignSelf "stretch",
    flex "1",
    fontSize "24px",
    padding "4px"
  ]);
  let topBar = Glamor.(css[
    fontSize "32px",
    padding "16px",
    flexDirection "row",
    alignItems "center",
  ]);
  let container = Glamor.(css[
    backgroundColor "white",
    /* border "1px solid #777", */
    boxShadow "0px 2px 10px #aaa",
    margin "0 16px",
    borderRadius "4px",
    position "sticky",
    top "16px"
  ]);
  let results = Glamor.(css[
    backgroundColor "white",
    boxShadow "0px 2px 10px #aaa",
    borderRadius "4px",
    position "absolute",
    top "100%",
    marginTop "16px",
    overflow "auto",
    maxHeight "500px",
    left "0",
    right "0",
  ]);
  let result = Glamor.(css[
    padding "12px",
    fontSize "20px",
    cursor "pointer",
    Selector ":hover" [
      backgroundColor "#eee"
    ]
  ]);
};

let str = ReasonReact.stringToElement;
let evtValue event => (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;

/**
 * For testing:
 * - garlic
 * - heavy cream
 * - dark chocolate
 */

/**
 * When doing tags, would be good to keep track of how many times a tag is used.
 * Will need transactions to  do that right I think.
**/

type result = Tag Models.tag | Ingredient Models.ingredient;

let search ingredients tags text => {
  [%guard let true = String.length text > 0][@else [||]];
  let text = Js.String.toLowerCase text;
  Array.append
  (Js.Array.filter
  (fun ingredient => {
    let name = Js.String.toLowerCase ingredient##name;
    Js.String.includes text name
  })
  ingredients |> Array.map (fun ing => Ingredient ing))
  (Js.Array.filter
  (fun tag => {
    let name = Js.String.toLowerCase tag##name;
    Js.String.includes text name
  })
  tags
  |> Array.map (fun tag => Tag tag))
};

/**
 * Ok, so the performance under offline is terrible, because it's doing
 * 
 * - try to make a request
 * - exponential backoff, twice
 * - decide we're not online
 * 
 * - try to make a request
 * - backoff from the past backoff
 * - wait forever
 * 
 * Things that need to change:
 * - reset the backoff when making a new query
 * - listen to browser's "online" vs "offline", and if you're offline, don't even try
 * 
 * 
 * Hmmmm maybe the problem is we're setting online state to healthy, when it's definitely not.
 * RemoteStore.prototype.onWatchStreamChange
 * 
 * 
 * PersistentListenStream.prototype.onMessage
 * huh we're resetting the backoff here, what's happening?
 * 
 * dunno how that's happening
 */

let spacer num => <div style=ReactDOMRe.Style.(make flexBasis::(string_of_int num ^ "px") ()) />;

let component = ReasonReact.reducerComponent "SearchBar";
let empty = ("", []: list Models.ingredient, []: list Models.tag);
let make ::onChange current::(text, selectedIngredients, selectedTags) ::ingredients ::tags ::enabled _children => ReasonReact.{
  ...component,
  initialState: fun () => "",
  reducer: fun action state => ReasonReact.Update action,
  render: fun {state, reduce} => {
    let removeIngredient ingredient => onChange (
      text,
      List.filter (fun ing => ing##id !== ingredient##id) selectedIngredients,
      selectedTags,
    );

    <div className=Styles.container>
      <div className=Styles.topBar>
        {List.map
          (fun ingredient => {
            <div
              key=ingredient##id
              className=Styles.selectedFilter
              onClick=(fun _ => removeIngredient ingredient)
            >(str ingredient##name)</div>
          })
         selectedIngredients
         |> Array.of_list
         |> Js.Array.reverseInPlace
         |> ReasonReact.arrayToElement
        }
        (selectedIngredients !== [] || selectedTags !== [] ? spacer 16 : ReasonReact.nullElement)
        <input
          value=state
          onChange=(reduce (fun evt => evtValue evt))
          placeholder="Search by ingredient, tag, or title"
          /* disabled=(not enabled |> Js.Boolean.to_js_boolean) */
          className=Styles.input
        />
      </div>

      {
        let results = (search ingredients tags state);
        if (results != [||]) {
          <div className=Styles.results>
            (ReasonReact.arrayToElement (Array.mapi
            (fun i result => {
              <div key=(string_of_int i) className=Styles.result onClick=(fun _ => {
                Js.log result;
                let (selectedIngredients, selectedTags) = switch result {
                | Tag tag => (selectedIngredients, [tag, ...selectedTags])
                | Ingredient ing => ([ing, ...selectedIngredients], selectedTags)
                };
                onChange (text, selectedIngredients, selectedTags);
                (reduce (fun _ => "")) ();
              })>
                {let name = switch result {
                | Tag tag => tag##name
                | Ingredient ingredient => ingredient##name
                };
                (str name)}
              </div>
            })
            results))
          </div>
        } else {
          ReasonReact.nullElement
        }
      }
    </div>
  }
};
