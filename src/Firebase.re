
type config;
type firebase;
type firestore;

/* external config: apiKey::string => authDomain::string => projectId::string => config = "" [@@bs.obj]; */

external initializeApp: config => firebase = "" [@@bs.module "firebase"];
external useFirestore: unit = "" [@@bs.module "firebase/firestore"];
external firestore: firebase => firestore = "" [@@bs.send];
external enablePersistence: firestore => Js.Promise.t unit = "" [@@bs.send];

type collection 't;
external collection' : firestore => string => collection 't = "collection" [@@bs.send];
module Collection (Config: {let name: string; type t;}) => {
  let get: firestore => collection Config.t = fun fb => collection' fb Config.name;
};

type snapshot 't;
external data: snapshot 't => 't = "" [@@bs.send];
external exists: snapshot 't => bool = "" [@@bs.get];

module Query = {
  type query 't;
  type querySnapshot 't;
  external size: querySnapshot 't => float = "" [@@bs.get];
  external docs: querySnapshot 't => array (snapshot 't) = "" [@@bs.get];

  external get: query 't => Js.Promise.t (querySnapshot 't) = "" [@@bs.send];
  external limit: int => query 't = "" [@@bs.send.pipe: query 't];
  external orderBy: fieldPath::string => direction::string => query 't = "" [@@bs.send.pipe: query 't];
  external startAt:    snapshot 't => query 't = "" [@@bs.send.pipe: query 't];
  external startAfter: snapshot 't => query 't = "" [@@bs.send.pipe: query 't];
  external endAt:      snapshot 't => query 't = "" [@@bs.send.pipe: query 't];
  external endBefore:  snapshot 't => query 't = "" [@@bs.send.pipe: query 't];

  external whereBool: string => op::string => Js.boolean => query 't = "where" [@@bs.send.pipe: query 't];
  external whereStr: string => op::string => string => query 't = "where" [@@bs.send.pipe: query 't];
  external whereNum: string => op::string => float  => query 't = "where" [@@bs.send.pipe: query 't];

  external onSnapshot: query 't => (querySnapshot 't => unit) => unit = "" [@@bs.send];
};

external asQuery: collection 't => Query.query 't = "%identity";

type doc 't;
external doc: collection 't => string => doc 't = "" [@@bs.send];
external set: doc 't => 't => Js.Promise.t unit = "" [@@bs.send];
external get: doc 't => Js.Promise.t (snapshot 't) = "" [@@bs.send];
external delete: doc 't => Js.Promise.t unit = "" [@@bs.send];
external onSnapshot: doc 't => (snapshot 't => unit) => unit = "" [@@bs.send];
/* external update: doc 't => */

/* Js.t {.
  apiKey: string,
  authDomain: string,
  databaseURL: string,
  projectId: string,
  storageBucket: string,
  messagingSenderId: string
} */
