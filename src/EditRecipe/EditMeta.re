
open Utils;
open BaseUtils;

let clone: Js.t 'a => Js.t 'a = fun obj => {
  Js.Obj.assign (Js.Obj.empty ()) obj
};

let inputStyle = Glamor.(css [
  width "50px",
  textAlign "center",
]);


let render ::meta ::onChange ::source ::onChangeSource => {

  /* let items = [
    maybe meta##ovenTemp (fun temp =>
      <div key="temp">
        (str @@ "Oven Temp: " ^ (string_of_int temp) ^ {j|°F|j})
      </div>
    ),
    maybe meta##totalTime (fun time =>
      <input
        key="total">
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
  ]; */

  <div>
  <div className=Glamor.(css[flexDirection "row", flexWrap "wrap"])>
    <div>
      (str "Oven Temp:")
    </div>
    (spacer 4)
    <IntInput
      value=(Js.Null.to_opt meta##ovenTemp)
      className=inputStyle
      onChange=(fun value => {
        let obj = Obj.magic (clone meta);
        obj##ovenTemp #= (Js.Null.from_opt value);
        onChange obj;
      })
    />
    (spacer 16)
    <div>
      (str "Total time:")
    </div>
    (spacer 4)
    <IntInput
      value=(Js.Null.to_opt meta##totalTime)
      className=inputStyle
      onChange=(fun value => {
        let obj = Obj.magic (clone meta);
        obj##totalTime #= (Js.Null.from_opt value);
        onChange obj;
      })
    />
    (spacer 16)
    <div>
      (str "Prep time:")
    </div>
    (spacer 4)
    <IntInput
      value=(Js.Null.to_opt meta##prepTime)
      className=inputStyle
      onChange=(fun value => {
        let obj = Obj.magic (clone meta);
        obj##prepTime #= (Js.Null.from_opt value);
        onChange obj;
      })
    />
    (spacer 16)
    <div>
      (str "Cook time:")
    </div>
    (spacer 4)
    <IntInput
      value=(Js.Null.to_opt meta##cookTime)
      className=inputStyle
      onChange=(fun value => {
        let obj = Obj.magic (clone meta);
        obj##cookTime #= (Js.Null.from_opt value);
        onChange obj;
      })
    />
  </div>
  (spacer 16)
  <input
    value=(Js.Null.to_opt source |> optOr "")
    placeholder="Source"
    onChange=(fun evt => {
      let text = (evtValue evt);
      if (text == "") {
        onChangeSource Js.null
      } else {
        onChangeSource (Js.Null.return text)
      }
    })
  />
  </div>
};

