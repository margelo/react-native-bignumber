# ⚡️ react-native-bignumber

The fastest Big Number library for React Native.

* 🏎️ Up to 300x faster than all other solutions
* ⚡️ Lightning fast implementation with pure C++ and JSI
* 🧪 Well tested in JS and C++ (OpenSSL)
* 💰 Made for crypto apps and Wallets
* 🔢 Store numbers as big as your Phone's RAM can store
* 🔁 Easy drop-in replacement for [BN.js](https://github.com/indutny/bn.js/)

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

### ..as a normal library

The exposed `BigNumber` class is used to create new BigNumber instances from strings (binary, hex, decimal), ArrayBuffers, Buffers, numbers, or other BigNumber instances.

```ts
import { BigNumber } from 'react-native-bignumber'

const a = new BigNumber(3274556)
const b = new BigNumber(9856712)
const c = a.mul(b) // 32.276.355.419.872
```

Refer to [BN.js' documentation](https://github.com/indutny/bn.js/#instructions) for a full API reference and usage guide.

For example, this is how you calculate large Fibonacci numbers:

```ts
function fibonacci(n: number): BigNumber {
  let prev = new BigNumber(0)
  let prevPrev = new BigNumber(1)
  let number = new BigNumber(1)

  for (let i = 1; i < n; i++) {
    prevPrev = prev
    prev = number
    number = prevPrev.add(prev)
  }

  return number
}

console.log(fibonacci(50).toString(10)) // 12.586.269.025
```

### ..as a drop-in replacement

Since popular libraries like [ethers.js](https://github.com/ethers-io/ethers.js/) use [BN.js](https://github.com/indutny/bn.js/) under the hood, react-native-bignumber exposes exactly the same API as [BN.js](https://github.com/indutny/bn.js/) so it can be used as a drop-in replacement and promises much greater speed at common crypto operations.

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

<a href="https://exodus.com">
  <img align="right" src="img/exodus.svg" height="65" alt="Exodus" />
</a>

This library is supported by [**Exodus**](https://exodus.com).
Send, receive, and exchange Bitcoin and 160+ cryptocurrencies with ease on the world's leading Desktop, Mobile and Hardware crypto wallets

## Adopting at scale

react-native-bignumber was built at Margelo, an elite app development agency. For enterprise support or other business inquiries, contact us at <a href="mailto:hello@margelo.io?subject=Adopting react-native-bignumber at scale">hello@margelo.io</a>!
