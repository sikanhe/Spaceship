type t('state) = {
  middlewares: list(Middleware.t('state)),
  httpServer: option(Http.Server.t),
};

let make = () => {middlewares: [], httpServer: None};

let middleware = (app, middleware) => {
  ...app,
  middlewares: [middleware, ...app.middlewares],
};

let start = (app, listener, ~port, ~defaultState) => {
  let rec apply = (middlewares, listener) =>
    switch (middlewares) {
    | [] => listener
    | [middleware, ...rest] => apply(rest, middleware(listener))
    };

  let listener' = apply(app.middlewares, listener);

  let server =
    Http.Server.create((req, res) => {
      let conn = Conn.make(req, res, defaultState);
      listener'(conn);
    })
    |> Http.Server.listen(~port);

  {...app, httpServer: Some(server)};
};