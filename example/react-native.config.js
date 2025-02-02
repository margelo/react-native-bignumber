const path = require('path');

module.exports = {
  dependencies: {
    'react-native-bignumber': {
      root: path.resolve(__dirname, '..'), // Path to package root
      platforms: {
        ios: {
          podspecPath: path.resolve(
            __dirname,
            '../react-native-bignumber.podspec'
          ),
          configurations: ['Debug', 'Release'],
        },
        android: {
          sourceDir: path.resolve(__dirname, '../android'),
          packageImportPath: 'import com.margelo.bignumber.BigNumberPackage;',
          packageInstance: 'new BigNumberPackage()',
        },
      },
    },
  },
};
