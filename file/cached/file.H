#define MAX_WRITEBUFFERLEN 1984
#define MAX_READBUFFERLEN 62
#define MAX_FILES 4

typedef struct
{
    unsigned part_number;
    char header_id; /* 'c' */
    byte last_part;
    unsigned internal_file_id;
    char header_other[7];
    char fake_date[9];
    char fake_time[9];
    char fake_alarm[5];
    char filename[MAX_FILENAMELEN+1];
    unsigned file_id;
} cfile_header_t;

#define O_RDONLY 1
#define O_WRONLY 2


typedef struct
{
    char *buffer;
    unsigned owner_id;
    unsigned file_id;
    unsigned cur_partoffset;
    unsigned cur_clusteroffset;
    unsigned cur_part;
    unsigned part_length;
    unsigned first_part_length;
    unsigned loc;
    unsigned startloc;
    byte last_part;
    byte last_cluster_of_part;
    byte mode;
    long curpos;
    char filename[MAX_FILENAMELEN+1];
} ozfile_t;

#define CFILE_ID (0x80 | 'c')

extern enum ozfile_errornumbers
{
    FILE_NOT_FOUND, OUT_OF_MEMORY, NO_SUCH_HANDLE
} _ozerrno;

unsigned int _ozhashpjw(char *s);
