module type t = {
  type payload;
  type server;

  let fetchBody: (payload, string => unit) => unit

  let sendResp:
    (
      ~payload: payload,
      ~statusCode: int,
      ~headers: Header.t,
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
        ~headers: Header.t
      ) =>
      unit,
      ~port: int
    ) =>
    server;
};