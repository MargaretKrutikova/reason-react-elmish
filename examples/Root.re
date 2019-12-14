let selector = (model: AppModel.model) => model.data;

[@react.component]
let make = () => {
  let dispatch = AppStore.useDispatch();
  let data = AppStore.useSelector(selector);

  <div>
    <button onClick={_event => dispatch(FetchUsersRequest)}>
      {React.string("Fetch users")}
    </button>
    <div>
      {switch (data) {
       | Loading => React.string("Loading...")
       | Success(data) =>
         data.users
         ->Belt.Array.map(user =>
             <div key={user.id |> string_of_int}>
               {user.first_name ++ " " ++ user.last_name |> React.string}
             </div>
           )
         |> React.array
       | Error => React.string("Error :(")
       | NotAsked => React.string("No users to show.")
       }}
    </div>
  </div>;
};
