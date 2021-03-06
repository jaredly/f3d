let rx = [%re {|/[^a-zа-яё0-9\-']+/i|}];

let stopMap: Js.Dict.t(bool) = [%bs.raw
  {| (function(){
  var stopwords = require('stopword/lib/stopwords_en.js').words;
  var stopMap = stopwords.reduce(function (m, w) { m[w] = true; return m }, {})
  return stopMap;
}()) |}
];

let unique: array(string) => array(string) = [%bs.raw
  {|
  function(items) {
    var res = [];
    var seen = {};
    items.forEach(function(item) {
      if (seen[item]) return;
      seen[item] = true;
      res.push(item);
    });
    return res;
  }
|}
];

let tokens = (text) =>
  text
  |> Js.String.toLowerCase
  |> Js.String.splitByRe(rx)
  |> Belt.Array.keepMap(_, x => x)
  |> Array.map(Js.String.trim)
  |> Js.Array.filter((item) => item !== "" && Js.Dict.get(stopMap, item) == None)
  |> unique;
