#include "random.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"
#include "nstring.h"


void random_bytes(TExecutor *exec)
{
    size_t count = number_to_uint32(top(exec->stack)->number); pop(exec->stack);

    Cell *r = cell_createBytesCell(count);

    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        cell_freeCell(r);
        exec->rtl_raise(exec, "RandomException", "could not open /dev/urandom");
        return;
    }
    char *p = r->string->data;
    while (count > 0) {
        ssize_t n = read(fd, p, count);
        if (n < 1) {
            close(fd);
            cell_freeCell(r);
            exec->rtl_raise(exec, "RandomException", "error reading from /dev/urandom");
            return;
        }
        count -= n;
        p += n;
    }
    close(fd);

    push(exec->stack, r);
}
