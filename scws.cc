#include <node.h>
#include <v8.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <node.h>
#include <scws/scws.h>
using namespace v8;
using namespace node;

class Scws: ObjectWrap {
  public :
    scws_t scws;
    Scws () {
      scws = scws_new();
      scws_set_charset(scws, "utf8");
    }
    ~Scws () {
      scws_free(scws);
    }
    static void Init();
    static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);
    static v8::Persistent<v8::Function> constructor;
    static v8::Handle<v8::Value> New(const v8::Arguments& args);

static Handle<Value> addDict(const Arguments& args){
  HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  Handle<Value> arg0 = args[0];
  String::Utf8Value dict(arg0);
  if (strstr(*dict, "xdb")) {
    scws_add_dict(scws->scws, *dict, SCWS_XDICT_MEM);
  } else {
    scws_add_dict(scws->scws, *dict, SCWS_XDICT_TXT);
  }
  return scope.Close(Undefined());
}

static Handle<Value> setDict(const Arguments& args){
  HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  Handle<Value> arg0 = args[0];
  String::Utf8Value dict(arg0);
  if (strstr(*dict, "xdb")) {
    scws_set_dict(scws->scws, *dict, SCWS_XDICT_MEM);
  } else {
    scws_set_dict(scws->scws, *dict, SCWS_XDICT_TXT);
  }
  return scope.Close(Undefined());
}

static Handle<Value> setMulti(const Arguments& args){
  HandleScope scope; 
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  int multi = args[0]->NumberValue();
  if (multi == 1) {
    scws_set_multi(scws->scws, SCWS_MULTI_SHORT);
  } else {
    scws_set_multi(scws->scws, 0);
  }
  return scope.Close(Undefined());
}

static Handle<Value> setRule(const Arguments& args){
  HandleScope scope;
  Scws *scws = ObjectWrap::Unwrap<Scws>(args.This());
  Handle<Value> arg0 = args[0];
  String::Utf8Value rule(arg0);
  scws_set_rule(scws->scws, *rule);
  return scope.Close(Undefined());
}

static Handle<Value> segment(const Arguments& args) {
    HandleScope scope;
    Scws *scwsObj = ObjectWrap::Unwrap<Scws>(args.This());
    scws_top_t res, cur;
    Handle<Value> arg0 = args[0];
    String::Utf8Value txt(arg0);
    int limit = args[1]->NumberValue();

    scws_t scws = scwsObj->scws;
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

static Handle<Value> serialize(const Arguments& args) {
  HandleScope scope;
  Scws *scwsObj = ObjectWrap::Unwrap<Scws>(args.This());
  scws_t scws = scwsObj->scws;
  scws_res_t res, cur;
  Handle<Value> arg0 = args[0];
  String::Utf8Value txt(arg0);
  int txtLen = strlen(*txt);
  scws_send_text(scws, *txt, txtLen);
  Local<Array> words = Array::New();
  int index = 0;
  while ((res = cur = scws_get_result(scws)))
  {
    while (cur != NULL)
    {
      Local<Object> word = Object::New();
      word->Set(String::NewSymbol("word"), String::New(*txt + cur->off, cur->len));
      if(cur->attr[1] == '\0'){
        word->Set(String::NewSymbol("attr"), String::New(cur->attr, 1));
      }else{
        word->Set(String::NewSymbol("attr"), String::New(cur->attr, 2));
      }
      word->Set(String::NewSymbol("idf"), Number::New(cur->idf));
      words->Set(index, word);
      //printf("WORD: %.*s/%s (IDF = %4.2f)\n", cur->len, text+cur->off, cur->attr, cur->idf);
      cur = cur->next;
      index ++;
      
    }
    scws_free_result(res);
    scws_free_result(cur);
  }
  return scope.Close(words);
}

};

Persistent<Function> Scws::constructor;

void Scws::Init() {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
  tpl->SetClassName(String::NewSymbol("MyObject"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor = Persistent<Function>::New(tpl->GetFunction());
}

Handle<Value> Scws::New(const Arguments& args) {
  HandleScope scope;
  Scws* obj = new Scws();
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> Scws::NewInstance(const Arguments& args) {
  HandleScope scope;
  Local<Object> instance = constructor->NewInstance();
  instance->Set(String::NewSymbol("setRule"), FunctionTemplate::New(setRule)->GetFunction());
  instance->Set(String::NewSymbol("setMulti"), FunctionTemplate::New(setMulti)->GetFunction());
  instance->Set(String::NewSymbol("setDict"), FunctionTemplate::New(setDict)->GetFunction());
  instance->Set(String::NewSymbol("addDict"), FunctionTemplate::New(addDict)->GetFunction());
  instance->Set(String::NewSymbol("segment"), FunctionTemplate::New(segment)->GetFunction());
  instance->Set(String::NewSymbol("serialize"), FunctionTemplate::New(serialize)->GetFunction());
  return scope.Close(instance);
}

Handle<Value> CreateObject(const Arguments& args) {
  HandleScope scope;
  return scope.Close(Scws::NewInstance(args));
}

void InitAll(Handle<Object> exports) {
  Scws::Init();
  exports->Set(String::NewSymbol("createWorker"),
      FunctionTemplate::New(CreateObject)->GetFunction());

}

NODE_MODULE(nscws, InitAll)
