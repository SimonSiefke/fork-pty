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



// TODO https://github.com/Vortex375/node-taglib/pull/1/files

// void childHandler(int signum){
//   pid_t childpid;
//   int childStatus;

//   while ((childpid = waitpid( -1, &childStatus, WNOHANG)) > 0) {
//     if (WIFEXITED(childStatus)) {
//       printf("PID %d exited normally.  Exit number:  %d\n", childpid, WEXITSTATUS(childStatus));
//     } else if (WIFSTOPPED(childStatus)) {
//       printf("PID %d was stopped by %d\n", childpid, WSTOPSIG(childStatus));
//     } else if (WIFSIGNALED(childStatus)) {
//       printf("PID %d exited due to signal %d\n.", childpid, WTERMSIG(childStatus));
//     } else {
//       perror("waitpid");
//     }
//   }
// }

// TODO how to handle multiple calls?
struct ExitCallbackInfo {
  napi_threadsafe_function cb;
  napi_env env;
};

std::map<int, ExitCallbackInfo> map;


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

  map[pid] = {cb,env};

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






// This function is responsible for converting data coming in from the worker
// thread to napi_value items that can be passed into JavaScript, and for
// calling the JavaScript function.
static void CallJs(napi_env env, napi_value js_cb, void* context, void* data) {
  // This parameter is not used.
  (void) context;
  napi_status status;

  // Retrieve the prime from the item created by the worker thread.
  int the_prime = *(int*)data;

  // env and js_cb may both be NULL if Node.js is in its cleanup phase, and
  // items are left over from earlier thread-safe calls from the worker thread.
  // When env is NULL, we simply skip over the call into Javascript and free the
  // items.
  if (env != NULL) {
    napi_value undefined, js_the_prime;

    // Convert the integer to a napi_value.
    status = napi_create_int32(env, the_prime, &js_the_prime);
    assert(status == napi_ok);

    // Retrieve the JavaScript `undefined` value so we can use it as the `this`
    // value of the JavaScript function call.
    status = napi_get_undefined(env, &undefined);
    assert(status == napi_ok);

    // Call the JavaScript function and pass it the prime that the secondary
    // thread found.
    status = napi_call_function(env,
                              undefined,
                              js_cb,
                              1,
                              &js_the_prime,
                              NULL);
    assert(status == napi_ok);
  }

  // Free the item created by the worker thread.
  free(data);
}


// napi_env _env;
// napi_value _cb;

// when child exits, call onExit function
void childHandler (int signum) {
  pid_t childpid;
  int childStatus;


  // CallbackInfo info = map[getpid()];

  // if(info.env==NULL){
  //   perror("env is null\n");
  // } else {
  //   perror("env is not null\n");
  // }

  // // create result object
  // // napi_value result;
  // // napi_create_object(env, &result);
  // napi_status status;
  // napi_value global;
  // status = napi_get_global(info.env, &global);
  // assert(status == napi_ok);

  // napi_value result_pid;
  // status = napi_call_function(env, global, cb, 1, argv2, &result_pid);
  // assert(status == napi_ok);
  while ((childpid = waitpid( -1, &childStatus, WNOHANG)) > 0) {
    printf("child pid %d\n", childpid);
    ExitCallbackInfo info = map[childpid];
    napi_env env= info.env;

    napi_threadsafe_function cb = info.cb;
    napi_status status;
    // napi_value argv[1];


    napi_value result_pid;
    printf("create int\n");
    status = napi_create_int32(env, 1, &result_pid);
    assert(status==napi_ok);





// napi_status status;
    // status = napi_create_string_utf8(env, "hello world", NAPI_AUTO_LENGTH, argv);
    // assert(status == napi_ok);




    napi_value global;
    status = napi_get_global(env, &global);
    assert(status == napi_ok);

    napi_value result2;
    napi_value argv2[0];
    // sleep(1);
    printf("call fn\n");
    // status = napi_call_function(env, global, cb, 0, argv2, &result2);
    // assert(status == napi_ok);
     int* the_prime =(int*) malloc(sizeof(*the_prime));
      *the_prime = 1;

    status = napi_call_threadsafe_function(cb,
                                           the_prime,
                                           napi_tsfn_blocking);

  status = napi_release_threadsafe_function(cb, napi_tsfn_release);
  assert(status == napi_ok);

    //  napi_value argv2[0];
    // // status = napi_create_string_utf8(env, "hello world", NAPI_AUTO_LENGTH, argv);
    // // assert(status == napi_ok);

    // napi_value global;
    // status = napi_get_global(env, &global);
    // assert(status == napi_ok);

    // napi_value result2;
    // printf("call fn");
    // status = napi_call_function(env, global, cb, 0, argv2, &result2);
    // assert(status == napi_ok);

    // status = napi_call_function(NULL, NULL, cb, 1, argv, childpid);
    // assert(status == napi_ok);

    // printf("map result %d\n", map[childpid]);
    if (WIFEXITED(childStatus)) {
      printf("PID %d exited normally.  Exit number:  %d\n", childpid, WEXITSTATUS(childStatus));

      // // set result.code
      // napi_value result_code;
      // napi_create_int32(env, WEXITSTATUS(childStatus), &result_code);
      // napi_set_named_property(env, result, "code", result_code);

      // napi_value global;
      // status = napi_get_global(env, &global);
      // assert(status == napi_ok);
      // napi_value argv2[1];
      // status = napi_call_function(env, global, cb, 1, argv2, &result_pid);
      // assert(status == napi_ok);
      // // set result.fd

      // return result;
    } else if (WIFSTOPPED(childStatus)) {
      printf("PID %d was stopped by %d\n", childpid, WSTOPSIG(childStatus));
    } else if (WIFSIGNALED(childStatus)) {
      printf("PID %d exited due to signal %d\n.", childpid, WTERMSIG(childStatus));
    } else {
      perror("waitpid");
    }
  }
};

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
  for(u_int32_t i=0;i<array_length;++i){
    napi_get_element(env, args[1], i, &element);
    napi_get_value_string_utf8(env, element, NULL, 0, &str_size);
    char *stringValue = (char*) malloc(str_size+1);
    napi_get_value_string_utf8(env, element, stringValue, str_size+1, &str_size);
    array[i] = stringValue;
  }

  napi_value result_pid;
  napi_create_int32(env, 1, &result_pid);


  // napi_handle_scope scope = napi_handle_scope(env);


  // get callback
  napi_value cb = args[2];


  napi_value work_name;

  napi_status status;
     status = napi_create_string_utf8(env,
                                 "Node-API Thread-safe Call from Async Work Item",
                                 NAPI_AUTO_LENGTH,
                                 &work_name);
  assert(status == napi_ok);

  // napi_value js_cb;

  napi_threadsafe_function tsfn;

  // Convert the callback retrieved from JavaScript into a thread-safe function
  // which we can call from a worker thread.
  status = napi_create_threadsafe_function(env,
                                         cb,
                                         NULL,
                                         work_name,
                                         0,
                                         1,
                                         NULL,
                                         NULL,
                                         NULL,
                                         CallJs,
                                         &tsfn);

// napi_status status;
//  napi_value argv2[1];
//   status = napi_create_string_utf8(env, "hello world", NAPI_AUTO_LENGTH, argv2);
//   assert(status == napi_ok);

//   napi_value global;
//   status = napi_get_global(env, &global);
//   assert(status == napi_ok);

//   printf("sleep");
//   sleep(1);
//   napi_value result2;
//   status = napi_call_function(env, global, cb, 1, argv2, &result2);
//   assert(status == napi_ok);


// napi_status status;
//   napi_value argv2[0];
//     // status = napi_create_string_utf8(env, "hello world", NAPI_AUTO_LENGTH, argv);
//     // assert(status == napi_ok);

//     napi_value global;
//     status = napi_get_global(env, &global);
//     assert(status == napi_ok);

//     napi_value result2;
//     printf("call fn\n");
//     status = napi_call_function(env, global, cb, 0, argv2, &result2);
//     assert(status == napi_ok);
//     status = napi_call_function(env, global, cb, 0, argv2, &result2);
//     assert(status == napi_ok);

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
