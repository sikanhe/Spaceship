module type t = {
  type payload;
  type server;

  let fetchBody: (payload, string => unit) => unit

  let sendResp:
    (
      ~payload: payload,
      ~statusCode: int,
      ~headers: Belt.Map.String.t(string),
      ~body: string,
      ~charEncoding: CharEncoding.t
    ) =>
    unit;

  let startServer:
    (
      (
        ~payload: payload,
        ~method: Method.t,
        ~url: string,
        ~path: list(string),
        ~port: int,
        ~query: string,
        ~host: string,
        ~protocol: [ | `https | `http],
        ~remoteIp: (int, int, int, int),
        ~headers: Belt.Map.String.t(string)
      ) =>
      unit,
      ~port: int
    ) =>
    server;
};

module NodeHttp: t = {
  open NodeHttp
  type payload = (Request.t, Response.t);
  type server = Server.t;

  let fetchBody = ((req, _res), cb) => {
    let data = ref("");
    let onData = chunk => data := data^ ++ Node.Buffer.toString(chunk);
    let onEnd = () => cb(data^);

    req
    ->Request.on(`data(onData))
    ->Request.on(`end_(onEnd))
    ->ignore;
  };

  let sendResp = (~payload, ~statusCode, ~headers, ~body, ~charEncoding) => {
    let (_req, res) = payload;
    Belt.Map.String.forEach(headers, (field, value) =>
      Response.setHeader(res, field, value)->ignore
    );

    res
    ->Response.setStatusCode(statusCode)
    ->Response.write(body, ~encoding=charEncoding)
    ->Response.end_;
    ();
  };

  let startServer = (cb, ~port) =>
    Server.create((req, res) =>
      cb(
        ~payload=(req, res),
        ~method=Request.method(req),
        ~url=Request.urlGet(req),
        ~path=Request.path(req),
        ~port,
        ~query="",
        ~host="",
        ~protocol=`http,
        ~remoteIp=(0, 0, 0, 0),
        ~headers=Belt.Map.String.empty,
      )
    )
    ->Server.listen(~port);
};