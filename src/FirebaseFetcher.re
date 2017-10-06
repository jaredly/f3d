
let module F (Collection: {let name: string; type t;}) => {

  type state =
    | Initial
    | Loaded (option (Firebase.snapshot Collection.t)) (array Collection.t)
    | Errored Js.Promise.error;

  let component = ReasonReact.reducerComponentWithRetainedProps ("FirebaseFetcher:" ^ Collection.name);

  let module FBCollection = Firebase.Collection Collection;

  let orr orr fn opt => switch opt { | None => orr | Some x => fn x };

  let make ::fb ::pageSize=10 ::query ::refetchKey="" ::render _children => {
    let collection = FBCollection.get fb;

    let handleResult reduce current prom => {
      prom
      |> Js.Promise.then_
      (fun snap => {
        let snaps = (Firebase.Query.docs snap);
        let items = Array.map Firebase.data snaps;
        let lastSnap = snaps.(Array.length snaps - 1);
        let total = Array.append current items;
        reduce (fun () => Array.length items === pageSize ? Loaded (Some lastSnap) total : Loaded None total) ();
        Js.Promise.resolve ();
      })
      |> Js.Promise.catch (fun err => {
        reduce (fun () => Errored err) ();
        Js.Promise.resolve ();
      })
      |> ignore;
    };

    let fetch state reduce => {
      let module Q = Firebase.Query;
      let q = Firebase.asQuery collection
        |> query
        |> Q.limit pageSize;

      let (q, current) = switch state {
      | Errored _
      | Initial => (q, [||])
      | Loaded (Some snap) items => (Q.startAfter snap q, items)
      | Loaded None items => (q, items)
      };

      q |> Q.get |> handleResult reduce current;
    };

    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: fun () => Initial,
      willReceiveProps: fun {state, reduce, retainedProps} => {
        if (retainedProps !== refetchKey) {
          fetch state reduce;
        };
        state
      },
      reducer: fun action state => {
        ReasonReact.Update action
      },
      didMount: fun {state, reduce} => {
        fetch state reduce;
        ReasonReact.NoUpdate
      },
      render: fun {state, reduce} => {
        render ::state fetchMore::(fun () => {
          fetch state reduce
        })
      }
    }
  };
};
