
type blob;

type ctx;
type canvas;

external asCanvas: Dom.element => canvas = "%identity";
[@bs.send] external getContext: canvas => string => ctx = "";
[@bs.send] external drawImage: ctx => Dom.element => int => int => int => int => unit = "";
[@bs.get] external naturalHeight: Dom.element => int = "";
[@bs.get] external naturalWidth: Dom.element => int = "";
[@bs.send] external save: ctx => unit = "";
[@bs.send] external restore: ctx => unit = "";
[@bs.send] external rotate: ctx => float => unit = "";

let getContext = (el) => {
  let canvas = asCanvas(el);
  getContext(canvas, "2d");
};

[@bs.scope "URL"][@bs.val] external createObjectURL: blob => string = "";

let cachedURLs = ref([]);

let rec myAssoc = (key, items) => switch items {
| [] => raise(Not_found)
| [(x, y), ..._] when key === x => y
| [_, ...rest] => myAssoc(key, rest)

};

let cachingCreateObjectURL = blob => {
  Js.log3("getting", blob, cachedURLs^);
  try (myAssoc(blob, cachedURLs^)) {
  | Not_found => {
    let url = createObjectURL(blob);
    Js.log3("Making new", url, blob);
    cachedURLs := [(blob, url), ...cachedURLs^];
    url
  }
  }
};
