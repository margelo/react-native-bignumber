import { NativeModules, Platform } from 'react-native';

export interface InternalNumber {}

export interface InternalRedNumber extends InternalNumber {
  isInternalRedBigNum: true;
}

export interface InternalModContext {
  isModContext: true;
}

interface NativeBigNumberSpec {
  createFromString: (
    strRep: string,
    base: number,
    endian: boolean
  ) => InternalNumber;
  createFromNumber: (number: number) => InternalNumber;
  createFromArray: (
    array: Array<number>,
    base: number,
    le: boolean
  ) => InternalNumber;
  createFromArrayBuffer: (
    array: ArrayBuffer,
    base: number,
    le: boolean
  ) => InternalNumber;
  createModCtx: (internalNumber: InternalNumber) => InternalModContext;
  createModCtxFromNumber: (mod: number) => InternalModContext;
  forceCreateRed: (
    this: InternalNumber,
    mctx: InternalModContext
  ) => InternalRedNumber;
  bn2Mod: (
    internalNumber: InternalNumber,
    mctx: InternalModContext
  ) => InternalRedNumber;
  mod2bn: (redNumber: InternalRedNumber) => InternalNumber;
  getPrimeContext: (primeType: string) => InternalModContext;
  getPrime: (primeType: string) => InternalNumber;

  //bigNumber
  toString: (this: InternalNumber, base: 2 | 10 | 16, len?: number) => string;
  toArray: (this: InternalNumber, le: boolean, len: number) => Array<number>;

  add: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  iadd: (this: InternalNumber, other: InternalNumber) => void;
  iaddn: (this: InternalNumber, other: number) => void;
  addn: (this: InternalNumber, other: number) => InternalNumber;

  sub: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  isub: (this: InternalNumber, other: InternalNumber) => void;
  isubn: (this: InternalNumber, other: number) => void;
  subn: (this: InternalNumber, other: number) => InternalNumber;

  mul: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  imul: (this: InternalNumber, other: InternalNumber) => void;
  imuln: (this: InternalNumber, other: number) => void;
  muln: (this: InternalNumber, other: number) => InternalNumber;

  div: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  idiv: (this: InternalNumber, other: InternalNumber) => void;
  idivn: (this: InternalNumber, other: number) => void;
  divn: (this: InternalNumber, other: number) => InternalNumber;

  mod: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  umod: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  imod: (this: InternalNumber, other: InternalNumber) => void;
  imodn: (this: InternalNumber, other: number) => void;
  modn: (this: InternalNumber, other: number) => InternalNumber;

  pow: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  ipow: (this: InternalNumber, other: InternalNumber) => void;

  setn: (this: InternalNumber, other: number, value: number) => void;
  testn: (this: InternalNumber, bit: number) => boolean;
  bincn: (this: InternalNumber, bit: number) => void;
  inotn: (this: InternalNumber, width: number) => void;
  notn: (this: InternalNumber, width: number) => InternalNumber;
  imaskn: (this: InternalNumber, bit: number) => void;
  maskn: (this: InternalNumber, bit: number) => InternalNumber;
  ishln: (this: InternalNumber, places: number) => void;
  shln: (this: InternalNumber, places: number) => InternalNumber;
  ishrn: (this: InternalNumber, places: number) => void;
  shrn: (this: InternalNumber, places: number) => InternalNumber;

  divmod: (
    this: InternalNumber,
    m: InternalNumber
  ) => { div: InternalNumber; mod: InternalNumber };
  divRound: (this: InternalNumber, other: InternalNumber) => InternalNumber;

  sqr: (this: InternalNumber) => InternalNumber;
  isqr: (this: InternalNumber) => void;
  abs: (this: InternalNumber) => InternalNumber;
  iabs: (this: InternalNumber) => void;
  neg: (this: InternalNumber) => InternalNumber;
  ineg: (this: InternalNumber) => void;

  isZero: (this: InternalNumber) => boolean;
  isOne: (this: InternalNumber) => boolean;
  clone: (this: InternalNumber) => InternalNumber;
  bitLength: (this: InternalNumber) => number;
  byteLength: (this: InternalNumber) => number;
  isNeg: (this: InternalNumber) => boolean;
  isOdd: (this: InternalNumber) => boolean;
  isEven: (this: InternalNumber) => boolean;

  cmp: (this: InternalNumber, other: InternalNumber) => -1 | 0 | 1;
  cmpn: (this: InternalNumber, other: number) => -1 | 0 | 1;
  ucmp: (this: InternalNumber, other: InternalNumber) => -1 | 0 | 1;
  lt: (this: InternalNumber, other: InternalNumber) => boolean;
  lte: (this: InternalNumber, other: InternalNumber) => boolean;
  gt: (this: InternalNumber, other: InternalNumber) => boolean;
  gte: (this: InternalNumber, other: InternalNumber) => boolean;
  eq: (this: InternalNumber, other: InternalNumber) => boolean;

  invm: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  gcd: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  igcd: (this: InternalNumber, other: InternalNumber) => boolean;

  ior: (this: InternalNumber, other: InternalNumber) => void;
  or: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  iand: (this: InternalNumber, other: InternalNumber) => void;
  and: (this: InternalNumber, other: InternalNumber) => InternalNumber;
  ixor: (this: InternalNumber, other: InternalNumber) => void;
  xor: (this: InternalNumber, other: InternalNumber) => InternalNumber;

  // InternalRedNumber
  redAdd: (
    this: InternalRedNumber,
    other: InternalRedNumber
  ) => InternalRedNumber;
  redIAdd: (this: InternalRedNumber, other: InternalRedNumber) => void;
  redSub: (
    this: InternalRedNumber,
    other: InternalRedNumber
  ) => InternalRedNumber;
  redISub: (this: InternalRedNumber, other: InternalRedNumber) => void;
  redShl: (this: InternalRedNumber, places: number) => InternalRedNumber;
  redMul: (
    this: InternalRedNumber,
    other: InternalRedNumber
  ) => InternalRedNumber;
  redIMul: (this: InternalRedNumber, other: InternalRedNumber) => void;
  redSqr: (this: InternalRedNumber) => InternalRedNumber;
  redISqr: (this: InternalRedNumber) => void;
  redSqrt: (this: InternalRedNumber) => InternalRedNumber;
  redInvm: (this: InternalRedNumber) => InternalRedNumber;
  redNeg: (this: InternalRedNumber) => InternalRedNumber;
  redPow: (this: InternalRedNumber, other: InternalNumber) => InternalRedNumber;
  redCmp: (this: InternalRedNumber, other: InternalNumber) => 0 | 1 | -1;
  cloneRed: (this: InternalRedNumber) => InternalRedNumber;
  isRedZero: (this: InternalRedNumber) => boolean;
}

// global func declaration for JSI functions
declare global {
  function nativeCallSyncHook(): unknown;
  var __BigNumberProxy: object | undefined;
}

// Check if the constructor exists. If not, try installing the JSI bindings.
if (global.__BigNumberProxy == null) {
  // Get the native BigNumber ReactModule
  const BigNumberModule = NativeModules.BigNumber;
  if (BigNumberModule == null) {
    let message =
      'Failed to install react-native-bignumber: The native `BigNumber` Module could not be found.';
    message +=
      '\n* Make sure react-native-bignumber is correctly autolinked (run `npx react-native config` to verify)';
    if (Platform.OS === 'ios' || Platform.OS === 'macos') {
      message += '\n* Make sure you ran `pod install` in the ios/ directory.';
    }
    if (Platform.OS === 'android') {
      message += '\n* Make sure gradle is synced.';
    }
    // check if Expo
    const ExpoConstants =
      NativeModules.NativeUnimoduleProxy?.modulesConstants?.ExponentConstants;
    if (ExpoConstants != null) {
      if (ExpoConstants.appOwnership === 'expo') {
        // We're running Expo Go
        throw new Error(
          'react-native-bignumber is not supported in Expo Go! Use EAS (`expo prebuild`) or eject to a bare workflow instead.'
        );
      } else {
        // We're running Expo bare / standalone
        message += '\n* Make sure you ran `expo prebuild`.';
      }
    }

    message += '\n* Make sure you rebuilt the app.';
    throw new Error(message);
  }

  // Check if we are running on-device (JSI)
  if (global.nativeCallSyncHook == null || BigNumberModule.install == null) {
    throw new Error(
      'Failed to install react-native-bignumber: React Native is not running on-device. BigNumber can only be used when synchronous method invocations (JSI) are possible. If you are using a remote debugger (e.g. Chrome), switch to an on-device debugger (e.g. Flipper) instead.'
    );
  }

  // Call the synchronous blocking install() function
  const result = BigNumberModule.install();
  if (result !== true)
    throw new Error(
      `Failed to install react-native-bignumber: The native BigNumber Module could not be installed! Looks like something went wrong when installing JSI bindings: ${result}`
    );

  // Check again if the constructor now exists. If not, throw an error.
  if (global.__BigNumberProxy == null)
    throw new Error(
      'Failed to install react-native-bignumber, the native initializer function does not exist. Are you trying to use BigNumber from different JS Runtimes?'
    );
}

const proxy = global.__BigNumberProxy;
export const NativeBigNumber = proxy as any as NativeBigNumberSpec;
