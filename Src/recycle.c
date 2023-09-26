/* Test */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>		/* unlink */
#include "merc.h"
#include <pthread.h>
#include "recycle.h"

/* stuff for recycling timed events */
EVENT_DATA *ev_free;

EVENT_DATA *new_event(void)
{
    static EVENT_DATA ev_zero;
    EVENT_DATA *ev;

    if ( ev_free == NULL )
       ev = alloc_perm(sizeof(*ev));
    else
    {
	ev = ev_free;
	ev_free = ev_free->next;
    }

    *ev = ev_zero;
    VALIDATE(ev);
    ev->to   = NULL;
    return ev;
}

void free_event(EVENT_DATA *ev)
{
    if (!IS_VALID(ev))
	return;

    INVALIDATE(ev);

    ev->next = ev_free;
    ev_free = ev;
}
