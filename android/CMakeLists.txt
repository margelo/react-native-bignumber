project("react-native-bignumber")
cmake_minimum_required(VERSION 3.9.0)

set(PACKAGE_NAME "react-native-bignumber")
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Consume shared libraries and headers from prefabs
find_package(fbjni REQUIRED CONFIG)
find_package(ReactAndroid REQUIRED CONFIG)
find_package(openssl REQUIRED CONFIG)
find_library(LOG_LIB log)

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
        ${LOG_LIB}
        fbjni::fbjni
        ReactAndroid::jsi
        android
        openssl::crypto
        openssl::ssl
)

if (ReactAndroid_VERSION_MINOR GREATER_EQUAL 76)
    target_link_libraries(
            ${PACKAGE_NAME}
            ReactAndroid::reactnative
    )
else ()
    target_link_libraries(
            ${PACKAGE_NAME}
            ReactAndroid::reactnativejni
            ReactAndroid::turbomodulejsijni
            ReactAndroid::react_nativemodule_core
    )
endif ()