type context = {userId: option(string)};

module Server = Spaceship.Make(Adapter.NodeHttp);

let handler: Server.handler(context) =
  (conn, ctx) =>
    switch (conn, ctx) {
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

let requestLogger = (next, conn, ctx) => {
  Js.log(conn.Conn.reqBody);
  Js.log("[" ++ Method.toStr(conn.method) ++ "] " ++ conn.url);
  next(conn, ctx);
};

let fetchBody = (next, conn, ctx) =>
  Server.fetchBody(conn, body =>
    next({...conn, reqBody: Fetched(body)}, ctx)
  );

Server.create()
->Server.middleware(requestLogger)
->Server.middleware(fetchBody)
->Server.start(handler, ~port=4000, ~createContext=() => {userId: None});