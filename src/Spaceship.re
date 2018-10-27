type middleware('ctx, 'payload) =
  ((Conn.t('payload), 'ctx) => unit, Conn.t('payload), 'ctx) => unit;


module Make = (Adapter: Adapter.t) => {
  type handler('ctx) = (Conn.t(Adapter.payload), 'ctx) => unit;

  let fetchBody = (conn, cb) => Adapter.fetchBody(conn.Conn.payload, cb);

  let sendResp = (conn, respType) => {
    let conn =
      switch (respType) {
      | `Text(string) =>
        conn->Conn.setContentType("text/plain")->Conn.setRespBody(string)
      | `Html(string) =>
        conn->Conn.setContentType("text/html")->Conn.setRespBody(string)
      | `Json(string) =>
        conn
        ->Conn.setContentType("application/json")
        ->Conn.setRespBody(string)
      };

    conn.beforeSend(conn)->ignore;

    Adapter.sendResp(
      ~payload=conn.payload,
      ~statusCode=conn.statusCode,
      ~body=conn.respBody,
      ~charEncoding=conn.charEncoding,
      ~headers=conn.respHeaders,
    );
  };

  let start = (listener, ~port, ~createContext) =>
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
            beforeSend: conn => conn,
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
        listener(conn, createContext());
      },
      ~port,
    );
};