type user = {
  id: int,
  email: string,
  first_name: string,
  last_name: string,
  avatar: string,
};

let decodeUser = json =>
  Json.Decode.{
    id: json |> field("id", int),
    email: json |> field("email", string),
    first_name: json |> field("first_name", string),
    last_name: json |> field("last_name", string),
    avatar: json |> field("avatar", string),
  };

type userApiResponse = {
  page: int,
  per_page: int,
  total: int,
  total_pages: int,
  users: array(user),
};

let decodeUserApiResponse = json =>
  Json.Decode.{
    page: json |> field("page", int),
    per_page: json |> field("per_page", int),
    total: json |> field("total", int),
    total_pages: json |> field("total_pages", int),
    users: json |> field("data", array(decodeUser)),
  };

module Caller: {let fetchUsers: unit => Js.Promise.t(userApiResponse);} = {
  let baseUrl = "https://reqres.in/api";
  let fetchUsers = () =>
    Js.Promise.(
      Fetch.fetch(baseUrl ++ "/users?per_page=12")
      |> then_(Fetch.Response.json)
      |> then_(json => json |> decodeUserApiResponse |> resolve)
    );
};
