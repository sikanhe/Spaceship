type middleware('ctx, 'payload) =
  ((Conn.t('payload), 'ctx) => unit, Conn.t('payload), 'ctx) => unit;

module Make = (Adapter: Adapter.t) => {
  type t('context) = {
    middlewares: list(middleware('context, Adapter.payload)),
    server: option(Adapter.server),
  };

  let make = () => {middlewares: [], server: None};

  let middleware = (app, middleware) => {
    ...app,
    middlewares: [middleware, ...app.middlewares],
  };

  let fetchBody = (conn, cb) => {
    Adapter.fetchBody(conn.Conn.payload, cb)
  }

  let sendResp = (conn, respType): unit => {
    let conn =
      switch (respType) {
      | `Text(string) =>
        conn
        ->Conn.setRespHeader("Content-Type", "text/plain")
        ->Conn.setRespBody(string)
      | `Html(string) =>
        conn
        ->Conn.setRespHeader("Content-Type", "text/html")
        ->Conn.setRespBody(string)
      | `Json(string) =>
        conn
        ->Conn.setRespHeader("Content-Type", "application/json")
        ->Conn.setRespBody(string)
      };

    Adapter.sendResp(
      ~payload=conn.payload,
      ~statusCode=conn.statusCode,
      ~body=conn.respBody,
      ~charEncoding=conn.charEncoding,
      ~headers=conn.respHeaders,
    );
  };

  let start = (app, listener, ~port, ~createContext) => {
    let rec apply = (middlewares, listener) =>
      switch (middlewares) {
      | [] => listener
      | [middleware, ...rest] => apply(rest, middleware(listener))
      };

    let listener' = apply(app.middlewares, listener);

    let server =
      Adapter.startServer(
        (
          ~payload,
          ~method,
          ~url,
          ~path,
          ~port,
          ~query,
          ~host,
          ~protocol,
          ~remoteIp,
          ~headers,
        ) => {
          let conn =
            Conn.{
              payload,
              method,
              url,
              path,
              port,
              queryParams: Unfetched,
              queryString: query,
              host,
              protocol,
              remoteIp,
              reqHeaders: headers,
              reqBody: Unfetched,
              statusCode: 200,
              respBody: "",
              respHeaders: Belt.Map.String.empty,
              charEncoding: `utf8,
              sent: false,
            };
          listener'(conn, createContext());
        },
        ~port,
      );
    {...app, server: Some(server)};
  };
};