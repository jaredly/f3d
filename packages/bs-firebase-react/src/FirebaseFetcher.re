/**
 * Get a single item of a collection, identified by ID
 */
module Single = (Config: {
                   let name: string;
                   type t;
                 }) => {
  type status = [
    | `Initial
    | `Loaded(Config.t)
    | `Errored(Js.Promise.error)
  ];
  let component =
    ReasonReact.reducerComponentWithRetainedProps(
      "FirebaseSingleFetcher:" ++ Config.name,
    );
  module FBCollection = Firebase.Collection(Config);
  let make = (~fb, ~id, ~listen=false, ~render, _children) => {
    let collection = FBCollection.get(fb);
    let fetch = (_status, send) => {
      Firebase.doc(collection, id)
      |> Firebase.get
      |> Js.Promise.then_(snap => {
           if (Firebase.exists(snap)) {
             send(`Loaded(Firebase.data(snap)));
           } else {
             Js.log(
               "Deleted",
               /*** TODO something useful? */
             );
           };
           Js.Promise.resolve();
         })
      |> Js.Promise.catch(err => {
           Js.log("bad");
           [%bs.debugger];
           /* [%bs.raw "debugger"]; */
           send(`Errored(err));
           Js.Promise.resolve();
         })
      |> ignore;
    };
    ReasonReact.{
      ...component,
      retainedProps: id,
      initialState: () => (ref(None), `Initial),
      willReceiveProps:
        ({state: (_, status) as state, send, retainedProps}) => {
        if (retainedProps !== id) {
          fetch(status, send);
        };
        state;
      },
      reducer: (action, (destruct, _)) =>
        ReasonReact.Update((destruct, action: status)),
      didMount: ({state: (destruct, status), send}) => {
        fetch(status, send);
        if (listen) {
          let doc = Firebase.doc(collection, id);
          destruct := Some(Firebase.onSnapshot(doc, snap =>
            send(`Loaded(Firebase.data(snap)))
          ))
        };
      },
      willUnmount: ({state: (destruct, _)}) =>
        switch (destruct^) {
        | None => ()
        | Some(destruct) => destruct()
        },
      render: ({state: (_, state), send: _}) => render(~state),
    };
  };
};

module Singleton = (Config: {
                   let path: string => string;
                   type t;
                 }) => {
  type status = [
    | `Initial
    | `Loaded(Config.t)
    | `Errored(Js.Promise.error)
  ];
  let component =
    ReasonReact.reducerComponent(
      "FirebaseSingleFetcher:" ++ Config.path("{arg}"),
    );
  module FBCollection = Firebase.Single(Config);
  let make = (~fb, ~listen=false, ~render, ~arg, _children) => {
    let docRef = FBCollection.get(fb, arg);
    let fetch = (_status, send) => {
      Firebase.Database.onceValue(docRef)
      |> Js.Promise.then_(snap => {
           if (Firebase.exists(snap)) {
             send(`Loaded(Firebase.data(snap)));
           } else {
             Js.log(
               "Deleted",
               /*** TODO something useful? */
             );
           };
           Js.Promise.resolve();
         })
      |> Js.Promise.catch(err => {
           Js.log("bad");
           [%bs.debugger];
           /* [%bs.raw "debugger"]; */
           send(`Errored(err));
           Js.Promise.resolve();
         })
      |> ignore;
    };
    ReasonReact.{
      ...component,
      initialState: () => (ref(None), `Initial),
      reducer: (action, (destruct, _)) =>
        ReasonReact.Update((destruct, action: status)),
      didMount: ({state: (destruct, status), send}) => {
        if (listen) {
          destruct := Some(Firebase.Database.onValue(docRef, (. snap) =>
            send(`Loaded(Firebase.data(snap)))
          ))
        } else {
          fetch(status, send);
        };
      },
      willUnmount: ({state: (destruct, _)}) =>
        switch (destruct^) {
        | None => ()
        | Some(destruct) => destruct()
        },
      render: ({state: (_, state), send: _}) => render(~state),
    };
  };
};



module Dynamic = (Collection: {
                    let name: string;
                    type t;
                  }) => {
  type state = [
    | `Initial
    | `Loaded(option(Firebase.snapshot(Collection.t)), array(Collection.t))
    | `Errored(Js.Promise.error)
  ];
  let component =
    ReasonReact.reducerComponentWithRetainedProps(
      "FirebaseFetcher:" ++ Collection.name,
    );
  module FBCollection = Firebase.Collection(Collection);
  let orr = (orr, fn, opt) =>
    switch (opt) {
    | None => orr
    | Some(x) => fn(x)
    };
  let make = (~fb, ~pageSize=?, ~query, ~refetchKey="", ~render, _children) => {
    let collection = FBCollection.get(fb);
    let handleResult = (send, current, prom) =>
      prom
      |> Js.Promise.then_(snap => {
           let snaps = Firebase.Query.docs(snap);
           let items = Array.map(Firebase.data, snaps);
           let total = Array.append(current, items);
           send(
             (switch (pageSize) {
               | Some(pageSize) => Array.length(items) === pageSize
               | None => false
             })
               ? `Loaded((Some(snaps[Array.length(snaps) - 1]), total))
               : `Loaded((None, total)),
           );
           Js.Promise.resolve();
         })
      |> Js.Promise.catch(err => {
           Js.log2("booo", err);
           [%bs.debugger];
           send(`Errored(err));
           Js.Promise.resolve();
         })
      |> ignore;
    let fetch = (state, send, clear) => {
      module Q = Firebase.Query;
      let q = Firebase.asQuery(collection) |> query;
      let q = switch pageSize {
        | None => q
        | Some(pageSize) => q |> Q.limit(pageSize)
      };
      let (q, current) =
        if (clear) {
          (q, [||]);
        } else {
          switch (state) {
          | `Errored(_)
          | `Initial => (q, [||])
          | `Loaded(Some(snap), items) => (Q.startAfter(snap, q), items)
          | `Loaded(None, items) => (q, items)
          };
        };
      q |> Q.get |> handleResult(send, current);
    };
    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: () => `Initial,
      willReceiveProps: ({state, send, retainedProps}) => {
        if (retainedProps !== refetchKey) {
          fetch(state, send, true);
        };
        state;
      },
      reducer: (action, state) => ReasonReact.Update(action: state),
      didMount: ({state, send}) => {
        fetch(state, send, false);
      },
      render: ({state, send}) =>
        render(~state, ~fetchMore=() => fetch(state, send, false)),
    };
  };
};

module Static =
       (
         Config: {
           let name: string;
           type t;
           let query: Firebase.Query.query(t) => Firebase.Query.query(t);
         },
       ) => {
  module Inner = Dynamic(Config);
  let make = (~fb, ~pageSize=?, ~render, children) =>
    Inner.make(
      ~fb,
      ~pageSize?,
      ~query=Config.query,
      ~refetchKey="",
      ~render,
      children,
    );
};

module Stream =
       (Collection: {
          let name: string;
          type t;
          let getId: t => string;
        }) => {
  type state = [
    | `Initial
    | `Loaded(list(Collection.t))
    | `Errored(Js.Promise.error)
  ];
  type action = [
    | `Errored(Js.Promise.error)
    | `Updated(array(Firebase.Query.docChange(Collection.t)))
  ];
  let component =
    ReasonReact.reducerComponentWithRetainedProps(
      "FirebaseFetcher:" ++ Collection.name,
    );
  module FBCollection = Firebase.Collection(Collection);
  let orr = (orr, fn, opt) =>
    switch (opt) {
    | None => orr
    | Some(x) => fn(x)
    };
  let make = (~fb, ~query, ~refetchKey="", ~render, _children) => {
    let collection = FBCollection.get(fb);
    let unlisten = ref(None);
    let fetch = (state, send) => {
      switch (unlisten^) {
      | Some(fn) => fn()
      | None => ()
      };
      module Q = Firebase.Query;
      let q = Firebase.asQuery(collection) |> query;
      unlisten :=
        Some(
          Q.onSnapshot(
            q,
            snapshot => send(`Updated(Q.docChanges(snapshot))),
            error => {
              Js.log2("Failed to load", error);
              send(`Errored(error));
            },
          ),
        );
    };
    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: () => `Initial,
      didMount: ({state, send}) => {
        fetch(state, send);
      },
      willReceiveProps: ({state, send, retainedProps}) => {
        if (retainedProps !== refetchKey) {
          fetch(state, send);
        };
        state;
      },
      willUnmount: _ =>
        switch (unlisten^) {
        | Some(fn) => fn()
        | None => ()
        },
      reducer: (action: action, state: state) =>
        switch ((action: action)) {
        | `Errored(error) => ReasonReact.Update(`Errored(error))
        | `Updated(changes) =>
          let docs =
            switch (state) {
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
                  [doc, ...docs];
                | "removed" =>
                  let id = Q.doc(change) |> Firebase.data |> Collection.getId;
                  List.filter(d => Collection.getId(d) != id, docs);
                | "modified" =>
                  let doc = Q.doc(change) |> Firebase.data;
                  let id = doc |> Collection.getId;
                  List.map(d => Collection.getId(d) == id ? doc : d, docs);
                | change =>
                  Js.log2("unexpected change type", change);
                  failwith("Invalid change type");
                },
              docs,
              changes,
            );
          ReasonReact.Update(`Loaded(docs));
        },
      render: ({state, send}) => render(~state),
    };
  };
};

module StaticStream =
       (
         Config: {
           let name: string;
           type t;
           let query: Firebase.Query.query(t) => Firebase.Query.query(t);
           let getId: t => string;
         },
       ) => {
  module Inner = Stream(Config);
  let make = (~fb, ~render, children) =>
    Inner.make(~fb, ~query=Config.query, ~refetchKey="", ~render, children);
};