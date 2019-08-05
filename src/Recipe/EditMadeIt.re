open Utils;

let oneMeg = 1024 * 1024;

let shrinkImage: Images.blob => Js.Promise.t(Images.blob) = [%bs.raw {|
  function(blob) {
    return new Promise((res, rej) => {
      setTimeout(() => rej(new Error('timeout')), 1000);
      console.log('shrinking', blob.size)
      var image = document.createElement('img')
      image.style.display = 'none'
      document.body.appendChild(image)
      image.onload = () => {
        var area = image.naturalWidth * image.naturalHeight;
        // shoot for 700k
        var resizeRatio = 0.7 * 1024 * 1024 / blob.size
        var newArea = area * resizeRatio
        var wToh = image.naturalWidth / image.naturalHeight;
        /*
          newArea = w * h
          w / h = wToh
          w = wToh * h
          w = newArea / h
          wToh * h = newArea / h
          h * h = newArea / wToh
        */
        var newH = Math.sqrt(newArea / wToh)
        var newW = wToh * newH;
        var canvas = document.createElement('canvas')
        document.body.appendChild(canvas)
        canvas.style.display = 'none'
        canvas.width = newW
        canvas.height = newH
        var ctx = canvas.getContext('2d')
        ctx.drawImage(image, 0, 0, newW, newH)
        canvas.toBlob(blob => {
          canvas.parentNode.removeChild(canvas)
          image.parentNode.removeChild(image)
          res(blob)
        }, 'image/jpeg', 0.9)
      }
      image.src = URL.createObjectURL(blob)
    })
  }
|}];

let ensureSmallEnough = (blob) => {
  if (Images.blobSize(blob) < oneMeg) {
    Js.Promise.resolve(blob)
  } else {
    shrinkImage(blob)
  }
};

let failAfter = t => Js.Promise.make((~resolve, ~reject) => Js.Global.setTimeout(() => [@bs]reject(Not_found), t) |> ignore);

let withTimeout = (prom, t) => {
  Js.Promise.race([|
    prom,
    failAfter(t)
  |])
};

let uploadImage = (~fb, ~uid, ~blob, ~recipeId, ~madeItId) => {
  let id = BaseUtils.uuid();
  let path = "images/" ++ recipeId ++ "/" ++ uid ++ "/" ++ madeItId ++ "/" ++ id ++ ".jpg";
  Js.log3("Uploading", path, blob);
  Firebase.Storage.get(Firebase.app(fb))
  |> Firebase.Storage.ref
  |> Firebase.Storage.child(path)
  |> reff => withTimeout(Firebase.Storage.put(blob, reff), 30000)
  |> Js.Promise.then_(snap => {
    Js.log2("Uploaded!", path);
    Js.Promise.resolve(Some(path))
  })
  |> Js.Promise.catch(err => {
    Js.log3("error uploading image", path, err);
    Js.Promise.resolve(None)
  })
  /* Js.Promise.resolve("") */
  /* |> Firebase.Storage. */
};

let filterNils = items => Js.Array.reduce((good, item) => switch item { | None => good | Some(x) => Js.Array.concat(good, [|x|])}, [||], items);

let ensureImagesUploaded = (~fb, ~uid, ~images, ~recipeId, ~madeItId) => {
  Js.Promise.all(
    images |> Array.map(image => switch image {
    | ImageUploader.AlreadyUploaded(id) => Js.Promise.resolve(Some(id))
    | NotUploaded(blob) =>
      ensureSmallEnough(blob)
      |> Js.Promise.then_(blob => uploadImage(~fb, ~uid, ~blob, ~recipeId, ~madeItId))
    })
  )
  |> Js.Promise.then_(items => Js.Promise.resolve(filterNils(items)))
  |> Js.Promise.catch(err => {
    Js.log2("Images error", err);
    Js.Promise.reject(Obj.magic(err))
  })
};

module Styles = {
  open Glamor;
  let label = css([fontSize("20px"), fontWeight("400")]);
  let title =
    css
      /* fontSize("32px"), */
      ([textAlign("center"), fontSize("20px"), fontWeight("400")]);
  /* borderBottom("2px solid " ++ Shared.dark), */
  /* fontFamily(Shared.titleFont), */
  let container = css([padding("16px"), border("2px solid #eee")]);
};

type state = {
  saving: bool,
  notes: string,
  rating: option(int),
  meta: Models.meta,
  created: MomentRe.Moment.t,
  /*
   ingredients: array(Models.maybeRecipeIngredient),
   ingredientHeaders: array(Models.header),

   instructions: array(Models.maybeRecipeIngredient),
   instructionHeaders: array(Models.header),

   imageUrl: Js.null(string),
   */
   images: array(ImageUploader.image)
};

type action =
  | SetText(string)
  | SetRating(option(int))
  | StartSaving
  | DoneSaving
  | SetImages(array(ImageUploader.image))
  | SetCreated(MomentRe.Moment.t);

let component = ReasonReact.reducerComponent("MadeItEntry");

let updateMadeIt = (~madeIt, ~state as {notes, rating, meta, created}, ~images) : Models.madeIt => {
  "id": madeIt##id,
  "recipeId": madeIt##recipeId,
  "authorId": madeIt##authorId,
  "collaborators": madeIt##collaborators,
  "isPrivate": madeIt##isPrivate,
  "created": MomentRe.Moment.valueOf(created),
  "updated": Js.Date.now(),
  "imageUrl": madeIt##imageUrl,
  "images": images,
  "instructions": madeIt##instructions,
  "instructionHeaders": madeIt##instructionHeaders,
  "ingredients": madeIt##ingredients,
  "ingredientHeaders": madeIt##ingredientHeaders,
  "ateWithRecipes": madeIt##ateWithRecipes |> BaseUtils.magicDefault([||]),
  "ateWithPeople": madeIt##ateWithPeople |> BaseUtils.magicDefault([||]),
  "batches": madeIt##batches,

  "meta": meta,
  "notes": notes,
  "rating": Js.Null.fromOption(rating)
};

let make = (~fb, ~uid, ~title, ~action, ~initial: Models.madeIt, ~onCancel, ~onSave, _children) => {
  ...component,
  initialState: (_) => {
    saving: false,
    notes: initial##notes,
    rating: initial##rating |> Js.Null.toOption,
    meta: initial##meta,
    created: initial##created |> MomentRe.momentWithTimestampMS,
    images: initial##images |> Array.map(name => ImageUploader.AlreadyUploaded(name)),
  },
  reducer: (action, state) =>
    switch action {
    | StartSaving => ReasonReact.Update({...state, saving: true})
    | DoneSaving => ReasonReact.Update({...state, saving: false})
    | _ when state.saving === true => ReasonReact.NoUpdate

    | SetText(notes) => ReasonReact.Update({...state, notes})
    | SetRating(rating) => ReasonReact.Update({...state, rating})
    | SetImages(images) => ReasonReact.Update({...state, images})
    | SetCreated(created) => ReasonReact.Update({...state, created})
    },
  render: ({send, state: {notes, rating, created, images} as state}) =>
    <div className=Styles.container>
      <div className=Styles.title> (str(title)) </div>
      (spacer(16))
      <div className=RecipeStyles.row>
        <div className=Styles.label> (str("Date")) </div>
        (spacer(16))
        <DatePicker
          selected=created
          onChange=(((time) => send(SetCreated(time))))
          placeholderText="Awesome"
        />
      </div>
      <div className=Styles.label> (str("Rating")) </div>
      (spacer(16))
      <RatingWidget rating onChange=(((rating) => send(SetRating(rating)))) />
      (spacer(32))
      <div className=Styles.label> (str("Notes")) </div>
      (spacer(16))
      <Textarea value=notes onChange=(((text) => send(SetText(text)))) />
      (spacer(32))
      <div className=Styles.label> (str("Images")) </div>
      (spacer(32))
      <ImageUploader
        fb
        images
        onChange=((newImages => send(SetImages(newImages))))
      />

      (spacer(32))
      <div className=RecipeStyles.row>
        <button
          onClick=(
            (_) => {
              send(StartSaving);
              ensureImagesUploaded(
                ~fb,
                ~uid,
                ~madeItId=initial##id,
                ~recipeId=initial##recipeId,
                ~images
              ) |> Js.Promise.then_(
                ids => {
                  let madeIt = updateMadeIt(~madeIt=initial, ~state, ~images=ids);
                  module FB = Firebase.Collection(Models.MadeIt);
                  let collection = FB.get(fb);
                  let doc = Firebase.doc(collection, madeIt##id);
                  Firebase.set(doc, madeIt)
                  |> Js.Promise.then_(
                      () => {
                        onSave();
                        Js.Promise.resolve()
                      }
                    ) |> ignore;
                  Js.Promise.resolve();
                }
              )
              |> Js.Promise.catch(
                err => {
                  send(DoneSaving);
                  Js.Promise.resolve()
                }
              ) |> ignore
            }
          )
          className=RecipeStyles.primaryButton>
          (str(action))
        </button>
        <button onClick=((_) => onCancel()) className=RecipeStyles.button>
          (str("Cancel"))
        </button>
      </div>
    </div>
};
