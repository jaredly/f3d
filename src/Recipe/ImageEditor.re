open Utils;

module Canvas = {
  let component = ReasonReact.reducerComponent("ImageEditor");
  [@react.component]
  let make = (~size as (width, height) , ~render) => ReactCompat.useRecordApi({
    ...component,
    initialState: () => ref(None),
    reducer: ((), _) => ReasonReact.NoUpdate,
    didMount: ({state}) => {
      switch (state^) {
      | None => ()
      | Some(canvas) => {
        let ctx = Images.getContext(canvas);

        render(ctx);
      }
      };
    },
    didUpdate: ({newSelf: {state}}) => {
      switch (state^) {
      | None => ()
      | Some(canvas) => {
        let ctx = Images.getContext(canvas);
        render(ctx);
      }
      }
    },
    render: ({state}) => {
      <canvas
        width=(string_of_int(width) ++ "px")
        height=(string_of_int(height) ++ "px")
        ref=ReactDOMRe.Ref.callbackDomRef(node =>  Js.Nullable.toOption(node) |> BaseUtils.optFold(node => state := Some(node), ()))
      />
    }
  });
};

module Styles = {
  open Glamor;
  let container = css([
    padding("20px")
  ])
};

let fldiv = (x, y) => float_of_int(x) /. float_of_int(y);

let shrinkToMax = ((width, height), max) => {
  if (width < max && height < max) {
    (width, height)
  } else if (width > height) {
    (max, int_of_float(fldiv(height, width) *. float_of_int(max)))
  } else {
    (int_of_float(fldiv(width, height) *. float_of_int(max)), max)
  }
};

type transform =
  | Orig
  | One80
  | Clockwise90
  | AntiClockwise90;

let transformSize = ((w, h), transform) => switch transform {
| Orig | One80 => (w, h)
| Clockwise90 | AntiClockwise90 => (h, w)
};

let drawTransformed = (transform, image, (w, h), ctx) => {
  let (x, y) = switch transform {
  | Orig => (0, 0)
  | Clockwise90 => (0, -h)
  | AntiClockwise90 => (-w, 0)
  | One80 => (-w, -h)
  };
  Images.save(ctx);
  switch transform {
  | Orig => ()
  | One80 => Images.rotate(ctx, 3.14159)
  | Clockwise90 => Images.rotate(ctx, 3.14159 /. 2.)
  | AntiClockwise90 => Images.rotate(ctx, -3.14159 /. 2.)
  };
  Images.drawImage(ctx, image, x, y, w, h);
  Images.restore(ctx);
};

let newCanvasWithSize: ((int, int)) => (Images.canvas, unit => unit) = [%bs.raw {|
  function(size) {
    var canvas = document.createElement('canvas')
    canvas.width = size[0]
    canvas.height = size[1]
    canvas.style.display = 'none'
    document.body.appendChild(canvas)
    return [canvas, () => canvas.parentNode.removeChild(canvas)]
  }
|}];

let transformImage = (transform, image, onDone) => {
  let oSize = shrinkToMax((Images.naturalWidth(image), Images.naturalHeight(image)), 2000);
  let size = transformSize(oSize, transform);
  let (canvas, dispose) = newCanvasWithSize(size);
  drawTransformed(transform, image, oSize, Images.getContextFromCanvas(canvas));
  Js.log2("Exporting, size", oSize);
  Images.toBlob(canvas, (blob) => {
    dispose();
    onDone(blob);
  }, "image/jpeg", 0.9)
};

let component = ReasonReact.reducerComponent("ImageEditor");

[@react.component] let make = (~onDone, ~onCancel, ~blob, ~img) => {
  let size = shrinkToMax((
    Images.naturalWidth(img),
    Images.naturalHeight(img)
  ), 1000); /* TODO make this min screen dimension or something, with margin */
  ReactCompat.useRecordApi({
    ...component,
    initialState: () => (Orig),
    reducer: (newState, _) => ReasonReact.Update(newState),
    render: ({send, state: (transform)}) => {
      <UtilComponents.Backdrop onCancel>
        <div className=Styles.container
        onClick=(evt => ReactEvent.Mouse.stopPropagation(evt))
        >
        (str("editor"))
        <Canvas
          size=transformSize(size, transform)
          render=(ctx => {
            drawTransformed(transform, img, size, ctx);
          })
        />
        <div className=RecipeStyles.row>
        <button
          onClick=(((_) => send((Orig))))
        >
          (str("Original"))
        </button>
        <button
          onClick=(((_) => send((Clockwise90))))
        >
          (str("Clockwise90"))
        </button>
        <button
          onClick=(((_) => send((One80))))
        >
          (str("180"))
        </button>
        <button
          onClick=(((_) => send((AntiClockwise90))))
        >
          (str("AntiClockwise90"))
        </button>
        <button
          onClick=(evt => {
            switch transform {
            | Orig => onCancel()
            | _ => transformImage(transform, img, onDone)
            }
          })
        >
          (str("Save"))
        </button>
        </div>
        </div>
      </UtilComponents.Backdrop>
    }
  })
};

module ImageLoader = {
  let component = ReasonReact.reducerComponent("ImageLoader");
  [@react.component] let make = (~url, ~render) => ReactCompat.useRecordApi({
    ...component,
    initialState: () => (None, false),
    reducer: (state, _) => ReasonReact.Update(state),
    render: ({state: (img, loaded), send}) => {
      Js.log2("Loadded", loaded);
      <div>
        <img
          src=url
          style=ReactDOMRe.Style.make(~display="none", ())
          onLoad=((_) => send((img, true)))
          ref=?(switch img {
          | None => Some(ReactDOMRe.Ref.callbackDomRef(node =>  Js.Nullable.toOption(node) |> BaseUtils.optFold(node => send((Some(node), false)), ())))
          | _ => None
          })
        />
        (switch (img, loaded) {
        | (Some(img), true) => render(img)
        | _ => ReasonReact.null
        })
      </div>
    }
  });
};


let innerProps = makeProps;

[@react.component]
let make = (~onDone, ~onCancel, ~blob) => ImageLoader.make(
  ImageLoader.makeProps(
  ~url=Images.cachingCreateObjectURL(blob),
  ~render=(img => {
    make(innerProps(~onDone, ~onCancel, ~blob, ~img, ()))
  }),
  ()
  )
);