[@bs.deriving {jsConverter: newType}]
type charEncoding = [
  | `ascii
  | `base64
  | `binary
  | `hex
  | `latin1
  | `ucs2
  | `utf16le
  | `utf8
];

[@bs.deriving {jsConverter: newType}]
type method = [
  | `GET
  | `POST
  | `PUT
  | `UPDATE
  | `DELETE
  | `HEAD
  | `OPTION
  | `CONNECT
  | `TRACE
  | `PATCH
];

let methodStr =
  fun
  | `GET => "GET"
  | `POST => "POST"
  | `PUT => "PUT"
  | `UPDATE => "UPDATE"
  | `DELETE => "DELETE"
  | `HEAD => "HEAD"
  | `OPTION => "OPTION"
  | `CONNECT => "CONNECT"
  | `TRACE => "TRACE"
  | `PATCH => "PATCH";

module Request = {
  [@bs.deriving abstract]
  type t = {
    method: abs_method,
    url: Js.String.t,
    port: int,
  };

  let method = req => req->methodGet->methodFromJs;

  let path = request =>
    switch (request->urlGet) {
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

  [@bs.send.pipe: t]
  external on:
    (
    [@bs.string]
    [
      | `data(Node.Buffer.t => unit)
      | [@bs.as "end"] `end_(unit => unit)
      | `error(unit => unit)
    ]
    ) =>
    t =
    "";
};

module Response = {
  open Status;
  [@bs.deriving abstract]
  type t = {mutable statusCode: int};

  [@bs.send] external write: (t, string, abs_charEncoding) => unit = "";
  [@bs.send] external setHeader: (t, string, string) => unit = "";
  [@bs.send] external getHeader: (t, string) => string = "";
  [@bs.send] external end_: t => unit = "end";

  let write = (response: t, ~encoding=`utf8, chunk: string) => {
    write(response, chunk, charEncodingToJs(encoding));
    response;
  };

  let setStatusCode = (res, code) => {
    res->statusCodeSet(code);
    res;
  };

  let setStatus = (res, status) => {
    res->statusCodeSet(codeOfStatus(status));
    res;
  };

  let setHeader = (res, field, value) => {
    res->setHeader(field, value);
    res;
  };

  let appendToHeader = (res, field, value) => {
    let headerValue = res->getHeader(field);
    res->setHeader(headerValue ++ value);
  };
};

module Server = {
  type t;
  [@bs.module "http"]
  external create: ((Request.t, Response.t) => 'a) => t = "createServer";
  [@bs.send.pipe: t] external listen: (~port: int) => t = "";

  [@bs.send.pipe: t]
  external on: ([@bs.string] [ | `error(unit => unit)]) => t = "";

  let listen = (server, ~port) => {
    Js.log("Server started, listening on port " ++ string_of_int(port));
    listen(server, ~port);
  };
};