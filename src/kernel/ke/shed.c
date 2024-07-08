#include <stdint.h>
#include <stdio.h>

typedef struct
{
    int r1;
    int ip;
    int sp;
} context;

typedef struct
{
    context ctx;
    int pri;
    int state;
    int mode;
} proc;

#define MAXPRI 127

#define PS_FREE 0
#define PS_RUNNABLE 1
#define PS_RUNNING 2

#define PM_SYS 0
#define PM_USR 1

#define NPROX 20

proc prox[NPROX];

proc* curproc;

context shedctx;

void switch_context(context* a, context* b)
{
    printf("%i -> %i\n", a->ip, b->ip);
}

proc* alloc_proc()
{
    for(int i = 0; i < NPROX; i++)
    {
        if(prox[i].state == PS_FREE)
            return &prox[i];
    }
    return NULL;
}

void add_proc(int pri, int pm)
{
    proc* aproc = alloc_proc();
    if(aproc == NULL)
        return;
    
    aproc->pri = pri;
    aproc->mode = pm;
    aproc->state = PS_RUNNABLE;
}

static int loopi = 0;
static int looppri = 0;

proc* nextproc()
{
    proc* p = NULL;
    for(;;)
    {
        p = &prox[loopi];
        
        if(--looppri < 0)
            looppri = MAXPRI;
        
        if(p->state == PS_RUNNABLE && p->pri >= looppri)
            break;
        
        loopi = (loopi + 1) % NPROX;
    }
    loopi = (loopi + 1) % NPROX;
    return p;
}

void shedule()
{
    for(;;)
    {
        proc* next = nextproc();
        
        curproc = next;
        curproc->state = PS_RUNNING;
        switch_context(&shedctx, &curproc->ctx);
        
        //TODO:delete
        curproc->state = PS_RUNNABLE;
        
        curproc = NULL;
    }
}

void yield()
{
    curproc->state = PS_RUNNABLE;
    switch_context(&curproc->ctx, &shedctx);
}

int main(int argc, char *argv[])
{
    for(int i = 0; i < NPROX; i++)
    {
        prox[i].ctx.ip = i;
    }
    shedctx.ip = 9999;
    add_proc(127, PM_SYS);
    add_proc(10, PM_USR);
    shedule();
}
