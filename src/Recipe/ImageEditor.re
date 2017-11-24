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
      ReasonReact.NoUpdate
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
        ref=(node => Js.Nullable.to_opt(node) |> BaseUtils.optFold(node => state := Some(node), ()))
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

let component = ReasonReact.reducerComponent("ImageEditor");

let make = (~onDone, ~onCancel, ~blob, ~img, _children) => {
  let size = shrinkToMax((
    Images.naturalWidth(img),
    Images.naturalHeight(img)
  ), 1000); /* TODO make this min screen dimension or something, with margin */
  {
    ...component,
    initialState: () => (size, Orig),
    reducer: (newState, _) => ReasonReact.Update(newState),
    render: ({reduce, state: (size, transform)}) => {
      <UtilComponents.Backdrop onCancel>
        <div className=Styles.container
        onClick=(evt => ReactEventRe.Mouse.stopPropagation(evt))
        >
        (str("editor"))
        <Canvas
          size=transformSize(size, transform)
          render=(ctx => {
            let (w, h) = size;
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
            Images.drawImage(ctx, img, x, y, w, h);
            Images.restore(ctx);
            ()
          })
        />
        <button
          onClick=(reduce((_) => (size, Orig)))
        >
          (str("Original"))
        </button>
        <button
          onClick=(reduce((_) => (size, Clockwise90)))
        >
          (str("Clockwise90"))
        </button>
        <button
          onClick=(reduce((_) => (size, One80)))
        >
          (str("180"))
        </button>
        <button
          onClick=(reduce((_) => (size, AntiClockwise90)))
        >
          (str("AntiClockwise90"))
        </button>
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
    render: ({state: (img, loaded), reduce}) => {
      Js.log2("Loadded", loaded);
      <div>
        <img
          src=url
          onLoad=((_) => reduce(() => (img, true))())
          ref=?(switch img {
          | None => Some(node => Js.Nullable.to_opt(node) |> BaseUtils.optFold(node => reduce(() => (Some(node), false))(), ()))
          | _ => None
          })
        />
        (switch (img, loaded) {
        | (Some(img), true) => render(img)
        | _ => ReasonReact.nullElement
        })
      </div>
    }
  };
};

let getBlobSize: Images.blob => Js.Promise.t((int, int)) = [%bs.raw {|
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
|}];

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