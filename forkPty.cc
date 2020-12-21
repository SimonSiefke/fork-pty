#include <node_api.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/* forkpty import is different dependeding on platform */
/* http://www.gnu.org/software/gnulib/manual/html_node/forkpty.html */
#if defined(__GLIBC__) || defined(__CYGWIN__)
#include <pty.h>
#elif defined(__APPLE__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <util.h>
#elif defined(__FreeBSD__)
#include <libutil.h>
#else
#include <pty.h>
#endif

int forkpty_and_execvp(napi_env &env, char* file,  char* argv[]) {
  int master;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if(pid == -1){
    napi_throw_error(env, nullptr, "forkpty failed");
  }
  if(pid == 0){
    execvp(file, argv);
  }
  return master;
}

napi_value ForkPtyAndExecvp(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2];
  napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

  // get string
  size_t str_size;
  napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  char *file = (char*) malloc(str_size+1);
  napi_get_value_string_utf8(env, args[0], file, str_size+1, &str_size);

  // get array length
  uint32_t array_length;
  napi_get_array_length(env, args[1], &array_length);

  // get string array
  char *array[array_length+1];
  array[array_length] = NULL;
  napi_value element;
  for(u_int32_t i=0;i<array_length;++i){
    napi_get_element(env, args[1], i, &element);
    napi_get_value_string_utf8(env, element, NULL, 0, &str_size);
    char *stringValue = (char*) malloc(str_size+1);
    napi_get_value_string_utf8(env, element, stringValue, str_size+1, &str_size);
    array[i] = stringValue;
  }

  int fd = forkpty_and_execvp(env,file,  array);

  // free string array
  for(u_int32_t i=0;i<array_length;++i){
    free(array[i]);
  }

  // free string
  free(file);

  napi_value result;
  napi_create_int32(env, fd, &result);
  return result;
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor forkPtyAndExecvpDescriptor = DECLARE_NAPI_METHOD("forkPtyAndExecvp", ForkPtyAndExecvp);
  napi_define_properties(env, exports, 1, &forkPtyAndExecvpDescriptor);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
