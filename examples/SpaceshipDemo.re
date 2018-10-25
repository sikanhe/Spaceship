type state = {userId: option(string)};

let listener = (conn: Conn.t(state)) =>
  switch (conn) {
  | {request: {method: `POST}, state: {userId: None}} =>
    conn->Conn.setStatus(Forbidden)->Conn.sendResp(`Text("Not logged in"))
  | {request: {method: `POST, path: ["data"], body: Fetched(data)}} =>
    conn
    ->Conn.setStatus(Ok)
    ->Conn.sendResp(`Html("Received data: " ++ data ++ ""))
  | _ =>
    conn->Conn.setStatus(NotFound)->Conn.sendResp(`Text("Page not found"))
  };

let requestLogger: Middleware.t('a) =
  (next, conn) => {
    Js.log(conn.request.body);
    Js.log(
      "[" ++ Http.methodStr(conn.request.method) ++ "] " ++ conn.request.url,
    );
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
  ->App.start(listener, ~port=4000, ~defaultState={userId: None});