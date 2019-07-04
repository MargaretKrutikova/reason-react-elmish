type effect = unit => option(unit => unit);

type t = {
  listeners: ref(list(unit => unit)),
  effects: ref(array(effect)),
};
let create = () => {listeners: ref([]), effects: ref([||])};

let unsubscribe = (manager, listener, ()) =>
  manager.listeners := List.filter(l => listener !== l, manager.listeners^);

let subscribe = (manager, listener) => {
  manager.listeners := [listener, ...manager.listeners^];
  unsubscribe(manager, listener);
};

let queueEffect = (manager, effect) => {
  manager.effects := Belt.Array.concat(manager.effects^, [|effect|]);
  List.iter(listener => listener(), manager.listeners^);
};

let runEffects = manager => {
  let cleanupFuncs = Belt.Array.keepMap(manager.effects^, func => func());
  manager.effects := [||];

  cleanupFuncs;
};
