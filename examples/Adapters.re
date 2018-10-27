module NodeHttp: Adapter.t = {
  open NodeHttp;
  type payload = (Request.t, Response.t);
  type server = Server.t;

  let fetchBody = ((req, _res), cb) => {
    let data = ref("");
    let onData = chunk => data := data^ ++ Node.Buffer.toString(chunk);
    let onEnd = () => cb(data^);

    req->Request.on(`data(onData))->Request.on(`end_(onEnd))->ignore;
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