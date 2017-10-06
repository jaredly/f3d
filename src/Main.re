
Firebase.useFirestore;

external config: Firebase.config = "../../../config.js" [@@bs.module];
let fb = Firebase.initializeApp config;

[%bs.raw "window.fb = fb"];
external hack: unit = "../../../src/hack.js" [@@bs.module];
hack;

Firebase.enablePersistence (Firebase.firestore fb)
|> Js.Promise.catch (fun err => {Js.log2 "error" err; Js.Promise.resolve ()})
|> Js.Promise.then_ (fun () => {
  ReactDOMRe.renderToElementWithId <App fb=(Firebase.firestore fb) /> "root";
  Js.Promise.resolve ();
}) |> ignore;
