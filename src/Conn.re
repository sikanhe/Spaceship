module StringMap = Belt.Map.String;

type fetchable('a) =
  | Unfetched
  | Fetched('a);

type t('ctx) = {
  _req: Http.Request.t,
  _res: Http.Response.t,
  context: 'ctx,
  /* Request fields */
  method: Http.method,
  url: string,
  path: list(string),
  port: int,
  queryParams: fetchable(StringMap.t(string)),
  queryString: string,
  host: string,
  hostname: string,
  protocol: [ | `https | `http],
  remoteIp: (int, int, int, int),
  reqHeaders: StringMap.t(string),
  reqBody: fetchable(string),
  /* Response fields */
  statusCode: int,
  respBody: string,
  respHeaders: StringMap.t(string),
  charEncoding: Http.charEncoding,
  sent: bool,
};

let make = (req, res, context) => {
  _req: req,
  _res: res,
  context,
  method: Http.Request.method(req),
  url: Http.Request.urlGet(req),
  path: Http.Request.path(req),
  port: 80,
  queryParams: Unfetched,
  queryString: "foobar",
  host: "",
  hostname: "",
  protocol: `http,
  remoteIp: (0, 0, 0, 0),
  reqHeaders: StringMap.empty,
  reqBody: Unfetched,
  /* Response */
  statusCode: Http.Response.statusCodeGet(res),
  respBody: "",
  respHeaders: StringMap.empty,
  charEncoding: `utf8,
  sent: false,
};

let methodStr = conn => Http.Request.methodGet(conn._req);

let setStatus = (res, status) => {
  ...res,
  statusCode: Status.codeOfStatus(status),
};

let setRespHeader = (res, field, value) => {
  ...res,
  respHeaders: StringMap.set(res.respHeaders, field, value),
};

let setRespBody = (res, body) => {...res, respBody: body};

let setContentType = (res, contentType) =>
  res->setRespHeader("Content-Type", contentType);

type respType = [ | `Text(string) | `Html(string) | `Json(string)];

let sendResp = (conn, respType) => {
  let conn =
    switch (respType) {
    | `Text(string) =>
      conn->setRespHeader("Content-Type", "text/plain")->setRespBody(string)
    | `Html(string) =>
      conn->setRespHeader("Content-Type", "text/html")->setRespBody(string)
    | `Json(string) =>
      conn
      ->setRespHeader("Content-Type", "application/json")
      ->setRespBody(string)
    };

  StringMap.forEach(conn.respHeaders, (field, value) =>
    Http.Response.setHeader(conn._res, field, value) -> ignore
  );

  conn._res
  ->Http.Response.setStatusCode(conn.statusCode)
  ->Http.Response.write(conn.respBody)
  ->Http.Response.end_;
};