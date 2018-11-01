type context = {userId: option(string)};

module Server = Spaceship.Make(Adapters.NodeHttp);

let handler: Server.handler(context) =
  (conn, ctx) =>
    switch (conn, ctx) {
    | ({method: `GET, path: []}, _ctx) =>
      Server.sendResp(conn, `Text("Hello World"))
    | ({method: `POST, path: ["data"], reqBody: Fetched(data)}, _ctx) =>
      conn
      ->Conn.setStatus(Ok)
      ->Server.sendResp(`Html("Received data: " ++ data ++ ""))

    | ({method: `PUT}, {userId: None}) =>
      conn
      ->Conn.setStatus(Forbidden)
      ->Server.sendResp(`Text("Not logged in"))
    | _ =>
      conn
      ->Conn.setStatus(NotFound)
      ->Server.sendResp(`Text("Page not found"))
    };

[@bs.val] [@bs.scope "process"] external hrtime: unit => (int, int) = "";

let getTimeMicro = () => {
  let (s, ns) = hrtime();
  s * int_of_float(1e6) + ns / 1000;
};

let logHeaders = (next, conn, ctx) => {
  Js.log(conn.Conn.reqHeaders |> Belt.List.toArray);
  next(conn, ctx);
};

let handler' =
  Server.requestLogger(~getTimeMicro, ~logger=Js.log) @@
  Server.bodyFetcher @@
  logHeaders @@
  handler;

Server.start(handler', ~port=4000, ~createContext=() => {userId: None});

Header.empty
->Header.set("CONTENT-TYPE", "world")
->Header.set("CONTENT-TYPE", "hello")
->Header.append("CONTENT-TYPE", "DFDFD")
->Belt.List.toArray
->Js.log