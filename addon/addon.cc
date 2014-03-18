#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "kchash.h"

using namespace v8;


void InitAll(Handle<Object> exports) {
      KCHash::Init(exports);
}

NODE_MODULE(addon, InitAll)
