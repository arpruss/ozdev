#include <oz.h>

extern byte _ozprofiler;

static unsigned saved_lcr,saved_baud;

void ozinitprofiler(void)
{
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD19200);
    _ozprofiler=1;
}

void ozstopprofiler(void)
{
    if(_ozprofiler)
    {
        ozsetlcr(saved_lcr);
        ozsetbaud(saved_baud);
        _ozprofiler=0;
    }
}
