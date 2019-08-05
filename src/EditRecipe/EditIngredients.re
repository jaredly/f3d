open Utils;

open BaseUtils;

let ingredientsMap = (list) => {
  let map = Js.Dict.empty();
  List.iter((ing) => Js.Dict.set(map, ing##id, ing), list);
  map
};

module Styles = {
  open Glamor;
  let name = css([]);
  let unknownName = css([fontStyle("italic")]);
  let comment = css([padding("2px"), fontStyle("italic"), color("#777")]);
  let remove =
    css([
      backgroundColor("transparent"),
      border("none"),
      padding("4px 8px"),
      cursor("pointer"),
      Selector(":hover", [backgroundColor("#eee")])
    ]);
};

let newIngredient = (id, text, uid) => {
  "id": id,
  "name": text,
  "authorId": uid,
  "plural": Js.null,
  "defaultUnit": Js.null,
  "alternativeNames": [||],
  "created": Js.Date.now(),
  "calories": Js.null,
  "diets": [||],
  "aisle": Js.null
};

let addIngredient = (fb, uid, text) => {
  let id = uuid();
  module C = Firebase.Collection(Models.Ingredient);
  let collection = C.get(fb);
  let doc = Firebase.doc(collection, id);
  let newIngredient = newIngredient(id, text, uid);
  Firebase.set(doc, newIngredient) |> Js.Promise.then_((_) => Js.Promise.resolve(id))
};

let blankIngredient = () => {
  "id": uuid(),
  "ingredient": Models.Text(""),
  "amount": Js.null,
  "unit": Js.null,
  "comments": Js.null
};

let clone: Js.t('a) => Js.t('a) = (obj) => Js.Obj.assign(Js.Obj.empty(), obj);

let change =
    (ingredients, ingredientHeaders, onChange, fn, i, ingredient: Models.maybeRecipeIngredient) => {
  let ingredient = Obj.magic(clone(ingredient));
  fn(ingredient);
  let ingredients = Array.copy(ingredients);
  if (i == Array.length(ingredients)) {
    Js.Array.push(ingredient, ingredients) |> ignore
  } else {
    ingredients[i] = ingredient
  };
  onChange((ingredientHeaders, ingredients))
};

let changeHeader = (ingredients, ingredientHeaders, onChange, i, pos, text) => {
  let ingredientHeaders = Array.copy(ingredientHeaders);
  if (i == Array.length(ingredientHeaders)) {
    Js.Array.push({"pos": pos, "text": text}, ingredientHeaders) |> ignore
  } else {
    ingredientHeaders[i] = {"pos": pos, "text": text}
  };
  onChange((ingredientHeaders, ingredients))
};

let emptyIngredient = () => {
  "id": uuid(),
  "ingredient": Models.Text(""),
  "amount": Js.null,
  "unit": Js.null,
  "comments": Js.null
};

let emptyToNull = (s) => s === "" ? Js.null : Js.Null.return(s);

let render = (~fb, ~ingredients, ~ingredientHeaders, ~allIngredients, ~onChange, ~onPaste) => {
  let%Lets.Opt.React uid = Firebase.Auth.fsUid(fb);
  /* let ingredientHeaders = [|(0, "the crumble"), (5, "For the eggs")|]; */
  let changeHeader = changeHeader(ingredients, ingredientHeaders, onChange);
  let change = change(ingredients, ingredientHeaders, onChange);
  let setIngredient = (idOrText: Models.idOrText) => change((ing) => ing##ingredient#=idOrText);
  let setUnit = (value: string) => change((ing) => ing##unit#=(emptyToNull(value)));
  let setComments = (value: string) => change((ing) => ing##comments#=(emptyToNull(value)));
  let setAmount = (value: option(float)) =>
    change((ing) => ing##amount#=(Js.Null.fromOption(value)));
  let remove = (i) => {
    let ingredients = Array.copy(ingredients);
    Js.Array.spliceInPlace(~pos=i, ~remove=1, ~add=[||], ingredients) |> ignore;

    /*** TODO move up headers */
    onChange((
      Array.map(
        (head) => head##pos > i ? {"pos": head##pos - 1, "text": head##text} : head,
        ingredientHeaders
      ),
      ingredients
    ))
  };
  let removeHeader = (i) => {
    let ingredientHeaders = Array.copy(ingredientHeaders);
    Js.Array.spliceInPlace(~pos=i, ~remove=1, ~add=[||], ingredientHeaders) |> ignore;
    onChange((ingredientHeaders, ingredients))
  };
  let ingredientsAndHeaders = Array.mapi((i, ing) => `Ingredient((i, ing)), ingredients);
  Array.iteri(
    (i, head) => {
      let dest = head##pos + i;
      Js.Array.spliceInPlace(
        ~pos=dest,
        ~remove=0,
        ~add=[|`Header((i, head))|],
        ingredientsAndHeaders
      )
      |> ignore
    },
    ingredientHeaders
  );
  let addEmptyAfter = (i) => {
    let ingredients = Array.copy(ingredients);
    Js.Array.spliceInPlace(~pos=i + 1, ~remove=0, ~add=[|emptyIngredient()|], ingredients)
    |> ignore;
    onChange((ingredientHeaders, ingredients))
  };
  let onDrop = ((key, destkey, isAfter)) => {
    let k1 = int_of_string(key);
    let k2 = int_of_string(destkey);
    if (! isAfter && k2 === k1 + 1) {
      ()
    } else {
      let ingredientsAndHeaders = Array.copy(ingredientsAndHeaders);
      let item = ingredientsAndHeaders[k1];
      Js.Array.spliceInPlace(~pos=k1, ~remove=1, ~add=[||], ingredientsAndHeaders) |> ignore;
      Js.Array.spliceInPlace(
        ~pos=k2 > k1 ? k2 - 1 : k2,
        ~remove=0,
        ~add=[|item|],
        ingredientsAndHeaders
      )
      |> ignore;
      Array.fold_left(
        ((headers, ingredients), item) =>
          switch item {
          | `BlankIngredient(_)
          | `BlankHeader(_) => (headers, ingredients)
          | `Ingredient(_, ing) => (headers, Js.Array.concat([|ing|], ingredients))
          | `Header(_, head) => (
              Js.Array.concat([|{"pos": Array.length(ingredients), "text": head##text}|], headers),
              ingredients
            )
          },
        ([||], [||]),
        ingredientsAndHeaders
      )
      |> (
        (a) => {
          Js.log(a);
          a
        }
      )
      |> onChange
    }
  };
  let positions = Js.Dict.empty();
  let map = ingredientsMap(allIngredients);
  let renderHeader = (joinedIndex, i, pos, text, isBlank) => {
    let key = string_of_int(joinedIndex);
    <tr
      key=("head:" ++ string_of_int(i))
      className=Glamor.(css([verticalAlign("top")]))
      ref=?(
        isBlank ?
          None :
          Some(
            (node) =>
               Js.Nullable.toOption(node) |> optMap((node) => Js.Dict.set(positions, key, node)) |> ignore
          )
      )>
      <td
        onMouseDown=?(
          isBlank ? None : Some(DragNDrop.startMoving(~positions, ~key, ~margin=4, ~onDrop))
        )
        style=?(isBlank ? None : Some(ReactDOMRe.Style.(make(~cursor="pointer", ()))))>
        (isBlank ? ReasonReact.null : str({j|↕|j}))
      </td>
      <td className=Glamor.(css([width("8px")])) />
      <td colSpan=7>
        <input
          value=text
          onChange=((evt) => changeHeader(i, pos, evtValue(evt)))
          className=Glamor.(
                      css([
                        width("100%"),
                        boxSizing("border-box"),
                        fontWeight("600"),
                        paddingLeft("8px")
                      ])
                    )
          placeholder="Header text"
        />
      </td>
      <td>
        (
          isBlank ?
            ReasonReact.null :
            <button onClick=((_) => removeHeader(i)) className=Styles.remove>
              (str("remove"))
            </button>
        )
      </td>
    </tr>
  };
  let renderIngredient = (joinedIndex, i, ingredient, isBlank) => {
    let key = string_of_int(joinedIndex);
    <tr
      key=("ing:" ++ string_of_int(i))
      className=Glamor.(css([verticalAlign("top")]))
      ref=?(
        isBlank ?
          None :
          Some(
            (node) =>
               Js.Nullable.toOption(node) |> optMap((node) => Js.Dict.set(positions, key, node)) |> ignore
          )
      )>
      <td
        onMouseDown=?(
          isBlank ? None : Some(DragNDrop.startMoving(~positions, ~key, ~margin=4, ~onDrop))
        )
        style=?(isBlank ? None : Some(ReactDOMRe.Style.(make(~cursor="pointer", ()))))>
        (isBlank ? ReasonReact.null : str({j|↕|j}))
      </td>
      <td className=Glamor.(css([width("8px")])) />
      <td>
        <AmountInput
          value=(ingredient##amount |> Js.Null.toOption)
          onChange=((value) => setAmount(value, i, ingredient))
          onPaste=(PasteUtils.parseIngredients(allIngredients, onPaste))
          className=Glamor.(css([width("70px"), paddingLeft("8px")]))
          placeholder="Num"
        />
      </td>
      <td className=Glamor.(css([width("8px")])) />
      <td>
        <input
          value=(ingredient##unit |> Js.Null.toOption |> optOr(""))
          onChange=((evt) => setUnit(evtValue(evt), i, ingredient))
          className=Glamor.(css([width("100px"), paddingLeft("8px")]))
          placeholder="Unit"
        />
      </td>
      <td className=Glamor.(css([width("16px")])) />
      <td>
        <IngredientInput
          value=ingredient##ingredient
          ingredientsMap=map
          addIngredient=(addIngredient(fb, uid))
          onChange=((id) => setIngredient(id, i, ingredient))
        />
      </td>
      <td className=Glamor.(css([width("16px")])) />
      <td>
        <Textarea
          value=(ingredient##comments |> Js.Null.toOption |> optOr(""))
          onChange=((text) => setComments(text, i, ingredient))
          onReturn=((_, _) => addEmptyAfter(i))
          className=Glamor.(css([border("1px solid rgb(200, 200, 200)"), padding("4px")]))
        />
      </td>
      <td>
        (
          isBlank ?
            ReasonReact.null :
            <button onClick=((_) => remove(i)) className=Styles.remove> (str("remove")) </button>
        )
      </td>
    </tr>
  };
  <table
    className=Glamor.(
                css([
                  borderCollapse("collapse"),
                  verticalAlign("top"),
                  fontSize("16px"),
                  width("100%")
                ])
              )>
    <tbody>
      (
        Array.mapi(
          (i, item) =>
            switch item {
            | `Ingredient(ii, ingredient) => renderIngredient(i, ii, ingredient, false)
            | `BlankIngredient(ingredient) =>
              renderIngredient(i, Array.length(ingredients), ingredient, true)
            | `BlankHeader(_) =>
              renderHeader(
                i,
                Array.length(ingredientHeaders),
                Array.length(ingredients) + 1,
                "",
                true
              )
            | `Header(ii, head) => renderHeader(i, ii, head##pos, head##text, false)
            },
          Js.Array.concat(
            [|`BlankIngredient(blankIngredient()), `BlankHeader((0, ""))|],
            ingredientsAndHeaders
          )
        )
        |> spacedArray(
             (i) => <tr key=(string_of_int(i) ++ "s") className=Glamor.(css([height("8px")])) />
           )
        |> ReasonReact.array
      )
    </tbody>
  </table>
};
