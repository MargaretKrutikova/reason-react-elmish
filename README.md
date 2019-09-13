# reason-react-elmish

The library implements the "model-update" part of the "model-view-update" elm architecture, leveraging React as the "view" part and implementing the missing part of state and effect management.

🚧 Not production ready (yet), but trying out is highly encouraged 🚧

## Installation

Can be installed via:

```sh
npm install --save "https://github.com/MargaretKrutikova/reason-react-elmish"
```

and added to bsconfig.json:

```sh
"bs-dependencies": [
  "reason-react-elmish"
]
```

Peer dependencies are `react`, `reason-react` and `bs-platform`.

## Example usage

First, define your types for message and model, implement the update function and effectful functions (if any).
Setup your elmish store with all of the above and your initial state with initial effect (if any).

```reason
// AppStore.re
module Config = {
  type message =
    | Click
    | Success
    | Error;

  type model = {
    count: int,
    result: option(bool),
  };

  let makeApiCall = dispatch => {
    let test = Js.Math.random_int(0, 2);
    if (test == 1) {
      dispatch(Success);
    } else {
      dispatch(Error);
    };
    None;
  };

  let update = (model, message) => {
    switch (message) {
    | Click => ({...model, count: model.count + 1}, Some(makeApiCall))
    | Success => ({...model, result: Some(true)}, None)
    | Error => ({...model, result: Some(false)}, None)
    };
  };

  let initialState = ({count: 0, result: None}, None);
};

include Elmish.Make({
  type model = Config.model;
  type message = Config.message;

  let update = Config.update;
  let storeEnhancer = None;
  let initialState = Config.initialState;
});
```

See example file [`./examples/AppStore.re`](./examples/AppStore.re).

Then hook in your store into the react component tree somewhere at the root:

```reason
// Index.re
ReactDOMRe.renderToElementWithId(
  <AppStore.ElmishProvider> <Root /> </AppStore.ElmishProvider>,
  "root",
);
```

and use the hooks to get access to the model and dispatch in your components:

```reason
let selector = (model: AppStore.model) => model.result;

[@react.component]
let make = () => {
  let dispatch = AppStore.useDispatch();
  let result = AppStore.useSelector(selector);

  <div>
    <button onClick={_event => dispatch(Click)}>
      {ReasonReact.string("Click")}
    </button>
  </div>;
};
```

## Run examples

```sh
npm install
npm start
# in another tab
npm run watch:bs
```
