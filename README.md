<a href="https://margelo.io">
  <img src="./docs/banner.svg" width="100%" />
</a>

# 🔢 react-native-bignumber

The fastest Big Number library for React Native.

* 🏎️ Up to 300x faster than all other solutions
* ⚡️ Lightning fast implementation with pure C++ and JSI
* 🧪 Well tested in JS and C++ (OpenSSL)
* 💰 Made for crypto apps and Wallets
* 🤌 Up to 5x smaller in JS-bundle size
* 🔢 Store numbers as big as your Phone's RAM can store
* 🔁 Easy drop-in replacement for [BN.js](https://github.com/indutny/bn.js/)

## Installation

<h3>
  React Native  <a href="#"><img src="./docs/react-native.png" height="15" /></a>
</h3>

```sh
yarn add react-native-bignumber
cd ios && pod install
```

<h3>
  Expo  <a href="#"><img src="./docs/expo.png" height="12" /></a>
</h3>

```sh
expo install react-native-bignumber
expo prebuild
```

## Usage

### ..as a normal library

The exposed `BN` class is used to create new BigNumber instances from strings (binary, hex, decimal), ArrayBuffers, Buffers, numbers, or other BigNumber instances.

```ts
import { BN } from 'react-native-bignumber'

const a = new BN(3274556)
const b = new BN(9856712)
const c = a.mul(b) // 32.276.355.419.872
```

Refer to [BN.js' documentation](https://github.com/indutny/bn.js/#instructions) for a full API reference and usage guide.

For example, this is how you calculate large Fibonacci numbers:

```ts
function fibonacci(n: number): BN {
  let prev = new BN(0)
  let prevPrev = new BN(1)
  let number = new BN(1)

  for (let i = 1; i < n; i++) {
    prevPrev = prev
    prev = number
    number = prevPrev.add(prev)
  }

  return number
}

const f = fibonacci(50) // 12.586.269.025
```

### ..as a drop-in replacement

Since popular libraries like [ethers.js](https://github.com/ethers-io/ethers.js/) or [elliptic](https://github.com/indutny/elliptic) use [BN.js](https://github.com/indutny/bn.js/) under the hood, react-native-bignumber exposes exactly the same API as [BN.js](https://github.com/indutny/bn.js/) so it can be used as a drop-in replacement and promises much greater speed at common crypto operations.

In your `babel.config.js`, add a module resolver to replace `bn.js` with `react-native-bignumber`:

```diff
+const path = require('path');

module.exports = {
  presets: ['module:metro-react-native-babel-preset'],
  plugins: [
+   [
+     'module-resolver',
+     {
+       alias: {
+         'bn.js': 'react-native-bignumber',
+       },
+     },
+   ],
    ...
  ],
};
```

Now, all imports for `bn.js` will be resolved as `react-native-bignumber` instead.

In the Exodus app, this single line change reduced app launch time by **4 seconds**! 🚀

## Community Discord

[Join the Margelo Community Discord](https://discord.gg/6CSHz2qAvA) to chat about react-native-bignumber or other Margelo libraries.

## Sponsors

<a href="https://exodus.com">
  <img align="right" src="docs/exodus.svg" height="65" alt="Exodus" />
</a>

This library is supported by [**Exodus**](https://exodus.com).
Send, receive, and exchange Bitcoin and 160+ cryptocurrencies with ease on the world's leading Desktop, Mobile and Hardware crypto wallets: [exodus.com](https://www.exodus.com/)

## Adopting at scale

react-native-bignumber was built at Margelo, an elite app development agency. For enterprise support or other business inquiries, contact us at <a href="mailto:hello@margelo.io?subject=Adopting react-native-bignumber at scale">hello@margelo.io</a>!
