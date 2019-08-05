open Utils;

module Adder = {
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let make = (~fb, ~uid, ~recipe, _children) => {
    ...component,
    initialState: (_) => false,
    reducer: ((), state) => ReasonReact.Update(! state),
    render: ({state, send}) =>
      <div className=Glamor.(css([]))>
        (
          state ?
            <EditMadeIt
              fb
              uid
              title="Add experience"
              action="Add"
              initial=Models.MadeIt.fromRecipe(recipe, uid)
              onCancel=((() => send( ())))
              onSave=((() => send( ())))
            /> :
            <div
              className=Glamor.(
                          css([
                            flexDirection("row"),
                            alignItems("flex-start"),
                            justifyContent("flex-start")
                          ])
                        )>
              <button className=RecipeStyles.primaryButton onClick=(((_) => send(())))>
                (str("Record experience"))
              </button>
            </div>
        )
      </div>
  };
};