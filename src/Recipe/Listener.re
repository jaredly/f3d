
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
  currently: [`Speaking | `Listening | `Off],
  currentIngredient: int,
  currentInstruction: int,
  unlisten: ref (option (unit => unit)),
};

type actions =
  | TurnOn
  | TurnOff
  | StartSpeaking
  | StartSpeakingAt (int, int)
  | DoneSpeaking;

let component = ReasonReact.reducerComponent "Speaker";

let filterMap fn items => {
  let res = [||];
  Js.Array.forEach (fun item => switch (fn item) {
  | None => ()
  | Some value => Js.Array.push value res |> ignore
  }) items;
  res
};

let listen handleCommand => {
  SpeechSynthesis.recognize handleCommand
};

let speak reduce text => {
  (reduce (fun _ => StartSpeaking)) ();
  SpeechSynthesis.speak text (reduce (fun _ => DoneSpeaking))
};

let speakMany reduce items => {
  (reduce (fun _ => StartSpeaking)) ();
  SpeechSynthesis.speakSeveral items (reduce (fun _ => DoneSpeaking))
};

let parseCommand command map ingredients => {
  switch command {
  | "what's the first instruction"
  | "what is the first instruction"
  | "first instruction" => `FirstInstruction

  | "repeat that"
  | "repeat"
  | "again" => `Repeat

  | "what's the next instruction"
  | "what is the next instruction"
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
  | "ingredients" => 
    `Ingredients
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

  | _ => {
    let ingredient = if (Js.String.startsWith "how much" command) {
      Some (Js.String.sliceToEnd from::(String.length "how much") command)
    } else if (Js.String.startsWith "how many" command) {
      Some (Js.String.sliceToEnd from::(String.length "how many") command)
    } else { None };
    [%guard let Some ingredient = ingredient][@else `UnknownCommand command];
    let ingredient = Js.String.trim ingredient |> Js.String.toLocaleLowerCase;
    let found = filterMap (fun item => {
      Js.Dict.get map item##ingredient |> optBind (fun ing => ing##name === ingredient ? Some item : None)
    }) ingredients;
    Js.log2 "found" found;
    switch found {
    | [||] => `IngredientNotFound ingredient
    | _ => `Ingredient found
    }        
  }
  }
};

let lastCommand = ref None;

let handleCommand reduce map ingredients instructions state text => {
  [%guard let Some text = Js.Null.to_opt text][@else
  /* SpeechSynthesis.speak "Turning off" (reduce (fun _ => TurnOff)) */
  (reduce (fun _ => TurnOn)) ()
  ];
  let command = (parseCommand text map ingredients);
  let command = command == `Repeat
  ? !lastCommand |> optOr `Repeat
  : {lastCommand := Some command; command};
  switch command {
  | `RestInstructions
  | `Repeat => speak reduce "Nothing to repeat"
  | `Stop => SpeechSynthesis.speak "Turning off" (reduce (fun _ => TurnOff))
  | `FullIngredients => {
    speakMany reduce (Array.map (SpeechSynthesis.fullIngredientText map) ingredients)
  }
  | `Ingredients => {
    speakMany reduce (Array.map (SpeechSynthesis.ingredientText map) ingredients)
  }
  | `IngredientNotFound name => speak reduce ("I can't find " ^ name ^ " in the recipe")
  | `Ingredient (ings: array Models.recipeIngredient) => speakMany reduce (Array.map (SpeechSynthesis.fullIngredientText map) ings)
  | `UnknownCommand command => {
    speak reduce ("Unknown command: " ^ command) 
  }
  | `FirstInstruction => {
    (reduce (fun _ => StartSpeakingAt (state.currentIngredient, 0))) ();
    SpeechSynthesis.speak (instructions.(0)##text) (reduce (fun _ => DoneSpeaking))
  }
  | `NextInstruction => {
    let next = state.currentInstruction + 1;
    if (next >= Array.length instructions) {
      speak reduce "No more instructions";
    } else {
      (reduce (fun _ => StartSpeakingAt (state.currentIngredient, next))) ();
      SpeechSynthesis.speak (instructions.(next)##text) (reduce (fun _ => DoneSpeaking))
    }
  }
  | `Instructions => speakMany reduce (Array.map (fun i => i##text) instructions)
}
};

let make ::allIngredients ::ingredients ::instructions _ => ReasonReact.{
  ...component,
  initialState: fun () => {currently: `Off, currentIngredient: 0, currentInstruction: 0, unlisten: ref None},
  reducer: fun action state => (switch action {
  | TurnOn => ReasonReact.UpdateWithSideEffects {...state, currently: `Listening} (fun {state, reduce} => {
    /* (handleCommand reduce (Ingredients.ingredientsMap allIngredients) ingredients instructions state (Js.Null.return "what is the first instruction")) */
    state.unlisten := Some (listen (handleCommand reduce (Ingredients.ingredientsMap allIngredients) ingredients instructions state))
  })
  | TurnOff => ReasonReact.UpdateWithSideEffects {...state, currently: `Off} (fun {state} => {
    switch (!state.unlisten) {
    | Some fn => fn ()
    | None => ()
    }
  })
  | StartSpeaking => ReasonReact.Update {...state, currently: `Speaking}
  | StartSpeakingAt (currentIngredient, currentInstruction) => ReasonReact.Update {...state, currently: `Speaking, currentInstruction, currentIngredient}
  | DoneSpeaking => ReasonReact.UpdateWithSideEffects {...state, currently: `Listening} (fun {state, reduce} => {
    SpeechSynthesis.beep();
    state.unlisten := Some (listen (handleCommand reduce (Ingredients.ingredientsMap allIngredients) ingredients instructions state))
  })
  }),
  render: fun {state, reduce} => {
    <button
      className=Glamor.(css[
        backgroundColor "transparent",
        border "none",
        cursor "pointer",
        fontSize "inherit",
        fontFamily "inherit",
        fontWeight "inherit",
        Selector ":hover" [
          backgroundColor "#eee",
        ]
      ])
      onClick=(fun _ => {
        switch state.currently {
        | `Off => {
          SpeechSynthesis.beep();
          reduce (fun _ => TurnOn) ()
        }
        | `Speaking
        | `Listening => reduce (fun _ => TurnOff) ()
        }
      })
    >
      (Utils.str (switch state.currently {
      | `Off => "Listen"
      | `Speaking => "Speaking"
      | `Listening => "Listening"
      }))
    </button>
  }
};

