#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "kchash.h"
using namespace v8;
using namespace kyotocabinet;

Persistent <Function> KCHash::constructor;
std::map<std::string, HashDB*> KCHash::db;

KCHash::KCHash()   {
}

KCHash::~KCHash() {
    for (auto it: db)
    {
        it.second->close();
        delete it.second;
    }
}

void KCHash::Init(Handle<Object> exports) {
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("KCHash"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->PrototypeTemplate()->Set(String::NewSymbol("get"),
            FunctionTemplate::New(get)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("add"),
            FunctionTemplate::New(add)->GetFunction());
    constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("KCHash"), constructor);
}

Handle<Value> KCHash::New(const Arguments& args) {
    HandleScope scope;

    if (args.IsConstructCall()) {
        KCHash* obj = new KCHash();
        obj->Wrap(args.This());
        return args.This();
    } else {
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        return scope.Close(constructor->NewInstance(argc, argv));
    }
}

Handle<Value> KCHash::add(const Arguments& args) {
    HandleScope scope;
    String::Utf8Value param (args[0]->ToString());
    std::string dbname= std::string(*param);
    String::Utf8Value param2 (args[1]->ToString());
    std::string dbpath= std::string(*param2);

    db[dbname]=new HashDB();
    if ( !db[dbname] -> open(dbpath))
        return scope.Close(String::New(std::string("FAILED TO LOAD" + dbpath).c_str()));
    else return scope.Close(String::New(std::string("LOADED" + dbpath).c_str()));
}
Handle<Value> KCHash::get(const Arguments& args) {
    HandleScope scope;
    String::Utf8Value param0 (args[0]->ToString());
    std::string dbname = std::string(*param0);
    String::Utf8Value param (args[1]->ToString());
    std::string key = std::string(*param);
    std::string val;
    if (db.find(dbname)==db.end())
        return scope.Close(String::New(std::string("NA").c_str()));
    if (!db[dbname]->get(key, &val))
        return scope.Close(String::New(std::string("NA").c_str()));
    else
        return scope.Close(String::New(val.c_str()));
}

