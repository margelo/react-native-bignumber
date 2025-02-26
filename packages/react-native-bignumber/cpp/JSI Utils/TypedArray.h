//
//  TypedArray.h
//  react-native-bignumber
//
//  Created by Marc Rousavy on 31.10.21.
//  Originally created by Expo (expo-gl)
//

#pragma once

#include <jsi/jsi.h>

namespace jsi = facebook::jsi;

enum class TypedArrayKind
{
    Int8Array,
    Int16Array,
    Int32Array,
    Uint8Array,
    Uint8ClampedArray,
    Uint16Array,
    Uint32Array,
    Float32Array,
    Float64Array,
};

template <TypedArrayKind T>
class TypedArray;

template <TypedArrayKind T>
struct typedArrayTypeMap;
template <>
struct typedArrayTypeMap<TypedArrayKind::Int8Array>
{
    typedef int8_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Int16Array>
{
    typedef int16_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Int32Array>
{
    typedef int32_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Uint8Array>
{
    typedef uint8_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Uint8ClampedArray>
{
    typedef uint8_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Uint16Array>
{
    typedef uint16_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Uint32Array>
{
    typedef uint32_t type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Float32Array>
{
    typedef float type;
};
template <>
struct typedArrayTypeMap<TypedArrayKind::Float64Array>
{
    typedef double type;
};

void invalidateJsiPropNameIDCache();

class TypedArrayBase : public jsi::Object
{
public:
    template <TypedArrayKind T>
    using ContentType = typename typedArrayTypeMap<T>::type;

    TypedArrayBase(jsi::Runtime &, size_t, TypedArrayKind);
    TypedArrayBase(jsi::Runtime &, const jsi::Object &);
    TypedArrayBase(TypedArrayBase &&) = default;
    TypedArrayBase &operator=(TypedArrayBase &&) = default;

    TypedArrayKind getKind(jsi::Runtime &runtime) const;

    template <TypedArrayKind T>
    TypedArray<T> get(jsi::Runtime &runtime) const &;
    template <TypedArrayKind T>
    TypedArray<T> get(jsi::Runtime &runtime) &&;
    template <TypedArrayKind T>
    TypedArray<T> as(jsi::Runtime &runtime) const &;
    template <TypedArrayKind T>
    TypedArray<T> as(jsi::Runtime &runtime) &&;

    size_t size(jsi::Runtime &runtime) const;
    size_t length(jsi::Runtime &runtime) const;
    size_t byteLength(jsi::Runtime &runtime) const;
    size_t byteOffset(jsi::Runtime &runtime) const;
    bool hasBuffer(jsi::Runtime &runtime) const;

    std::vector<uint8_t> toVector(jsi::Runtime &runtime);
    jsi::ArrayBuffer getBuffer(jsi::Runtime &runtime) const;

private:
    template <TypedArrayKind>
    friend class TypedArray;
};

bool isTypedArray(jsi::Runtime &runtime, const jsi::Object &jsObj);
TypedArrayBase getTypedArray(jsi::Runtime &runtime, const jsi::Object &jsObj);

std::vector<uint8_t> arrayBufferToVector(jsi::Runtime &runtime, jsi::Object &jsObj);
void arrayBufferUpdate(
    jsi::Runtime &runtime,
    jsi::ArrayBuffer &buffer,
    std::vector<uint8_t> data,
    size_t offset);

template <TypedArrayKind T>
class TypedArray : public TypedArrayBase
{
public:
    TypedArray(jsi::Runtime &runtime, size_t size);
    TypedArray(jsi::Runtime &runtime, std::vector<ContentType<T>> data);
    TypedArray(TypedArrayBase &&base);
    TypedArray(TypedArray &&) = default;
    TypedArray &operator=(TypedArray &&) = default;

    std::vector<ContentType<T>> toVector(jsi::Runtime &runtime);
    void update(jsi::Runtime &runtime, const std::vector<ContentType<T>> &data);
};

template <TypedArrayKind T>
TypedArray<T> TypedArrayBase::get(jsi::Runtime &runtime) const &
{
    assert(getKind(runtime) == T);
    (void)runtime; // when assert is disabled we need to mark this as used
    return TypedArray<T>(jsi::Value(runtime, jsi::Value(runtime, *this).asObject(runtime)));
}

template <TypedArrayKind T>
TypedArray<T> TypedArrayBase::get(jsi::Runtime &runtime) &&
{
    assert(getKind(runtime) == T);
    (void)runtime; // when assert is disabled we need to mark this as used
    return TypedArray<T>(std::move(*this));
}

template <TypedArrayKind T>
TypedArray<T> TypedArrayBase::as(jsi::Runtime &runtime) const &
{
    if (getKind(runtime) != T)
    {
        throw jsi::JSError(runtime, "Object is not a TypedArray");
    }
    return get<T>(runtime);
}

template <TypedArrayKind T>
TypedArray<T> TypedArrayBase::as(jsi::Runtime &runtime) &&
{
    if (getKind(runtime) != T)
    {
        throw jsi::JSError(runtime, "Object is not a TypedArray");
    }
    return std::move(*this).get<T>(runtime);
}
