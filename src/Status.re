[@bs.deriving jsConverter]
type t =
  | Continue
  | SwitchingProtocols
  | Processing
  | EarlyHints
  | Ok
  | Created
  | Accepted
  | NonAuthoritativeInformation
  | NoContent
  | ResetContent
  | ParticalContent
  | MultiStatus
  | AlreadyReported
  | IMUsed
  | MultipleChoices
  | MovedPermanently
  | Found
  | SeeOther
  | NotModified
  | UseProxy
  | SwitchProxy
  | TemporaryRedirect
  | PermanentRedirect
  | BadRequest
  | Unauthorized
  | PaymentRequired
  | Forbidden
  | NotFound
  | MethodNotAllowed
  | NotAcceptable
  | ProxyAuthenticationRequired
  | RequestTimeout
  | Conflict
  | Gone
  | LengthRequired
  | PreconditionFailed
  | PayloadTooLarge
  | URITooLong
  | UnsupportedMediaType
  | RangeNotSatisfiable
  | ExpectationFailed
  | ImATeapot
  | MisdirectedRequest
  | UnprocessableEntity
  | Locked
  | FailedDependency
  | UpgradeRequired
  | PreconditionRequired
  | TooManyRequests
  | RequestHeaderFieldsTooLarge
  | UnavailableForLegalReasons
  | InternalServerError
  | NotImplemented
  | BadGateway
  | ServiceUnavailable
  | GatewayTimedout
  | HTTPVersionNotSupported
  | VariantAlsoNegotiates
  | InsufficientStorage
  | LoopDetected
  | NotExtended
  | NetWorkdAuthenticationRequired


let codeOfStatus =
  fun
  | Continue => 100
  | SwitchingProtocols => 101
  | Processing => 102
  | EarlyHints => 103
  | Ok => 200
  | Created => 201
  | Accepted => 204
  | NonAuthoritativeInformation => 203
  | NoContent => 204
  | ResetContent => 205
  | ParticalContent => 206
  | MultiStatus => 207
  | AlreadyReported => 208
  | IMUsed => 226
  | MultipleChoices => 300
  | MovedPermanently => 301
  | Found => 302
  | SeeOther => 303
  | NotModified => 304
  | UseProxy => 305
  | SwitchProxy => 306
  | TemporaryRedirect => 307
  | PermanentRedirect => 308
  | BadRequest => 400
  | Unauthorized => 401
  | PaymentRequired => 402
  | Forbidden => 403
  | NotFound => 404
  | MethodNotAllowed => 405
  | NotAcceptable => 406
  | ProxyAuthenticationRequired => 407
  | RequestTimeout => 408
  | Conflict => 409
  | Gone => 410
  | LengthRequired => 411
  | PreconditionFailed => 412
  | PayloadTooLarge => 413
  | URITooLong => 414
  | UnsupportedMediaType => 415
  | RangeNotSatisfiable => 416
  | ExpectationFailed => 417
  | ImATeapot => 418
  | MisdirectedRequest => 421
  | UnprocessableEntity => 422
  | Locked => 423
  | FailedDependency => 424
  | UpgradeRequired => 426
  | PreconditionRequired => 428
  | TooManyRequests => 429
  | RequestHeaderFieldsTooLarge => 431
  | UnavailableForLegalReasons => 451
  | InternalServerError => 500
  | NotImplemented => 501
  | BadGateway => 502
  | ServiceUnavailable => 503
  | GatewayTimedout => 504
  | HTTPVersionNotSupported => 505
  | VariantAlsoNegotiates => 506
  | InsufficientStorage => 507
  | LoopDetected => 508
  | NotExtended => 510
  | NetWorkdAuthenticationRequired => 511;