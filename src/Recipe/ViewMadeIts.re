open Utils;

module MadeItsFetcher =
  FirebaseFetcher.Stream(
    {
      include Models.MadeIt;
      let getId = (ing) => ing##id;
    }
  );

let madeItForRecipe = (render, ~fb, ~id, children: array(ReasonReact.reactElement)) =>
  MadeItsFetcher.make(
    ~fb,
    ~refetchKey=id,
    ~query=
      (q) =>
        Firebase.Query.whereStr("recipeId", ~op="==", id, q)
        |> Firebase.Query.whereBool("isPrivate", ~op="==", Js.false_)
        |> Firebase.Query.orderBy(~fieldPath="updated", ~direction="desc"),
    ~render=(~state) => render(~state, ~fb),
    children
  );

let make =
  madeItForRecipe(
    (~state, ~fb) =>
      switch state {
      | `Initial => <div> (str("Loading")) </div>
      | `Loaded(madeits) =>
        <div>
          (
            Array.map(
              (madeit) => <MadeIt key=madeit##id uid=(Firebase.Auth.fsUid(fb)) fb madeit />,
              madeits |> Array.of_list
            )
            |> ReasonReact.arrayToElement
          )
        </div>
      | `Errored(_err) => <div> (str("Failed Loading")) </div>
      }
  );