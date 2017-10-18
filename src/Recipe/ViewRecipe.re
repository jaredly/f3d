
open Utils;

let module Styles = RecipeStyles;
type action =
  | ToggleMaking
  | StartEditing
  | StopEditing
  | StartSaving
  | DoneSaving
  | SetMaking (StringSet.t, IntSet.t)
  | SetBatches float;

type state =
  | Making (StringSet.t, IntSet.t)
  | Editing
  | Saving
  | Normal;

let component = ReasonReact.reducerComponent "Recipe";

let make ::navigate ::recipe ::ingredients ::fb ::id _children => ReasonReact.{
  ...component,
  initialState: fun () => (1., Normal),
  reducer: fun action (batches, making) => ReasonReact.Update (switch action {
  | ToggleMaking => (batches, (making === Normal) ? Making (StringSet.empty, IntSet.empty) : Normal)
  | SetMaking (ing, inst) => (batches, Making (ing, inst))
  | StartEditing => (batches, Editing)
  | StopEditing => (batches, Normal)
  | StartSaving => (batches, Saving)
  | DoneSaving => (batches, Normal)
  | SetBatches batches => (batches, making)
  }),

  render: fun {state: (batches, making), reduce} => {
    [%guard let false = making === Editing || making === Saving][@else
      <EditRecipe
        saving=(making === Saving)
        allIngredients=ingredients
        recipe
        fb
        id
        onCancel=(reduce (fun _ => StopEditing))
        onSave=(fun recipe => {
          (reduce (fun _ => StartSaving)) ();
          let module FB = Firebase.Collection Models.Recipe;
          let collection = FB.get fb;
          let doc = Firebase.doc collection recipe##id;
          Firebase.set doc recipe
          |> Js.Promise.then_ (fun () => {
            (reduce (fun _ => DoneSaving)) ();
            Js.Promise.resolve ()
          })
          |> ignore;
          ()
        })
        onDelete=(fun () => {
          let module FB = Firebase.Collection Models.Recipe;
          let collection = FB.get fb;
          let doc = Firebase.doc collection recipe##id;
          Firebase.delete doc
          |> Js.Promise.then_ (fun () => {
            navigate "/";
            Js.Promise.resolve ()
          })
          |> ignore;
          ()
        })
      />
    ];

    <div className=Styles.container>
      <div className=Styles.header>
        <div className=Styles.title>
          (str recipe##title)
        </div>
        <button className=Styles.button onClick=(reduce (fun _ => ToggleMaking))>
          (str (making !== Normal ? "Stop making" : "Make"))
        </button>
        <button className=Styles.button onClick=(reduce (fun _ => StartEditing))>
          (str "Edit")
        </button>
      </div>
      (spacer 8)
      (Meta.metaLine
        meta::recipe##meta
        source::recipe##source
        )
      (spacer 16)
      <div className=Glamor.(css[whiteSpace "pre-wrap"])>
        (orr "" (Js.Null.to_opt recipe##description) |> ifEmpty "No description" |> str)
      </div>
      (spacer 32)
      <div className=Styles.subHeader>
        (str "ingredients")
        (spacer 32)
        <div className=Glamor.(css[
          fontSize "20px",
          flexDirection "row",
        ])>
        <AmountInput
          value=(Some batches)
          onChange=(fun value => value |> optMap (fun (value: float) => (reduce (fun _ => SetBatches value) ())) |> ignore)
          className=Glamor.(css [
            width "40px",
          ])
        />
        (spacer 8)
        <div className=Glamor.(css[
          textTransform "none",
          fontVariant "none",
          fontWeight "200",
        ])>
          (str (batches === 1. ? "batch" : "batches"))
        </div>
        </div>
        /** TODO num input */
      </div>
      (spacer 16)
      (Ingredients.render
        ::batches
        ingredients::recipe##ingredients
        allIngredients::ingredients
        making::(switch making {
        | Making (ing, inst) => Some (ing, (reduce (fun ing => SetMaking (ing, inst))))
        | _ => None
        })
      )
      (spacer 32)
      <div className=Styles.subHeader>
        (str "instructions")
      </div>
      (spacer 16)
      (Instructions.render instructions::recipe##instructions
        making::(switch making {
        | Making (ing, inst) => Some (inst, (reduce (fun inst => SetMaking (ing, inst))))
        | _ => None
        })
      )
      (spacer 64)
    </div>
  }
};

let loadingRecipe () => {
  <div className=Styles.container>
    <div className=Styles.loading>
    (str "Loading")
    </div>
  </div>
};

let failedLoading err => {
  Js.log err;
  <div>
    (str "Errored though")
  </div>
};

/**
 * Hmmm can I like monadify this loading thing,
 * such that I can easily combine two?
 */

/** Data loading part */
let module RecipeFetcher = FirebaseFetcher.Single Models.Recipe;

let module IngredientsFetcher = FirebaseFetcher.StaticStream {
  include Models.Ingredient;
  let query q => q;
  let getId ing => ing##id;
};

let make ::fb ::navigate ::id children => {
  Js.log id;
  RecipeFetcher.make ::fb ::id
  listen::true
  render::(fun ::state => {
    <IngredientsFetcher
      fb
      render=(fun state::ingredients => {
        switch (doubleState state ingredients) {
        | `Initial => loadingRecipe ()
        | `Loaded (recipe, ingredients) => make ::navigate ::recipe ::ingredients ::fb ::id [||] |> ReasonReact.element
        | `Errored err => failedLoading err
        }
      })
    />
  }) children
}
