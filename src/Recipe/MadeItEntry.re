open Utils;

module Adder = {
  let component = ReasonReact.reducerComponent("MadeItEntry");
  let make = (~fb, ~uid, ~recipe, _children) => {
    ...component,
    initialState: (_) => false,
    reducer: ((), state) => ReasonReact.Update(! state),
    render: ({state, reduce}) =>
      <div className=Glamor.(css([]))>
        (
          state ?
            <EditMadeIt
              fb
              uid
              title="Add experience"
              action="Add"
              initial=Models.MadeIt.fromRecipe(recipe, uid)
              onCancel=(reduce(() => ()))
              onSave=(reduce(() => ()))
            /> :
            <div
              className=Glamor.(
                          css([
                            flexDirection("row"),
                            alignItems("flex-start"),
                            justifyContent("flex-start")
                          ])
                        )>
              <button className=RecipeStyles.primaryButton onClick=(reduce((_) => ()))>
                (str("Record experience"))
              </button>
            </div>
        )
      </div>
  };
};