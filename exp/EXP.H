#define BCD_DIGITS 14
typedef struct bcd_struct
{
    byte digits[BCD_DIGITS/2];
    char decimalpoint;
} bcd;

struct expense_entry_head
{
    char type_id[4]; /* "EXPE" */
    byte header[9];  /* unknown */
    byte category;   /* 2=BUSINESS, 3=user 3 */
    byte zero1;      /* zero */
    char year[4];
    char month[2];
    char day[2];
    byte zero2;      /* zero terminator for date */
    bcd amount;      /* five byte BCD */
    byte zero4;
    char payment_type[14];  /* space filled on right; credit card, */
                            /* "cash", etc. */
    byte zero5;      /* zero terminator for payment_type */
};

/* Next a zero terminated memo follows, variable length
and then after the zero termination of the memo, we have
one byte that gives the submission status (3=SUBMITTED, 4=NONE) */
/* And finally a zero, marking the end of the record */
