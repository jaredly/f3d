
open Utils;

let ingredientsMap list => {
  let map = Js.Dict.empty ();
  List.iter
  (fun ing => Js.Dict.set map ing##id ing)
  list;
  map
};

let module Styles = {
  open Glamor;
  let name = css [

  ];
  let unknownName = css [
    fontStyle "italic",
  ];
  let comment = css [
    padding "2px",
    fontStyle "italic",
    color "#777",
  ];
};

let clone: Js.t 'a => Js.t 'a = fun obj => {
  Js.Obj.assign (Js.Obj.empty ()) obj
};

let render ::fb ::ingredients ::allIngredients ::onChange => {

  let setIngredient i (ingredient: Models.maybeRecipeIngredient) (idOrText: Models.idOrText) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##ingredient #= idOrText;
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let setUnit i (ingredient: Models.maybeRecipeIngredient) (value: string) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##unit #= (value === "" ? Js.null : Js.Null.return value);
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let setComments i (ingredient: Models.maybeRecipeIngredient) (value: string) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##comments #= (value === "" ? Js.null : Js.Null.return value);
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let setAmount i (ingredient: Models.maybeRecipeIngredient) (value: option float) => {
    let ingredient = Obj.magic (clone ingredient);
    ingredient##amount #= (Js.Null.from_opt value);
    let ingredients = Array.copy ingredients;
    ingredients.(i) = ingredient;
    onChange ingredients;
  };

  let remove i => {
    let ingredients = Array.copy ingredients;
    Js.Array.spliceInPlace pos::i remove::1 add::[||] ingredients
    |> ignore;
    onChange ingredients;
  };

  let addEmptyAfter i => {
    let ingredients = Array.copy ingredients;
    Js.Array.spliceInPlace pos::(i + 1) remove::0 add::[|
    {"id": uuid(), "ingredient": Models.Text "", "amount": Js.null, "unit": Js.null, "comments": Js.null}
    |] ingredients
    |> ignore;
    /** TODO transfer focus... maybe with a nextFocus::(i + 1) on the onChange?
     * Then track it here, retainedProp + refs?
     */
    onChange ingredients;
  };

  let map = ingredientsMap allIngredients;
  [%guard let Some uid = Firebase.Auth.fsUid fb][@else ReasonReact.nullElement];

  <table className=Glamor.(css[
    borderCollapse "collapse",
    verticalAlign "top",
    width "100%",
  ])>
  <tbody>
  (Array.mapi
    (fun i ingredient => {
      <tr key=(string_of_int i) className=Glamor.(css[verticalAlign "top"])>
        <td>
          <AmountInput
            value=(ingredient##amount |> Js.Null.to_opt)
            onChange=(fun value => setAmount i ingredient value)
            className=Glamor.(css[width "70px"])
          />
        </td>
        <td className=Glamor.(css[width "8px"]) />
        <td>
          <input
            value=(ingredient##unit |> Js.Null.to_opt |> optOr "")
            onChange=(fun evt => setUnit i ingredient (evtValue evt))
          />
        </td>
        <td className=Glamor.(css[width "16px"]) />
        <td>
          <IngredientInput
            value=ingredient##ingredient
            ingredientsMap=map
            addIngredient=(fun text => {
              let id = uuid();
              let module C = Firebase.Collection(Models.Ingredient);
              let collection = C.get fb;
              let doc = Firebase.doc collection id;
              let newIngredient = {
                "id": id,
                "name": text,
                "authorId": uid,
                "plural": Js.null,
                "defaultUnit": Js.null,
                "alternativeNames": [||],
                "created": Js.Date.now(),
                "calories": Js.null,
                "diets": [||],
                "aisle": Js.null,
              };
              Firebase.set doc newIngredient
              |> Js.Promise.then_ (fun _ => Js.Promise.resolve id)
            })
            onChange=(fun id => setIngredient i ingredient id)
          />
        </td>
        <td className=Glamor.(css[width "16px"]) />
        <td>
          <Textarea
            value=(ingredient##comments |> Js.Null.to_opt |> optOr "")
            onChange=(fun text => setComments i ingredient text)
            onReturn=(fun _ _ => addEmptyAfter i)
            className=Glamor.(css[
              border "1px solid rgb(200, 200, 200)",
              padding "4px",
            ])
          />
        </td>
        <td>
          <button
            onClick=(fun _ => remove i)
            className=Glamor.(css[
              backgroundColor "transparent",
              border "none",
              padding "4px 8px",
              cursor "pointer",
              Selector ":hover" [
                backgroundColor "#eee",
              ]
            ])
          >
            (str "remove")
          </button>
        </td>
      </tr>
    })
  ingredients
  /* |> Array.mapi (fun i row => <tr key={string_of_int i}>row</tr>) */
  |> spacedArray (fun i => <tr
    key=(string_of_int i ^ "s") 
    className=Glamor.(css[height "16px"])
  />)
  |> ReasonReact.arrayToElement)
  </tbody>
  </table>
};
