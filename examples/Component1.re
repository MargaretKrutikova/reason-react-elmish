let selector = (model: AppStore.model) => model.result;

[@react.component]
let make = () => {
  let dispatch = AppStore.useDispatch();
  let result = AppStore.useSelector(selector);

  <div>
    <button onClick={_event => dispatch(Click)}>
      {ReasonReact.string("Click")}
    </button>
    <div>
      {switch (result) {
       | Some(true) => ReasonReact.string("Success")
       | Some(false) => ReasonReact.string("Failure")
       | None => ReasonReact.string("Not run")
       }}
    </div>
  </div>;
};
