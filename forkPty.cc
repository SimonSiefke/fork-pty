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

std::map<int, CallbackInfo> map;

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

  map[pid] = { cb, childpid:0, child_exit_code:0 };

  // create result object
  napi_value result, result_pid;
  napi_status status;
  status=napi_create_object(env, &result);
  assert(status==napi_ok);
  // set result.pid
  // napi_value result_pid;
  status=napi_create_int32(env, pid, &result_pid);
  assert(status==napi_ok);
  status=napi_set_named_property(env, result, "pid", result_pid);
  assert(status==napi_ok);
  // set result.fd
  napi_value result_fd;
  status=napi_create_int32(env, master, &result_fd);
  assert(status==napi_ok);
  status=napi_set_named_property(env, result, "fd", result_fd);
  assert(status==napi_ok);
  return result;
}

static void CallJs(napi_env env, napi_value js_cb, void* context, void* data) {
  CallbackInfo info = *(CallbackInfo*)data;
  if (env != NULL) {
    napi_status status;
    napi_value undefined;
    napi_value js_the_prime;
    status=napi_create_int32(env, info.childpid, &js_the_prime);
    assert(status==napi_ok);
    status=napi_get_undefined(env, &undefined);
    assert(status==napi_ok);
    status=napi_call_function(env, undefined, js_cb, 1, &js_the_prime, NULL);
    assert(status==napi_ok);
  }
  // free(data);
}

//  int the_prime;

// when child exits, call onExit function
void childHandler (int signum) {
  pid_t childpid;
  int childStatus;
  // printf("before first wait %d\n", childpid);
  childpid = waitpid(-1, &childStatus, WNOHANG);
  // printf("after first wait %d\n", childpid);
  // while ((childpid = waitpid(-1, &childStatus, WNOHANG)) > 0) {
  // printf("child done\n");
  // printf("count%d\n", count);
  // if(count==0){
  //   perror("should not happen");
  //   exit(1);
  // }
  // CallbackInfo info = map[childpid];
  // int* the_prime =(int*) malloc(sizeof(int));
  // int the_prime = 456;
  // printf("after malloc %d\n", the_prime);
  //   *the_prime = 456;
  // printf("after malloc%d\n", childpid);
  // if (WIFEXITED(childStatus)) {
  //   printf("PID %d exited normally.  Exit number:  %d\n", childpid, WEXITSTATUS(childStatus));
  // } else if (WIFSTOPPED(childStatus)) {
  //   printf("PID %d was stopped by %d\n", childpid, WSTOPSIG(childStatus));
  // } else if (WIFSIGNALED(childStatus)) {
  //   printf("PID %d exited due to signal %d\n.", childpid, WTERMSIG(childStatus));
  // } else {
  //   printf("else");
  //   perror("waitpid");
  // }
  // printf("in callback%d\n", childpid);
  CallbackInfo info = map[childpid];
  napi_threadsafe_function cb =info.cb;

  // if(cb==NULL){
  //   perror("cb cannot be null");
  // }
  map.erase(childpid);
  napi_status status;

  status = napi_call_threadsafe_function(cb, &info, napi_tsfn_nonblocking);
  // printf("called function\n");
  assert(status==napi_ok);
  // status=napi_release_threadsafe_function(cb, napi_tsfn_release);
  // assert(status==napi_ok);
  // }
};

napi_value ForkPtyAndExecvp(napi_env env, napi_callback_info info) {
  size_t argc = 3;
  napi_value args[3];
  napi_status status;
  status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  assert(status==napi_ok);

  // get string
  size_t str_size;
  status=napi_get_value_string_utf8(env, args[0], NULL, 0, &str_size);
  assert(status==napi_ok);
  char *file = (char*) malloc(str_size+1);
  status=napi_get_value_string_utf8(env, args[0], file, str_size+1, &str_size);
  assert(status==napi_ok);

  // get array length
  uint32_t array_length;
  status=napi_get_array_length(env, args[1], &array_length);
  assert(status==napi_ok);

  // get string array
  char *array[array_length+1];
  array[array_length] = NULL;
  napi_value element;
  for(u_int32_t i=0; i<array_length; ++i){
    status=napi_get_element(env, args[1], i, &element);
    assert(status==napi_ok);
    status=napi_get_value_string_utf8(env, element, NULL, 0, &str_size);
    assert(status==napi_ok);
    char *stringValue = (char*) malloc(str_size+1);
    status=napi_get_value_string_utf8(env, element, stringValue, str_size+1, &str_size);
    assert(status==napi_ok);
    array[i] = stringValue;
  }

  // get callback
  napi_threadsafe_function tsfn;
  napi_value work_name;
  status=napi_create_string_utf8(env, "Node-API Thread-safe Call from Async Work Item", NAPI_AUTO_LENGTH, &work_name);
  assert(status==napi_ok);
  status=napi_create_threadsafe_function(env, args[2], NULL, work_name, 0, 1, NULL, NULL, NULL, CallJs, &tsfn);
  assert(status==napi_ok);

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
