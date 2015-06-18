#include <ozmenu.h>
#include <oz.h>

int ozmenu(int _x,int _y,char *title,unsigned start,unsigned num_entries,
struct ozmenuentry *_menu, byte options)
{
    static byte max_width,w;
    static byte x,y;
    static byte adj;
    static byte menu_y_adj;
    static byte adj_numbered;
    static byte window_height,window_width;
    static byte num_per_window;
    static byte fontheight;
    static byte menu_x,menu_x_adj;
    static byte menu_y,menu_width,menu_height;
    static byte title_right;
    static byte fontnum;
    static unsigned offset,old_offset;
    static byte pos,old_pos;
    static unsigned i;
    static unsigned k;
    register struct ozmenuentry *menu=_menu;
    ozsavescreen();
    fontnum=ozgetfont();
    max_width=0;
    if(options&OZMENU_NUMBERED)
        adj_numbered=ozputs(0,-1,"4 ");
    else adj_numbered=0;
    if(_x<0 || _x>238-5 || _y<0 || _y>79-5 || !num_entries) return -2;
    x=_x;
    y=_y;
    fontheight=ozgetfontheight(ozgetfont());
    if(fontheight==8)
    {
        adj=1;
        fontheight=9;
    }
    else adj=0;
    if(y+fontheight*(num_entries+(title!=NULL))+5<80)
        num_per_window=num_entries+(title!=NULL);
    else
    {
        num_per_window=(80-5-y)/fontheight;
        if(num_per_window==0 || (title!=NULL && num_per_window==1)) return -2;
    }
    window_height=3+num_per_window*fontheight;
    for(i=0;i<num_entries;i++)
      if( (w=ozputs(1+adj_numbered,-1,menu[i].label)) > max_width )
         max_width=w;
    if(title!=NULL && (w=ozputs(0,-1,title)+5) > max_width)
         max_width=w;
    if(num_per_window<num_entries) max_width+=5;
    if(5+max_width+x>238) menu_width=238-5-x;
         else
             menu_width=max_width-1;
    window_width=menu_width+4;
    _ozbox(x,y,window_width+2,window_height+1);
    _ozfilledbox(x+1,y+1,window_width,window_height-1,WHITE);
    menu_x=x+2;
    if(title!=NULL)
    {
        title_right=ozputs(menu_x+4,y+2,title);
        for(i=0;i<fontheight;i+=2)
        {
            _ozhline(menu_x,y+2+i,3,BLACK);
            if(title_right<window_width+x-1)
              _ozhline(title_right,y+2+i,window_width+x-title_right,BLACK);
        }
        menu_y=y+2+fontheight;
        menu_height=window_height-3-fontheight;
        num_per_window--;
    }
    else
    {
        menu_y=y;
        menu_height=window_height-3;
    }
    menu_y_adj=menu_y+adj;
    menu_x_adj=menu_x+adj_numbered;
    menu_width=window_width-2;
    old_offset=0xFFFF;
    if(start>=num_entries)
        start=num_entries-1;
    if(start<num_per_window)
    {
        offset=0;
        pos=start;
    }
    else
    if(num_entries-start<num_per_window)
    {
        if(num_entries<=num_per_window)
        {
            offset=0;
            pos=start;
        }
        else
        {
            offset=num_entries-num_per_window;
            pos=start-offset;
        }
    }
    else
    {
        offset=start;
        pos=0;
    }
    for(;;)
    {
        if(old_offset!=offset)
        {
           _ozfilledbox(menu_x,menu_y,menu_width,menu_height,WHITE);
           for(i=0;i<num_per_window && offset+i<num_entries;i++)
           {
              ozputs(menu_x_adj+1,menu_y_adj+fontheight*i,menu[offset+i].label);
              if(options&OZMENU_NUMBERED)
                 ozputch(menu_x+1,menu_y_adj+fontheight*i,'1'+i);
           }
           if(offset>0)
           {
              ozfont(FONT_OZ_NORMAL);
              ozputch(menu_x+max_width-5,menu_y_adj,0x0A);
              ozfont(fontnum);
           }
           if(offset+num_per_window<num_entries)
           {
              ozfont(FONT_OZ_NORMAL);
              ozputch(menu_x+max_width-5,menu_y_adj+fontheight*(num_per_window)-9,0x0B);
              ozfont(fontnum);
           }
           old_offset=offset;
           old_pos=0xFF;
        }
        if(pos!=old_pos)
        {
           if(old_pos!=0xFF)
              _ozfilledbox(menu_x_adj,menu_y+old_pos*fontheight,menu_width-adj_numbered,fontheight,XOR);
           _ozfilledbox(menu_x_adj,menu_y+pos*fontheight,menu_width-adj_numbered,fontheight,XOR);
           old_pos=pos;
        }
        switch(k=ozgetch())
        {
            case KEY_LOWER_ESC:
            case KEY_UPPER_ESC:
                ozrestorescreen();
                return -1;
            case KEY_LOWER_ENTER:
            case KEY_UPPER_ENTER:
            case '\r':
                ozrestorescreen();
                return pos+offset;
            case KEY_DOWN:
                if(pos>=num_per_window-1)
                {
                    if(pos+offset>=num_entries-1)
                        break;
                    else
                        offset++;
                }
                else
                    pos++;
                break;
            case KEY_UP:
                if(pos)
                   pos--;
                else
                   if(offset) offset--;
                break;
            case KEY_PAGEUP:
                if(offset<num_per_window)
                {
                    if(offset+pos <= num_per_window)
                    {
                        offset=0;
                        pos=0;
                    }
                    else
                    {
                        pos=offset+pos-num_per_window;
                        offset=0;
                    }
                }
                else
                    offset-=num_per_window;
                break;
            case KEY_PAGEDOWN:
                if(offset+num_per_window>=num_entries)
                {
                    if(num_per_window>=num_entries)
                    {
                        offset=0;
                        pos=num_entries-1;
                    }
                    else
                    {
                        offset=num_entries-num_per_window;
                        pos=num_per_window-1;
                    }
                }
                else
                {
                    offset+=num_per_window;
                    if(num_per_window>num_entries-offset)
                        offset=num_entries-num_per_window;
                }
                break;
           case KEY_BACKLIGHT:
                if(!(options&OZMENU_NOLIGHT)) oztogglelight();
                break;
           case KEY_MYPROGRAMS:
           case KEY_MEMO:
           case KEY_CALENDAR:
           case KEY_MAIN:
                if(!(options&OZMENU_NOEXIT)) ozexitto(k);
           default:
                if((options&OZMENU_NUMBERED)
                && '1'<=k && '9'>=k)
                {
                    k-='1';
                    i=k+offset;
                    if(k<num_per_window && i<num_entries)
                    {
                        ozrestorescreen();
                        return i;
                    }
                    else
                        break;
                }
                for(i=0;i<num_entries;i++)
                    if(k==menu[i].key)
                    {
                        ozrestorescreen();
                        return i;
                    }
                break;
        }
    }
}

