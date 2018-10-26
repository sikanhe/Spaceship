type context = {userId: option(string)};

let handler = (conn: Conn.t(context)) =>
  switch (conn) {
  | {method: `POST, context: {userId: None}} =>
    conn->Conn.setStatus(Forbidden)->Conn.sendResp(`Text("Not logged in"))
  | {method: `POST, path: ["data"], reqBody: Fetched(data)} =>
    conn
    ->Conn.setStatus(Ok)
    ->Conn.sendResp(`Html("Received data: " ++ data ++ ""))
  | _ =>
    conn->Conn.setStatus(NotFound)->Conn.sendResp(`Text("Page not found"))
  };

let requestLogger: Middleware.t('a) =
  (next, conn) => {
    Js.log(conn.reqBody);
    Js.log("[" ++ Http.methodStr(conn.method) ++ "] " ++ conn.url);
    next(conn);
  };

let earlySend = (_next, conn) =>
  Conn.sendResp(conn, `Text("Early response"));

let send500 = conn =>
  conn
  ->Conn.setStatus(InternalServerError)
  ->Conn.sendResp(`Text("Internal Server Error"));

let app =
  App.make()
  ->App.middleware(requestLogger)
  ->App.middleware(earlySend)
  ->App.middleware(Middleware.bodyFetcher)
  ->App.start(handler, ~port=4000, ~createContext=() => {userId: None});