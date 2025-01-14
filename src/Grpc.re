/* AUTO-GENERATED BY bs-grpc --- EDIT AT YOUR OWN PERIL */

exception BsGrpcDecoderError(string);
exception ImpossibleError(string);
let (<<) = (f, g, x) => f(g(x));
/** an opaque type for error objects from grpc-node when the client invokes
   an RPC */
type grpcClientRpcInvokeError;
/* simply wraps grpcClientRpcInvokeError when an exception is needed */
exception GrpcClientRpcInvokeError(grpcClientRpcInvokeError);
/* protobufjs prefers the `long` package on npm for 64-bit type representation */
module Long = {
  type t;
  [@bs.module "long"] external fromString: string => t;
  [@bs.module "long"] external fromFloat: float => t = "fromNumber";
  [@bs.module "long"] external fromInt: int => t = "fromNumber";
  [@bs.module "long"] external fromValue: 'a => t;
  [@bs.send] external toFloat: t => float = "toNumber";
  [@bs.send] external toString: t => string = "toString";
};

[@bs.deriving abstract]
type protobufjsToObjectOptions;
let myProtobufjsToObjectOptions: protobufjsToObjectOptions = [%bs.raw
  {|{
      oneofs: true,
      defaults: false,
      keepCase: true,
      enums: String,
  }|}
];

/* "channel credentials" seem to be needed for creating a client */
type channelCredentials;
type callCredentials;
type serverCredentials;

/* protobufjs uses ByteBuffer abstraction over Node Buffer */
type byteBuffer;
[@bs.send] external bufferOfByteBuffer: byteBuffer => Node.buffer = "finish";

/* flatMap an array of Futures */
let futureFlatMapArray =
    (f: 'a => Future.t('b), maxConcurrent, xs: array('a)) =>
  Future.(
    make(resolve => {
      let result: array(option('b)) =
        xs->Array.length->(Belt.Array.make(None));
      let xlen = Array.length(xs);
      let numJobs = ref(0);
      let cursor = ref(0);
      let rec pump = () => {
        if (cursor^ == xlen) {
          if (numJobs^ == 0) {
            (result |> Array.map(Belt.Option.getExn))->resolve;
          };
        } else {
          let i = cursor^;
          cursor := cursor^ + 1;
          numJobs := numJobs^ + 1;
          xs[i]
          ->f
          ->(
              get(x => {
                result[i] = Some(x);
                numJobs := numJobs^ - 1;
                pump();
              })
            );
        };
        ();
      };
      for (_ in 1 to min(maxConcurrent, xlen)) {
        pump();
      };
      ();
    })
  );

/** return Some(x) for the first x of an array for which predictate f
   returns true, starting from element at index n, or None if no such element is
   found */
let rec arrayFirst = (f, n, a) =>
  n < Array.length(a)
    ? f(a[n]) ? Some(a[n]) : arrayFirst(f, n + 1, a) : None;

/* utility eunction; TODO is there a stdlib answer? */
let optCall = (x, ~f) =>
  switch (x) {
  | None => ()
  | Some(x) => f(x)
  };

/** Convenience function for simplifying server replies */
let reply = (callback, x) =>
  callback(.
    Js.Nullable.null,
    x,
    Js.Nullable.undefined,
    Js.Nullable.undefined,
  );

module Validation = {
  exception MissingFieldError;
  exception StringTooLongError;
  exception StringTooShortError;
  exception ArrayTooShortError;
  exception FractionalNumberError;
  exception WrongStringError;
  exception UnimplementedError;
  exception WrongNumberError;
  exception NumberOutOfRangeError;
  exception NumberNotPositiveError;
  exception RegexMatchError;
  exception TransFieldError(string);
  exception EmptyOneof(string);

  /* invokes user-supplied function to perform multi-field
   * validation, if such a function is supplied
   * TODO move to Validation module */
  let transFieldError = errMsg => Belt.Result.Error(TransFieldError(errMsg));

  type field('a) =
    | Field(string, Future.t(Belt.Result.t(option('a), exn)));
  let fieldMap = (f, Field(fieldName, x)) => Field(fieldName, f(x));
  let fieldGet = (Field(_, x)) => x;
  let futureMap = (a, b) => Future.map(b, a);
  let futureFlatMap = (a, b) => Future.map(b, a);
  let resultMap = (a, b) => Belt.Result.map(b, a);
  let resultFlatMap = (a, b) => Belt.Result.flatMap(b, a);
  let optionMap = (a, b) => Belt.Option.map(b, a);
  let optionFlatMap = (a, b) => Belt.Option.flatMap(b, a);
  let resultOptionFlatMap = f =>
    fun
    | Belt.Result.Ok(Some(x)) => f(x)
    | x => x;

  let okSome = x => Belt.Result.Ok(Some(x));
  let error = e => Belt.Result.Error(e);

  let value = (fieldName, x) =>
    Field(fieldName, Future.value(Belt.Result.Ok(x)));
  let fieldValidatorOfMessageValidator = (f, Field(name, future)) =>
    future
    ->(
        Future.flatMapOk(x =>
          switch (x) {
          | None => None->Belt.Result.Ok->Future.value
          | Some(thing) =>
            f(thing)
            ->(Future.mapError(str => str->TransFieldError))
            ->(Future.mapOk(x => Some(x)))
          }
        )
      )
    ->(fresult => Field(name, fresult));

  let numberRange = (lo, hi) =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => lo <= x && x <= hi ? okSome(x) : error(NumberOutOfRangeError));
  let maxNumber = numberRange(Js.Int.min);
  let minNumber = n => numberRange(n, Js.Int.max);
  /* TODO can isEqualNumber and isEqualString be merged? */
  let isEqualNumber = n =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => x == n ? okSome(x) : error(WrongNumberError));
  let isEqualString = s =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => x == s ? okSome(x) : error(WrongStringError));
  let isInt =
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => Js.Math.trunc(x) == x ? okSome(x) : error(FractionalNumberError));
  /* TODO can notEqualNumber and notEqualString be merged? */
  let notEqualNumber = n =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => x != n ? okSome(x) : error(WrongNumberError));
  let notEqualString = s =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => s == x ? okSome(x) : error(WrongStringError));
  /* TODO is this in the stdlib? */
  [@bs.new] external newRegExp: string => Js.Re.t = "RegExp";
  let matchRegex = re => {
    let re = newRegExp(re);
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => x->(Js.Re.test(re)) ? okSome(x) : error(RegexMatchError));
  };
  let required = x =>
    x
    |> fieldMap @@
    futureMap(
      fun
      | Belt.Result.Ok(None) => error(MissingFieldError)
      | x => x,
    );
  let maxStrLen = len =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => x->String.length <= len ? okSome(x) : error(StringTooLongError));
  let minStrLen = len =>
    fieldMap @@
    futureMap @@
    resultOptionFlatMap @@
    (x => x->String.length >= len ? okSome(x) : error(StringTooShortError));
  let nonEmptyString = minStrLen(1);
  let trimString =
    fieldMap @@ futureMap @@ resultMap @@ optionMap @@ Js.String.trim;

  /* TODO implement in terms of previous validators */
  let positive = minNumber(1);
  /* Is US Employer Identification Numebr? */
  /* let isUSEIN */
  let repeated:
    (field('a) => field('a), field(array('a))) => field(array('a)) =
    (f, Field(fieldName, future)) => {
      let foo = x =>
        switch (x) {
        | None => None->Belt.Result.Ok->Future.value
        | Some(thing) =>
          let bar =
            thing
            |> futureFlatMapArray(
                 x =>
                   (
                     Field(fieldName, Future.value(Belt.Result.Ok(Some(x))))
                     |> f
                     |> fieldGet
                   )
                   ->(Future.mapOk(Belt.Option.getExn)),
                 0x7fffffff,
               );

          bar
          ->(
              Future.map((x: array(Belt.Result.t('a, exn))) => {
                let firstError = arrayFirst(Belt.Result.isError, 0, x);
                switch (firstError) {
                | Some(Belt.Result.Error(x)) => Belt.Result.Error(x)
                | None => Belt.Result.Ok(Array.map(Belt.Result.getExn, x))
                | Some(Belt.Result.Ok(_)) =>
                  raise(ImpossibleError("found unfound element"))
                };
              })
            )
          ->(Future.mapOk(x => Some(x)));
        };
      let future = Future.flatMapOk(future, foo);
      Field(fieldName, future);
    };

  let minItemCount = (len, x: field(array('a))) =>
    x
    |> fieldMap @@
    futureMap(
      fun
      | Belt.Result.Ok(Some(x)) =>
        x->Array.length < len
          ? Belt.Result.Error(ArrayTooShortError) : Belt.Result.Ok(Some(x))
      | x => x,
    );
  let maxItemCount = (len, x: field(array('a))) =>
    x
    |> fieldMap @@
    futureMap(
      fun
      | Belt.Result.Ok(Some(x)) =>
        x->Array.length > len
          ? Belt.Result.Error(ArrayTooShortError) : Belt.Result.Ok(Some(x))
      | x => x,
    );

  let err2msg =
    fun
    | MissingFieldError => "required field missing"
    | StringTooLongError => "string value too long"
    | StringTooShortError => "string value too short"
    | NumberNotPositiveError => "number value non-positive"
    | ArrayTooShortError => "not enough items"
    | FractionalNumberError => "number is fractional"
    | WrongStringError => "string mismatch"
    | UnimplementedError => "unimplemented validation"
    | WrongNumberError => "wrong number"
    | NumberOutOfRangeError => "number ouf ot range"
    | RegexMatchError => "string pattern mismatch"
    | TransFieldError(s) => "involving multiple fields: " ++ s
    | _ => "unknown error";

  let fold = (Field(fieldName, Future.Future(get)), happy, sad) =>
    get(x =>
      switch (x) {
      | Belt.Result.Ok(x) => happy(x)
      | Belt.Result.Error(err) => err |> err2msg |> sad(fieldName)
      }
    );
};

/** a type that cannot be instantiated */
type uninstantiable;
/** a type to only hold values like Js.Nullable.null and
   * Js.Nullable.undefined */
type justNull = Js.Nullable.t(uninstantiable);
type grpcLoadResult;

[@bs.module "bs-grpc"]
external grpcLoadProto: string => grpcLoadResult = "load";

/* Represents a grpc.Server object */
type server;

/* A node.js bufffer TODO see if there is a more standard type somewhere
 * for this. We treat it as opaque, though, so it's probably okay.
 */
type buffer;

/* This type is part of the type of grpc.ServerCredentials.createSsl() */
module ServerKeyAndCert = {
  [@bs.deriving abstract]
  type t = {
    [@bs.as "private_key"]
    privateKey: buffer,
    [@bs.as "cert_chain"]
    certChain: buffer,
  };
};
/* fileName = "undefined" */
/* moduleName = "*root*" */
module Tin = {
  /* fileName = "tin.proto" */
  /* moduleName = "Tin" */

  type grpcProtoHandle;
  [@bs.get] external getProtoHandle: grpcLoadResult => grpcProtoHandle = "tin";
  let myProtoHandle = grpcLoadProto("tin.proto") |> getProtoHandle;
};
module Assemblyline = {
  /* fileName = "assemblyline.proto" */
  /* moduleName = "Assemblyline" */

  type grpcProtoHandle;
  [@bs.get]
  external getProtoHandle: grpcLoadResult => grpcProtoHandle = "assemblyline";
  let myProtoHandle = grpcLoadProto("assemblyline.proto") |> getProtoHandle;
  module WidgetCondition = {
    /* fileName = "undefined" */
    /* moduleName = "Assemblyline.WidgetCondition" */

    /* enum type */
    type t =
      | WidgetConditionAdequate
      | WidgetConditionDefective;

    /** convert a grpc enum ordinal to its WidgetCondition.t counterpart; internal */
    let widgetConditionOfInt =
      fun
      | 0 => WidgetConditionAdequate
      | 1 => WidgetConditionDefective

      | x =>
        raise(
          BsGrpcDecoderError(
            {j|bs-grpc encountered invalid WidgetCondition enum value $x|j},
          ),
        );
    /** convert a grpc symbolic enum string to its WidgetCondition.t counterpart; internal */
    let widgetConditionOfString =
      fun
      | "WIDGET_CONDITION_ADEQUATE" => WidgetConditionAdequate
      | "WIDGET_CONDITION_DEFECTIVE" => WidgetConditionDefective

      | x =>
        raise(
          BsGrpcDecoderError(
            {j|bs-grpc encountered invalid WidgetCondition enum value $x|j},
          ),
        );
    /** convert a WidgetCondition.t to its the grpc enum ordinal counterpart; internal */
    let intOfWidgetCondition =
      fun
      | WidgetConditionAdequate => 0
      | WidgetConditionDefective => 1

      | x =>
        raise(
          BsGrpcDecoderError(
            {j|bs-grpc encountered invalid WidgetCondition enum value $x|j},
          ),
        );
    /** convert a WidgetCondition.t to its the grpc symbolic enum string counterpart; internal */
    let stringOfWidgetCondition =
      fun
      | WidgetConditionAdequate => "WIDGET_CONDITION_ADEQUATE"
      | WidgetConditionDefective => "WIDGET_CONDITION_DEFECTIVE";
  };
  module BlankWidget = {
    /* fileName = "undefined" */
    /* moduleName = "Assemblyline.BlankWidget" */
    /* Message module */
    [@bs.deriving abstract]
    type t = {
      [@bs.optional]
      condition: string,
    };
    /** message constructor (shadows the deriving abstract constructor) */
    let t = (~condition=?, ()) =>
      t(
        ~condition=?
          (Validation.optionMap @@ WidgetCondition.stringOfWidgetCondition) @@
          condition,
        (),
      );
    /* enum converting getter */
    let conditionGet =
      Validation.optionMap @@
      WidgetCondition.widgetConditionOfString
      << conditionGet;
    /* safe message constructor (may replace t()) */
    let make = (~condition=?, ()) => t(~condition?, ());

    /* sanitize, validate, normalize */
    let validate = x =>
      Future.make(resolve => {
        let conditionRef = ref(x->conditionGet);

        let n = ref(1);
        let failed = ref(false);
        /* message is valid; build the normalized message */
        /* TODO makeMessage is probably vestigial at this point
               let makeMessage = () => {
                 Belt.Result.Ok(t(
           ~condition=?conditionRef^,

                 ()))
               };
               */
        let wholeMessageValidation = x => x;
        let fail = (fieldName, errMsg) => {
          failed := true;
          resolve(
            Belt.Result.Error(
              {j|error validating field $fieldName: $errMsg|j},
            ),
          );
        };
        let tick = (field, y) =>
          if (! failed^) {
            field := y;
            n := n^ - 1;
            if (n^ == 0) {
              (
                t(~condition=?conditionRef^, ())
                |> (x => Some(x))
                |> Validation.value("BlankWidget")
                |> wholeMessageValidation
                |> Validation.required
              )
              ->(
                  Validation.fold(
                    opt => resolve(Belt.Result.Ok(Belt.Option.getExn(opt))),
                    fail,
                  )
                );
            };
          };
        Validation.
          /* field validation */
          (
            (conditionRef^ |> value("condition") |> required)
            ->(fold(tick(conditionRef), fail))
            ->ignore
          );
      });

    type codec;
    type intermediateMessageObject;

    [@bs.send]
    external encode:
      (codec, intermediateMessageObject, justNull, justNull) => byteBuffer;
    [@bs.send]
    external decode:
      (codec, Node.buffer, justNull, justNull) => intermediateMessageObject;
    [@bs.send] external fromObject: (codec, t) => intermediateMessageObject;
    [@bs.send]
    external toObject:
      (codec, intermediateMessageObject, protobufjsToObjectOptions) => t;

    [@bs.get] external codec: grpcProtoHandle => codec = "BlankWidget";

    let codec = myProtoHandle->codec;

    let encode = x =>
      (x |> fromObject(codec))
      ->encode(codec, _, Js.Nullable.undefined, Js.Nullable.undefined)
      ->bufferOfByteBuffer;
    let decode = x =>
      decode(codec, x, Js.Nullable.undefined, Js.Nullable.undefined)
      ->toObject(codec, _, myProtobufjsToObjectOptions);
  };
  module CutWidget = {
    /* fileName = "undefined" */
    /* moduleName = "Assemblyline.CutWidget" */
    /* Message module */
    [@bs.deriving abstract]
    type t = {
      [@bs.optional]
      condition: string,
      [@bs.optional]
      numTeeth: int,
    };
    /** message constructor (shadows the deriving abstract constructor) */
    let t = (~condition=?, ~numTeeth=?, ()) =>
      t(
        ~condition=?
          (Validation.optionMap @@ WidgetCondition.stringOfWidgetCondition) @@
          condition,
        ~numTeeth?,
        (),
      );
    /* enum converting getter */
    let conditionGet =
      Validation.optionMap @@
      WidgetCondition.widgetConditionOfString
      << conditionGet;
    /* safe message constructor (may replace t()) */
    let make = (~condition=?, ~numTeeth=?, ()) =>
      t(~condition?, ~numTeeth?, ());

    /* sanitize, validate, normalize */
    let validate = x =>
      Future.make(resolve => {
        let conditionRef = ref(x->conditionGet);
        let numTeethRef = ref(x->numTeethGet);

        let n = ref(2);
        let failed = ref(false);
        /* message is valid; build the normalized message */
        /* TODO makeMessage is probably vestigial at this point
                           let makeMessage = () => {
                             Belt.Result.Ok(t(
                       ~condition=?conditionRef^,
           ~numTeeth=?numTeethRef^,

                             ()))
                           };
                           */
        let wholeMessageValidation = x => x;
        let fail = (fieldName, errMsg) => {
          failed := true;
          resolve(
            Belt.Result.Error(
              {j|error validating field $fieldName: $errMsg|j},
            ),
          );
        };
        let tick = (field, y) =>
          if (! failed^) {
            field := y;
            n := n^ - 1;
            if (n^ == 0) {
              (
                t(~condition=?conditionRef^, ~numTeeth=?numTeethRef^, ())
                |> (x => Some(x))
                |> Validation.value("CutWidget")
                |> wholeMessageValidation
                |> Validation.required
              )
              ->(
                  Validation.fold(
                    opt => resolve(Belt.Result.Ok(Belt.Option.getExn(opt))),
                    fail,
                  )
                );
            };
          };
        open Validation;
        /* field validation */
        (conditionRef^ |> value("condition") |> required)
        ->(fold(tick(conditionRef), fail))
        ->ignore;
        /* field validation */
        (
          numTeethRef^ |> value("numTeeth") |> required |> numberRange(32, 63)
        )
        ->(fold(tick(numTeethRef), fail))
        ->ignore;
      });

    type codec;
    type intermediateMessageObject;

    [@bs.send]
    external encode:
      (codec, intermediateMessageObject, justNull, justNull) => byteBuffer;
    [@bs.send]
    external decode:
      (codec, Node.buffer, justNull, justNull) => intermediateMessageObject;
    [@bs.send] external fromObject: (codec, t) => intermediateMessageObject;
    [@bs.send]
    external toObject:
      (codec, intermediateMessageObject, protobufjsToObjectOptions) => t;

    [@bs.get] external codec: grpcProtoHandle => codec = "CutWidget";

    let codec = myProtoHandle->codec;

    let encode = x =>
      (x |> fromObject(codec))
      ->encode(codec, _, Js.Nullable.undefined, Js.Nullable.undefined)
      ->bufferOfByteBuffer;
    let decode = x =>
      decode(codec, x, Js.Nullable.undefined, Js.Nullable.undefined)
      ->toObject(codec, _, myProtobufjsToObjectOptions);
  };
  module WidgetColor = {
    /* fileName = "undefined" */
    /* moduleName = "Assemblyline.WidgetColor" */

    /* enum type */
    type t =
      | WidgetColorUnpainted
      | WidgetColorRed
      | WidgetColorGreen;

    /** convert a grpc enum ordinal to its WidgetColor.t counterpart; internal */
    let widgetColorOfInt =
      fun
      | 0 => WidgetColorUnpainted
      | 1 => WidgetColorRed
      | 2 => WidgetColorGreen

      | x =>
        raise(
          BsGrpcDecoderError(
            {j|bs-grpc encountered invalid WidgetColor enum value $x|j},
          ),
        );
    /** convert a grpc symbolic enum string to its WidgetColor.t counterpart; internal */
    let widgetColorOfString =
      fun
      | "WIDGET_COLOR_UNPAINTED" => WidgetColorUnpainted
      | "WIDGET_COLOR_RED" => WidgetColorRed
      | "WIDGET_COLOR_GREEN" => WidgetColorGreen

      | x =>
        raise(
          BsGrpcDecoderError(
            {j|bs-grpc encountered invalid WidgetColor enum value $x|j},
          ),
        );
    /** convert a WidgetColor.t to its the grpc enum ordinal counterpart; internal */
    let intOfWidgetColor =
      fun
      | WidgetColorUnpainted => 0
      | WidgetColorRed => 1
      | WidgetColorGreen => 2

      | x =>
        raise(
          BsGrpcDecoderError(
            {j|bs-grpc encountered invalid WidgetColor enum value $x|j},
          ),
        );
    /** convert a WidgetColor.t to its the grpc symbolic enum string counterpart; internal */
    let stringOfWidgetColor =
      fun
      | WidgetColorUnpainted => "WIDGET_COLOR_UNPAINTED"
      | WidgetColorRed => "WIDGET_COLOR_RED"
      | WidgetColorGreen => "WIDGET_COLOR_GREEN";
  };
  module PaintedWidget = {
    /* fileName = "undefined" */
    /* moduleName = "Assemblyline.PaintedWidget" */
    /* Message module */
    [@bs.deriving abstract]
    type t = {
      [@bs.optional]
      condition: string,
      [@bs.optional]
      numTeeth: int,
      [@bs.optional]
      paintJob: string,
    };
    /** message constructor (shadows the deriving abstract constructor) */
    let t = (~condition=?, ~numTeeth=?, ~paintJob=?, ()) =>
      t(
        ~condition=?
          (Validation.optionMap @@ WidgetCondition.stringOfWidgetCondition) @@
          condition,
        ~numTeeth?,
        ~paintJob=?
          (Validation.optionMap @@ WidgetColor.stringOfWidgetColor) @@ paintJob,
        (),
      );
    /* enum converting getter */
    let conditionGet =
      Validation.optionMap @@
      WidgetCondition.widgetConditionOfString
      << conditionGet;
    /* enum converting getter */
    let paintJobGet =
      Validation.optionMap @@ WidgetColor.widgetColorOfString << paintJobGet;
    /* safe message constructor (may replace t()) */
    let make = (~condition=?, ~numTeeth=?, ~paintJob=?, ()) =>
      t(~condition?, ~numTeeth?, ~paintJob?, ());

    /* sanitize, validate, normalize */
    let validate = x =>
      Future.make(resolve => {
        let conditionRef = ref(x->conditionGet);
        let numTeethRef = ref(x->numTeethGet);
        let paintJobRef = ref(x->paintJobGet);

        let n = ref(3);
        let failed = ref(false);
        /* message is valid; build the normalized message */
        /* TODO makeMessage is probably vestigial at this point
                           let makeMessage = () => {
                             Belt.Result.Ok(t(
                       ~condition=?conditionRef^,
           ~numTeeth=?numTeethRef^,
           ~paintJob=?paintJobRef^,

                             ()))
                           };
                           */
        let wholeMessageValidation = x => x;
        let fail = (fieldName, errMsg) => {
          failed := true;
          resolve(
            Belt.Result.Error(
              {j|error validating field $fieldName: $errMsg|j},
            ),
          );
        };
        let tick = (field, y) =>
          if (! failed^) {
            field := y;
            n := n^ - 1;
            if (n^ == 0) {
              (
                t(
                  ~condition=?conditionRef^,
                  ~numTeeth=?numTeethRef^,
                  ~paintJob=?paintJobRef^,
                  (),
                )
                |> (x => Some(x))
                |> Validation.value("PaintedWidget")
                |> wholeMessageValidation
                |> Validation.required
              )
              ->(
                  Validation.fold(
                    opt => resolve(Belt.Result.Ok(Belt.Option.getExn(opt))),
                    fail,
                  )
                );
            };
          };
        open Validation;
        /* field validation */
        (conditionRef^ |> value("condition") |> required)
        ->(fold(tick(conditionRef), fail))
        ->ignore;
        /* field validation */
        (
          numTeethRef^ |> value("numTeeth") |> required |> numberRange(32, 63)
        )
        ->(fold(tick(numTeethRef), fail))
        ->ignore;
        /* field validation */
        (paintJobRef^ |> value("paintJob") |> required)
        ->(fold(tick(paintJobRef), fail))
        ->ignore;
      });

    type codec;
    type intermediateMessageObject;

    [@bs.send]
    external encode:
      (codec, intermediateMessageObject, justNull, justNull) => byteBuffer;
    [@bs.send]
    external decode:
      (codec, Node.buffer, justNull, justNull) => intermediateMessageObject;
    [@bs.send] external fromObject: (codec, t) => intermediateMessageObject;
    [@bs.send]
    external toObject:
      (codec, intermediateMessageObject, protobufjsToObjectOptions) => t;

    [@bs.get] external codec: grpcProtoHandle => codec = "PaintedWidget";

    let codec = myProtoHandle->codec;

    let encode = x =>
      (x |> fromObject(codec))
      ->encode(codec, _, Js.Nullable.undefined, Js.Nullable.undefined)
      ->bufferOfByteBuffer;
    let decode = x =>
      decode(codec, x, Js.Nullable.undefined, Js.Nullable.undefined)
      ->toObject(codec, _, myProtobufjsToObjectOptions);
  };
  module BoxOfWidgets = {
    /* fileName = "undefined" */
    /* moduleName = "Assemblyline.BoxOfWidgets" */
    /* Message module */
    [@bs.deriving abstract]
    type t = {
      [@bs.optional]
      paintedWidgets: array(PaintedWidget.t),
    };
    /* safe message constructor (may replace t()) */
    let make = (~paintedWidgets=?, ()) => t(~paintedWidgets?, ());

    /* sanitize, validate, normalize */
    let validate = x =>
      Future.make(resolve => {
        let paintedWidgetsRef = ref(x->paintedWidgetsGet);

        let n = ref(1);
        let failed = ref(false);
        /* message is valid; build the normalized message */
        /* TODO makeMessage is probably vestigial at this point
               let makeMessage = () => {
                 Belt.Result.Ok(t(
           ~paintedWidgets=?paintedWidgetsRef^,

                 ()))
               };
               */
        let wholeMessageValidation = x => x;
        let fail = (fieldName, errMsg) => {
          failed := true;
          resolve(
            Belt.Result.Error(
              {j|error validating field $fieldName: $errMsg|j},
            ),
          );
        };
        let tick = (field, y) =>
          if (! failed^) {
            field := y;
            n := n^ - 1;
            if (n^ == 0) {
              (
                t(~paintedWidgets=?paintedWidgetsRef^, ())
                |> (x => Some(x))
                |> Validation.value("BoxOfWidgets")
                |> wholeMessageValidation
                |> Validation.required
              )
              ->(
                  Validation.fold(
                    opt => resolve(Belt.Result.Ok(Belt.Option.getExn(opt))),
                    fail,
                  )
                );
            };
          };
        Validation.
          /* message field validation */
          (
            (
              paintedWidgetsRef^
              |> value("paintedWidgets")
              |> repeated(
                   fieldValidatorOfMessageValidator @@ PaintedWidget.validate,
                 )
            )
            ->(fold(tick(paintedWidgetsRef), fail))
            ->ignore
          );
      });

    type codec;
    type intermediateMessageObject;

    [@bs.send]
    external encode:
      (codec, intermediateMessageObject, justNull, justNull) => byteBuffer;
    [@bs.send]
    external decode:
      (codec, Node.buffer, justNull, justNull) => intermediateMessageObject;
    [@bs.send] external fromObject: (codec, t) => intermediateMessageObject;
    [@bs.send]
    external toObject:
      (codec, intermediateMessageObject, protobufjsToObjectOptions) => t;

    [@bs.get] external codec: grpcProtoHandle => codec = "BoxOfWidgets";

    let codec = myProtoHandle->codec;

    let encode = x =>
      (x |> fromObject(codec))
      ->encode(codec, _, Js.Nullable.undefined, Js.Nullable.undefined)
      ->bufferOfByteBuffer;
    let decode = x =>
      decode(codec, x, Js.Nullable.undefined, Js.Nullable.undefined)
      ->toObject(codec, _, myProtobufjsToObjectOptions);
  };
};

module Server = {
  module Credentials = {
    /* These are the public static constructor functions for
     * grpc.ServerCredentials
     */
    module Ssl = {
      [@bs.module "bs-grpc"] [@bs.scope "ServerCredentials"]
      external make:
        (buffer, array(ServerKeyAndCert.t), bool) => serverCredentials =
        "createSsl";
      let make = (~rootCert: buffer, ~privateKey: buffer, ~certChain: buffer) =>
        make(
          rootCert,
          [|ServerKeyAndCert.t(~privateKey, ~certChain)|],
          true,
        );
    };
    module Insecure = {
      [@bs.module "bs-grpc"] [@bs.scope "ServerCredentials"]
      external make: unit => serverCredentials = "createInsecure";
    };
  };

  [@bs.module "bs-grpc"] [@bs.new]
  external newServer: unit => server = "Server";

  [@bs.send]
  external serverBind: (server, string, serverCredentials) => unit = "bind";

  [@bs.send] external start: server => unit = "start";

  /** Convenience function to instantiate and configure a GRPC server */
  let make = (~credentials, host) => {
    let server = newServer();
    serverBind(server, host, credentials);

    start(server);
    server;
  };
};

module Client = {
  module Metadata = {
    type t;
    [@bs.module "bs-grpc"] [@bs.new] external make: unit => t = "Metadata";
    [@bs.send] external set: (t, string, string) => unit;
    let set = (t, key, value) => {
      set(t, key, value);
      t;
    };
    module Generator = {
      /** the type of the function you must call to furnish an RPC invocation with
         either its metadata or a metadata generation error. see `makeGenerator`
        for details */
      type complete = (Js.Nullable.t(exn), t) => unit;
      /** consumers of bs-grpc who want to use metadata must implement a function
         of this type. see `makeGenerator` for more details */
      type generatorImplementation = (unit, complete) => unit;
      /** to populate a grpc request with metadata, grpc-node requires you to
         supply a metadata generator function, which receives as its first argument
         the call object reflecting the request payload (not available in this
         binding) while the second argument is a function your metadata generator
         function must invoke with either an exception or the resulting metadata */
      [@bs.module "bs-grpc"] [@bs.scope "credentials"]
      external make: generatorImplementation => callCredentials =
        "createFromMetadataGenerator";
    };
  };

  module Credentials = {
    [@bs.module "bs-grpc"] [@bs.scope "credentials"]
    external createInsecure: unit => channelCredentials;

    [@bs.module "bs-grpc"] [@bs.scope "credentials"]
    external createSsl: (buffer, buffer, buffer) => channelCredentials;

    [@bs.module "bs-grpc"] [@bs.scope "credentials"]
    external combine:
      (channelCredentials, callCredentials) => channelCredentials =
      "combineChannelCredentials";

    [@bs.module "bs-grpc"] [@bs.scope "credentials"]
    external combine3:
      (channelCredentials, callCredentials, callCredentials) =>
      channelCredentials =
      "combineChannelCredentials";
  };
};

/* use this to load a nodejs Buffer object containing a base64-encoded
 * PEM format key/certificate
 */
[@bs.val] [@bs.module "fs"]
external loadCert: string => buffer = "readFileSync";
