
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
        flexDirection "row",
        alignItems "center",
      ])>
        {List.map
          (fun ingredient => {
            <div
              key=ingredient##id
              className=Glamor.(css [
                fontSize "20px",
                padding "4px 8px",
                cursor "pointer",
                borderRadius "4px",
                Selector ":hover" [
                  backgroundColor "#eee"
                ]
              ])
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
          className=Glamor.(css[
            border "none",
            outline "none",
            alignSelf "stretch",
            flex "1",
            fontSize "24px",
            padding "4px"
          ])
        />
      </div>

      {
        let results = (search ingredients tags state);
        if (results != [||]) {
          <div className=Glamor.(css[
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
          ])>
            (ReasonReact.arrayToElement (Array.mapi
            (fun i result => {
              <div key=(string_of_int i) className=Glamor.(css[
                padding "12px",
                fontSize "20px",
                cursor "pointer",
                Selector ":hover" [
                  backgroundColor "#eee"
                ]
              ]) onClick=(fun _ => {
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
