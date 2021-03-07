#include <node_api.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <map>
// #include <iostream>
// #include <cassert>

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

struct CallbackInfo {
  napi_threadsafe_function cb;
  pid_t childpid;
  int child_exit_code;
};

std::map<int, CallbackInfo*> map;

static void CallJs(napi_env env, napi_value js_cb, void* context, void* data) {
  CallbackInfo info = *(CallbackInfo*)data;
  printf("callback for child id%d\n", info.childpid);
  printf("exit for child id%d\n", info.child_exit_code);
  if (env != NULL) {
    napi_value undefined;
    napi_value js_the_prime;
    napi_status status;
    status=napi_create_int32(env, 456, &js_the_prime);
    assert(status==napi_ok);
    status=napi_get_undefined(env, &undefined);
    assert(status==napi_ok);
    status=napi_call_function(env, undefined, js_cb, 1, &js_the_prime, NULL);
    assert(status==napi_ok);
  }
  // map.erase(info.childpid);
  // free(data);
}

// when child exits, call onExit function
void childHandler (int signum) {
  pid_t childpid;
  int childStatus;
  childpid = waitpid(-1, &childStatus, WNOHANG);
  // printf("after first wait %d\n", childpid);
  // while ((childpid = waitpid(-1, &childStatus, WNOHANG)) > 0) {
  if (WIFEXITED(childStatus)) {
    printf("PID %d exited normally.  Exit number:  %d\n", childpid, WEXITSTATUS(childStatus));
  } else if (WIFSTOPPED(childStatus)) {
    printf("PID %d was stopped by %d\n", childpid, WSTOPSIG(childStatus));
  } else if (WIFSIGNALED(childStatus)) {
    printf("PID %d exited due to signal %d\n.", childpid, WTERMSIG(childStatus));
  } else {
    printf("else");
    perror("waitpid");
  }
  CallbackInfo* info = map[childpid];
  napi_threadsafe_function cb = info->cb;
  info->child_exit_code=123;
  napi_status status;
  printf("calling");
  status=napi_call_threadsafe_function(cb, info, napi_tsfn_nonblocking);
  assert(status==napi_ok);
}

napi_value forkpty_and_execvp(napi_env env, char* file,  char* argv[], napi_threadsafe_function cb) {
  int master;
  pid_t pid = forkpty(&master, NULL, NULL, NULL);
  if(pid == -1){
    napi_throw_error(env, NULL, "forkpty failed");
  }
  if(pid == 0){
    execvp(file, argv);
    perror("execvp failed");
    exit(1);
  }
  CallbackInfo * info = (CallbackInfo *) malloc(sizeof(CallbackInfo));
  *info = { cb, childpid:pid, child_exit_code:0 };
  map[pid] = info;

  // create result object
  napi_value result;
  napi_create_object(env, &result);
  // set result.pid
  napi_value result_pid;
  napi_create_int32(env, pid, &result_pid);
  napi_set_named_property(env, result, "pid", result_pid);
  // set result.fd
  napi_value result_fd;
  napi_create_int32(env, master, &result_fd);
  napi_set_named_property(env, result, "fd", result_fd);
  return result;
}

napi_value ForkPtyAndExecvp(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value args[3];
  napi_get_cb_info(env, info, &argc, args, NULL, NULL);

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
  for(u_int32_t i=0; i<array_length; ++i){
    napi_get_element(env, args[1], i, &element);
    napi_get_value_string_utf8(env, element, NULL, 0, &str_size);
    char *stringValue = (char*) malloc(str_size+1);
    napi_get_value_string_utf8(env, element, stringValue, str_size+1, &str_size);
    array[i] = stringValue;
  }

  // get callback
  napi_threadsafe_function tsfn;
  napi_value work_name;
  napi_create_string_utf8(env, "Node-API Thread-safe Call from Async Work Item", NAPI_AUTO_LENGTH, &work_name);
  napi_create_threadsafe_function(env, args[2], NULL, work_name, 0, 1, NULL, NULL, NULL, CallJs, &tsfn);

  // fork pty
  napi_value result = forkpty_and_execvp(env, file, array, tsfn);

  // free string array
  for(u_int32_t i=0;i<array_length;++i){
    free(array[i]);
  }

  // free string
  free(file);

  return result;
}

#define DECLARE_NAPI_METHOD(name, func)                                        \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor forkPtyAndExecvpDescriptor = DECLARE_NAPI_METHOD("forkPtyAndExecvp", ForkPtyAndExecvp);
  napi_define_properties(env, exports, 1, &forkPtyAndExecvpDescriptor);
  signal(SIGCHLD, childHandler);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
