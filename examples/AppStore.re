module Config = {
  type message =
    | Click(unit)
    | Success(unit)
    | Error(unit)
    | SetName(string);

  type model = {
    count: int,
    name: string,
    result: option(bool),
  };

  let makeApiCall = ((), dispatch: message => unit) => {
    let test = Js.Math.random_int(0, 2);
    if (test == 1) {
      dispatch(Success());
    } else {
      dispatch(Error());
    };
    None;
  };

  let update = (model, message) => {
    switch (message) {
    | Click () => ({...model, count: model.count + 1}, Some(makeApiCall()))
    | Success () => ({...model, result: Some(true)}, None)
    | Error () => ({...model, result: Some(false)}, None)
    | SetName(name) => ({...model, name}, None)
    };
  };

  let initialState = ({count: 0, result: None, name: ""}, None);
};

include Elmish.Make({
  type model = Config.model;
  type message = Config.message;

  let update = Config.update;
  let storeEnhancer = None;
  let initialState = Config.initialState;
});
