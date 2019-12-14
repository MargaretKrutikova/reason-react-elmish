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

```reason
// AppModel.re
type message =
  | FetchUsersRequest
  | FetchUsersSuccess(userApiResponse)
  | FetchUsersError;

type data =
  | NotAsked
  | Loading
  | Error
  | Success(userApiResponse);

type model = {data};

let fetchUsers = dispatch => {
  Js.Promise.(
    Caller.fetchUsers()
    |> then_(response => dispatch(FetchUsersSuccess(response)) |> resolve)
    |> catch(_ => dispatch(FetchUsersError) |> resolve)
  )
  |> ignore;
  None;
};

let update = (_, message) => {
  switch (message) {
  | FetchUsersRequest => ({data: Loading}, Some(fetchUsers))
  | FetchUsersSuccess(data) => ({data: Success(data)}, None)
  | FetchUsersError => ({data: Error}, None)
  };
};

let initModel = ({data: NotAsked}, None);
```

Setup your elmish store with all of the above and your initial model with initial effect (if any).

```reason
// AppStore.re
include Elmish.Make({
  type model = AppModel.model;
  type message = AppModel.message;

  let update = AppModel.update;
  let storeEnhancer = None;
  let initialModel = AppModel.initModel;
});

```

See example file [`./examples/AppModel.re`](./examples/AppModel.re) and [`./examples/AppStore.re`](./examples/AppStore.re).

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
let selector = (model: AppModel.model) => model.data;

[@react.component]
let make = () => {
  let dispatch = AppStore.useDispatch();
  let result = AppStore.useSelector(selector);

  <div>
    <button onClick={_event => dispatch(FetchUsersRequest)}>
      {React.string("Fetch users")}
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
