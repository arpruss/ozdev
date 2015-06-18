#define size_t unsigned

void bsort(void *basep, size_t nelems, size_t _size,
                            int (*comp)( void *,  void *))
{
    static char *iptr;
    register char *jptr;
    static char *end;
    static unsigned size;
    size=_size;
    if(nelems<2) return;
    end=(char*)basep+(nelems-1)*size;
    for(iptr=basep;iptr<=end;iptr+=size)
        for(jptr=end;jptr>iptr;jptr-=size)
            if((*comp)(iptr,jptr)>0) swap_chars(iptr,jptr,size);
}

