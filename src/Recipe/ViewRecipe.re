open Utils;

open BaseUtils;

module Styles = RecipeStyles;

type action =
  | ToggleMaking
  | StartEditing
  | StopEditing
  | StartSaving
  | DoneSaving
  | SetMaking((StringSet.t, IntSet.t))
  | SetBatches(float);

type state =
  | Making((StringSet.t, IntSet.t))
  | Editing
  | Saving
  | Normal;

let component = ReasonReact.reducerComponent("Recipe");

let saveRecipe = (~fb, ~recipe, onDone) => {
  module FB = Firebase.Collection(Models.Recipe);
  let collection = FB.get(fb);
  let doc = Firebase.doc(collection, recipe##id);
  Firebase.set(doc, recipe)
  |> Js.Promise.then_(
       () => {
         onDone();
         Js.Promise.resolve()
       }
     )
  |> ignore
};

let deleteRecipe = (~fb, ~recipe, onDone) => {
  module FB = Firebase.Collection(Models.Recipe);
  let collection = FB.get(fb);
  let doc = Firebase.doc(collection, recipe##id);
  Firebase.delete(doc)
  |> Js.Promise.then_(
       () => {
         onDone();
         Js.Promise.resolve()
       }
     )
  |> ignore
};

let renderEditor =
    (~making, ~ingredients, ~self as {ReasonReact.send}, ~fb, ~recipe, ~id, ~navigate) =>
  <EditRecipe
    saving=(making === Saving)
    allIngredients=ingredients
    recipe
    fb
    id
    onCancel=(((_) => send(StopEditing)))
    onSave=(
      (recipe) => {
        (send(StartSaving));
        saveRecipe(~fb, ~recipe, () => (send(DoneSaving)));
        ()
      }
    )
    onDelete=(
      () => {
        deleteRecipe(~fb, ~recipe, () => navigate("/"));
        ()
      }
    )
  />;

let make = (~navigate, ~recipe, ~ingredients, ~fb, ~id, _children) =>
  ReasonReact.{
    ...component,
    initialState: () => (1., Normal),
    reducer: (action, (batches, making)) =>
      ReasonReact.Update(
        switch action {
        | ToggleMaking => (
            batches,
            making === Normal ? Making((StringSet.empty, IntSet.empty)) : Normal
          )
        | SetMaking((ing, inst)) => (batches, Making((ing, inst)))
        | StartEditing => (batches, Editing)
        | StopEditing => (batches, Normal)
        | StartSaving => (batches, Saving)
        | DoneSaving => (batches, Normal)
        | SetBatches(batches) => (batches, making)
        }
      ),
    render: ({state: (batches, making), send} as self) => {
      let uid = Firebase.Auth.fsUid(fb);
      let canEdit = uid == Some(recipe##authorId);
      if ((making === Editing || making === Saving) && canEdit) {
      renderEditor(~making, ~ingredients, ~self, ~fb, ~recipe, ~id, ~navigate)

      } else {

      <div className=Styles.container>
        <div className=Styles.header>
          <div className=Styles.title> (str(recipe##title)) </div>
          (spacer(~className=Styles.notPhoneWidth, 16))
          <div
            className=(
              Styles.rightSide
              ++ " "
              ++ Glamor.(
                   css([
                     bottom("0"),
                     alignItems("flex-end"),
                     paddingBottom("16px"),
                     backgroundColor("white")
                   ])
                 )
            )>
            <button className=Styles.button onClick=(((_) => send(ToggleMaking)))>
              (str(making !== Normal ? "Done" : "Make"))
            </button>
            (
              canEdit ?
                <button className=Styles.button onClick=(((_) => send(StartEditing)))>
                  (str("Edit"))
                </button> :
                ReasonReact.null
            )
          </div>
        </div>
        (spacer(8))
        <div className=Glamor.(css([position("relative")]))>
        (Meta.metaLine(~meta=recipe##meta, ~source=recipe##source))
        ((making === Normal ? uid : None) |> BaseUtils.optFoldReact((uid) => {
          <div className=Styles.rightSide>
            <RecipeLists
              fb recipeId=id navigate uid
            />
          </div>
        }))
        </div>
        (spacer(16))
        <div className=Glamor.(css([whiteSpace("pre-wrap")]))>
          (orr("", Js.Null.toOption(recipe##description)) |> ifEmpty("No description") |> str)
        </div>
        (spacer(32))
        <div className=Styles.subHeader>
          (str("Ingredients"))
          (spacer(32))
          <div
            className=Glamor.(
                        css([
                          position("relative"),
                          fontSize("20px"),
                          flexDirection("row"),
                          flex("1")
                        ])
                      )>
            <AmountInput
              value=(Some(batches))
              onChange=(
                (value) =>
                  value |> optMap((value: float) => send(SetBatches(value))) |> ignore
              )
              className=Glamor.(css([width("40px")]))
            />
            (spacer(8))
            <div
              className=Glamor.(
                          css([textTransform("none"), fontVariant("none"), fontWeight("200")])
                        )>
              (str(batches === 1. ? "batch" : "batches"))
            </div>
            spring
            (making !== Normal ? <div className=Styles.rightSide>
              /* <Speaker
                   instructions=recipe##instructions
                   ingredients=recipe##ingredients
                   allIngredients=ingredients
                 />
                 (spacer(16)) */

                <Listener
                  ingredients=recipe##ingredients
                  allIngredients=ingredients
                  instructions=recipe##instructions
                />
              </div> : ReasonReact.null)
          </div>
        </div>
        (spacer(16))
        (
          Ingredients.render(
            ~batches,
            ~ingredients=recipe##ingredients,
            ~allIngredients=ingredients,
            ~making=
              switch making {
              | Making((ing, inst)) => Some((ing, ((ing) => send(SetMaking((ing, inst))))))
              | _ => None
              }
          )
        )
        (spacer(32))
        <div className=Styles.subHeader> (str("Instructions")) </div>
        (spacer(16))
        (
          Instructions.render(
            ~instructions=recipe##instructions,
            ~making=
              switch making {
              | Making((ing, inst)) => Some((inst, ((inst) => send(SetMaking((ing, inst))))))
              | _ => None
              }
          )
        )
        (spacer(32))
        <div className=Styles.subHeader> (str("Notes")) </div>
        (spacer(16))
        <div className=Glamor.(css([whiteSpace("pre-wrap")]))>
          (orr("", Js.Null.toOption(recipe##notes)) |> ifEmpty("No notes") |> str)
        </div>
        (spacer(32))
        <div className=Styles.subHeader> (str("Experiences")) </div>
        (spacer(16))
        (
          switch uid {
          | None => ReasonReact.null
          | Some(uid) => <MadeItEntry.Adder recipe uid fb />
          }
        )
        (spacer(32))
        <ViewMadeIts id=recipe##id fb />
        (spacer(128))
      </div>
      }
    }
  };

let loadingRecipe = () =>
  <div className=Styles.container> <div className=Styles.loading> (str("Loading")) </div> </div>;

let failedLoading = (err) => {
  Js.log(err);
  <div> (str("Errored though")) </div>
};


/***
 * Hmmm can I like monadify this loading thing,
 * such that I can easily combine two?
 */

/*** Data loading part */
module RecipeFetcher = FirebaseFetcher.Single(Models.Recipe);

module IngredientsFetcher =
  FirebaseFetcher.StaticStream(
    {
      include Models.Ingredient;
      let query = (q) => q;
      let getId = (ing) => ing##id;
    }
  );

let make = (~fb, ~navigate, ~id, children) => {
  Js.log(id);
  RecipeFetcher.make(
    ~fb,
    ~id,
    ~listen=true,
    ~render=
      (~state) =>
        <IngredientsFetcher
          fb
          render=(
            (~state as ingredients) =>
              switch (doubleState(state, ingredients)) {
              | `Initial => loadingRecipe()
              | `Loaded(recipe, ingredients) =>
                make(~navigate, ~recipe, ~ingredients, ~fb, ~id, [||]) |> ReasonReact.element
              | `Errored(err) => failedLoading(err)
              }
          )
        />,
    children
  )
};