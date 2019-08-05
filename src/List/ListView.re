open Utils;

module RecipeFetcher = FirebaseFetcher.Single(Models.Recipe);

let module RecipeSummary = {
  let module Styles = {
    open Glamor;
    let title = css(RecipeStyles.leftBorderItem @ [
      textDecoration("none"),
      color("currentColor"),
    ]);
  };
  let show = (recipe, navigate) => {
    <div>
      <Link className=Styles.title navigate dest=("/recipe/" ++ recipe##id) text=(recipe##title)/>
    </div>
  };

  let make = (~id, ~fb, ~navigate, _children) => {
    RecipeFetcher.make(
      ~fb,
      ~id,
      ~render=(
        (~state) => switch state {
        | `Initial => <div/>
        | `Errored(err) => <div>(str("Failed to load recipe"))</div>
        | `Loaded(recipe) => show(recipe, navigate)
        }
      ),
      [||]
    )
  }
};

let module Styles = {
  open Glamor;
  let container = css([maxWidth("100%"), width("800px"), alignSelf("center")]);
  let item = css([
    padding("8px 16px"),
    cursor("pointer"),
    color("currentColor"),
    textDecoration("none"),
    Selector(":hover", [
      backgroundColor("#eee")
    ])
  ])
};

let showList = (~fb, ~list, ~uid, ~navigate) => {
  <div className=Styles.container>
    <div>
      <BlurryInput
        value=list##title
        onChange=((value) => {
          let module Collection = Firebase.Collection(Models.List);
          let collection = Collection.get(fb);
          let newList = Js.Obj.assign(Js.Obj.empty(), list);
          newList##title #= value;
          /** TODO loading state */
          Firebase.set(Firebase.doc(collection, list##id), newList) |> ignore;
        })
        render=(
          (~value, ~onChange as onChangeInner, ~onBlur) =>
            <input
              className=RecipeStyles.title
              onChange=((evt) => onChangeInner(Utils.evtValue(evt)))
              placeholder="Type new list name & hit enter"
              onKeyDown=(
                (evt) =>
                  if (ReactEvent.Keyboard.key(evt) === "Enter") {
                    onBlur(())
                  }
              )
              value
              onBlur=(Utils.ignoreArg(onBlur))
            />
        )
      />
      /* <div className=RecipeStyles.title> (str(list##title))</div> */
      <div>(str("To add to this list, navigate to a recipe and select this list in the right sidebar."))</div>
      (spacer(32))
      <div>
        (Js.Dict.keys(list##recipes)
        |> Array.map((id) => {
          <RecipeSummary key=id id fb navigate />
        }) |> ReasonReact.array)
      </div>
      (spacer(128))
      <div className=Glamor.(css([alignItems("flex-end")]))>
      <DeleteButton
        title="Delete list"
        onDelete=(() => {
          let module Collection = Firebase.Collection(Models.List);
          let collection = Collection.get(fb);
          Firebase.delete(Firebase.doc(collection, list##id))
          |> Js.Promise.then_((_) => {
            navigate("/lists");
            Js.Promise.resolve(())
          }) |> ignore;

        })
      />
      </div>
    </div>
  </div>
};

module Fetcher = FirebaseFetcher.Single(Models.List);
let make = (~fb, ~id, ~navigate, _children) => {
  Fetcher.make(
    ~fb,
    ~id,
    ~render=(
      (~state) =>
        switch state {
        | `Initial => <div/>
        | `Loaded(list) => showList(~fb, ~list, ~uid=Firebase.Auth.fsUid(fb), ~navigate)
        | `Errored(err) => <div>(str("Failed to load"))</div>
        }
    ),
    [||]
  )
};
