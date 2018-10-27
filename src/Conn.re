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
  queryParams: fetchable(Belt.Map.String.t(string)),
  queryString: string,
  host: string,
  protocol: [ | `https | `http],
  remoteIp: (int, int, int, int),
  reqHeaders: Belt.Map.String.t(string),
  reqBody: fetchable(string),
  /* Response fields */
  statusCode: int,
  respBody: string,
  respHeaders: Belt.Map.String.t(string),
  charEncoding: CharEncoding.t,
  sent: bool,
};

let setStatus = (conn, status) => {
  ...conn,
  statusCode: Status.codeOfStatus(status),
};

let setRespHeader = (conn, field, value) => {
  ...conn,
  respHeaders: Belt.Map.String.set(conn.respHeaders, field, value),
};

let setRespBody = (conn, body) => {...conn, respBody: body};

let setContentType = (conn, contentType) =>
  conn->setRespHeader("Content-Type", contentType);

type respType = [ | `Text(string) | `Html(string) | `Json(string)];

let registerBeforeSend = (conn, f) => {
  ...conn,
  beforeSend: c => f(conn.beforeSend(c)),
};