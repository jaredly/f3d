type config;
type firebase;
type firestore;
type auth;
type collection('t);
type snapshot('t);
type doc('t);

[@bs.module "firebase"] external initializeApp : config => firebase = "";
[@bs.module "firebase/firestore"] external useFirestore : unit = "";
[@bs.send] external firestore : firebase => firestore = "";
[@bs.send] external auth : firebase => auth = "";
[@bs.send] external enablePersistence : firestore => Js.Promise.t(unit) = "";
[@bs.get] external app : firestore => firebase = "";

/** TODO maybe use this */
let module Storage = {
  type storage;
  type ref;
  type snapshot;
  type file;
  [@bs.send] external get : firebase => storage = "storage";
  [@bs.send] external ref : storage => ref = "";
  [@bs.send.pipe: ref] external child : string => ref = "";
  [@bs.send.pipe: ref] external put : file => Js.Promise.t(snapshot) = "";
  [@bs.send] external delete : ref => Js.Promise.t(unit) = "";
  [@bs.send] external getDownloadURL : ref => Js.Promise.t(string) = "";
};

let optMap = (fn, value) =>
  switch value {
  | None => None
  | Some(v) => Some(fn(v))
  };

module Auth = {
  type user;
  [@bs.get] external currentUser : auth => Js.nullable(user) = "";
  [@bs.send] external signOut : auth => unit = "";
  [@bs.get] external displayName : user => Js.nullable(string) = "";
  [@bs.get] external email : user => string = "";
  [@bs.send] external signInWithEmailAndPassword : (auth, string, string) => Js.Promise.t(user) = "";
  [@bs.send] external onAuthStateChanged : (auth, Js.nullable(user) => unit) => unit = "";
  [@bs.get] external uid : user => string = "";
  let fsUid = (fs) =>
    app(fs) |> auth |> currentUser |>  Js.Nullable.toOption |> optMap(uid);
};

[@bs.send] external collection' : (firestore, string) => collection('t) = "collection";

module Collection = (Config: {let name: string; type t;}) => {
  let get: firestore => collection(Config.t) = (fb) => collection'(fb, Config.name);
};

type unsubscribe = unit => unit;

module Database = {
  type ref('t);
  [@bs.send] external ref : (firestore, string) => ref('t) = "ref";
  [@bs.send] external onValue : (ref('t), [@bs.as "value"] _, (. snapshot('t)) => unit) => unsubscribe = "on";
  [@bs.send] external onceValue : (ref('t), [@bs.as "value"] _) => Js.Promise.t(snapshot('t)) = "once";
}

module Single = (Config: {let path: string => string; type t;}) => {
  let get: (firestore, string) => Database.ref(Config.t) = (fb, arg) => Database.ref(fb, Config.path(arg));
};

[@bs.send] external data : snapshot('t) => 't = "";

[@bs.get] external exists : snapshot('t) => bool = "";

module Query = {
  type query('t);
  type querySnapshot('t);
  [@bs.get] external size : querySnapshot('t) => float = "";
  [@bs.get] external docs : querySnapshot('t) => array(snapshot('t)) = "";
  type docChange('t);
  [@bs.get] external doc : docChange('t) => snapshot('t) = "";
  [@bs.get] external type_ : docChange('t) => string = "type";
  [@bs.get] external docChanges : querySnapshot('t) => array(docChange('t)) = "";
  [@bs.send] external get : query('t) => Js.Promise.t(querySnapshot('t)) = "";
  [@bs.send.pipe : query('t)] external limit : int => query('t) = "";
  [@bs.send.pipe : query('t)]
  external orderBy : (~fieldPath: string, ~direction: string) => query('t) =
    "";
  [@bs.send.pipe : query('t)] external startAt : snapshot('t) => query('t) = "";
  [@bs.send.pipe : query('t)] external startAfter : snapshot('t) => query('t) = "";
  [@bs.send.pipe : query('t)] external endAt : snapshot('t) => query('t) = "";
  [@bs.send.pipe : query('t)] external endBefore : snapshot('t) => query('t) = "";
  [@bs.send.pipe : query('t)] external whereBool : (string, ~op: string, bool) => query('t) =
    "where";
  [@bs.send.pipe : query('t)] external whereStr : (string, ~op: string, string) => query('t) =
    "where";
  [@bs.send.pipe : query('t)] external whereNum : (string, ~op: string, float) => query('t) =
    "where";
  [@bs.send]
  external onSnapshot :
    (query('t), querySnapshot('t) => unit, Js.Promise.error => unit) => unsubscribe =
    "";
};

external asQuery : collection('t) => Query.query('t) = "%identity";

[@bs.send] external doc : (collection('t), string) => doc('t) = "";
[@bs.send] external set : (doc('t), 't) => Js.Promise.t(unit) = "";
[@bs.send] external get : doc('t) => Js.Promise.t(snapshot('t)) = "";
[@bs.send] external delete : doc('t) => Js.Promise.t(unit) = "";
[@bs.send] external onSnapshot : (doc('t), snapshot('t) => unit) => unsubscribe = "";
