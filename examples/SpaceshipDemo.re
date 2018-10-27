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

let requestLogger = (next, conn, ctx) => {
  Js.log(Method.toStr(conn.Conn.method) ++ " " ++ conn.url);
  let (s1, ns1) = hrtime();

  conn
  ->Conn.registerBeforeSend(conn => {
      let (s2, ns2) = hrtime();
      /* let diffS = s2 - s1; */
      let (sd, nd): (int, int) = {
        let nsDiff = ns2 - ns1;
        let sDiff = s2 - s1;
        if (nsDiff < 0) {
          (sDiff - 1, nsDiff + int_of_float(1e9));
        } else {
          (sDiff, nsDiff);
        };
      };

      let diffNano = sd * int_of_float(1e9) + nd;
      let diffMicro = diffNano / 1000;
      let diffMilli = diffMicro / 1000;

      let diffStr =
        diffMilli > 1 ?
          string_of_int(diffMilli) ++ "ms" :
          string_of_int(diffMicro) ++ {j|µs|j};

      Js.log("Sent " ++ string_of_int(conn.statusCode) ++ " in " ++ diffStr);
      conn;
    })
  ->next(ctx);
};

let fetchBody = (next, conn, ctx) =>
  Server.fetchBody(conn, body =>
    next({...conn, reqBody: Fetched(body)}, ctx)
  );

let handler' = requestLogger @@ fetchBody @@ handler;

Server.start(handler', ~port=4000, ~createContext=() => {userId: None});