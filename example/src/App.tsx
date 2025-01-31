import * as React from 'react';
import { Root } from './navigators/Root';

// @ts-expect-error
const isNewArchitectureEnabled = Boolean(global?.nativeFabricUIManager);
console.log('New Architecture:', isNewArchitectureEnabled);

export default function App() {
  return <Root />;
}
