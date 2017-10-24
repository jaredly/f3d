
/** This causes the voices to load */
let x = [%bs.raw{|console.log(speechSynthesis.getVoices())|}];

let speak: string => (unit => unit) => unit = [%bs.raw {|
  function(text, done) {
    console.log('speaking', text)
    speechSynthesis.cancel()
    var u = new SpeechSynthesisUtterance(text.slice(0, 500));
    var voiceName = 'Google UK English Female'
    voiceName = 'Google US English'
    if (localStorage.deutch === 'true') {
      voiceName = 'Google Deutsch'
    }
    u.voice = speechSynthesis.getVoices().filter(s => s.voiceURI === voiceName)[0]
    u.rate = 0.8
    var called = false
    var interval = setInterval(() => {
      if (called) return
      if (!speechSynthesis.speaking) {
        console.log('timeout')
        u.onend()
      }
    }, 50)
    var pauser = setInterval(() => {
      speechSynthesis.pause()
      speechSynthesis.resume()
    }, 10 * 1000)
    u.onend = function(event) {
      if (called) return
      clearInterval(interval)
      clearInterval(pauser)
      called = true
      console.log('done')
      done()
    }
    speechSynthesis.speak(u)
  }
|}][@bs];

let speakSeveral: array string => (unit => unit) => unit = fun items ondone => {
  let rec loop i => {
    if (i >= Array.length items) {
      ondone ()
    } else {
      speak items.(i) (fun () => loop (i + 1))
    }
  };
  loop 0;
};

let recognize: (Js.null string => unit) => (unit => unit) = [%bs.raw {|
  function(done) {
    console.log('recognizing');
    var r = new webkitSpeechRecognition();
    r.continuous = false
    r.interimResults = false
    var isDone = false
    r.onresult = evt => {
      if (isDone) return
      isDone = true
      done(evt.results[0][0].transcript)
    }
    r.onend = () => {
      if (!isDone) {
        isDone = true
        done(null)
      }
    }
    r.start()
    return () => {isDone = true; r.abort()}
  }
|}];

let beep: (unit => unit) = [%bs.raw {|
  function () {

    var context = new AudioContext();
    var oscillator = context.createOscillator();
    var gainNode = context.createGain();

    oscillator.frequency.value = 340;
    oscillator.type = 'sine';

    oscillator.connect(gainNode);
    gainNode.connect(context.destination)
    gainNode.gain.setTargetAtTime(0, context.currentTime, 0);
    gainNode.gain.setTargetAtTime(1, context.currentTime, 0.03);
    oscillator.start();

    setTimeout(() => {
      gainNode.gain.setTargetAtTime(0, context.currentTime, 0.015);
      setTimeout(() => {
        oscillator.stop()
        context.close()
      }, 100)
    }, 100)
}
|}];

let ingredientText map ring => {
  open BaseUtils;
  let ing = Js.Dict.get map ring##ingredient;
  [%guard let Some ing = ing][@else ""];
  ing##name ^ "."
};

let fullIngredientText map ring => {
  open BaseUtils;
  let ing = Js.Dict.get map ring##ingredient;
  [%guard let Some ing = ing][@else ""];
  let amount = ring##amount |> Js.Null.to_opt
  |> optMap (fun amount => Utils.fractionify amount ^ " ") |> optOr "";
  let plural = ring##amount |> Js.Null.to_opt |> optMap (fun a => a != 1.) |> optOr false;
  let unit = ring##unit |> Js.Null.to_opt
  |> optMap (fun unit => Utils.speakableUnit unit plural ^ " ") |> optOr "";
  let comments = ring##comments |> Js.Null.to_opt
  |> optMap (fun comments => " " ^ comments) |> optOr "";
  amount ^ unit ^ ing##name ^ comments ^ "."
};

let ingredientsText map ingredients => {
  Js.Array.joinWith "\n" (Array.map
  (ingredientText map)
  ingredients)
};

let handleCommand command map ingredients instructions => {
  
};
