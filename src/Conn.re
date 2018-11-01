type fetchable('a) =
  | Unfetched
  | Fetched('a);

type t('payload) = {
  payload: 'payload,
  beforeSend: t('payload) => t('payload),
  /* Request fields */
  method: Method.t,
  url: string,
  path: list(string),
  port: int,
  queryString: string,
  host: string,
  protocol: [ | `https | `http],
  remoteIp: (int, int, int, int),
  reqHeaders: Header.t,
  reqBody: fetchable(string),
  /* Response fields */
  statusCode: int,
  respBody: string,
  respHeaders: Header.t,
  charEncoding: CharEncoding.t,
  sent: bool,
};

let getReqHeader = (conn, field) => Header.get(conn.reqHeaders, field);

let setStatus = (conn, status) => {
  ...conn,
  statusCode: Status.codeOfStatus(status),
};

let getRespHeader = (conn, field) => Header.get(conn.respHeaders, field);

let setRespHeader = (conn, field, value) => {
  ...conn,
  respHeaders: Header.set(conn.respHeaders, field, value),
};

let setRespBody = (conn, body) => {...conn, respBody: body};

let setContentType = (conn, contentType) =>
  conn->setRespHeader("Content-Type", contentType);

type respType = [ | `Text(string) | `Html(string) | `Json(string)];

let registerBeforeSend = (conn, f) => {
  ...conn,
  beforeSend: c => f(conn.beforeSend(c)),
};