open Utils;

module Styles = {
  open Glamor;
  let container = css([maxWidth("100%"), width("800px"), alignSelf("center")]);
  let item = css([flexDirection("row"), alignItems("center")]);
  let link = css(RecipeStyles.leftBorderItem @ [color("currentColor"), textDecoration("none"),
  flex("1"),
  ]);
  let textContainer = "";
  let addText = "";
};

let recipeCount = (recipes) => Js.Dict.values(recipes) |> Js.Array.filter(Js.to_bool) |> Array.length;

let showLists = (~addList, ~lists, ~uid, ~navigate) =>
  <div className=Styles.container>
    (spacer(32))
    <BlurryInput
      value=""
      resetOnBlur=true
      render=(
        (~value, ~onChange as onChangeInner, ~onBlur) =>
          <input
            className=Styles.addText
            onChange=((evt) => onChangeInner(Utils.evtValue(evt)))
            placeholder="Type new list name & hit enter"
            onKeyDown=(
              (evt) =>
                if (ReactEventRe.Keyboard.key(evt) === "Enter") {
                  if (value !== "") {
                    addList(value);
                  };
                  onChangeInner("")
                }
            )
            value
            onBlur
          />
      )
    />
    (spacer(32))
    (
      lists
      |> Array.of_list
      |> Array.map(
           (list) =>
           <div className=Styles.item>
             <Link key=list##id className=Styles.link navigate dest=("/list/" ++ list##id) text=list##title />
             (str(string_of_int(recipeCount(list##recipes))))
             (spacer(16))
            </div>
         )
      |> ReasonReact.arrayToElement
    )
  </div>;

module Fetcher = FirebaseFetcher.Stream(Models.List);

let component = ReasonReact.statelessComponent("UserLists");

let make = (~fb, ~uid, ~navigate, _children) => {
  let query = Firebase.Query.((q) => q |> whereStr("authorId", ~op="==", uid));
  ReasonReact.{
    ...component,
    render: (_self) =>
      <Fetcher
        fb
        query
        render=(
          (~state) =>
            switch state {
            | `Initial => <div />
            | `Loaded(lists) => showLists(~lists, ~uid, ~navigate, ~addList=((title) => {
              let module Col = Firebase.Collection(Models.List);
              let col = Col.get(fb);
              let id = BaseUtils.uuid();
              let doc = Firebase.doc(col, id);
              Firebase.set(doc, {
                "id": id,
                "authorId": uid,
                "collaborators": Js.Dict.empty(),
                "isPrivate": Js.false_,
                "title": title,
                "created": Js.Date.now(),
                "updated": Js.Date.now(),
                "recipes": Js.Dict.empty()
              }) |> ignore;
            }))
            | `Errored(err) => <div> (str("Failed to load")) </div>
            }
        )
      />
  }
};