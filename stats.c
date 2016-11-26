#include <stats.h>
#include <utils.h>

void update_stats(unsigned long *v, unsigned long *elapsed, int b)
{
  unsigned long current_ticks;
  
  current_ticks=get_ticks();
  
  *v += current_ticks - *elapsed;
  
  if (!b) *elapsed=current_ticks;
  
}
