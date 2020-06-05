type dispatch('message) = 'message => unit;
type effect('message) = dispatch('message) => option(unit => unit);
type update('model, 'message) =
  ('model, 'message) => ('model, option(effect('message)));

type enhancer('action, 'state) =
  (Reductive.Store.t('action, 'state), 'action => unit, 'action) => unit;

type storeEnhancer('action, 'state) =
  (
    ~reducer: ('state, 'action) => 'state,
    ~preloadedState: 'state,
    ~enhancer: enhancer('action, 'state)=?,
    unit
  ) =>
  Reductive.Store.t('action, 'state);

module type Config = {
  type model;
  type message;

  let update: update(model, message);
  let initialModel: (model, option(effect(message)));
  let storeEnhancer:
    option(storeEnhancer(message, model) => storeEnhancer(message, model));
};

module Make = (Config: Config) => {
  open Reductive;
  include ReductiveContext.Make({
    type state = Config.model;
    type action = Config.message;
  });

  let effectManager = EffectManager.create();

  let (initModel, initEffect) = Config.initialModel;

  let storeCreator =
    switch (Config.storeEnhancer) {
    | None => Store.create
    | Some(enhancer) => enhancer @@ Store.create
    };

  let modelStore =
    storeCreator(
      ~reducer=(state, _) => state,
      ~preloadedState=initModel,
      (),
    );

  let customReducer = (model, message) => {
    let (newModel, effect) = Config.update(model, message);
    switch (effect) {
    | Some(effectToRun) =>
      EffectManager.queueEffect(effectManager, () =>
        effectToRun(Store.dispatch(modelStore))
      )
    | None => ()
    };

    newModel;
  };

  let dispatch = Store.dispatch(modelStore);

  Store.replaceReducer(modelStore, customReducer);

  switch (initEffect) {
  | Some(effect) => effect(Store.dispatch(modelStore))
  | None => None
  };

  let useRunEffects = () => {
    open Subscription;

    let source =
      React.useMemo0(() =>
        {
          subscribe: EffectManager.subscribe(effectManager),
          getCurrentValue: () => EffectManager.getEffects(effectManager),
        }
      );

    let effects = useSubscription(source);

    React.useEffect1(
      () => {
        let cleanupFuncs = EffectManager.runEffects(effectManager);
        Belt.Array.length(cleanupFuncs) > 0
          ? Some(() => Belt.Array.forEach(cleanupFuncs, func => func()))
          : None;
      },
      [|effects|],
    );
  };

  module EffectRunner = {
    [@react.component]
    let make = (~children) => {
      useRunEffects();
      children;
    };
  };

  module ElmishProvider = {
    [@react.component]
    let make = (~children) => {
      <Provider store=modelStore>
        <EffectRunner> children </EffectRunner>
      </Provider>;
    };
  };
};
