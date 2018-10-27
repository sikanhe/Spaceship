[@bs.deriving {jsConverter: newType}]
type t = [
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

let toStr =
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