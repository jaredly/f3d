open Utils;

type blob = Images.blob;

type status =
  | Initial
  | LoadingPreviews
  | Preview
  | Uploading;

type image =
  | AlreadyUploaded(string)
  | NotUploaded(blob);

type state = {
  triggerInput: ref(option((unit => unit))),
  /* imageRefs: Js.Dict.t(Dom.element), */
  /* objectUrls: array(string), */
  edit: option((int, blob)),
  status
};

type action =
  | SetStatus(status)
  | Edit(int, blob)
  | StopEditing
  /* | SetFiles(array(string)) */
  ;

let clickDom = (el) => ReactDOMRe.domElementToObj(el)##click();

let component = ReasonReact.reducerComponent("ImageUploader");

let module S = ReactDOMRe.Style;

let make = (~fb, ~images, ~onChange, _children) => {
  ...component,
  initialState: () => {
    triggerInput: ref(None),
    edit: None,
    status: Initial
  },
  reducer: (action, state) => switch action {
  | SetStatus(status) => ReasonReact.Update({...state, status})
  | Edit(i, blob) => ReasonReact.Update({...state, edit: Some((i, blob))})
  | StopEditing => ReasonReact.Update({...state, edit: None})
  /* | SetFiles(objectUrls) => ReasonReact.Update({...state, objectUrls}) */
  },
  render: ({reduce, state}) =>
    <div>
      <div style=S.make(~flexDirection="row", ())>
        (
          images |> Array.mapi((i, file) =>
          <div
            key=string_of_int(i)
          >
            (switch file {
            | AlreadyUploaded(id) => <FirebaseImage fb id render=(src => <img width="100px" src />)/>
            | NotUploaded(blob) => <img
               width="100px"
               src=(Images.cachingCreateObjectURL(blob))
               onClick=(reduce ((_) => Edit(i, blob)))
              />
            })
          </div>)
          |> ReasonReact.arrayToElement
        )
      </div>
      <input
        _type="file"
        style=(ReactDOMRe.Style.make(~visibility="hidden", ()))
        onChange=(evt => {
          let obj = ReactEventRe.Form.target(evt) |> ReactDOMRe.domElementToObj;
          let files = obj##files;
          Js.log2("files", files);
          onChange(Array.append(images, Array.map(file => NotUploaded(file), files)));
          /* reduce(() => SetFiles(
            Array.append(state.objectUrls, Array.map(file => createObjectURL(file), files))
          ))(); */
          /* reduce(() => SetStatus(Preview))(); */

        })
        multiple=Js.true_
        ref=(
          (node) =>
            Js.Nullable.to_opt(node)
            |> BaseUtils.optFold(
                 (node) => state.triggerInput.contents = Some(() => clickDom(node)),
                 ()
               )
        )
      />
      <button
        onClick=?{state.triggerInput^ |> BaseUtils.optMap(Utils.ignoreArg)}
      >
        (str("Add Images"))
      </button>
      (state.edit |> BaseUtils.optFoldReact(
        ((i, blob)) => <ImageEditor blob onDone=((newBlob) => {
          let images = Js.Array.copy(images);
          images[i] = NotUploaded(newBlob);
          onChange(images);
        }) onCancel=(() => {
          reduce(() => StopEditing)()
        })/>
      ))
      /* (
        switch state.status {
        | Initial => <div/>
        | LoadingPreviews => <div>(str("Loading previews"))</div>
        | Preview => <button onClick=((_) => {
          /**
           * TODO I actually want to uploading to happen when you save then MadeIt I think?
           * OR would I rather have it happen in the real time? let's try at the end and
           * see if it takes too long.
           * Soo I want to be setting state back to our `EditMadeIt`, where images is a list
           * of `UploadedImage(string) |  NonUploadedImage(blobUrl)`
           * and then at game time, we take the bloburl images, drop them into <img>s,
           * render them to a canvas, make sure it's a small enough size, and then go to town.
           * AAAaaactually, that's wayy to removed. b/c if it's a fine size, why go though
           * the dance?
           *
           * So: UploadedImage(string) | NonUploadedImage(blob)
           */
          /* let imageElements = Js.Dict.values(state.images); */
          /* Array.map(smallEnoughFileFromImage, imageElements)
          |> Js.Promise.all
          |> Js.Promise.then_(blobs => {
            Array.map(uploadImage(~recipeId, ~fb, ~uid), blobs) |> Js.Promise.all
          })
          |> Js.Promise.then_(imageIds => {
          }); */
          /* if they're under 5 megs, upload them. Otherwise,  */
          ()
        })>(str("Upload images"))</button>
        | Uploading => <div>(str("Uploading..."))</div>
        }
      ) */
    </div>
};