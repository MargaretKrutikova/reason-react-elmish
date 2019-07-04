# reason-react-elmish

The library attempts to implement the "model-update" part of the "model-view-update" elm architecture, leveraging React as the "view" part and implementing the missing part of state and effect management.

🚧 Very much under development and as WIP as it can get 🚧

## Installation

Can be installed via:

```sh
npm install "https://github.com/MargaretKrutikova/reason-react-elmish"
```

and added to bsconfig.json:

```sh
"bs-dependencies": [
  "reason-react-elmish"
]
```

Peer dependencies are `react`, `reason-react` and `bs-platform`.

## Example usage

```reason
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

## Run examples

```sh
npm install
npm start
# in another tab
npm run watch:bs
```
