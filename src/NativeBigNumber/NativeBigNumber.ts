import { NativeModules, Platform } from 'react-native';

export interface InternalNumber {
  toString: (base: 2 | 10 | 16, len?: number) => string;

  add: (other: InternalNumber) => InternalNumber;
  iadd: (other: InternalNumber) => void;
  iaddn: (other: number) => void;
  addn: (other: number) => InternalNumber;

  sub: (other: InternalNumber) => InternalNumber;
  isub: (other: InternalNumber) => void;
  isubn: (other: number) => void;
  subn: (other: number) => InternalNumber;

  mul: (other: InternalNumber) => InternalNumber;
  imul: (other: InternalNumber) => void;
  imuln: (other: number) => void;
  muln: (other: number) => InternalNumber;

  div: (other: InternalNumber) => InternalNumber;
  idiv: (other: InternalNumber) => void;
  idivn: (other: number) => void;
  divn: (other: number) => InternalNumber;

  mod: (other: InternalNumber) => InternalNumber;
  umod: (other: InternalNumber) => InternalNumber;
  imod: (other: InternalNumber) => void;
  imodn: (other: number) => void;
  modn: (other: number) => InternalNumber;

  pow: (other: InternalNumber) => InternalNumber;
  ipow: (other: InternalNumber) => void;

  setn: (other: number) => void;
  testn: (bit: number) => boolean;
  bincn: (bit: number) => void;
  inotn: (width: number) => void;
  notn: (width: number) => InternalNumber;
  imaskn: (bit: number) => void;
  maskn: (bit: number) => InternalNumber;
  ishln: (places: number) => void;
  shln: (places: number) => InternalNumber;
  ishrn: (places: number) => void;
  shrn: (places: number) => InternalNumber;

  divmod: (m: InternalNumber) => { div: InternalNumber; mod: InternalNumber };
  divRound: (other: InternalNumber) => InternalNumber;

  sqr: () => InternalNumber;
  isqr: () => void;
  abs: () => InternalNumber;
  iabs: () => void;
  neg: () => InternalNumber;
  ineg: () => void;

  isZero: () => boolean;
  isOne: () => boolean;
  clone: () => InternalNumber;
  bitLength: () => number;
  byteLength: () => number;
  isNeq: () => boolean;
  isOdd: () => boolean;
  isEven: () => boolean;

  cmp: (other: InternalNumber) => -1 | 0 | 1;
  ucmp: (other: InternalNumber) => -1 | 0 | 1;
  lt: (other: InternalNumber) => boolean;
  lte: (other: InternalNumber) => boolean;
  gt: (other: InternalNumber) => boolean;
  gte: (other: InternalNumber) => boolean;
  eq: (other: InternalNumber) => boolean;

  invm: (other: InternalNumber) => InternalNumber;
  gcd: (other: InternalNumber) => InternalNumber;
  igcd: (other: InternalNumber) => boolean;

  ior: (other: InternalNumber) => void;
  or: (other: InternalNumber) => InternalNumber;
  iand: (other: InternalNumber) => void;
  and: (other: InternalNumber) => InternalNumber;
  ixor: (other: InternalNumber) => void;
  xor: (other: InternalNumber) => InternalNumber;

  isInternalBigNum: true;
}

export interface InternalRedNumber {
  redAdd: (other: InternalRedNumber) => InternalRedNumber;
  redIAdd: (other: InternalRedNumber) => void;
  redSub: (other: InternalRedNumber) => InternalRedNumber;
  redISub: (other: InternalRedNumber) => void;
  redShl: (places: number) => void;
  redMul: (other: InternalRedNumber) => InternalRedNumber;
  redIMul: (other: InternalRedNumber) => void;
  redSqr: () => InternalRedNumber;
  redISqr: () => void;
  redSqrt: () => InternalRedNumber;
  redInvm: () => InternalRedNumber;
  redNeg: () => InternalRedNumber;
  redPow: (other: InternalRedNumber) => InternalRedNumber;
  isInternalRedBigNum: true;
}

export interface InternalModContext {}

interface NativeBigNumberSpec {
  createFromString: (strRep: string, base: number) => InternalNumber;
  createFromNumber: (number: number) => InternalNumber;
  createModCtx: (internalNumber: InternalNumber) => InternalModContext;
  bn2Mod: (internalNumber: InternalNumber) => InternalRedNumber;
  Mod2bn: (redNumber: InternalRedNumber) => InternalNumber;
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
