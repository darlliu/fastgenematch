#ifndef KCHASH_H
#define KCHASH_H
#include <node.h>
#include <string>
#include <map>
#include <iostream>
#include <kchashdb.h>
using namespace kyotocabinet;


class KCHash: public node::ObjectWrap {
    public:
        static void Init(v8::Handle<v8::Object> exports);

    private:
        explicit KCHash( );
        ~KCHash();

        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        static v8::Handle<v8::Value> get(const v8::Arguments& args);
        static v8::Handle<v8::Value> add(const v8::Arguments& args);
        static v8::Persistent<v8::Function> constructor;
        static std::map<std::string, HashDB*> db;
};

#endif
