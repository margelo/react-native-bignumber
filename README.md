# ⚡️ react-native-bignumber

The fastest Big Number library for React Native.

* 🏎️ Up to 10x faster than all other solutions
* ⚡️ Lightning fast implementation with pure C++ and JSI
* 🧪 Well tested in JS and C++ (OpenSSL)
* 💰 Made for crypto apps and Wallets
* 🔁 Easy drop-in-replacement for [BN.js](https://github.com/indutny/bn.js/) to speed up existing libraries

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

## Sponsors

- TODO: List sponsors here

## Adopting at scale

react-native-bignumber was built at Margelo, an elite app development agency. For enterprise support or other business inquiries, contact us at <a href="mailto:hello@margelo.io?subject=Adopting react-native-bignumber at scale">hello@margelo.io</a>!
