interface NativeBigNumberSpec {
    runAsync: () => Promise<number>;
}
declare global {
    function nativeCallSyncHook(): unknown;
    var __BigNumberProxy: object | undefined;
}
export declare const NativeBigNumber: NativeBigNumberSpec;
export {};
