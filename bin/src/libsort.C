#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

struct node
{
    char type;
    char value[32];
};

struct entry
{
    char filename[32];
    int count;
    struct node entry_list[100];
};

struct entry the_list[500];

int getword(char *w)
{
    int c;
    while(-1!=(c=getchar()) && isspace(c));
    if(c==-1) return -1;
    *w++=c;
    while(-1!=(c=getchar()) && !isspace(c))
      *w++=c;
    *w=0;
    return 0;
}

int compar(struct entry *e,struct entry *f)
{
    int e_on_f=0;
    int f_on_e=0;
    int i,j;
    for(i=0;i<e->count;i++)
       for(j=0;j<f->count;j++)
       {
          if(!strcmp( e->entry_list[i].value,f->entry_list[j].value ))
          {
             if(e->entry_list[i].type=='U'
               && f->entry_list[j].type=='D')
               {
                  e_on_f=1;
               }
             if(e->entry_list[i].type=='D'
               && f->entry_list[j].type=='U')
               {
                  f_on_e=1;
               }
          }
       }
    return f_on_e-e_on_f;
}

void swap(char *s,char *t,unsigned n)
{
    int i;
    char c;
    for(i=0;i<n;i++)
    {
        c=s[i];
        s[i]=t[i];
        t[i]=c;
    }
}


    int swapped;
void bsort(void *list,unsigned count,unsigned size,int (*compar)(void*,void*))
{
    int i,j,k=0;
    do
    {
        swapped=0;
    for(i=0;i<count;i++)
      for(j=i+1;j<count;j++)
      {
        if(compar((char*)list+i*size,(char*)list+j*size) > 0)
        {
          swap((char*)list+i*size,(char*)list+j*size,size);
          swapped=1;
        }
      }
    } while(swapped && ++k<100);
}

main()
{
    int i=0,j;
    int r;
    int count;
    char word[32];
    while(!getword(word))
    {
TOP:
        strcpy(the_list[i].filename,word);
        fprintf(stderr,"[%s]",word);
        j=0;
        while(!(r=getword(word)) && word[1]!=':')
        {
            the_list[i].entry_list[j].type=*word;
            getword(word);
            strcpy(the_list[i].entry_list[j].value,word);
            j++;
        }
        the_list[i++].count=j;
        if(!r) goto TOP;
    }
    count=i;

#if 0
    for(i=0;i<count;i++)
    {
        printf("%s depends on: ",the_list[i].filename);
        for(j=0;j<the_list[i].count;j++)
        {
            if(the_list[i].entry_list[j].type=='U')
               printf("%s ",the_list[i].entry_list[j].value);
        }
        printf("\n");
        printf(" and defines: ",the_list[i].filename);
        for(j=0;j<the_list[i].count;j++)
        {
            if(the_list[i].entry_list[j].type=='D')
               printf("%s ",the_list[i].entry_list[j].value);
        }
        printf("\n");
    }
#endif

    bsort(the_list,count,sizeof(*the_list),compar);
    for(i=0;i<count;i++)
      puts(the_list[i].filename);
    if(swapped)
    {
        fprintf(stderr,"Results not guaranteed.\n");
    }
    return 0;
}
