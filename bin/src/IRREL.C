/* irrel - remove irrelevant prototypes		Author: Andy Tanenbaum
 * Copyright 2002, Prentice Hall, see <http://www.cs.vu.nl/pub/minix/LICENSE>
 */

/* Modified for use with wizard SDK compiler enviroment (L.C. 01/02/02) */

/* If a program includes a large number of header files, the compiler may
 * run out of table space.  The solution is to invoke this program on the
 * output of cpp, before it goes into cem.  It removes irrelevant protos.
 */

/* define DOS to compile in DOS/Windows environments */

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0		/* DOS/Windows only */
#endif

#ifndef DOS
#define MAX_CHUNK 25600		/* buffer size */
#define RESERVE     200		/* refill the buffer when it gets this low */
#else /* DOS */
/* Increased MAX_CHUNK and RESERVE (L.C. 01/02/02) */
#define MAX_CHUNK 40000		/* buffer size */
#define RESERVE    2000		/* refill the buffer when it gets this low */
#endif /* DOS */
#define HASH_SLOTS 4096		/* number of identifiers allowed */
#define HASH_SIZE 15000		/* hash table size */
#define MAX_ID       64		/* max identifier size */
#define HASH_NR     137		/* constant used for hashing */
#define HULL (struct hash *) 0

#define SINGLE_QUOTE '\''	/* apostrophe */
#define DOUBLE_QUOTE '\"'	/* quotation marks */
#define BACKSL       '\\'	/* backslash char */
#define isidchar(c) (isalnum(c) || c == '_')

static char buf[MAX_CHUNK + 1];	/* input buffer */

static struct hash {
  char *h_name;			/* pointers to the names */
  char h_count;			/* # times name occurs (0, 1, or many (2)) */
  char h_def;			/* function definition seen */
} hash[HASH_SLOTS];

static char hash_names[HASH_SIZE];	/* hashed names */
static char *next_hash = hash_names;	/* next free byte in hash table */
static char *curp = buf;	/* ptr to current token */
static char *prevp;		/* ptr to previous token */
static char *namep;		/* ptr to token before last '(' */
static char *semip;		/* ptr to previous semicolon or } */
static char *endp;		/* ptr to 1 beyond end of data */

static int depth = 0;		/* depth of nesting in curly brackets */
static int body = 0;		/* inside function body or initializer? */
static int entries;		/* number of hash table slots in use */
static off_t ord;		/* file offset of buf[0] */
static int eof_seen;		/* flag set whenb end of file read */
static int fd;			/* file descriptor for reading input */
static int fd2;			/* file descriptor for erasing prototypes */
static int mflag;		/* max memory conservation flag */

static struct hash *present(char *);
static void next(void);
static void enter(char *s);
static void pass(int passno);
static struct hash *present(char *s);
static int hashval(char *s);
static void delete(char *p, int count, int kr);

int main(int argc, char *argv[])
{
  if (argc == 3 && strcmp(argv[1], "-m") == 0) {
	mflag = 1;
	argc--;
	argv++;
  }

  if (argc != 2) {
	fprintf(stderr, "Usage: irrel arg\n");
	exit(1);
  }

  /* Read in as much of the file as fits. */
  fd = open(argv[1], O_RDONLY|O_BINARY);
  fd2 = open(argv[1], O_WRONLY|O_BINARY);
  if (fd < 0 || fd2 < 0) {
	fprintf(stderr, "irrel: Cannot open %s\n", argv[1]);
	exit(1);
  }

#if XXX
  count = read(fd, buf, MAX_CHUNK);
  if (count != MAX_CHUNK) eof_seen = 1;		/* argv[1] must be DISK file */
  if (count < 0) {
	fprintf(stderr, "irrel: Unable to read output of cpp\n");
	exit(1);
  }
#endif

  /* Initialize. */
  enter("auto");
  enter("char");
  enter("const");
  enter("double");
  enter("enum");
  enter("extern");
  enter("float");
  enter("int");
  enter("long");
  enter("register");
  enter("short");
  enter("signed");
  enter("static");
  enter("struct");
  enter("typedef");
  enter("union");
  enter("unsigned");
  enter("void");
  enter("volatile");

#if XXX
  endp = &buf[count];
  namep = buf;
  curp = buf;
  semip = buf;
  *endp = '\0';
  while (isspace(*curp)) curp++;	/* skip initial white space */
#endif

  pass(1);
  pass(2);
  return 0;
}

static void next(void)
{
/* Advance curp to the next token.  Tokens are identifiers, numbers, strings,
 * character literals like 'c', and punctuation marks (== is a sequence of two
 * tokens).  The string 14.6e+7 is a sequence of 6 tokens.  Good enough for
 * government work.
 */

  int n, count, k, reloc;
  char c, *smallp;

  prevp = curp;
  c = *curp;

  if (*curp == '\0') return;
  if (*curp == ';' && depth == 0) semip = curp;
  if (*curp == '}' && depth == 0) semip = curp;
  if (body) semip = curp;
  if (namep < semip) namep= semip;   /* Don't let namep linger behind semip. */


  if (isidchar(c)) {
	/* Skip identifier. */
	while(isidchar(*curp)) curp++;
  } else if (c == DOUBLE_QUOTE || c == SINGLE_QUOTE) {
	/* Skip string, including quoted quotes. */
	char stopc = c;
	curp++;
	while (*curp != '\0' && *curp != stopc) {
		if (*curp++ == BACKSL) {
			curp++;
		}
	}
	curp++;
  } else {
	/* Must be punctuation or white space. */
	curp++;
  }

  /* Skip white space. */
  while (isspace(*curp)) curp++;

  /* Check to see if the end of buffer is approaching. */
  if (curp > endp) {
	fprintf(stderr, "Buffer error.  Ran off the end.  Sorry.\n");
	exit(1);
  }

  n = endp - curp;	/* # chars left */
  if ( n > RESERVE || eof_seen) return;	/* no need to read more */

  /* Move unused data near the end of the file to the top, then read in
   * more.  Several pointers into the data might be valid.  Be careful
   * to keep them all valid, then relocate them.
   */
  smallp = curp;
  if (prevp < smallp) smallp = prevp;
  if (semip < smallp) smallp = semip;
  if (smallp == buf) {
	fprintf(stderr, "Internal error. Procedure is too large\n");
	abort();
	exit(1);
  }
  k = endp - smallp; 		/* # of residual bytes in buf to move */
  reloc = smallp - buf;

  /* We need to read in some more data from the file. */
  ord += reloc;
  memmove(buf, smallp, k);
  endp = &buf[k];
  count = read(fd, endp, MAX_CHUNK - k);
  if (count < MAX_CHUNK - k) eof_seen = 1;
  if (count > 0) endp += count;
  *endp = '\0';
  curp  -= reloc;
  prevp -= reloc;
  namep -= reloc;
  semip -= reloc;
  return;
}

static void enter(char *s)
{
/* Enter name in the symbol table. */

  char *p;
  register struct hash *hp;
  int h;

  /* See if the name is already present. */
  if ( (hp = present(s)) != HULL) {
	/* The name is already present in the table.  Increment counter. */
	if (hp->h_count < 2) hp->h_count++;
	return;
  }

  /* The name is not present.  Hash and enter it. */
  if (entries == HASH_SLOTS || next_hash > &hash_names[HASH_SIZE] - MAX_ID) {
	fprintf(stderr, "Hash table overflow\n");
	exit(1);
  }
  h = hashval(s);
  hp = &hash[h];
  while (hp->h_name != NULL) {
	hp++;
	if (hp == &hash[HASH_SLOTS]) hp = &hash[0];
  }
  hp->h_name = next_hash;
  hp->h_count = 1;
  hp->h_def = 0;
  entries++;
  p = s;
  while (isidchar(*p)) *next_hash++ = *p++;
  *next_hash++ = '\0';
}

static void pass(int passno)
{
/* Pass 1: Scan for identifiers and enter them in the hash table.
 * Pass 2: Scan for ')' at depth 0.  All header prototypes end this way,
 *         although other statements can also end this way.  Wipe them if
 *         you can.
 */

  int n, len;
  char *p;
  struct hash *pp;

  /* Reset everything to make another pass over the input. */
  lseek(fd, 0L, SEEK_SET);	/* rewind input file */
  depth = 0;
  ord = 0;
  curp = &buf[0];
  prevp =curp;
  namep = curp;
  semip = curp - 1;
  eof_seen = 0;

  n = read(fd, buf, MAX_CHUNK);
  if (n != MAX_CHUNK) eof_seen = 1;		/* argv[1] must be DISK file */
  if (n < 0) {
	fprintf(stderr, "irrel: Could not read input file on pass 2\n");
	exit(1);
  }
  endp = &buf[n];
  *endp = '\0';

  /* Scan file until 0 byte hit. */
  while (*curp != '\0') {
	if (passno == 1) {
		/* Pass 1: Enter all identifiers. */
		if (isidchar(*curp)) enter(curp);
	}

	/* Manage the depth-of-nesting counter. */
	if (*curp == '{') {
		if (depth == 0 && (*prevp == ')' || *prevp == '=')) {
			/* The body of a function or a large initializer is
			 * better not sucked into core, and so semip should
			 * not linger behind.
			 */
			body = 1;
		}
		depth++;
		next();
		continue;
	}
	if (*curp == '}') {
		depth--;
		if (depth == 0) body = 0;
		next();
		continue;
	}

	/* Manage parentheses. */
	if (*curp == '(') {
		if (depth == 0) namep = prevp;
		next();
		continue;
	}

	if (passno == 1) {
		/* See if this might possibly be a function definition. */
		if (*curp != ';' && *prevp == ')'
					&& depth == 0 && isidchar(*namep)) {
			/* Mark the name as being a function definition.  A
			 * associated prototype for this definition should
			 * not be touched, lest declaration and definition
			 * become different.
			 */
			pp = present(namep);
			if (pp == HULL) {
				fprintf(stderr, "Error. name not found \n");
				exit(1);
			}
			pp->h_def = 1;
		}
	}

	if (passno == 2) {
		/* See if this is a potential end of a header prototype. */
		if (*curp == ';' && *prevp == ')'
					&& depth == 0 && isidchar(*namep)) {
			/* Structurally, it looks like a prototype. */
			pp = present(namep);
			if (pp == HULL) {
				fprintf(stderr, "Error. name not found \n");
				exit(1);
			}

			p = semip + 1;	/* probable start of prototype */
			while (isspace(*p)) p++;	/* skip white space */

			/* p points to start, curp to end. */
			len = curp - p;
			if (*curp == ';') len++;  /* Also remove last ';'. */

			if (pp->h_count < 2) {
				/* The prototype is not needed. Erase it. */
				delete(p, len, 0);
			} else {
				/* The prototype is needed, but may we perhaps
				 * reduce it to a K&R style declation?
				 */
				if (mflag && !pp->h_def) delete(p, len, 1);
			}
		}
	}
	next();
  }
  return;
}

static struct hash *present(char *s)
{
  int h;
  register struct hash *hp;

  h = hashval(s);
  hp = &hash[h];
  while (hp->h_name != NULL) {
	if (strncmp(hp->h_name, s, strlen(hp->h_name)) == 0) return(hp);
	hp++;
	if (hp == &hash[HASH_SLOTS]) hp = &hash[0];
  }
  return(HULL);
}

static int hashval(char *s)
{
  char *p;
  int sum = 0;

  p = s;
  while (isidchar(*p)) {
	sum += (int) *p++;
	sum = sum << 1;
	sum = sum & 077777;
  }
  return(sum % HASH_SLOTS);
}

static void delete(char *p, int count, int kr)
{
  /* If this string contains multiple declarations, as in:
   *	int abc, xyz();
   * Don't delete it (too much trouble to get it right).
   */

  int pdepth;
  char *s, *end, *left=0, *right=0;
  off_t offset;
  int n;
  char overwrite[128];

  /* This code examines the text between the previous ; or quote and the
   * the ); to figure out if it is really a prototype.  The presence of a
   * comma at depth 0 or the presence of an equal sign kills it.
   *
   * Added check for '#' to kill anything that looks like it may have a #
   * directive in it. (L.C. 01/02/02) 
   */
  pdepth = 0;
  s = p;
  end = p + count;
  while (s < end) {
	if (*s == '(') if (pdepth++ == 0) left = s;
	if (*s == ')') if (--pdepth == 0) right = s;
	if (*s == ',' && pdepth == 0) return;
	if (*s == '=') return;
	if (*s == '#') return;
	s++;
  }
  if (kr) {
	/* Wipe the arguments of a prototype, making it a less memory hungry
	 * K&R style declaration.
	 */
	if (left >= right) return;	/* Huh? */
	p = left+1;
	end = right;
  }

  /* Overwrite (part of) the prototype in the input file. */
  offset = ord + (p - buf);
  n = 0;
  s = p;
  for (;;) {
	if (n == sizeof(overwrite) || s == end) {
		if (n > 0) {
			lseek(fd2, offset, SEEK_SET);
			write(fd2, overwrite, n);
			offset += n;
			n = 0;
		}
		if (s == end) break;
	}
	overwrite[n++] = isspace(*s) ? *s : ' ';
	s++;
  }
}
