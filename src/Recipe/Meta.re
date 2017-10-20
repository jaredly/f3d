
open Utils;
open BaseUtils;

let humanTime time => {
  if (time < 60) {
    (string_of_int time) ^ "m"
  } else {
    (string_of_int (time / 60)) ^ "h " ^ (string_of_int (time mod 60)) ^ "m"
  }
};

let module Styles = {
  open Glamor;
  let item = css [
    flexDirection "row"
  ];
  let label = css [
    fontWeight "600",
    marginRight "8px",
  ];
  let value = css [];
};

let metaLine ::meta ::source => {
  let items = [
    maybe meta##ovenTemp (fun temp =>
      <div key="temp" className=Styles.item>
        <div className=Styles.label>(str "Oven temp:")</div>
        <div className=Styles.value>
        (str @@ (string_of_int temp) ^ {j|°F|j})
        </div>
      </div>
    ),
    maybe meta##totalTime (fun time =>
      <div key="total" className=Styles.item>
        <div className=Styles.label>(str "Total time:")</div>
        <div className=Styles.value>
        (str @@ (humanTime time))
        </div>
      </div>
    ),
    maybe meta##prepTime (fun time =>
      <div key="prep" className=Styles.item>
        <div className=Styles.label>(str "Prep time:")</div>
        <div className=Styles.value>
        (str @@ (humanTime time))
        </div>
      </div>
    ),
    maybe meta##cookTime (fun time =>
      <div key="cook" className=Styles.item>
        <div className=Styles.label>(str "Cook time:")</div>
        <div className=Styles.value>
        (str @@ (humanTime time))
        </div>
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
