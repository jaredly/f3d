
let module Single (Config: {let name: string; type t;}) => {
  type status =
    [ `Initial
    | `Loaded Config.t
    | `Errored Js.Promise.error ];

  let component = ReasonReact.reducerComponentWithRetainedProps ("FirebaseSingleFetcher:" ^ Config.name);

  let module FBCollection = Firebase.Collection Config;

  let make ::fb ::id ::listen=false ::render _children => {
    let collection = FBCollection.get fb;

    let fetch status reduce => {
      Js.log2 "fetching" Config.name;
      Firebase.doc collection id
      |> Firebase.get
      |> Js.Promise.then_ (fun snap => {reduce (fun _ => `Loaded (Firebase.data snap)) ();Js.Promise.resolve ()})
      |> Js.Promise.catch (fun err => {
        Js.log "bad";
        [%bs.debugger];
        /* [%bs.raw "debugger"]; */
        reduce (fun _ => `Errored err) ();Js.Promise.resolve ()})
      |> ignore;
    };

    ReasonReact.{
      ...component,
      retainedProps: id,
      initialState: fun () => (None, `Initial),
      willReceiveProps: fun {state: (_, status) as state, reduce, retainedProps} => {
        if (retainedProps !== id) {
          fetch status reduce;
        };
        state
      },
      reducer: fun action (destruct, _) => {
        ReasonReact.Update (destruct, action: status)
      },
      didMount: fun {state: (_, status), reduce} => {
        fetch status reduce;
        if listen {
          let doc = Firebase.doc collection id;
          Firebase.onSnapshot doc (fun snap => {
            reduce (fun _ => `Loaded (Firebase.data snap)) ();
          }) |> (fun x => ReasonReact.Update (Some x, status));
          /** TODO TODO TODO tear down this listener. How? */
        } else {
          ReasonReact.NoUpdate
        }
      },
      willUnmount: fun {state: (destruct, _)} => {
        switch destruct {
        | None => ()
        | Some destruct => destruct ()
        }
      },
      render: fun {state: (_, state), reduce} => {
        render ::state 
      }
    }
  };
};

let module Dynamic (Collection: {let name: string; type t;}) => {

  type state =
    [ `Initial
    | `Loaded (option (Firebase.snapshot Collection.t), array Collection.t)
    | `Errored Js.Promise.error ];

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
        let total = Array.append current items;
        reduce (fun () => Array.length items === pageSize ? `Loaded (Some snaps.(Array.length snaps - 1), total) : `Loaded (None, total)) ();
        Js.Promise.resolve ();
      })
      |> Js.Promise.catch (fun err => {
        Js.log "booo";
        [%bs.debugger];
        reduce (fun () => `Errored err) ();
        Js.Promise.resolve ();
      })
      |> ignore;
    };

    let fetch state reduce clear => {
      Js.log2 "fetching" Collection.name;
      let module Q = Firebase.Query;
      let q = Firebase.asQuery collection
        |> query
        |> Q.limit pageSize;

      let (q, current) = if clear {
        (q, [||])
      } else {switch state {
      | `Errored _
      | `Initial => (q, [||])
      | `Loaded (Some snap, items) => (Q.startAfter snap q, items)
      | `Loaded (None, items) => (q, items)
      }};

      q |> Q.get |> handleResult reduce current;
    };

    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: fun () => `Initial,
      willReceiveProps: fun {state, reduce, retainedProps} => {
        if (retainedProps !== refetchKey) {
          fetch state reduce true;
        };
        state
      },
      reducer: fun action state => {
        ReasonReact.Update (action: state)
      },
      didMount: fun {state, reduce} => {
        fetch state reduce false;
        ReasonReact.NoUpdate
      },
      render: fun {state, reduce} => {
        render ::state fetchMore::(fun () => {
          fetch state reduce false
        })
      }
    }
  };
};

let module Static (Config: {let name: string; type t; let query: Firebase.Query.query t => Firebase.Query.query t; }) => {
  let module Inner = Dynamic Config;
  let make ::fb ::pageSize=10 ::render children => {
    Inner.make ::fb ::pageSize query::Config.query refetchKey::"" ::render children
  };
};


let module Stream (Collection: {let name: string; type t; let getId: t => string;}) => {

  type state =
    [ `Initial
    | `Loaded (list Collection.t)
    | `Errored Js.Promise.error ];

  type action =
    [`Errored Js.Promise.error
    | `Updated (array (Firebase.Query.docChange Collection.t)) ];

  let component = ReasonReact.reducerComponentWithRetainedProps ("FirebaseFetcher:" ^ Collection.name);

  let module FBCollection = Firebase.Collection Collection;

  let orr orr fn opt => switch opt { | None => orr | Some x => fn x };

  let make ::fb ::query ::refetchKey="" ::render _children => {
    let collection = FBCollection.get fb;
    let unlisten = ref None;

    let fetch state reduce => {
      switch (!unlisten) {
      | Some fn => fn ()
      | None => ()
      };
      Js.log2 "fetching" Collection.name;
      let module Q = Firebase.Query;
      let q = Firebase.asQuery collection |> query;

      unlisten := Some (
        Q.onSnapshot q
        (fun snapshot => {
          (reduce (fun _ => `Updated (Q.docChanges snapshot))) ()
        })
        (fun error => {
          (reduce (fun _ => `Errored error)) ()
        })
      )
    };

    ReasonReact.{
      ...component,
      retainedProps: refetchKey,
      initialState: fun () => `Initial,
      didMount: fun {state, reduce} => {
        fetch state reduce;
        ReasonReact.NoUpdate
      },
      willReceiveProps: fun {state, reduce, retainedProps} => {
        if (retainedProps !== refetchKey) {
          fetch state reduce;
        };
        state
      },
      willUnmount: fun _ => {
        switch (!unlisten) {
        | Some fn => fn ()
        | None => ()
        };
      },
      reducer: fun (action: action) (state: state) => {
        switch (action: action) {
        | `Errored error => ReasonReact.Update (`Errored error)
        | `Updated changes => {
          let docs = switch state {
          | `Errored _
          | `Initial => []
          | `Loaded docs => docs
          };
          let module Q = Firebase.Query;
          let docs = Array.fold_left
          (fun docs change => {
            switch (Q.type_ change) {
            | "added" => {
              let doc = Q.doc change |> Firebase.data;
              [doc, ...docs]
            }
            | "removed" => {
              let id = Q.doc change |> Firebase.data |> Collection.getId;
              List.filter (fun d => (Collection.getId d) != id) docs
            }
            | "changed" => {
              let doc = Q.doc change |> Firebase.data;
              let id = doc |> Collection.getId;
              List.map (fun d => (Collection.getId d) == id ? doc : d) docs
            }
            | _ => failwith "Invalid change type"
            }
          })
          docs
          changes;
          ReasonReact.Update (`Loaded docs);
        }
        }
      },
      render: fun {state, reduce} => {
        render ::state
      }
    }
  };
};

let module StaticStream (Config: {let name: string; type t; let query: Firebase.Query.query t => Firebase.Query.query t; let getId: t => string;}) => {
  let module Inner = Stream Config;
  let make ::fb ::render children => {
    Inner.make ::fb query::Config.query refetchKey::"" ::render children
  };
};
