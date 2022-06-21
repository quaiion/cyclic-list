#ifndef LIST_ACTIVE
#define LIST_ACTIVE

#define AUTO_VERIFICATION_ON

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

constexpr int OS_RESERVED_ADDR = 13;

enum CTOR_OPER_CODE {CTOR_MEM_ERROR = 0, CONSTRUCTED = 1};
enum DEL_FR_OPER_CODE {HEAD_DELETED = 0, NO_HEAD_TO_DELETE = 1, DEL_FR_VER_FAILED = 2};
enum DEL_BK_OPER_CODE {TAIL_DELETED = 0, NO_TAIL_TO_DELETE = 1, DEL_BK_VER_FAILED = 2};
enum DEL_OPER_CODE    {DELETED = 0, DEL_VER_FAILED = 2, DEL_WRONG_INPUT = 3};
enum DEL_SQ_OPER_CODE {SQ_DELETED = 0, DEL_SQ_VER_FAILED = 2, DEL_SQ_WRONG_INPUT = 3};
enum SORT_OPER_CODE {SORTED = 0, SRT_VER_FAILED = 2};
enum DUMP_OPER_CODE {DUMPED = 0, COMMON_DMP_ERROR = 1, DMP_VER_FAILED = 2};
enum VERIFICATION_CODE {NO_FLAWS, DATA_FLAW, CAP_FLAW, FREE_FLAW, FICT_FLAW, LST_IDX_FLAW,
                        LST_SEQUENCE_FLAW, FREE_MARKER_FLAW, FREE_IDX_FLAW, INCOMPLETENESS_FLAW};

constexpr ssize_t OPER_ERROR_MEM = -1;
constexpr ssize_t OPER_ERROR_VER = -2;
constexpr ssize_t OPER_ERROR_INP = -3;

enum ELEM_VALUES {FREE_NODE_ELEM = 0, FICT_NODE_ELEM = -1};
enum SPECIAL_IDX {FREE_NODE_MARKER = -1, NO_HEAD = 0, NO_TAIL = 0, FICT = 0};

typedef int elem_t;

struct node_t {

    elem_t elem;
    ssize_t next;
    ssize_t prev;
};

struct list_t {

    node_t *data;
    ssize_t free;
    ssize_t cap;
    bool quick_mode;
};

CTOR_OPER_CODE list_ctor (list_t *lst, ssize_t cap = 8);
void list_dtor (list_t *lst);

ssize_t list_insert_front (list_t *lst, elem_t val);
ssize_t list_insert_back (list_t *lst, elem_t val);
ssize_t list_insert_before (list_t *lst, elem_t val, ssize_t pos);
ssize_t list_insert_after (list_t *lst, elem_t val, ssize_t pos);

/*
Here and wherever *nseq* parameter is used or
"in the order of the list" is printed it is supposed
that nodes' sequential numbers are starting from 1 (head)
and ending on *size* (tail)

*size* + 1 and 0 both imply the fictional node's
sequential number, but TO USE FICTIONAL NODE'S
NUMBER as a function argument always use 0
*/

ssize_t list_take (list_t *lst, ssize_t nseq);

DEL_FR_OPER_CODE list_delete_front (list_t *lst);
DEL_BK_OPER_CODE list_delete_back (list_t *lst);
DEL_OPER_CODE list_delete (list_t *lst, ssize_t pos);

VERIFICATION_CODE list_verify (list_t *lst);
DUMP_OPER_CODE list_dump (list_t *lst, const char *file_name);

SORT_OPER_CODE list_sort (list_t *lst);
ssize_t list_seq_insert_before (list_t *lst, elem_t val, ssize_t nseq);
ssize_t list_seq_insert_after (list_t *lst, elem_t val, ssize_t nseq);
DEL_SQ_OPER_CODE list_seq_delete (list_t *lst, ssize_t nseq);

#endif
