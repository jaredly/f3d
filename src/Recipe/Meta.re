
open Utils;

let humanTime time => {
  if (time < 60) {
    (string_of_int time) ^ "m"
  } else {
    (string_of_int (time / 60)) ^ "h " ^ (string_of_int (time mod 60)) ^ "m"
  }
};

let metaLine ::meta ::source => {
  let items = [
    maybe meta##ovenTemp (fun temp =>
      <div key="temp">
        (str @@ "Oven Temp: " ^ (string_of_int temp) ^ {j|°F|j})
      </div>
    ),
    maybe meta##totalTime (fun time =>
      <div key="total">
        (str @@ "Total time: " ^ (humanTime time))
      </div>
    ),
    maybe meta##prepTime (fun time =>
      <div key="prep">
        (str @@ "Prep time: " ^ (humanTime time))
      </div>
    ),
    maybe meta##cookTime (fun time =>
      <div key="cook">
        (str @@ "Cook time: " ^ (humanTime time))
      </div>
    ),
    maybe source (fun source => (
      <a key="source" href=source target="_blank" rel="noreferrer noopener"
        className=Glamor.(css[textDecoration "none"])
      >
        (str "source")
      </a>
    ))
  ];
  <div className=Glamor.(css[flexDirection "row"])>
    (items
      |> filterMaybe
      |> spacedItems (fun i => spacer key::(string_of_int i) 16)
      |> Array.of_list
      |> ReasonReact.arrayToElement)
  </div>
};
