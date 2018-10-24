open Conn;

type t('state) = (Conn.t('state) => unit, Conn.t('state)) => unit;

let bodyFetcher = (next, conn) => {
  let req = conn._req;
  let data = ref("");
  let onData = chunk => data := data^ ++ Node.Buffer.toString(chunk);
  let onEnd = () => {
    let conn = {
      ...conn,
      request: {
        ...conn.request,
        body: Fetched(data^),
      },
    };
    next(conn);
  };
  req
  |> Http.Request.on(`data(onData))
  |> Http.Request.on(`end_(onEnd))
  |> ignore;
};