#include <node.h>
#include <nan.h>
#include <objidl.h>
#define NOMINMAX
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <algorithm>
namespace Gdiplus
{
  using std::min;
  using std::max;
};
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

struct myBuffer {
	LONGLONG size;/*buf size*/
	char* data;
};

#include "saveiconaspng.cc"

namespace demo {

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Number;
using v8::Value;

HICON getIconHandle(LPCWSTR filepath, UINT sizeFlg){
//from https://msdn.microsoft.com/ja-jp/library/windows/desktop/bb762179(v=vs.85).aspx
  CoInitialize(0);
  SHFILEINFOW sfi = {0};
  HRESULT hr = (HRESULT)SHGetFileInfoW(
    filepath,
    0,
    &sfi,
    sizeof(sfi),
    SHGFI_ICON | sizeFlg);
  if (SUCCEEDED(hr)){
    return sfi.hIcon;
  } 
  return NULL;
}
void GetFileIcon(const Nan::FunctionCallbackInfo<Value>& args, UINT sizeFlg);
  
NAN_METHOD(GetLargeIcon){
  GetFileIcon(info, SHGFI_LARGEICON);
}

NAN_METHOD(GetSmallIcon){
  GetFileIcon(info, SHGFI_SMALLICON);
}
  
void GetFileIcon(const Nan::FunctionCallbackInfo<Value>& args, UINT sizeFlg ) {
  Isolate* isolate = args.GetIsolate();
  LPCWSTR tg_item_path = (LPCWSTR)*v8::String::Value(args[0]->ToString());
  HICON hIcon = getIconHandle(tg_item_path, sizeFlg);
  if(hIcon == NULL){
     args.GetIsolate()->ThrowException(
      args[0]);
    return;
  }
  myBuffer myb;
  myb = {0,0};
  saveFileIconAsPng(hIcon, &myb);
  args.GetReturnValue().Set(v8::Uint8Array::New(
    v8::ArrayBuffer::New(
      isolate, 
      myb.data,
      (size_t)myb.size
    )
    ,0
    , (size_t)myb.size
  ));
}

NAN_MODULE_INIT(InitAll){
  Nan::Set(target, Nan::New<String>("getLargeIcon").ToLocalChecked(),
          Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetLargeIcon)).ToLocalChecked());
  Nan::Set(target, Nan::New<String>("getSmallIcon").ToLocalChecked(),
          Nan::GetFunction(Nan::New<v8::FunctionTemplate>(GetSmallIcon)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)

}  // namespace demo