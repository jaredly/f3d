open Utils;

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
  | SetImages(array(ImageUploader.image))
  | SetCreated(MomentRe.Moment.t);

let component = ReasonReact.reducerComponent("MadeItEntry");

let updateMadeIt = (~madeIt, ~state as {notes, rating, meta, created}) : Models.madeIt => {
  "id": madeIt##id,
  "recipeId": madeIt##recipeId,
  "authorId": madeIt##authorId,
  "collaborators": madeIt##collaborators,
  "isPrivate": madeIt##isPrivate,
  "created": MomentRe.Moment.valueOf(created),
  "updated": Js.Date.now(),
  "imageUrl": madeIt##imageUrl,
  "images": madeIt##images,
  "instructions": madeIt##instructions,
  "instructionHeaders": madeIt##instructionHeaders,
  "ingredients": madeIt##ingredients,
  "ingredientHeaders": madeIt##ingredientHeaders,
  "ateWithRecipes": madeIt##ateWithRecipes |> BaseUtils.magicDefault([||]),
  "ateWithPeople": madeIt##ateWithPeople |> BaseUtils.magicDefault([||]),
  "batches": madeIt##batches,

  "meta": meta,
  "notes": notes,
  "rating": Js.Null.from_opt(rating)
};

let make = (~fb, ~uid, ~title, ~action, ~initial: Models.madeIt, ~onCancel, ~onSave, _children) => {
  ...component,
  initialState: (_) => {
    notes: initial##notes,
    rating: initial##rating |> Js.Null.to_opt,
    meta: initial##meta,
    created: initial##created |> MomentRe.momentWithTimestampMS,
    images: [||],
  },
  reducer: (action, state) =>
    switch action {
    | SetText(notes) => ReasonReact.Update({...state, notes})
    | SetRating(rating) => ReasonReact.Update({...state, rating})
    | SetImages(images) => ReasonReact.Update({...state, images})
    | SetCreated(created) => ReasonReact.Update({...state, created})
    },
  render: ({reduce, state: {notes, rating, created, images} as state}) =>
    <div className=Styles.container>
      <div className=Styles.title> (str(title)) </div>
      (spacer(16))
      <div className=RecipeStyles.row>
        <div className=Styles.label> (str("Date")) </div>
        (spacer(16))
        <DatePicker
          selected=created
          onChange=(reduce((time) => SetCreated(time)))
          placeholderText="Awesome"
        />
      </div>
      <div className=Styles.label> (str("Rating")) </div>
      (spacer(16))
      <RatingWidget rating onChange=(reduce((rating) => SetRating(rating))) />
      (spacer(32))
      <div className=Styles.label> (str("Notes")) </div>
      (spacer(16))
      <Textarea value=notes onChange=(reduce((text) => SetText(text))) />
      (spacer(32))
      <div className=Styles.label> (str("Images")) </div>
      (spacer(32))
      /* (images |> Array.map(source => <FirebaseImage fb source render=(url => {
        <div>
        <img src=url />
        /** TODO add a "delete" button that deletes it. */
        </div>
      })/>) |> ReasonReact.arrayToElement) */
      <ImageUploader
        fb
        images
        onChange=(reduce(newImages => SetImages(newImages)))
      />

      (spacer(32))
      <div className=RecipeStyles.row>
        <button
          onClick=(
            (_) => {
              Js.log("adding");
              let madeIt = updateMadeIt(~madeIt=initial, ~state);
              module FB = Firebase.Collection(Models.MadeIt);
              let collection = FB.get(fb);
              let doc = Firebase.doc(collection, madeIt##id);
              Firebase.set(doc, madeIt)
              |> Js.Promise.then_(
                   () => {
                     onSave();
                     Js.Promise.resolve()
                   }
                 )
              |> ignore
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
