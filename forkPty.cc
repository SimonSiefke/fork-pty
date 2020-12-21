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


char * get_string(napi_env env, napi_value element){
  size_t str_size;
  napi_status status;
  status = napi_get_value_string_utf8(env, element, NULL, 0, &str_size);
  assert(status == napi_ok);
  char *stringValue = (char*) malloc(str_size+1);
  status = napi_get_value_string_utf8(env, element, stringValue, str_size+1, &str_size);
  assert(status == napi_ok);
  return stringValue;
}

napi_value get_element(napi_env env, napi_value array, int i){
  napi_status status;
  napi_value element;
  status = napi_get_element(env, array, i, &element);
  assert(status == napi_ok);
  return element;
}

uint32_t get_array_length(napi_env env, napi_value value){
  napi_status status;
  uint32_t array_length;
  status = napi_get_array_length(env, value, &array_length);
  assert(status == napi_ok);
  return array_length;
}

char** get_string_array(napi_env env, napi_value value, uint32_t array_length){
  char **array = (char**) malloc(array_length+1);
  array[array_length] = NULL;
  for(u_int32_t i=0;i<array_length;++i){
    napi_value element = get_element(env, value, i);
    char *stringValue = get_string(env, element);
    array[i] = stringValue;
  }
  return array;
}


void free_string(char* string){
  free(string);
}

void free_string_array(char** array, uint32_t array_length){
  for(u_int32_t i=0;i<array_length;++i){
    free_string(array[i]);
  }
  free(array);
}

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
  napi_status status;
  size_t argc = 2;
  napi_value args[2];
  status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
  assert(status == napi_ok);

  char* file = get_string(env, args[0]);

  u_int32_t array_length = get_array_length(env, args[1]);
  char** array = get_string_array(env, args[1], array_length);

  int fd = forkpty_and_execvp(env,file,  array);

  free_string_array(array, array_length);
  free_string(file);

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
