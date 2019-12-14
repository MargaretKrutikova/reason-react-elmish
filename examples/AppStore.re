include Elmish.Make({
  type model = AppModel.model;
  type message = AppModel.message;

  let update = AppModel.update;
  let storeEnhancer = None;
  let initialModel = AppModel.initModel;
});
