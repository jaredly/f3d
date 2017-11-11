open Utils;

module UserFetcher = FirebaseFetcher.Single({
  include Models.PublicUserData;
  let getId = (user) => user##id;
});

let loading = () => <div>(str("Loading"))</div>;
let failedLoading = (_err) => <div>(str("Unknown user"))</div>;

let make = (~fb, ~id, children) => {
  UserFetcher.make(
    ~fb,
    ~id,
    ~render=((~state) => switch (state) {
      | `Initial => loading()
      | `Loaded(userData) => <div>(str(userData##name))</div>
      | `Errored(err) => failedLoading(err)
    }),
    children
  )
};


