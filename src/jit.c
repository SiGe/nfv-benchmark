#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "log.h"

#include "jit.h"

void jit_test_load(struct jit_t *jit, char const *name) {
    char buffer[256] = {0};

    // make the file
    log_info("Compiling the shared library.");
    sprintf(buffer, "make jit-test BENCHMARK=%s PROFILE=optimized 2>./logs/jit.gcc.err 1>./logs/jit.gcc.out", name);
    if (system(buffer) != 0) {
        return;
    }
    jit->handle = dlopen("./jit-test.so", RTLD_LAZY);

    if (jit->handle == 0) {
        log_err(dlerror());
        return;
    }
    log_info("Successfully loaded the shared library.");

    *(void **)(&jit->entry.test) = dlsym(jit->handle, "jit_test");
    char const* dlsym_err = dlerror();

    if (dlsym_err) {
        log_err(dlsym_err);
        jit_test_unload(jit);
        return;
    }

    jit->type = JIT_TEST;
}

void jit_test_unload(struct jit_t *jit) {
    if (jit->handle != 0) {
        dlclose(jit->handle);
        jit->handle = 0;
        jit->entry.test = 0;
        jit->type = JIT_NONE;
    }
}
