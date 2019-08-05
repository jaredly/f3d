type nu('t) = Js.null('t);

type parseResult = {
  .
  "title": string,
  "description": string,
  "ingredients": array(Models.recipeIngredient),
  "instructions": array(Models.instruction),
  "meta": Models.meta
};

[@bs.module "js/importRecipe.js"] external importRecipe : string => Js.Promise.t(parseResult) = "";

let fillOutRecipe = (url, fb, allIngredients, recipe) => {
  "title": recipe##title,
  "source": url,
  "instructions": recipe##instructions,
  "ingredients": recipe##ingredients |> Array.map(PasteUtils.matchIngredient(allIngredients)),
  "description": recipe##description,
  "meta": recipe##meta
};

let component = ReasonReact.reducerComponent("ImportInput");

let make = (~fb, ~allIngredients, ~onImport, _) =>
  ReasonReact.{
    ...component,
    initialState: () => None,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, reduce}) =>
      <input
        value=(
          switch state {
          | None => ""
          | Some(v) => v
          }
        )
        disabled=(bool.to_js_boolean(state !== None))
        placeholder="Paste in URL to import"
        onChange=(
          (evt) => {
            let url = Utils.evtValue(evt);
            (reduce((_) => Some(url)))();
            importRecipe(url)
            |> Js.Promise.then_(
                 (recipe) => {
                   (reduce((_) => None))();
                   onImport(fillOutRecipe(url, fb, Array.of_list(allIngredients), recipe));
                   Js.Promise.resolve()
                 }
               )
            |> Js.Promise.catch(
                 (err) => {
                   (reduce((_) => None))();
                   Js.log2("Failed to import", err);
                   Js.Promise.resolve()
                 }
               )
            |> ignore
          }
        )
      />
  };
