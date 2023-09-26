/* ---
Recycle.h
--- */

#define IS_VALID(data)		((data) != NULL && (data)->valid)
#define VALIDATE(data)		((data)->valid = TRUE)
#define INVALIDATE(data)	((data)->valid = FALSE)

/* timed event recycling */
#define ED EVENT_DATA
ED *new_event args( (void) );
void free_event args( (ED *ev) );
#undef ED
