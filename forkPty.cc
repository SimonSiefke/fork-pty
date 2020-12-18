#include <node_api.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
  for(int i=0;i<array_length;++i){
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
  for(int i=0;i<array_length;++i){
    free_string(array[i]);
  }
  free(array);
}

napi_value create_int32(napi_env env, int value){
  napi_status status;
  napi_value result;
  status = napi_create_int32(env, value, &result);
  return result;
}

int forkpty_and_execlp(napi_env &env, char** array, uint32_t array_length) {
  int master;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if(pid == -1){
    napi_throw_error(env, nullptr, "forkpty failed");
  }
  if(pid == 0){
    switch (array_length){
      case 1:
        execlp(array[0], NULL);
        break;
      case 2:
        execlp(array[0], array[1], NULL);
        break;
      case 3:
        execlp(array[0], array[1], array[2], NULL);
        break;
      case 4:
        execlp(array[0], array[1], array[2], array[3], NULL);
        break;
      case 5:
        execlp(array[0], array[1], array[2], array[3], array[4], NULL);
        break;
      case 6:
        execlp(array[0], array[1], array[2], array[3], array[4], array[5], NULL);
        break;
      case 7:
        execlp(array[0], array[1], array[2], array[3], array[4], array[5], array[6], NULL);
        break;
      case 8:
        execlp(array[0], array[1], array[2], array[3], array[4], array[5], array[6], array[8], NULL);
        break;
      case 9:
        execlp(array[0], array[1], array[2], array[3], array[4], array[5], array[6], array[8], array[9], NULL);
        break;
      case 10:
        execlp(array[0], array[1], array[2], array[3], array[4], array[5], array[6], array[8], array[9], array[10], NULL);
        break;
      default:
        napi_throw_error(env, nullptr, "invalid arguments");
        break;
    }
  }
  return master;
}

napi_value ForkPtyAndExeclp(napi_env env, napi_callback_info info) {
  napi_status status;

  // Get arguments length
  size_t array_length;
  status = napi_get_cb_info(env, info, &array_length, NULL, NULL, NULL);
  assert(status == napi_ok);

  // Get array of arguments
  napi_value *args = (napi_value *) malloc(array_length * sizeof(uintptr_t));
  status = napi_get_cb_info(env, info, &array_length, args, NULL, NULL);
  assert(status == napi_ok);

  // Convert to C array
  char *array[array_length];
  for (size_t i = 0; i < array_length; ++i) {
    array[i] = get_string(env, args[i]);
  }

  int fd = forkpty_and_execlp(env, array, array_length);
  return create_int32(env, fd);
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

  return create_int32(env, fd);
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor forkPtyAndExeclpDescriptor = DECLARE_NAPI_METHOD("forkPtyAndExeclp", ForkPtyAndExeclp);
  napi_define_properties(env, exports, 1, &forkPtyAndExeclpDescriptor);
  napi_property_descriptor forkPtyAndExecvpDescriptor = DECLARE_NAPI_METHOD("forkPtyAndExecvp", ForkPtyAndExecvp);
  napi_define_properties(env, exports, 1, &forkPtyAndExecvpDescriptor);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
