#include <node_api.h>
#include <pty.h>
#include <unistd.h>

int do_forkpty(napi_env &env) {
  int master;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if(pid == -1){
    napi_throw_error(env, nullptr, "forkpty failed");
  }
  if(pid == 0){
    execlp("bash", "-i", NULL);
  }
  return master;
}

napi_value Add(napi_env env, napi_callback_info info) {
  int fd = do_forkpty(env);
  napi_value sum;
  napi_create_int32(env, fd, &sum);
  return sum;
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor addDescriptor = DECLARE_NAPI_METHOD("add", Add);
  napi_define_properties(env, exports, 1, &addDescriptor);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
