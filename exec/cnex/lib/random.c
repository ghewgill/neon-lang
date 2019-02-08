#include "random.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cell.h"
#include "exec.h"
#include "number.h"
#include "stack.h"

void random_initModule()
{
    srand((unsigned int)time(NULL));
}

void random_uint32(TExecutor *exec)
{
    push(exec->stack, cell_fromNumber(number_from_uint32(rand())));
}
