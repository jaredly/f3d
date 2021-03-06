open Utils;

type state =
  | Saving
  | Editing;

let component = ReasonReact.reducerComponent("Recipe");

[@react.component] let make = (~ingredients, ~fb, ~uid, ~navigate) =>
  ReactCompat.useRecordApi( ReasonReact.{
    ...component,
    initialState: () => Editing,
    reducer: (action, _) => ReasonReact.Update(action),
    render: ({state, send}) => {
      let id = BaseUtils.uuid();
      <EditRecipe
        saving=(state === Saving)
        recipe=(Models.Recipe.blank(id, uid))
        allIngredients=ingredients
        fb
        id
        onSave=(
          (recipe) => {
            (send(Saving));
            module FB = Firebase.Collection(Models.Recipe);
            let collection = FB.get(fb);
            let doc = Firebase.doc(collection, recipe##id);
            Js.log2("Saving", recipe);
            Firebase.set(doc, recipe)
            |> Js.Promise.then_(
                 () => {
                   navigate("/recipes/" ++ recipe##id);
                   Js.Promise.resolve()
                 }
               )
            |> Js.Promise.catch(
                 (err) => {
                   Js.log(err);
                   [%bs.debugger];
                   Js.Promise.resolve()
                 }
               )
            |> ignore;
            ()
          }
        )
        onCancel=((_) => ())
      />
    }
  });

module Styles = RecipeStyles;

let loadingRecipe = () =>
  <div className=Styles.container> <div className=Styles.loading> (str("Loading")) </div> </div>;

let failedLoading = (err) => {
  Js.log(err);
  <div> (str("Errored though")) </div>
};


/*** Data loading part */
module IngredientsFetcher =
  FirebaseFetcher.StaticStream(
    {
      include Models.Ingredient;
      let query = (q) => q;
      let getId = (ing) => ing##id;
    }
  );

let innerProps = makeProps;

[@react.component] let make = (~fb, ~uid, ~navigate) =>
  IngredientsFetcher.make(
    IngredientsFetcher.makeProps(

    ~fb,
    ~render=
      (~state as ingredients) =>
        switch ingredients {
        | `Initial => loadingRecipe()
        | `Loaded(ingredients) => make(innerProps(~ingredients, ~uid, ~fb, ~navigate, ()))
        | `Errored(err) => failedLoading(err)
        },
    ()
    )
  );
