import type React from 'react';
import type { RootStackParamList } from '../../RootProps';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';

type BenchmarksProps = NativeStackScreenProps<RootStackParamList, 'Benchmarks'>;

export const Benchmarks: React.FC<BenchmarksProps> = () => null;
