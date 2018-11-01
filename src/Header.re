open Belt;

type t = list((string, string));

let empty = [];

let get = (headers, k) => List.getAssoc(headers, k, (==));

let getAll = (headers, key) => List.keep(headers, ((k, _v)) => k == key);

let set = (headers, k, v) => List.setAssoc(headers, k, v, (==));

let setIfNotExist = (headers, key, value) =>
  switch (headers->get(key)) {
  | Some(_) => headers
  | None => headers->set(key, value)
  };

let append = (headers, k, v) => List.concat(headers, [(k, v)]);

let forEach = List.forEach;

let fromFlatArray = (array: array(string)) =>
  Belt.Array.reduceWithIndex(array, [], (headers, x, i) =>
    switch (i mod 2, array[i + 1]) {
    | (0, Some(value)) => append(headers, String.lowercase(x), value)
    | _ => headers
    }
  );