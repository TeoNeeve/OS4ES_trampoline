#include "tpl_os.h"

int	TickerTime = 0;

void TickerCb_callback(void)
{
	TickerTime += 1;
}
