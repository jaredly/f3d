let useState = initial => {
  React.useReducer((_, action) => action, initial);
};

let useStateGetter = getter => {
  let (state, awesome) = React.useState(getter());
  React.useRef(None);
};

// let useHash = () => {
//   let (hash, setHash) = useState(Web.Location.hash());
//   React.useEffect0(() => Some(Web.Location.addHashListener(setHash)));
//   hash;
// };

// let hashIt: string => string = [%bs.raw
//   {|
// function(input) {
//     var hash = 0;
//     if (!input || input.length == 0) {
//         return hash;
//     }
//     for (var i = 0; i < input.length; i++) {
//         var char = input.charCodeAt(i);
//         hash = ((hash<<5)-hash)+char;
//         hash = hash & hash; // Convert to 32bit integer
//     }
//     return hash;
// }
// |}
// ];

// let anyHash = data => {
//   hashIt(Js.Json.stringify(Obj.magic(data)));
// };

// (unit, state) => (state, (state, state) => unit)
// (unit, state) => (state, (state) => (state => unit))

let useLastValue = (value) => {
  let stored = React.useRef(value);
  let lastValue = stored->React.Ref.current;
  stored->React.Ref.setCurrent(value);
  lastValue
}

let useOnChange = (value, onChange) => {
  let lastValue = React.useRef(value);
  // if (log) {
  //   Js.log3("In use (new vs current)", anyHash(value), anyHash(lastValue->React.Ref.current))
  // }
  React.useEffect2(
    () => {
      // if (log) {
      //   Js.log3("In effect (new vs current)", anyHash(value), anyHash(lastValue->React.Ref.current))
      // };
      if (lastValue->React.Ref.current != value) {
        // if (log) {
        //     Js.log3("In effect different!", anyHash(value), anyHash(lastValue->React.Ref.current))
        // }
        lastValue->React.Ref.setCurrent(value);
        onChange(value);
      };
      None;
    },
    (value, lastValue->React.Ref.current),
  );
  lastValue;
};

let useUpdatingState = initial => {
  let (current, setState) = useState(initial);
  let lastValue = useLastValue(initial);
  if (lastValue != initial) {
    setState(initial);
  };
  // React.useEffect1(
  //   () => {
  //     if (initial !== current) {
  //       setState(initial);
  //     };
  //     None;
  //   },
  //   [|initial|],
  // );
  (current, setState);
};

let useLoading = getter => {
  let (data, setData) = useState(None);
  React.useEffect1(
    () => {
      if (data != None) {
        setData(None);
      };
      getter()
      |> Js.Promise.then_(result => {
           setData(Some(result));
           Js.Promise.resolve();
         })
      |> ignore;
      None;
    },
    [|getter|],
  );
  data;
};

let lazyRef = fn => {
  let r = React.useRef(None);
  switch (r->React.Ref.current) {
  | Some(v) => v
  | None =>
    let v = fn();
    React.Ref.setCurrent(r, Some(v));
    v;
  };
};

// let usePortal = () => {
//   let rootElement = lazyRef(() => Web.document->Web.createElement("div"));

//   React.useEffect0(() => {
//     Web.documentEl->Web.body->Web.appendChild(rootElement);
//     Some(() => Web.documentEl->Web.body->Web.removeChild(rootElement));
//   });

//   rootElement;
// };