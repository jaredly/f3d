open BaseUtils;

/* type command =
   | FirstInstruction
   | NextInstruction
   | Repeat
   | RestInstructions
   | Instructions
   | Ingredients
   | Stop
   | FullIngredients
   | UnknownCommand strig */
type state = {
  currently: [ | `Speaking | `Listening | `Off],
  currentIngredient: int,
  currentInstruction: int,
  unlisten: ref(option((unit => unit)))
};

type actions =
  | TurnOn
  | TurnOff
  | StartSpeaking
  | StartSpeakingAt((int, int))
  | DoneSpeaking;

let component = ReasonReact.reducerComponent("Speaker");

let filterMap = (fn, items) => {
  let res = [||];
  Js.Array.forEach(
    (item) =>
      switch (fn(item)) {
      | None => ()
      | Some(value) => Js.Array.push(value, res) |> ignore
      },
    items
  );
  res
};

let listen = (handleCommand) => SpeechSynthesis.recognize(handleCommand);

let speak = (send, text) => {
  (send(StartSpeaking));
  SpeechSynthesis.speak(text, ((_) => send(DoneSpeaking)))
};

let speakMany = (send, items) => {
  (send(StartSpeaking));
  SpeechSynthesis.speakSeveral(items, ((_) => send(DoneSpeaking)))
};

let parseCommand = (command, map, ingredients) =>
  switch command {
  | "what's the first instruction"
  | "what's the first construction"
  | "what is the first instruction"
  | "what is the first construction"
  | "first construction"
  | "first step"
  | "first instruction" => `FirstInstruction
  | "repeat that"
  | "repeat"
  | "again" => `Repeat
  | "what's the next instruction"
  | "what's the next step"
  | "what is the next instruction"
  | "what is the next step"
  | "next step"
  | "next instruction" => `NextInstruction
  | "what are the rest of the instructions"
  | "rest of instructions"
  | "rest instructions" => `RestInstructions
  | "read me the instructions"
  | "read the instructions"
  | "what are the instructions"
  | "instructions" => `Instructions
  | "list the ingredients"
  | "read me the ingredients"
  | "read the ingredients"
  | "list of ingredients"
  | "what are the ingredients"
  | "ingredients" => `Ingredients
  /* SpeechSynthesis.speakSeveral
     (Array.map (SpeechSynthesis.ingredientText map) ingredients)
     listen */
  | "stop"
  | "done" => `Stop
  | "list the full ingredients"
  | "list of full ingredients"
  | "what are the full ingredients"
  | "full ingredients" => `FullIngredients
  /* SpeechSynthesis.speakSeveral
     (Array.map (SpeechSynthesis.fullIngredientText map) ingredients)
     listen */
  | _ =>
    let ingredient =
      if (Js.String.startsWith("how much", command)) {
        Some(Js.String.sliceToEnd(~from=String.length("how much"), command))
      } else if (Js.String.startsWith("how many", command)) {
        Some(Js.String.sliceToEnd(~from=String.length("how many"), command))
      } else {
        None
      };
    switch ingredient {
      | None => `UnknownCommand(command)
      | Some(ingredient) => {

        let ingredient = Js.String.trim(ingredient) |> Js.String.toLocaleLowerCase;
        let found =
          filterMap(
            (item) =>
              Js.Dict.get(map, item##ingredient)
              |> optBind((ing) => ing##name === ingredient ? Some(item) : None),
            ingredients
          );
        Js.log2("found", found);
        switch found {
        | [||] => `IngredientNotFound(ingredient)
        | _ => `Ingredient(found)
      }
      }
    }
  };

let lastCommand = ref(None);

let handleCommand = (send, map, ingredients, instructions, state, text) => {
  /* SpeechSynthesis.speak "Turning off" (reduce (fun _ => TurnOff)) */
  switch (Js.Null.toOption(text)) {
    | None => send(TurnOn)
    | Some(text) =>
  let command = parseCommand(text, map, ingredients);
  let command =
    command == `Repeat ?
      lastCommand^ |> optOr(`Repeat) :
      {
        lastCommand := Some(command);
        command
      };
  switch command {
  | `RestInstructions
  | `Repeat => speak(send, "Nothing to repeat")
  | `Stop => SpeechSynthesis.speak("Turning off", ((_) => send(TurnOff)))
  | `FullIngredients =>
    speakMany(send, Array.map(SpeechSynthesis.fullIngredientText(map), ingredients))
  | `Ingredients => speakMany(send, Array.map(SpeechSynthesis.ingredientText(map), ingredients))
  | `IngredientNotFound(name) => speak(send, "I can't find " ++ (name ++ " in the recipe"))
  | `Ingredient((ings: array(Models.recipeIngredient))) =>
    speakMany(send, Array.map(SpeechSynthesis.fullIngredientText(map), ings))
  | `UnknownCommand(_command) => {
    SpeechSynthesis.beep2();
    send(DoneSpeaking)
  }
        /* speak(send, "Unknown command: " ++ command) */
  | `FirstInstruction =>
    (send(StartSpeakingAt((state.currentIngredient, 0))));
    SpeechSynthesis.speak(instructions[0]##text, ((_) => send(DoneSpeaking)))
  | `NextInstruction =>
    let next = state.currentInstruction + 1;
    if (next >= Array.length(instructions)) {
      speak(send, "No more instructions")
    } else {
      (send(StartSpeakingAt((state.currentIngredient, next))));
      SpeechSynthesis.speak(instructions[next]##text, ((_) => send(DoneSpeaking)))
    }
  | `Instructions => speakMany(send, Array.map((i) => i##text, instructions))
  }
  }
};

let make = (~allIngredients, ~ingredients, ~instructions, _) =>
  ReasonReact.{
    ...component,
    initialState: () => {
      currently: `Off,
      currentIngredient: 0,
      currentInstruction: 0,
      unlisten: ref(None)
    },
    reducer: (action, state) =>
      switch action {
      | TurnOn =>
        ReasonReact.UpdateWithSideEffects(
          {...state, currently: `Listening},
          (
            ({state, send}) =>
              /* (handleCommand reduce (Ingredients.ingredientsMap allIngredients) ingredients instructions state (Js.Null.return "what is the first instruction")) */
              state.unlisten :=
                Some(
                  listen(
                    handleCommand(
                      send,
                      Ingredients.ingredientsMap(allIngredients),
                      ingredients,
                      instructions,
                      state
                    )
                  )
                )
          )
        )
      | TurnOff =>
        ReasonReact.UpdateWithSideEffects(
          {...state, currently: `Off},
          (
            ({state}) =>
              switch state.unlisten^ {
              | Some(fn) => fn()
              | None => ()
              }
          )
        )
      | StartSpeaking => ReasonReact.Update({...state, currently: `Speaking})
      | StartSpeakingAt((currentIngredient, currentInstruction)) =>
        ReasonReact.Update({...state, currently: `Speaking, currentInstruction, currentIngredient})
      | DoneSpeaking =>
        ReasonReact.UpdateWithSideEffects(
          {...state, currently: `Listening},
          (
            ({state, send}) => {
              SpeechSynthesis.beep();
              state.unlisten :=
                Some(
                  listen(
                    handleCommand(
                      send,
                      Ingredients.ingredientsMap(allIngredients),
                      ingredients,
                      instructions,
                      state
                    )
                  )
                )
            }
          )
        )
      },
    render: ({state, send}) =>
      <button
        className=RecipeStyles.button
        onClick=(
          (_) =>
            switch state.currently {
            | `Off =>
              SpeechSynthesis.beep();
              send(TurnOn)
            | `Speaking
            | `Listening => send(TurnOff)
            }
        )>
        (
          Utils.str(
            switch state.currently {
            | `Off => "Listen"
            | `Speaking => "Speaking"
            | `Listening => "Listening"
            }
          )
        )
      </button>
  };

/* Glamor.(
                    css([
                      backgroundColor("transparent"),
                      border("none"),
                      cursor("pointer"),
                      fontSize("inherit"),
                      fontFamily("inherit"),
                      fontWeight("inherit"),
                      Selector(":hover", [backgroundColor("#eee")])
                    ])
                  ) */