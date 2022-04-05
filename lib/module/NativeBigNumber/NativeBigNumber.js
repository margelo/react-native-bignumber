import { NativeModules, Platform } from 'react-native';

// Check if the constructor exists. If not, try installing the JSI bindings.
if (global.__BigNumberProxy == null) {
  // Get the native BigNumber ReactModule
  const BigNumberModule = NativeModules.BigNumber;

  if (BigNumberModule == null) {
    var _NativeModules$Native, _NativeModules$Native2;

    let message = 'Failed to install react-native-bignumber: The native `BigNumber` Module could not be found.';
    message += '\n* Make sure react-native-bignumber is correctly autolinked (run `npx react-native config` to verify)';

    if (Platform.OS === 'ios' || Platform.OS === 'macos') {
      message += '\n* Make sure you ran `pod install` in the ios/ directory.';
    }

    if (Platform.OS === 'android') {
      message += '\n* Make sure gradle is synced.';
    } // check if Expo


    const ExpoConstants = (_NativeModules$Native = NativeModules.NativeUnimoduleProxy) === null || _NativeModules$Native === void 0 ? void 0 : (_NativeModules$Native2 = _NativeModules$Native.modulesConstants) === null || _NativeModules$Native2 === void 0 ? void 0 : _NativeModules$Native2.ExponentConstants;

    if (ExpoConstants != null) {
      if (ExpoConstants.appOwnership === 'expo') {
        // We're running Expo Go
        throw new Error('react-native-bignumber is not supported in Expo Go! Use EAS (`expo prebuild`) or eject to a bare workflow instead.');
      } else {
        // We're running Expo bare / standalone
        message += '\n* Make sure you ran `expo prebuild`.';
      }
    }

    message += '\n* Make sure you rebuilt the app.';
    throw new Error(message);
  } // Check if we are running on-device (JSI)


  if (global.nativeCallSyncHook == null || BigNumberModule.install == null) {
    throw new Error('Failed to install react-native-bignumber: React Native is not running on-device. BigNumber can only be used when synchronous method invocations (JSI) are possible. If you are using a remote debugger (e.g. Chrome), switch to an on-device debugger (e.g. Flipper) instead.');
  } // Call the synchronous blocking install() function


  const result = BigNumberModule.install();
  if (result !== true) throw new Error(`Failed to install react-native-bignumber: The native BigNumber Module could not be installed! Looks like something went wrong when installing JSI bindings: ${result}`); // Check again if the constructor now exists. If not, throw an error.

  if (global.__BigNumberProxy == null) throw new Error('Failed to install react-native-bignumber, the native initializer function does not exist. Are you trying to use BigNumber from different JS Runtimes?');
}

const proxy = global.__BigNumberProxy;
export const NativeBigNumber = proxy;
//# sourceMappingURL=NativeBigNumber.js.map