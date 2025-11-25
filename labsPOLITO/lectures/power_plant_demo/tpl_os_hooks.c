#include "tpl_os.h"

int	my_time = 0;

void TickerCb_callback(void)
{
	my_time += 1;
}

