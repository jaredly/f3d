Firebase.useFirestore;

[@bs.module] external config : Firebase.config = "../../../config.js";

let fb = Firebase.initializeApp(config);

[%bs.raw "window.fb = fb"];

// [@bs.module] external hack : unit = "../../../src/hack.js";

// hack;

Firebase.enablePersistence(Firebase.firestore(fb))
|> Js.Promise.catch(
     (err) => {
       Js.log2("error", err);
       Js.Promise.resolve()
     }
   )
|> Js.Promise.then_(
     () => {
       ReactDOMRe.renderToElementWithId(
         <TopErrorBoundary>
         <App fb=(Firebase.firestore(fb)) auth=(Firebase.auth(fb)) />
         </TopErrorBoundary>,
         "root"
       );
       Js.Promise.resolve()
     }
   )
|> ignore;
