
open Glamor;
let container = css [
  padding "0 16px",
  width "800px",
  maxWidth "100%",
  alignSelf "center",
  fontSize "20px",
  /* letterSpacing "1px", */
];
let loading = css [
  alignItems "center",
];
let title = css [
  fontSize "44px",
  fontFamily "Abril Fatface",
];
let header = css [
  backgroundColor "white",
  flexDirection "row",
  alignItems "center",
  position "sticky",
  top "0px",
  paddingBottom "8px",
  paddingTop "8px",
];
let button = css [
  fontSize "inherit",
  border "none",
  backgroundColor "transparent",
  padding "0",
  margin "0",
  fontWeight "inherit",

  color "#777",
  cursor "pointer",
  padding "8px 16px",
  Selector ":hover" [
    color "black",
  ]
];
let editButton = css [
  textDecoration "none",
  color "currentColor",
  padding "8px 16px",
  cursor "pointer",
  color "#777",
  Selector ":hover" [
    color "black",
  ]
];
let subHeader = css [
  alignItems "center",
  flexDirection "row",
  fontSize "36px",
  fontWeight "400",
  /* letterSpacing "1px", */
  fontVariant "small-caps",
];
