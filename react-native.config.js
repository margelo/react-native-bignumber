module.exports = {
  dependencies: {
    'react-native-bignumber': {
      platforms: {
        ios: {
          podspecPath: './react-native-bignumber.podspec',
        },
        android: {
          sourceDir: './android',
          packageImportPath: 'import com.margelo.bignumber.BigNumberPackage;',
          packageInstance: 'new BigNumberPackage()',
        },
      },
    },
  },
};
