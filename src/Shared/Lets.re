
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
