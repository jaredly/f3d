/* Shared stuff probably */
let splitNumbers = (lines) => {
  /* let delimeters = [|".", ")"|]; */
  let rx = [%bs.re {|/^[0-9][\.)]/|}];
  let count =
    Array.fold_left(
      (count, line) => Js.String.match(rx, line) !== None ? count + 1 : count,
      0,
      lines
    );
  [@else None] [%guard let true = count > 0];
  let result = [||];
  Array.iter(
    (line) =>
      if (Js.String.match(rx, line) !== None) {
        Js.Array.push(Js.String.sliceToEnd(~from=2, line) |> Js.String.trim, result) |> ignore
      } else if (result == [||]) {
        Js.Array.push(Js.String.trim(line), result) |> ignore
      } else {
        let last = Array.length(result) - 1;
        result[last] = result[last] ++ (" " ++ line)
      },
    lines
  );
  Some(result)
};

let splitLines = (lines, initialChar) => {
  let result = [||];
  Array.iter(
    (line) =>
      if (Js.String.startsWith(initialChar, line)) {
        Js.Array.push(Js.String.sliceToEnd(~from=1, line), result) |> ignore
      } else if (result == [||]) {
        Js.Array.push(Js.String.trim(line), result) |> ignore
      } else {
        let last = Array.length(result) - 1;
        result[last] = result[last] ++ (" " ++ line)
      },
    lines
  );
  result
};

let chars = [
  {j|•|j},
  {j|•|j},
  {j|*|j},
  {j|+|j},
  {j|•|j},
  {j|‣|j},
  {j|∙|j},
  {j|⋅|j},
  {j|◦|j},
  {j|⦾|j},
  {j|⦿|j}
];

let findInitialChar = (lines) => {
  let rec loop = (chars) =>
    switch chars {
    | [] => None
    | [c, ...rest] =>
      let count =
        Array.fold_left(
          (count, line) => Js.String.startsWith(c, line) ? count + 1 : count,
          0,
          lines
        );
      if (count > 1) {
        Some(c)
      } else {
        loop(rest)
      }
    };
  loop(chars)
};

let splitList = (text) => {
  let lines =
    Js.String.split("\n", text) |> Array.map(Js.String.trim) |> Js.Array.filter((t) => t != "");
  let initialChar = findInitialChar(lines);
  switch initialChar {
  | Some(char) => splitLines(lines, char)
  | None =>
    let numbered = splitNumbers(lines);
    switch numbered {
    | Some(numbered) => numbered
    | None => lines
    }
  }
};

/* Ingredients section */
let units = [
  ("cup", ["cups", "cup", "Cups", "C", "c"]),
  ("tablespoon", ["tablespoons", "tablespoon", "Tablespoons", "Tablespoon", "Tbs", "tbs", "T"]),
  ("teaspoon", ["teaspoons", "teaspoon", "Teaspoons", "Teaspoon", "tsp", "Tsp", "t"]),
  ("ounce", ["oz", "ounces", "ounce"]),
  ("pound", ["lbs", "lb", "pounds", "pound"]),
  ("gram", ["g", "grams", "G", "Grams", "gram", "Gram"]),
  ("kilogram", ["kg", "Kg", "Kilograms", "Kilogram", "kilograms", "kilogram"]),
  ("quart", ["quarts", "Quarts", "quart", "Quart", "qts", "qt"]),
  ("can", ["cans", "can", "Cans", "Can"]),
  ("package", ["packages", "package", "Packages", "Package", "pkg", "Pkg"]),
  ("packet", ["packets", "packet", "pkts", "pkt"]),
  ("pinch", ["pinch", "Pinch"]),
  ("jar", ["jar"]),
  ("clove", ["cloves", "clove"]),
  ("bag", ["bag"])
];

let weightUnits = [|"ounce", "pound", "gram", "kilogram"|];

let bagUnits = ["can", "package", "bag", "packet", "jar"];

let maybeFind = (alist, fn) => {
  let rec loop = (items) =>
    switch items {
    | [] => None
    | [one, ...rest] =>
      switch (fn(one)) {
      | Some(value) => Some(value)
      | None => loop(rest)
      }
    };
  loop(alist)
};

let parseBag = (unit, text) => {
  let result =
    maybeFind(
      bagUnits,
      (bag) =>
        maybeFind(
          List.assoc(bag, units),
          (name) =>
            if (Js.String.startsWith(name ++ " ", text)) {
              Some((unit ++ (" " ++ bag), text))
            } else {
              None
            }
        )
    );
  switch result {
  | None => (unit, text)
  | Some((unit, text)) => (unit, text)
  }
};

let parseUnit = (amount, text) => {
  let text = Js.String.trim(text);
  let (parenthetical, text) =
    switch (Js.String.match([%bs.re {|/\([^\)]+\)\s/|}], text)) {
    | None => ("", text)
    | Some(arr) => (arr[0], Js.String.sliceToEnd(~from=String.length(arr[0]), text))
    };
  let result =
    maybeFind(
      units,
      ((canon, names)) =>
        maybeFind(
          names,
          (name) => {
            [@else None]
            [%guard
              let true =
                Js.String.startsWith(name ++ " ", text) || Js.String.startsWith(name ++ ".", text)
            ];
            let text = Js.String.sliceToEnd(~from=String.length(name) + 1, text) |> Js.String.trim;
            let (unit, text) =
              if (Js.Array.includes(canon, weightUnits)) {
                parseBag(canon, text)
              } else {
                (canon, text)
              };
            let text =
              Js.String.startsWith("of ", text) ? Js.String.sliceToEnd(~from=3, text) : text;
            Some((unit, text))
          }
        )
    );
  switch result {
  | None => {
      "id": BaseUtils.uuid(),
      "amount": amount,
      "unit": Js.null,
      "ingredient": "",
      "comments": Js.Null.return(parenthetical ++ text)
    }
  | Some((unit, text)) => {
      "id": BaseUtils.uuid(),
      "amount": amount,
      "unit": Js.Null.return(parenthetical ++ unit),
      "ingredient": "",
      "comments": Js.Null.return(text)
    }
  }
};

let englishInOrder = "one|two|three|four|five|six|seven|eight|nine|ten" |> Js.String.split("|");

let englishNums = [%bs.re {|/^(one|two|three|four|five|six|seven|eight|nine|ten)\b/|}];

let numberLike = [%bs.re {|/^([\d\s\/]|½|⅓|⅔|¼|¾|⅙|⅚|⅛|⅜|⅝|⅞)+/|}];

let parseIngredient = (line) => {
  Js.log2("parse ing", line);
  let (num, rest) =
    switch (Js.String.match(englishNums, line)) {
    | Some(switch_) =>
      Js.log2("english", switch_);
      let num = switch_[1];
      let int = Js.Array.indexOf(num, englishInOrder) + 1;
      (Js.Null.return(float_of_int(int)), Js.String.sliceToEnd(~from=String.length(num), line))
    | None =>
      switch (Js.String.match(numberLike, line)) {
      | Some(switch_) =>
        let rec loop = ((num, text)) =>
          switch (Js.String.match([%bs.re {|/^(\d+)\/(\d+)/|}], text)) {
          | Some([|whole, numer, denom|]) =>
            let float = float_of_string(numer) /. float_of_string(denom);
            let rest = Js.String.sliceToEnd(~from=String.length(whole), text);
            loop((num +. float, rest))
          | Some(_) => failwith("bad match")
          | None =>
            switch (Js.String.match([%bs.re {|/^\d+(\.\d+)?/|}], text)) {
            | Some(matches) =>
              let float = float_of_string(matches[0]);
              let rest = Js.String.sliceToEnd(~from=String.length(matches[0]), text);
              loop((num +. float, rest))
            | None => (num == 0. ? Js.null : Js.Null.return(num), text)
            }
          };
        loop((0., line))
      | None => (Js.null, line)
      }
    };
  if (num == Js.null) {
    {
      "id": BaseUtils.uuid(),
      "amount": Js.null,
      "unit": Js.null,
      "ingredient": "",
      "comments": Js.Null.return(line)
    }
  } else {
    parseUnit(num, rest)
  }
};

[%%bs.raw
  {|

const findIngredient = (allIngredients, text) => {
  const names = [].concat(...allIngredients.map(i => i.plural
    ? [[i.plural, i.id], [i.name, i.id]]
    : [[i.name, i.id]]))
  // TODO include alternative names
  names.sort((a, b) => b[0].length - a[0].length)
  const haystack = text.toLowerCase()
  for (let [name, id] of names) {
    const rx = new RegExp('\\b' + name + '\\b', 'i')
    const match = haystack.match(rx)
    if (match && match.index === 0) {
    // if (haystack.indexOf(name.toLowerCase()) === 0) {
      let rest = text.slice(name.length)
      if (rest[0] === ',') rest = rest.slice(1).trim()
      return [id, rest]
    }
  }
  for (let [name, id] of names) {
    const rx = new RegExp('\\b' + name + '\\b', 'i')
    const match = haystack.match(rx)
    if (match) {
    // if (haystack.indexOf(name.toLowerCase()) != -1) {
      return [id, text]
    }
  }
  return [null, text]
}

const modifiers = [
  'frozen', 'thawed',
  'peeled', 'chopped', 'minced',
  'boiled', 'drained',
  'rinsed', 'washed',
  'crushed',
  'whipped',
  'medium',
  'medium-sized', 'medium sized', 'large', 'small',
]

const guessIngredient = text => {
  if (!text) return ['', text]
  let i = text.indexOf(',')
  let j = text.indexOf('*')
  if (i === -1 || (j !== -1 && j < i)) i = j
  j = text.indexOf('(')
  if (i === -1 || (j !== -1 && j < i)) i = j
  let guess
  let rest
  if (i !== -1) {
    guess = text.slice(0, i).trim()
    rest = text.slice(i).trim()
    if (rest[0] === ',') rest = rest.slice(1)
    return [guess, rest]
  } else {
    guess = text
    rest = ''
  }
  for (let mod of modifiers) {
    if (guess.indexOf(mod + ' ') === 0) {
      guess = guess.slice(mod.length + 1)
      rest = rest ? mod + ', ' + rest : mod
    }
  }
  console.log('guessed', [guess, rest,text])
  return [guess, rest]
}

const maybeFindIngredient = (allIngredients, ingredient) => {
  const [id, comments] = findIngredient(allIngredients, ingredient.comments)
  if (!id) {
    const [guess, comments] = guessIngredient(ingredient.comments)
    return [guess, Object.assign({}, ingredient, {comments})]
  }
  return [null, Object.assign({}, ingredient, {ingredient: id, comments})]
}

|}
];

[@bs.val]
external maybeFindIngredient :
  (array(Models.ingredient), Models.recipeIngredient) => (Js.null(string), Models.recipeIngredient) =
  "";

let matchIngredient = (allIngredients, ingredient) =>
  maybeFindIngredient(allIngredients, ingredient)
  |> (
    ((guess, ingredient)) =>
      Models.maybeRecipeIngredient(~guess=?Js.Null.to_opt(guess), ingredient)
  );

let parseIngredientsText = (allIngredients, text) => {
  let text =
    switch (Js.String.match([%bs.re {|/ingredients\b/i|}], text)) {
    | Some([|switch_|]) => Js.String.sliceToEnd(~from=String.length(switch_), text)
    | Some(_) => failwith("Bad match")
    | None => text
    };
  let allIngredients = Array.of_list(allIngredients);
  splitList(text) |> Array.map(parseIngredient) |> Array.map(matchIngredient(allIngredients))
};

/* Instructions section */
[@bs.scope "clipboardData"] [@bs.send]
external getData : (ReactEventRe.Clipboard.t, string) => string =
  "";

let parseInstructionsText = (text) => {
  let items = splitList(text);
  if (items == [||]) {
    None
  } else {
    Some(Array.map((text) => {"text": text, "ingredientsUsed": Js.Dict.empty()}, items))
  }
};

let parseInstructions = (onPaste, evt) => {
  let text = getData(evt, "text/plain") |> Js.String.trim;
  [@else ()] [%guard let Some(items) = parseInstructionsText(text)];
  ReactEventRe.Clipboard.preventDefault(evt);
  onPaste(items)
};

let parseIngredients = (allIngredients, onPaste, evt) => {
  let text = getData(evt, "text/plain") |> Js.String.trim;

  /*** TODO maybe make this check more sophisticated */
  [@else ()] [%guard let None = Js.String.match([%bs.re {|/^[\d\s]+$/|}], text)];
  let partsRe = [%bs.re {|/\ninstructions\n/i|}];
  let parts = Js.String.splitByRe(partsRe, text);
  let (ingText, instText) =
    if (Array.length(parts) === 1) {
      (text, None)
    } else {
      (parts[0], Some(Js.Array.joinWith("\ninstructions\n", Js.Array.sliceFrom(1, parts))))
    };
  let ingredients = parseIngredientsText(allIngredients, ingText);
  let instructions = BaseUtils.optBind(parseInstructionsText, instText);
  ReactEventRe.Clipboard.preventDefault(evt);
  onPaste((ingredients, instructions))
};
