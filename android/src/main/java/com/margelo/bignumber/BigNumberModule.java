package com.margelo.bignumber;

import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.facebook.jni.HybridData;
import com.facebook.proguard.annotations.DoNotStrip;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl;
import com.facebook.react.turbomodule.core.interfaces.CallInvokerHolder;

@ReactModule(name = BigNumberModule.NAME)
public class BigNumberModule extends ReactContextBaseJavaModule {
  public static final String NAME = "BigNumber";

  @DoNotStrip
  private HybridData mHybridData;

  private native HybridData initHybrid();

  public BigNumberModule(ReactApplicationContext reactContext) {
    super(reactContext);
  }

  @NonNull
  @Override
  public String getName() {
    return NAME;
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  public boolean install() {
    try {
      if (mHybridData != null) {
        return false;
      }
      Log.i(NAME, "Loading C++ library...");
      System.loadLibrary("react-native-bignumber");

      JavaScriptContextHolder jsContext = getReactApplicationContext().getJavaScriptContextHolder();
      CallInvokerHolderImpl jsCallInvokerHolder = (CallInvokerHolderImpl) getReactApplicationContext()
                                                  .getCatalystInstance()
                                                  .getJSCallInvokerHolder();


      Log.i(NAME, "Installing JSI Bindings for react-native-bignumber...");
      mHybridData = initHybrid();
      nativeInstall(jsContext.get(), jsCallInvokerHolder);
      Log.i(NAME, "Successfully installed JSI Bindings for react-native-bignumber!");

      return true;
    } catch (Exception exception) {
      Log.e(NAME, "Failed to install JSI Bindings for react-native-bignumber!", exception);
      return false;
    }
  }

  public void destroy() {
    if (mHybridData == null) {
      return;
    }
    mHybridData.resetNative();
  }

  private native void nativeInstall(long jsiPtr, CallInvokerHolderImpl jsCallInvokerHolder);
}
