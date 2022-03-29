# ⚡️ react-native-bignumber

A fast implementation of bignumber module written in C/C++ JSI.

BigNumber can be used as a drop-in replacement for your bn.js library.

---

## Installation

<h3>React Native  <img src="./img/react-native.png" height="15"></h3>

```sh
yarn add react-native-bignumber
cd ios && pod install
```

<h3>Expo  <img src="./img/expo.png" height="12"></h3>

```sh
expo install react-native-bignumber
expo prebuild
```

## Usage

In your `metro.config.js`, add a module resolver to replace `crypto` with `react-native-bignumber`:

```diff
+const path = require('path');

 module.exports = {
+  resolver: {
+    extraNodeModules: {
+      bn: path.resolve(__dirname, './node_modules/react-native-bignumber'),
+    },
+  },
   ...
```

Now, all imports for `bn` will be resolved as `react-native-bignumber` instead.

---

## Sponsors

- TODO: List sponsors here

## Limitations

As the library uses JSI for synchronous native methods access, remote debugging (e.g. with Chrome) is no longer possible. Instead, you should use [Flipper](https://fbflipper.com).

## Adopting at scale

react-native-bignumber was built at Margelo, an elite app development agency. For enterprise support or other business inquiries, contact us at <a href="mailto:hello@margelo.io?subject=Adopting react-native-bignumber at scale">hello@margelo.io</a>!

## Contributing

See the [contributing guide](CONTRIBUTING.md) to learn how to contribute to the repository and the development workflow.

## License

MIT
