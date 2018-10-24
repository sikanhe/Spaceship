module StringMap = Belt.Map.String;

module Request = {
  type fetchable('a) =
    | Unfetched
    | Fetched('a);

  type t= {
    method: Http.method,
    url: string,
    path: list(string),
    port: int,
    query: StringMap.t(string),
    queryString: string,
    host: string,
    hostname: string,
    protocol: [ | `https | `http],
    remote_ip: (int, int, int, int),
    body: fetchable(string),
  };

  let make = (req) => {
    method: Http.Request.method(req),
    url: Http.Request.urlGet(req),
    path: Http.Request.path(req),
    /* TODO: */
    port: 80,
    query: StringMap.empty,
    queryString: "foobar",
    host: "",
    hostname: "",
    protocol: `http,
    remote_ip: (0, 0, 0, 0),
    body: Unfetched,
  };

  let urlToPath = url =>
    switch (url) {
    | ""
    | "/" => []
    | path =>
      /* remove the preceeding /, which every pathname seems to have */
      let raw = Js.String.sliceToEnd(~from=1, path);
      /* remove the trailing /, which some pathnames might have. Ugh */
      let raw =
        switch (Js.String.get(raw, Js.String.length(raw) - 1)) {
        | "/" => Js.String.slice(~from=0, ~to_=-1, raw)
        | _ => raw
        };
      raw |> Js.String.split("/") |> Belt.List.fromArray;
    };
};

module Response = {
  type t = {
    statusCode: int,
    body: string,
    headers: StringMap.t(string),
    charEncoding: Http.charEncoding,
    sent: bool,
  };

  let make = res => {
    statusCode: res->Http.Response.statusCodeGet,
    body: "",
    headers: StringMap.empty,
    charEncoding: `utf8,
    sent: false,
  };

  let setStatus = (res, status) => {
    ...res,
    statusCode: Status.codeOfStatus(status),
  };

  let setHeader = (res, field, value) => {
    ...res,
    headers: StringMap.set(res.headers, field, value),
  };

  let setBody = (res, body) => {...res, body};

  let setContentType = (res, contentType) =>
    res->setHeader("Content-Type", contentType);
};

type t('state) = {
  _req: Http.Request.t,
  _res: Http.Response.t,
  request: Request.t,
  response: Response.t,
  state: 'state
};

let methodStr = conn => Http.Request.methodGet(conn._req);

let make = (req, res, state) => {
  _req: req,
  _res: res,
  request: req->Request.make,
  response: res->Response.make,
  state
};

let setStatus = (conn, status) => {
  ...conn,
  response: {
    ...conn.response,
    statusCode: Status.codeOfStatus(status),
  },
};

let setHeader = (conn, field, value) => {
  ...conn,
  response: {
    ...conn.response,
    headers: StringMap.set(conn.response.headers, field, value),
  },
};

let setBody = (conn, body) => {
  ...conn,
  response: {
    ...conn.response,
    body,
  },
};

type respType = [ | `Text(string) | `Html(string) | `Json(string)];

let sendResp = (conn, respType) => {
  let conn =
    switch (respType) {
    | `Text(string) =>
      conn->setHeader("Content-Type", "text/plain")->setBody(string)
    | `Html(string) =>
      conn->setHeader("Content-Type", "text/html")->setBody(string)
    | `Json(string) =>
      conn->setHeader("Content-Type", "application/json")->setBody(string)
    };

  StringMap.forEach(conn.response.headers, (field, value) =>
    Http.Response.setHeader(conn._res, field, value) |> ignore
  );

  conn._res
  ->Http.Response.setStatusCode(conn.response.statusCode)
  ->Http.Response.write(conn.response.body)
  ->Http.Response.end_;
};