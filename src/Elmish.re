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
  let initialState: (model, option(effect(message)));
  let storeEnhancer:
    option(storeEnhancer(message, model) => storeEnhancer(message, model));
};

module Make = (Config: Config) => {
  open Reductive;
  include Config;

  let effectManager = EffectManager.create();

  let (initModel, initEffect) = initialState;

  let storeCreator =
    switch (storeEnhancer) {
    | None => Store.create
    | Some(enhancer) => enhancer @@ Store.create
    };

  let store =
    storeCreator(
      ~reducer=(state, _) => state,
      ~preloadedState=initModel,
      (),
    );

  let customReducer = (model, message) => {
    let (newModel, effect) = update(model, message);
    switch (effect) {
    | Some(effectToRun) =>
      EffectManager.queueEffect(effectManager, () =>
        effectToRun(Store.dispatch(store))
      )
    | None => ()
    };

    newModel;
  };

  Store.replaceReducer(store, customReducer);

  module StoreContext = {
    type t = Store.t(message, model);

    include Context.Make({
      type context = t;
      let defaultValue = store;
    });
  };

  let useRunEffects = () => {
    let (updater, forceUpdate) = React.useReducer((s, _) => s + 1, 0);

    React.useLayoutEffect0(() =>
      Some(EffectManager.subscribe(effectManager, forceUpdate))
    );

    React.useEffect1(
      () => {
        let cleanupFuncs = EffectManager.runEffects(effectManager);
        Belt.Array.length(cleanupFuncs) > 0
          ? Some(() => Belt.Array.forEach(cleanupFuncs, func => func()))
          : None;
      },
      [|updater|],
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
      <StoreContext.Provider value=store>
        <EffectRunner> children </EffectRunner>
      </StoreContext.Provider>;
    };
  };

  let useSelector = selector => {
    let storeFromContext = React.useContext(StoreContext.context);
    let (_, forceUpdate) = React.useReducer((s, _) => s + 1, 0);

    let latestSelectedModel = React.useRef(selector(Store.getState(store)));

    React.useLayoutEffect1(
      () => {
        let checkForUpdates = () => {
          let newSelectedState = selector(Store.getState(store));

          let hasStateChanged =
            newSelectedState !== React.Ref.current(latestSelectedModel);

          if (hasStateChanged) {
            React.Ref.setCurrent(latestSelectedModel, newSelectedState);
            forceUpdate();
          };
        };
        Some(Store.subscribe(storeFromContext, checkForUpdates));
      },
      [|storeFromContext|],
    );

    React.Ref.current(latestSelectedModel);
  };

  let useDispatch = () => {
    let storeFromContext = React.useContext(StoreContext.context);
    Store.dispatch(storeFromContext);
  };
};
