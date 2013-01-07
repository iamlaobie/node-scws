#include <node.h>
#include <v8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <node.h>
#include <scws/scws.h>
using namespace v8;

scws_t scws;

Handle<Value> addDict(const Arguments& args){
  HandleScope scope;
  Handle<Value> arg0 = args[0];
  String::Utf8Value dict(arg0);
  scws_add_dict(scws, *dict, SCWS_XDICT_MEM);
  return scope.Close(Undefined());
}

Handle<Value> setDict(const Arguments& args){
  HandleScope scope;
  Handle<Value> arg0 = args[0];
  String::Utf8Value dict(arg0);
  scws_set_dict(scws, *dict, SCWS_XDICT_MEM);
  return scope.Close(Undefined());
}

Handle<Value> setMulti(const Arguments& args){
  HandleScope scope;
  int multi = args[0]->NumberValue();
  if (multi == 1) {
    scws_set_multi(scws, SCWS_MULTI_SHORT);
  } else {
    scws_set_multi(scws, 0);
  }
  return scope.Close(Undefined());
}

Handle<Value> segment(const Arguments& args) {
    HandleScope scope;
    scws_top_t res, cur;
    Handle<Value> arg0 = args[0];
    String::Utf8Value txt(arg0);
    int limit = args[1]->NumberValue();

    int txtLen = strlen(*txt);
    scws_send_text(scws, *txt, txtLen);
    Local<Array> tops = Array::New();
    int index = 0;

    cur = res = scws_get_tops(scws, limit, NULL);
    while(cur != NULL){
        Local<Object> objWord = Object::New();
        objWord->Set(String::NewSymbol("word"), String::New(cur->word));
        objWord->Set(String::NewSymbol("weight"), Number::New(cur->weight));
        objWord->Set(String::NewSymbol("times"), Integer::New(cur->times));
        if(cur->attr[1] == '\0'){
            objWord->Set(String::NewSymbol("attr"), String::New(cur->attr, 1));
        }else{
            objWord->Set(String::NewSymbol("attr"), String::New(cur->attr, 2));
        }
        cur = cur->next;
        tops->Set(index, objWord);
        index += 1;
    }
    scws_free_tops(res);
    scws_free_tops(cur);
    return scope.Close(tops);
}

void Init(Handle<Object> target) {
  if (!(scws = scws_new())) {
      ThrowException(Exception::TypeError(String::New("initial failure")));
  }
  scws_set_charset(scws, "utf8");
  target->Set(String::NewSymbol("addDict"),
      FunctionTemplate::New(addDict)->GetFunction());
  target->Set(String::NewSymbol("setDict"),
      FunctionTemplate::New(setDict)->GetFunction());
  target->Set(String::NewSymbol("setMulti"),
      FunctionTemplate::New(setMulti)->GetFunction());
  target->Set(String::NewSymbol("segment"),                                                                                                                                                        
      FunctionTemplate::New(segment)->GetFunction());                                                                                                                                        
}
NODE_MODULE(nscws, Init)  
