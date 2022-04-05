//
// Created by Szymon on 24/02/2022.
//

#include "MGSmartHostObject.h"

namespace margelo
{

    namespace jsi = facebook::jsi;
    namespace react = facebook::react;

    std::vector<jsi::PropNameID> MGSmartHostObject::getPropertyNames(
        jsi::Runtime &runtime)
    {
        std::vector<jsi::PropNameID> propertyNames;
        for (auto field : fields)
        {
            propertyNames.push_back(jsi::PropNameID::forAscii(runtime, field.first));
        }
        return propertyNames;
    }

    // TODO(Szymon) maybe add memoization here
    jsi::Value MGSmartHostObject::get(jsi::Runtime &runtime,
                                      const jsi::PropNameID &propNameId)
    {
        auto name = propNameId.utf8(runtime);
        for (auto field : fields)
        {
            auto fieldName = field.first;
            if (fieldName == name)
            {
                return (field.second)(runtime);
            }
        }
        return jsi::Value::undefined();
    }

} // namespace margelo