#import "BigNumberModule.h"

#import <React/RCTBridge+Private.h>
#import <React/RCTUtils.h>
#import <ReactCommon/RCTTurboModule.h>
#import <jsi/jsi.h>

#import "../cpp/BigNumberHostObject.h"

@implementation BigNumberModule

RCT_EXPORT_MODULE(BigNumber)

RCT_EXPORT_BLOCKING_SYNCHRONOUS_METHOD(install)
{
  NSLog(@"Installing JSI bindings for react-native-bignumber...");
  RCTBridge* bridge = [RCTBridge currentBridge];
  RCTCxxBridge* cxxBridge = (RCTCxxBridge*)bridge;
  if (cxxBridge == nil) {
    return @false;
  }

  using namespace facebook;

  auto jsiRuntime = (jsi::Runtime*) cxxBridge.runtime;
  if (jsiRuntime == nil) {
    return @false;
  }
  auto& runtime = *jsiRuntime;
  auto callInvoker = bridge.jsCallInvoker;

  auto workerQueue = std::make_shared<margelo::DispatchQueue::dispatch_queue>("Margelo BigNumber Thread");
  auto hostObject = std::make_shared<margelo::BigNumberHostObject>(callInvoker, workerQueue);
  auto object = jsi::Object::createFromHostObject(runtime, hostObject);
  runtime.global().setProperty(runtime, "__BigNumberProxy", std::move(object));

  NSLog(@"Successfully installed JSI bindings for react-native-bignumber!");
  return @true;
}

@end
