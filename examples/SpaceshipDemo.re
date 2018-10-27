type context = {userId: option(string)};

module App = Spaceship.Make(Adapter.NodeHttp);

let handler = (conn: Conn.t('a), ctx) =>
  switch (conn, ctx) {
  | ({method: `POST, path: ["data"], reqBody: Fetched(data)}, _ctx) =>
    conn
    ->Conn.setStatus(Ok)
    ->App.sendResp(`Html("Received data: " ++ data ++ ""))

  | ({method: `PUT}, {userId: None}) =>
    conn->Conn.setStatus(Forbidden)->App.sendResp(`Text("Not logged in"))
  | _ =>
    conn->Conn.setStatus(NotFound)->App.sendResp(`Text("Page not found"))
  };

let requestLogger = (next, conn, ctx) => {
  Js.log(conn.Conn.reqBody);
  Js.log("[" ++ Method.toStr(conn.method) ++ "] " ++ conn.url);
  next(conn, ctx);
};

let fetchBody = (next, conn, ctx) =>
  App.fetchBody(conn, body => next({...conn, reqBody: Fetched(body)}, ctx));

let app =
  App.make()
  ->App.middleware(requestLogger)
  ->App.middleware(fetchBody)
  ->App.start(handler, ~port=4000, ~createContext=() => {userId: None});