type effect = unit => option(unit => unit);

type t = {
  listeners: ref(list(unit => unit)),
  effects: ref(array(effect)),
};
let create = () => {listeners: ref([]), effects: ref([||])};

let unsubscribe = (manager, listener, ()) =>
  manager.listeners := Belt.List.keep(manager.listeners^, l => listener !== l);

let subscribe = (manager, listener) => {
  manager.listeners := [listener, ...manager.listeners^];
  unsubscribe(manager, listener);
};

let queueEffect = (manager, effect) => {
  manager.effects := Belt.Array.concat(manager.effects^, [|effect|]);
  Belt.List.forEach(manager.listeners^, listener => listener());
};

let runEffects = manager => {
  let cleanupFuncs = Belt.Array.keepMap(manager.effects^, func => func());
  manager.effects := [||];

  cleanupFuncs;
};
