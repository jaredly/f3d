open Utils;

module MadeItsFetcher =
  FirebaseFetcher.Stream(
    {
      include Models.MadeIt;
      let getId = (ing) => ing##id;
    }
  );

let make = (~fb, ~id, children) =>
  MadeItsFetcher.make(
    ~fb,
    ~refetchKey=id,
    ~query=Firebase.Query.whereStr("recipeId", ~op="==", id),
    ~render=
      (~state) =>
        switch state {
        | `Initial => <div> (str("Loading")) </div>
        | `Loaded(madeits) =>
          <div>
            (
              Array.map(
                (madeit) => <MadeIt uid=(Firebase.Auth.fsUid(fb)) fb madeit />,
                madeits |> Array.of_list
              )
              |> ReasonReact.arrayToElement
            )
          </div>
        | `Errored(_err) => <div> (str("Failed Loading")) </div>
        },
    children
  );