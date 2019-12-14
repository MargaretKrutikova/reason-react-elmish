open Api;

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
