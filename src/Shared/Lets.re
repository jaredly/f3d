
module Opt = {
  let let_ = (v, fin) => switch v {
    | None => None
    | Some(x) => fin(x)
  };
  let force = x => switch x {
    | None => failwith("Unwrapped optional")
    | Some(x) => x
  };
  module Wrap = {
    let let_ = (v, fin) => switch v {
      | None => None
      | Some(x) => Some(fin(x))
    }
  }
  module Force = {
    let let_ = (v, fin) => switch v {
      | None => failwith("Unwrapped empty optional")
      | Some(x) => fin(x)
    }
  }
  module Consume = {
    let let_ = (v, fin) => switch v {
      | None => ()
      | Some(x) => fin(x)
    }
  }
  module React = {
    let let_ = (v, fin) => switch v {
      | None => React.null
      | Some(x) => fin(x)
    }
  }
}

module Try = {
  let let_ = (v, fin) => switch v {
    | Belt.Result.Ok(v) => fin(v)
    | Error(e) => Belt.Result.Error(e)
  }
  let try_ = (v, fin) => switch v {
    | Belt.Result.Ok(v) => Belt.Result.Ok(v)
    | Error(e) => fin(e)
  };
  module Wrap = {
    let let_ = (v, fin) => switch v {
      | Belt.Result.Ok(v) => Belt.Result.Ok(fin(v))
      | Error(e) => Belt.Result.Error(e)
    }
  };
  module Force = {
    let let_ = (v, fin) => switch v {
      | Belt.Result.Ok(v) => fin(v)
      | Error(e) => failwith("Force unwrapping an Error'd Result")
    }
  };
  module Consume = {
    let let_ = (v, fin) => switch v {
      | Belt.Result.Ok(v) => fin(v)
      | Error(e) => ()
    }
  };
}

module Async = {
  let let_ = (v, fin) => Js.Promise.then_(fin, v);
  let try_ = (v, fin) => Js.Promise.catch(fin, v);
  module Wrap = {
    let let_ = (v, fin) => Js.Promise.then_(v => Js.Promise.resolve(fin(v)), v);
    let try_ = (v, fin) => Js.Promise.catch(e => Js.Promise.resolve(fin(e)), v);
  };
  module Consume = {
    let let_ = (v, fin) => Js.Promise.then_(v => {let () = fin(v); Js.Promise.resolve(())}, v);
    let try_ = (v, fin) => Js.Promise.catch(e => {let () = fin(e); Js.Promise.resolve(())}, v);
  };
}