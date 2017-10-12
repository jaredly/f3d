
open Utils;

let component = ReasonReact.statelessComponent "Textarea";

/**
  resize = () => {
    const style = window.getComputedStyle(this.shadow)
    let height
    if (!this.props.value.trim()) {
      const lineHeight = parseFloat(style.lineHeight)
      const paddingTop = parseFloat(style.paddingTop)
      const paddingBottom = parseFloat(style.paddingBottom)
      height = lineHeight + paddingTop + paddingBottom + 'px'
    } else {
      height = style.height
    }
    if (
      this.props.onHeightChange &&
      this._prevHeight &&
      this._prevHeight !== height
    ) {
      this.props.onHeightChange(parseFloat(height))
    }
    this.textarea.style.height = height
    this._prevHeight = height
  }
 */

type style = Js.t {.
  height: string,
  lineHeight: string,
  fontSize: string,
  paddingTop: string,
  paddingBottom: string
};
external getComputedStyle: Dom.element => style = "" [@@bs.val] [@@bs.scope "window"];
external parseFloat: string => float = "" [@@bs.val];
external setHeight: Dom.element => string => unit = "height" [@@bs.set] [@@bs.scope "style"];
external setPosition: Dom.element => string => unit = "position" [@@bs.set] [@@bs.scope "style"];
external setDisplay: Dom.element => string => unit = "display" [@@bs.set] [@@bs.scope "style"];

let getShadowHeight value node => {
  let style = getComputedStyle node;
  if (Js.String.trim value === "") {
    let lineHeight = parseFloat (style##lineHeight === "normal" ? style##fontSize : style##lineHeight);
    let paddingTop = parseFloat style##paddingTop;
    let paddingBottom = parseFloat style##paddingBottom;
    /* [%bs.debugger]; */
    (string_of_float (lineHeight +. paddingTop +. paddingBottom)) ^ "px";
  } else {
    style##height;
  }
};

let make ::value ::onChange ::onReturn=? ::className=? _children => {
  let shadow = ref None;
  let textarea = ref None;

  let resize () => {
    [%guard let Some textarea = !textarea][@else ()];
    [%guard let Some shadow = !shadow][@else ()];
    let height = getShadowHeight value shadow;
    setHeight textarea height;
  };

  let setRef dest node => Js.Null.to_opt node |> optMap (fun node => dest := Some node) |> ignore;

  ReasonReact.{
    ...component,
    didUpdate: fun _ => resize (),
    didMount: fun _ => {
      resize ();
      !shadow |> optMap (fun shadow => setPosition shadow "absolute") |> ignore;
      !textarea |> optMap (fun shadow => setDisplay shadow "block") |> ignore;
      ReasonReact.NoUpdate
    },
    render: fun _ => {
      <div style=ReactDOMRe.Style.(
        make position::"relative" cursor::"text" ()
      )>
        <textarea
          className=?className
          ref={setRef textarea}
          onChange={fun evt => onChange (evtValue evt)}
          onKeyDown=?(onReturn |> optMap (
            fun onReturn evt => {
              switch (ReactEventRe.Keyboard.key evt) {
              | "Return"
              | "Enter" => {
                ReactEventRe.Keyboard.preventDefault evt;
                onReturn ()
              }
              | _ => ()
              }
            }
          ))
          style=ReactDOMRe.Style.(
            make
              cursor::"text"
              width::"100%"
              resize::"none"
              overflow::"hidden"
              display::"none"
            ()
          )
          value
        />

        <div
          className=?className
          ref={setRef shadow}
          style=ReactDOMRe.Style.(
            make
              whiteSpace::"pre-wrap"
              wordBreak::"break-word"
              width::"100%"
              top::"0"
              left::"0"
              visibility::"hidden"
            ()
          )
        >
          {str @@ value ^ " "}
        </div>
      </div>
    }
  }
};

    /* 
const styles = {
  textarea: {
    cursor: 'text',
    width: '100%',
    resize: 'none',
    overflow: 'hidden',
    display: 'none',
  },

  shadow: {
    whiteSpace: 'pre-wrap',
    width: '100%',
    top: 0,
    left: 0,
    visibility: 'hidden',
  },
    
    
    const {onHeightChange, style, ...props} = this.props
    return (
      <div style={{...styles.container, ...style}}>
        <textarea
          {...props}
          ref={n => (this.textarea = n)}
          style={styles.textarea}
          className={this.props.className}
        />
        <div
          ref={n => (this.shadow = n)}
          style={styles.shadow}
          className={this.props.className}
        >
          {this.props.value + ' '}
        </div>
      </div>
    ) */

/* // @flow

import React, {Component} from 'react'

const getTestContent = (text, pos) => {
  let start = text.slice(0, pos)
  if (!text[pos].match(/\s/)) {
    start += text.slice(pos).match(/[^\s]+/)[0]
  }
  return start
}

type Props = {
  value: string,
  onHeightChange?: (height: number) => void,
  className: string,
  // anything you want to pass to textarea
}

export default class GrowingTextarea extends Component {
  textarea: any
  shadow: any
  _prevHeight: ?string

  componentDidMount() {
    this.resize()
    this.shadow.style.position = 'absolute'
    this.textarea.style.setProperty('display', 'block', 'important')
    window.addEventListener('resize', this.resize)
    this._prevHeight = null
  }

  componentDidUpdate() {
    this.resize()
  }

  componentWillUnmount() {
    window.removeEventListener('resize', this.resize)
  }

  getCursorSplit() {
    return this.textarea.selectionEnd
  }

  resize = () => {
    const style = window.getComputedStyle(this.shadow)
    let height
    if (!this.props.value.trim()) {
      const lineHeight = parseFloat(style.lineHeight)
      const paddingTop = parseFloat(style.paddingTop)
      const paddingBottom = parseFloat(style.paddingBottom)
      height = lineHeight + paddingTop + paddingBottom + 'px'
    } else {
      height = style.height
    }
    if (
      this.props.onHeightChange &&
      this._prevHeight &&
      this._prevHeight !== height
    ) {
      this.props.onHeightChange(parseFloat(height))
    }
    this.textarea.style.height = height
    this._prevHeight = height
  }

  render() {
    const {onHeightChange, style, ...props} = this.props
    return (
      <div style={{...styles.container, ...style}}>
        <textarea
          {...props}
          ref={n => (this.textarea = n)}
          style={styles.textarea}
          className={this.props.className}
        />
        <div
          ref={n => (this.shadow = n)}
          style={styles.shadow}
          className={this.props.className}
        >
          {this.props.value + ' '}
        </div>
      </div>
    )
  }
}

*/
