open Utils;

let module Canvas = {
  let component = ReasonReact.reducerComponent("ImageEditor");
  let make = (~size as (width, height) , ~render, _children) => {
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
        ref=(node =>  Js.Nullable.toOption(node) |> BaseUtils.optFold(node => state := Some(node), ()))
      />
    }
  };
};

/*
Wierd bug for posterity

let make = (~onDone, ~blob, ~size= (0, 0), _children) => {
  ...component,
  initialState: () => (blob, size),
  /* the bug is v here, where I pretend state is unit */
  reducer: ((), ()) => ReasonReact.NoUpdate,
  render: ({state}) => {
    <div>
    </div>
  }
};

make(~onDone, ~blob, ~size) |> ReasonReact.element
/* but I get the error here, saying super weird things about retainedprops or something */

*/

let module Styles = {
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

let make = (~onDone, ~onCancel, ~blob, ~img, _children) => {
  let size = shrinkToMax((
    Images.naturalWidth(img),
    Images.naturalHeight(img)
  ), 1000); /* TODO make this min screen dimension or something, with margin */
  {
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
  }
};

let module ImageLoader = {
  let component = ReasonReact.reducerComponent("ImageLoader");
  let make = (~url, ~render, _children) => {
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
          | None => Some(node =>  Js.Nullable.toOption(node) |> BaseUtils.optFold(node => send((Some(node), false)), ()))
          | _ => None
          })
        />
        (switch (img, loaded) {
        | (Some(img), true) => render(img)
        | _ => ReasonReact.null
        })
      </div>
    }
  };
};

/* let getBlobSize: Images.blob => Js.Promise.t((int, int)) = [%bs.raw {|
  function(blob) {
    return new Promise((res, rej) => {
      var img = document.createElement('img')
      img.style.display = 'none'
      document.body.appendChild(img)
      img.onload = () => {
        res([img.naturalHeight, img.naturalWidth])
      }
      img.src = URL.createObjectURL(blob);
    })
  }
|}]; */

/* let module ThingLoader = UtilComponents.Loader({type t = (int, int); });
let make = (~onDone, ~onCancel, ~blob, _children) => ThingLoader.make(
  ~promise=getBlobSize(blob),
  ~loading=(<div>(str("Loading..."))</div>),
  ~render=((size) =>
  <ImageLoader
    url=
  make(~onDone, ~onCancel, ~blob, [||]) |> ReasonReact.element
  )
); */

let make = (~onDone, ~onCancel, ~blob, _children) => ImageLoader.make(
  ~url=Images.cachingCreateObjectURL(blob),
  ~render=(img => {
    make(~onDone, ~onCancel, ~blob, ~img, [||]) |> ReasonReact.element
  }),
  [||]
);