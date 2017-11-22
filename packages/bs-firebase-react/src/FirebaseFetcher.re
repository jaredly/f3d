module Single = (Config: {let name: string; type t;}) => {
  type status = [ | `Initial | `Loaded(Config.t) | `Errored(Js.Promise.error)];
  let component =
    ReasonReact.reducerComponentWithRetainedProps("FirebaseSingleFetcher:" ++ Config.name);
  module FBCollection = Firebase.Collection(Config);
  let make = (~fb, ~id, ~listen=false, ~render, _children) => {
    let collection = FBCollection.get(fb);
    let fetch = (_status, reduce) => {
      Firebase.doc(collection, id)
      |> Firebase.get
      |> Js.Promise.then_(
           (snap) => {
             if (Firebase.exists(snap)) {
               reduce((_) => `Loaded(Firebase.data(snap)), ())
             } else {
               Js.log("Deleted")
                /*** TODO something useful? */
             };
             Js.Promise.resolve()
           }
         )
      |> Js.Promise.catch(
           (err) => {
             Js.log("bad");
             [%bs.debugger];
             /* [%bs.raw "debugger"]; */
             reduce((_) => `Errored(err), ());
             Js.Promise.resolve()
           }
         )
      |> ignore
    };
    ReasonReact.{
      ...component,
      retainedProps: id,
      initialState: () => (None, `Initial),
      willReceiveProps: ({state: (_, status) as state, reduce, retainedProps}) => {
        if (retainedProps !== id) {
          fetch(status, reduce)
        };
        state
      },
      reducer: (action, (destruct, _)) => ReasonReact.Update((destruct, action: status)),
      didMount: ({state: (_, status), reduce}) => {
        fetch(status, reduce);
        if (listen) {
          let doc = Firebase.doc(collection, id);
          Firebase.onSnapshot(doc, (snap) => reduce((_) => `Loaded(Firebase.data(snap)), ()))
          |> ((x) => ReasonReact.Update((Some(x), status)))
        } else {
          ReasonReact.NoUpdate
                      /*** TODO TODO TODO tear down this listener. How? */
        }
      },
      willUnmount: ({state: (destruct, _)}) =>
        switch destruct {
        | None => ()
        | Some(destruct) => destruct()
        },
      render: ({state: (_, state), reduce}) => render(~state)
    }
  };
};

module Dynamic = (Collection: {let name: string; type t;}) => {
  type state = [
    | `Initial
    | `Loaded(option(Firebase.snapshot(Collection.t)), array(Collection.t))
    | `Errored(Js.Promise.error)
  ];
  let component =
    ReasonReact.reducerComponentWithRetainedProps("FirebaseFetcher:" ++ Collection.name);
  module FBCollection = Firebase.Collection(Collection);
  let orr = (orr, fn, opt) =>
    switch opt {
    | None => orr
    | Some(x) => fn(x)
    };
  let make = (~fb, ~pageSize=10, ~query, ~refetchKey="", ~render, _children) => {
    let collection = FBCollection.get(fb);
    let handleResult = (reduce, current, prom) =>
      prom
      |> Js.Promise.then_(
           (snap) => {
             let snaps = Firebase.Query.docs(snap);
             let items = Array.map(Firebase.data, snaps);
             let total = Array.append(current, items);
             reduce(
               () =>
                 Array.length(items) === pageSize ?
                   `Loaded((Some(snaps[Array.length(snaps) - 1]), total)) : `Loaded((None, total)),
               ()
             );
             Js.Promise.resolve()
           }
         )
      |> Js.Promise.catch(
           (err) => {
             Js.log2("booo", err);
             [%bs.debugger];
             reduce(() => `Errored(err), ());
             Js.Promise.resolve()
           }
         )
      |> ignore;
    let fetch = (state, reduce, clear) => {
      module Q = Firebase.Query;
      let q = Firebase.asQuery(collection) |> query |> Q.limit(pageSize);
      let (q, current) =
        if (clear) {
          (q, [||])
        } else {
          switch state {
          | `Errored(_)
          | `Initial => (q, [||])
          | `Loaded(Some(snap), items) => (Q.startAfter(snap, q), items)
          | `Loaded(None, items) => (q, items)
          }
        };
      q |> Q.get |> handleResult(reduce, current)
    };
    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: () => `Initial,
      willReceiveProps: ({state, reduce, retainedProps}) => {
        if (retainedProps !== refetchKey) {
          fetch(state, reduce, true)
        };
        state
      },
      reducer: (action, state) => ReasonReact.Update((action: state)),
      didMount: ({state, reduce}) => {
        fetch(state, reduce, false);
        ReasonReact.NoUpdate
      },
      render: ({state, reduce}) => render(~state, ~fetchMore=() => fetch(state, reduce, false))
    }
  };
};

module Static =
       (
         Config: {
           let name: string;
           type t;
           let query: Firebase.Query.query(t) => Firebase.Query.query(t);
         }
       ) => {
  module Inner = Dynamic(Config);
  let make = (~fb, ~pageSize=10, ~render, children) =>
    Inner.make(~fb, ~pageSize, ~query=Config.query, ~refetchKey="", ~render, children);
};

module Stream = (Collection: {let name: string; type t; let getId: t => string;}) => {
  type state = [ | `Initial | `Loaded(list(Collection.t)) | `Errored(Js.Promise.error)];
  type action = [
    | `Errored(Js.Promise.error)
    | `Updated(array(Firebase.Query.docChange(Collection.t)))
  ];
  let component =
    ReasonReact.reducerComponentWithRetainedProps("FirebaseFetcher:" ++ Collection.name);
  module FBCollection = Firebase.Collection(Collection);
  let orr = (orr, fn, opt) =>
    switch opt {
    | None => orr
    | Some(x) => fn(x)
    };
  let make = (~fb, ~query, ~refetchKey="", ~render, _children) => {
    let collection = FBCollection.get(fb);
    let unlisten = ref(None);
    let fetch = (state, reduce) => {
      switch unlisten^ {
      | Some(fn) => fn()
      | None => ()
      };
      module Q = Firebase.Query;
      let q = Firebase.asQuery(collection) |> query;
      unlisten :=
        Some(
          Q.onSnapshot(
            q,
            (snapshot) => (reduce((_) => `Updated(Q.docChanges(snapshot))))(),
            (error) => {
              Js.log2("Failed to load", error);
              (reduce((_) => `Errored(error)))()
            }
          )
        )
    };
    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: () => `Initial,
      didMount: ({state, reduce}) => {
        fetch(state, reduce);
        ReasonReact.NoUpdate
      },
      willReceiveProps: ({state, reduce, retainedProps}) => {
        if (retainedProps !== refetchKey) {
          fetch(state, reduce)
        };
        state
      },
      willUnmount: (_) =>
        switch unlisten^ {
        | Some(fn) => fn()
        | None => ()
        },
      reducer: (action: action, state: state) =>
        switch (action: action) {
        | `Errored(error) => ReasonReact.Update(`Errored(error))
        | `Updated(changes) =>
          let docs =
            switch state {
            | `Errored(_)
            | `Initial => []
            | `Loaded(docs) => docs
            };
          module Q = Firebase.Query;
          let docs =
            Array.fold_left(
              (docs, change) =>
                switch (Q.type_(change)) {
                | "added" =>
                  let doc = Q.doc(change) |> Firebase.data;
                  [doc, ...docs]
                | "removed" =>
                  let id = Q.doc(change) |> Firebase.data |> Collection.getId;
                  List.filter((d) => Collection.getId(d) != id, docs)
                | "modified" =>
                  let doc = Q.doc(change) |> Firebase.data;
                  let id = doc |> Collection.getId;
                  List.map((d) => Collection.getId(d) == id ? doc : d, docs)
                | change => {
                  Js.log2("unexpected change type", change);
                  failwith("Invalid change type")
                }
                },
              docs,
              changes
            );
          ReasonReact.Update(`Loaded(docs))
        },
      render: ({state, reduce}) => render(~state)
    }
  };
};

module StaticStream =
       (
         Config: {
           let name: string;
           type t;
           let query: Firebase.Query.query(t) => Firebase.Query.query(t);
           let getId: t => string;
         }
       ) => {
  module Inner = Stream(Config);
  let make = (~fb, ~render, children) =>
    Inner.make(~fb, ~query=Config.query, ~refetchKey="", ~render, children);
};
