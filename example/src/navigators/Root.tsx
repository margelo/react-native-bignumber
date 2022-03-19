import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import type { RootStackParamList } from './RootProps';

const Stack = createNativeStackNavigator<RootStackParamList>();

export function Root(): React.ReactNode {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen
          name="Entry"
          getComponent={() => {
            const { Entry } = require('./children/Entry/Entry');
            return Entry;
          }}
        />
        <Stack.Screen
          name="Benchmarks"
          getComponent={() => {
            const { Benchmarks } = require('./children/benchmarks/Benchmarks');
            return Benchmarks;
          }}
        />
        <Stack.Screen
          name="TestingScreen"
          getComponent={() => {
            const {
              TestingScreen,
            } = require('./children/TestingScreen/TestingScreen');
            return TestingScreen;
          }}
        />
      </Stack.Navigator>
    </NavigationContainer>
  );
}
