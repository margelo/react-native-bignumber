project("react-native-bignumber")
cmake_minimum_required(VERSION 3.10.2)

set(PACKAGE_NAME "reactnativeBigNumber")
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_CXX_STANDARD 17)

# Consume shared libraries and headers from prefabs
find_package(fbjni REQUIRED CONFIG)
find_package(ReactAndroid REQUIRED CONFIG)
find_package(openssl REQUIRED CONFIG)
find_library(log-lib log)

include_directories(
        "../cpp"
        "../cpp/BigNumber"
        "../cpp/JSI Utils"
        "../cpp/Utils"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni/react/turbomodule"
        "${NODE_MODULES_DIR}/react-native/ReactCommon"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/turbomodule/core"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/react/nativemodule/core"
)

add_library(
        ${PACKAGE_NAME}
        SHARED
        "src/main/cpp/cpp-adapter.cpp"
        "../cpp/MGBigNumberHostObject.cpp"
        "../cpp/JSI Utils/TypedArray.cpp"
        "../cpp/Utils/MGDispatchQueue.cpp"
        "../cpp/JSI Utils/MGThreadAwareHostObject.cpp"
        "../cpp/JSI Utils/MGSmartHostObject.cpp"
        "../cpp/BigNumber/MGBigNumber.cpp"
        "../cpp/Utils/MGBigNumHelper.cpp"
        "../cpp/BigNumber/MGRedBigNum.cpp"
        "../cpp/BigNumber/MGModContext.cpp"
)

set_target_properties(
        ${PACKAGE_NAME}
        PROPERTIES
        CXX_STANDARD 17
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(
        ${PACKAGE_NAME}
        fbjni::fbjni
        ${log-lib}
        ReactAndroid::jsi
        ReactAndroid::reactnative
        android
        openssl::crypto
        openssl::ssl
)
