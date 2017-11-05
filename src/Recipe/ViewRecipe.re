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
    render: ({state: (batches, making), reduce}) => {
      let uid = Firebase.Auth.fsUid(fb);
      [@else
        <EditRecipe
          saving=(making === Saving)
          allIngredients=ingredients
          recipe
          fb
          id
          onCancel=(reduce((_) => StopEditing))
          onSave=(
            (recipe) => {
              (reduce((_) => StartSaving))();
              module FB = Firebase.Collection(Models.Recipe);
              let collection = FB.get(fb);
              let doc = Firebase.doc(collection, recipe##id);
              Firebase.set(doc, recipe)
              |> Js.Promise.then_(
                   () => {
                     (reduce((_) => DoneSaving))();
                     Js.Promise.resolve()
                   }
                 )
              |> ignore;
              ()
            }
          )
          onDelete=(
            () => {
              module FB = Firebase.Collection(Models.Recipe);
              let collection = FB.get(fb);
              let doc = Firebase.doc(collection, recipe##id);
              Firebase.delete(doc)
              |> Js.Promise.then_(
                   () => {
                     navigate("/");
                     Js.Promise.resolve()
                   }
                 )
              |> ignore;
              ()
            }
          )
        />
      ]
      [%guard let false = (making === Editing || making === Saving) && uid !== None];
      <div className=Styles.container>
        <div className=Styles.header>
          <div className=Styles.title> (str(recipe##title)) </div>
          <button className=Styles.primaryButton onClick=(reduce((_) => ToggleMaking))>
            (str(making !== Normal ? "Stop making" : "Make"))
          </button>
          (
            uid === None ?
              ReasonReact.nullElement :
              <button className=Styles.button onClick=(reduce((_) => StartEditing))>
                (str("Edit"))
              </button>
          )
        </div>
        (spacer(8))
        (Meta.metaLine(~meta=recipe##meta, ~source=recipe##source))
        (spacer(16))
        <div className=Glamor.(css([whiteSpace("pre-wrap")]))>
          (orr("", Js.Null.to_opt(recipe##description)) |> ifEmpty("No description") |> str)
        </div>
        (spacer(32))
        <div className=Styles.subHeader>
          (str("ingredients"))
          (spacer(32))
          <div className=Glamor.(css([fontSize("20px"), flexDirection("row"), flex("1")]))>
            <AmountInput
              value=(Some(batches))
              onChange=(
                (value) =>
                  value |> optMap((value: float) => reduce((_) => SetBatches(value), ())) |> ignore
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
              | Making((ing, inst)) => Some((ing, reduce((ing) => SetMaking((ing, inst)))))
              | _ => None
              }
          )
        )
        (spacer(32))
        <div className=Styles.subHeader> (str("instructions")) </div>
        (spacer(16))
        (
          Instructions.render(
            ~instructions=recipe##instructions,
            ~making=
              switch making {
              | Making((ing, inst)) => Some((inst, reduce((inst) => SetMaking((ing, inst)))))
              | _ => None
              }
          )
        )
        (spacer(64))
      </div>
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
