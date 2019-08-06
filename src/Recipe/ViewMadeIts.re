open Utils;

module MadeItsFetcher =
  FirebaseFetcher.Stream(
    {
      include Models.MadeIt;
      let getId = (ing) => ing##id;
    }
  );

let myMadeItForRecipe = (render, ~fb, ~id, ~uid) =>
  MadeItsFetcher.make(
    MadeItsFetcher.makeProps(

    ~fb,
    ~refetchKey=id,
    ~query=
      (q) =>
        Firebase.Query.whereStr("recipeId", ~op="==", id, q)
        |> BaseUtils.optFold(
          (uid) => Firebase.Query.whereStr("authorId", ~op="==", uid),
          Firebase.Query.whereBool("isPrivate", ~op="==", false),
          uid
        )
        |> Firebase.Query.orderBy(~fieldPath="updated", ~direction="desc"),
    ~render=(~state) => render(~state, ~fb, ~uid),
    ()
    )
  );

let madeItForRecipe = (render, ~fb, ~id) =>
  MadeItsFetcher.make(
    MadeItsFetcher.makeProps(
    ~fb,
    ~refetchKey=id,
    ~query=
      (q) =>
        Firebase.Query.whereStr("recipeId", ~op="==", id, q)
        |> Firebase.Query.whereBool("isPrivate", ~op="==", false)
        |> Firebase.Query.orderBy(~fieldPath="updated", ~direction="desc"),
    ~render=(~state) => render(~state, ~fb),
    ())
  );

[@react.component] let make = (~fb, ~id) =>
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
            |> Js.Array.reverseInPlace
            |> ReasonReact.array
          )
        </div>
      | `Errored(_err) => <div> (str("Failed Loading")) </div>
      },
      ~fb,~id
  );