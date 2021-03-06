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

[@react.component] let make =
    (
      ~value,
      ~onChange,
      ~onPaste=?,
      ~containerClassName=?,
      ~onReturn=?,
      ~onDelete=?,
      ~onBlur=?,
      ~className=?
    ) => {
  let shadow = ref(None);
  let textarea = ref(None);
  let resize = () => {
    let%Lets.Opt.Consume (textarea) = textarea^;
    let%Lets.Opt.Consume (shadow) = shadow^;
    let height = getShadowHeight(value, shadow);
    setHeight(textarea, height)
  };
  let setRef = (dest, node) =>
     Js.Nullable.toOption(node) |> BaseUtils.optMap((node) => dest := Some(node)) |> ignore;
  let handleReturn = (evt) => {
    let%Lets.Opt.Consume (onReturn) = onReturn;
    let%Lets.Opt.Consume (textarea) = textarea^;
    let obj = ReactDOMRe.domElementToObj(textarea);
    let start: int = obj##selectionStart;
    let send: int = obj##selectionEnd;
    if (start === send) {

    ReactEvent.Keyboard.preventDefault(evt);
    let before: string = Js.String.slice(~from=0, ~to_=start, value);
    let after: string = Js.String.sliceToEnd(~from=start, value);
    onReturn(before, after)
    }
  };
  let handleDelete = (evt) => {
    let%Lets.Opt.Consume (onDelete) = onDelete;
    let%Lets.Opt.Consume (textarea) = textarea^;
    let obj = ReactDOMRe.domElementToObj(textarea);
    let start: int = obj##selectionStart;
    let send: int = obj##selectionEnd;
    if (start === send && start === 0) {

      ReactEvent.Keyboard.preventDefault(evt);
      onDelete(value)
    }
  };
  ReactCompat.useRecordApi( ReasonReact.{
    ...component,
    didUpdate: (_) => resize(),
    didMount: (_) => {
      resize();
      shadow^ |> BaseUtils.optMap((shadow) => setPosition(shadow, "absolute")) |> ignore;
      textarea^ |> BaseUtils.optMap((shadow) => setDisplay(shadow, "block")) |> ignore;
    },
    render: (_) =>
      <div
        style=ReactDOMRe.Style.(make(~position="relative", ~cursor="text", ()))
        className=?containerClassName>
        <textarea
          ?className
          ?onPaste
          ref=(setRef(textarea)->ReactDOMRe.Ref.callbackDomRef)
          onChange=((evt) => onChange(evtValue(evt)))
          ?onBlur
          onKeyDown=?(
            onReturn == None && onDelete == None ?
              None :
              Some(
                (evt) =>
                  switch (ReactEvent.Keyboard.key(evt)) {
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
          ref=(setRef(shadow)->ReactDOMRe.Ref.callbackDomRef)
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
  })
};
