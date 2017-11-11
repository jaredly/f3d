open Utils;

let component = ReasonReact.statelessComponent("Textarea");

type style = {
  .
  "height": string,
  "lineHeight": string,
  "fontSize": string,
  "paddingTop": string,
  "paddingBottom": string
};

[@bs.val] [@bs.scope "window"] external getComputedStyle : Dom.element => style = "";

[@bs.val] external parseFloat : string => float = "";

[@bs.set] [@bs.scope "style"] external setHeight : (Dom.element, string) => unit = "height";

[@bs.set] [@bs.scope "style"] external setPosition : (Dom.element, string) => unit = "position";

[@bs.set] [@bs.scope "style"] external setDisplay : (Dom.element, string) => unit = "display";

let getShadowHeight = (_value, node) => {
  let style = getComputedStyle(node);
  /* if (Js.String.trim value === "") {
       let lineHeight = parseFloat (style##lineHeight === "normal" ? style##fontSize : style##lineHeight);
       let paddingTop = parseFloat style##paddingTop;
       let paddingBottom = parseFloat style##paddingBottom;
       (string_of_float (lineHeight +. paddingTop +. paddingBottom)) ^ "px";
     } else { */
  style##height
  /* } */
};

let make =
    (
      ~value,
      ~onChange,
      ~onPaste=?,
      ~containerClassName=?,
      ~onReturn=?,
      ~onDelete=?,
      ~onBlur=?,
      ~className=?,
      _children
    ) => {
  let shadow = ref(None);
  let textarea = ref(None);
  let resize = () => {
    [@else ()] [%guard let Some(textarea) = textarea^];
    [@else ()] [%guard let Some(shadow) = shadow^];
    let height = getShadowHeight(value, shadow);
    setHeight(textarea, height)
  };
  let setRef = (dest, node) =>
    Js.Null.to_opt(node) |> BaseUtils.optMap((node) => dest := Some(node)) |> ignore;
  let handleReturn = (evt) => {
    [@else ()] [%guard let Some(onReturn) = onReturn];
    [@else ()] [%guard let Some(textarea) = textarea^];
    let obj = ReactDOMRe.domElementToObj(textarea);
    let start: int = obj##selectionStart;
    let send: int = obj##selectionEnd;
    [@else ()] [%guard let true = start === send];
    ReactEventRe.Keyboard.preventDefault(evt);
    let before: string = Js.String.slice(~from=0, ~to_=start, value);
    let after: string = Js.String.sliceToEnd(~from=start, value);
    onReturn(before, after)
  };
  let handleDelete = (evt) => {
    [@else ()] [%guard let Some(onDelete) = onDelete];
    [@else ()] [%guard let Some(textarea) = textarea^];
    let obj = ReactDOMRe.domElementToObj(textarea);
    let start: int = obj##selectionStart;
    let send: int = obj##selectionEnd;
    [@else ()] [%guard let true = start === send && start === 0];
    ReactEventRe.Keyboard.preventDefault(evt);
    onDelete(value)
  };
  ReasonReact.{
    ...component,
    didUpdate: (_) => resize(),
    didMount: (_) => {
      resize();
      shadow^ |> BaseUtils.optMap((shadow) => setPosition(shadow, "absolute")) |> ignore;
      textarea^ |> BaseUtils.optMap((shadow) => setDisplay(shadow, "block")) |> ignore;
      ReasonReact.NoUpdate
    },
    render: (_) =>
      <div
        style=ReactDOMRe.Style.(make(~position="relative", ~cursor="text", ()))
        className=?containerClassName>
        <textarea
          ?className
          ?onPaste
          ref=(setRef(textarea))
          onChange=((evt) => onChange(evtValue(evt)))
          ?onBlur
          onKeyDown=?(
            onReturn == None && onDelete == None ?
              None :
              Some(
                (evt) =>
                  switch (ReactEventRe.Keyboard.key(evt)) {
                  | "Return"
                  | "Enter" => handleReturn(evt)
                  | "Backspace"
                  | "Delete" => handleDelete(evt)
                  | _ => ()
                  }
              )
          )
          style=ReactDOMRe.Style.(
                  make(
                    ~cursor="text",
                    ~width="100%",
                    ~resize="none",
                    ~boxSizing="border-box",
                    ~overflow="hidden",
                    ~display="none",
                    ()
                  )
                )
          value
        />
        <div
          ?className
          ref=(setRef(shadow))
          style=ReactDOMRe.Style.(
                  make(
                    ~whiteSpace="pre-wrap",
                    ~wordBreak="break-word",
                    ~boxSizing="border-box",
                    ~width="100%",
                    ~top="0",
                    ~left="0",
                    ~visibility="hidden",
                    ()
                  )
                )>
          (str @@ (value === "" ? "M" : value ++ " "))
        </div>
      </div>
  }
};
