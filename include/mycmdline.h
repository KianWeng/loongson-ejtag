#ifndef __MYCMDLINE_H__
#define __MYCMDLINE_H__
typedef struct label{
struct label *next;
struct cmdline **pcmd;
int no;
int type;
char label[0];
} label_t;

enum{
LABEL_NORMAL,
LABEL_FUNCTION
};

typedef struct cmdline{
struct cmdline *next;
struct cmdline **prev;
struct cmdline *child;
struct cmdline *parent;
struct cmdline *nchild;
struct cmdline **cont;
int no;
char cmd[0];
} cmdline_t;

typedef struct line{
struct line *next;
char str[0];
} line_t;

extern line_t *headline;
extern line_t  **ptailline;

extern label_t *label_head;
extern cmdline_t **pcur;
extern cmdline_t **cmdline_ptail;
extern label_t **label_ptail;

char *get_line();
char *get_next_cmdline(cmdline_t ***cmdline_ptail);
char *append_cmdline(char *line, cmdline_t ***cmdline_ptail);
int flush_lines(cmdline_t ***ppchild);
cmdline_t **parse_block(cmdline_t **pchild);
char *parse_cmd(cmdline_t ***ppchild);
int process_oneline(cmdline_t ***cmdline_pptail);
#endif
