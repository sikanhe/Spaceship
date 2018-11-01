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

module Request = {
  [@bs.deriving abstract]
  type t = {
    httpVersion: string,
    httpVersionMajor: int,
    httpVersionMinor: int,
    method: abs_method,
    url: string,
    port: int,
    rawHeaders: array(string),
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

  [@bs.send]
  external on:
    (
      t,
      [@bs.string] [
        | `data(Node.Buffer.t => unit)
        | [@bs.as "end"] `end_(unit => unit)
        | `error(unit => unit)
      ]
    ) =>
    t =
    "";
};

module Response = {
  [@bs.deriving abstract]
  type t = {
    mutable statusCode: int,
    mutable statusMessage: string,
  };

  [@bs.send] external write: (t, string, abs_charEncoding) => unit = "";
  [@bs.send] external setHeader: (t, string, string) => unit = "";
  [@bs.send] external getHeader: (t, string) => string = "";
  [@bs.send] external end_: t => unit = "end";
};

module Server = {
  type t;
  [@bs.module "http"]
  external create: ((Request.t, Response.t) => 'a) => t = "createServer";

  [@bs.send] external listen: (t, ~port: int) => t = "";
  [@bs.send]
  external listenWithCallback: (t, ~port: int, unit => unit) => t = "listen";

  [@bs.send]
  external on: (t, [@bs.string] [ | `error(unit => unit)]) => t = "";
};