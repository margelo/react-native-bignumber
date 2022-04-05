//
// Created by Szymon on 24/02/2022.
//

#ifndef BigNumberEXAMPLE_SMARTHOSTOBJECT_H
#define BigNumberEXAMPLE_SMARTHOSTOBJECT_H

#include "MGThreadAwareHostObject.h"
#include "MGJSIMacros.h"
#include <ReactCommon/TurboModuleUtils.h>

namespace margelo
{

    namespace jsi = facebook::jsi;
    namespace react = facebook::react;

    typedef std::function<jsi::Value(jsi::Runtime &runtime)> JSIValueBuilder;

    typedef std::pair<std::string, JSIValueBuilder> FieldDefinition;

    class JSI_EXPORT MGSmartHostObject : public MGThreadAwareHostObject
    {

    public:
        MGSmartHostObject(std::shared_ptr<react::CallInvoker> jsCallInvoker,
                          std::shared_ptr<DispatchQueue::dispatch_queue> workerQueue)
            : MGThreadAwareHostObject(jsCallInvoker, workerQueue)
        {
        }

        virtual ~MGSmartHostObject()
        {
        }

        std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime &runtime);

        jsi::Value get(jsi::Runtime &runtime, const jsi::PropNameID &propNameId);

        std::vector<std::pair<std::string, JSIValueBuilder>> fields;
    };

} // namespace margelo

#endif // BigNumberEXAMPLE_SMARTHOSTOBJECT_H
