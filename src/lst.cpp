#include "lst.hpp"

enum RESIZE_OPER_CODE {RSZ_MEM_ERROR = 0, RESIZED = 1};

static RESIZE_OPER_CODE list_resize_up (list_t *lst);
static void node_swap (list_t *lst, ssize_t idx1, ssize_t idx2);
static void ins_before (list_t *lst, ssize_t idx, elem_t val);
static void ins_after (list_t *lst, ssize_t idx, elem_t val);
static void del (list_t *lst, ssize_t idx);
static void del_head (list_t *lst);
static void del_tail (list_t *lst);

#define DUMP_POSITION()                                                             \
    do {                                                                            \
        printf("^^^ %s : %s : %d ^^^\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);  \
    } while (0)

CTOR_OPER_CODE list_ctor (list_t *lst, ssize_t cap /* = 8 */) {

    assert (lst);

    lst->data = (node_t *) calloc (cap + 1, sizeof (node_t));
    if (lst->data == NULL) {

        printf ("\nConstruction failed: memory error\n");
        return CTOR_MEM_ERROR;
    }

    lst->data [FICT].elem = FICT_NODE_ELEM;
    lst->data [FICT].prev = NO_TAIL;
    lst->data [FICT].next = NO_HEAD;

    ssize_t idx = 1;
    if (cap != 0) {

        for ( ; idx < cap; ++ idx) {

            lst->data [idx].prev = FREE_NODE_MARKER;
            lst->data [idx].next = idx + 1;
        }
        lst->data [idx].prev = FREE_NODE_MARKER;
        lst->data [idx].next = FICT;

        lst->free = 1;

    } else {

        lst->free = FICT;
    }

    lst->quick_mode = true;
    lst->cap = cap;

    return CONSTRUCTED;
}

void list_dtor (list_t *lst) {

    assert (lst);

    memset (lst->data, 0, (lst->cap + 1) * sizeof (node_t));
    free (lst->data);
    lst->data = (node_t *) OS_RESERVED_ADDR;

    lst->cap = -1;
    lst->free = -1;
    lst->quick_mode = false;
}

ssize_t list_insert_front (list_t *lst, elem_t val) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (lst->free == FICT) {
        
        if (list_resize_up (lst) == RSZ_MEM_ERROR) {

            printf ("\nResize failed: memory error while trying to resize up \
                    from capacity %lld to capacity %lld, in function list_insert_front ()\n",
                    lst->cap, lst->cap * 2 + 1);
            return OPER_ERROR_MEM;
        }
    }

    ins_after (lst, FICT, val);

    lst->quick_mode = false;
    return lst->data [FICT].next;
}

ssize_t list_insert_back (list_t *lst, elem_t val) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (lst->free == FICT) {
        
        if (list_resize_up (lst) == RSZ_MEM_ERROR) {

            printf ("\nResize failed: memory error while trying to resize up \
                    from capacity %lld to capacity %lld, in function list_insert_back ()\n",
                    lst->cap, lst->cap * 2 + 1);
            return OPER_ERROR_MEM;
        }
    }

    ins_before (lst, FICT, val);

    return lst->data [FICT].prev;
}

ssize_t list_insert_before (list_t *lst, elem_t val, ssize_t pos) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (lst->free == FICT) {
        
        if (list_resize_up (lst) == RSZ_MEM_ERROR) {

            printf ("\nResize failed: memory error while trying to resize up \
                    from capacity %lld to capacity %lld, in function list_insert_before ()\n",
                    lst->cap, lst->cap * 2 + 1);
            return OPER_ERROR_MEM;
        }
    }

    if (pos > lst->cap) {

        printf ("\nInsertion failed: *pos* argument exceeds lists capacity value while trying to insert \
                an element before one on position %lld, in function list_insert_before ()\n",
                pos);
        return OPER_ERROR_INP;
    }

    if (pos < 0) {

        printf ("\nInsertion failed: *pos* argument ran below zero while trying to insert \
                an element before one on position %lld, in function list_insert_before ()\n",
                pos);
        return OPER_ERROR_INP;
    }

    if (lst->data [pos].prev == FREE_NODE_MARKER) {

        printf ("\nInsertion failed: *pos* argument is pointing at a free node while trying to insert \
                an element before one on position %lld, in function list_insert_before ()\n",
                pos);
        return OPER_ERROR_INP;
    }

    ins_before (lst, pos, val);

    lst->quick_mode = false;
    return lst->data [pos].prev;
}

ssize_t list_insert_after (list_t *lst, elem_t val, ssize_t pos) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (lst->free == FICT) {
        
        if (list_resize_up (lst) == RSZ_MEM_ERROR) {

            printf ("\nResize failed: memory error\n while trying to resize up \
                    from capacity %lld to capacity %lld, in function list_insert_after ()\n",
                    lst->cap, lst->cap * 2 + 1);
            return OPER_ERROR_MEM;
        }
    }

    if (pos > lst->cap) {

        printf ("\nInsertion failed: *pos* argument exceeds lists capacity value while trying to insert \
                an element after one on position %lld, in function list_insert_after ()\n",
                pos);
        return OPER_ERROR_INP;
    }

    if (pos < 0) {

        printf ("\nInsertion failed: *pos* argument ran below zero while trying to insert \
                an element after one on position %lld, in function list_insert_after ()\n",
                pos);
        return OPER_ERROR_INP;
    }

    if (lst->data [pos].prev == FREE_NODE_MARKER) {

        printf ("\nInsertion failed: *pos* argument is pointing at a free node while trying to insert \
                an element after one on position %lld, in function list_insert_after ()\n",
                pos);
        return OPER_ERROR_INP;
    }

    ins_after (lst, pos, val);

    lst->quick_mode = false;
    return lst->data [pos].next;
}

DEL_FR_OPER_CODE list_delete_front (list_t *lst) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return DEL_FR_VER_FAILED;
    }

#endif

    if (lst->data [FICT].next == NO_HEAD) {

        return NO_HEAD_TO_DELETE;
    }

    del_head (lst);

    lst->quick_mode = false;
    return HEAD_DELETED;
}

DEL_BK_OPER_CODE list_delete_back (list_t *lst) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return DEL_BK_VER_FAILED;
    }

#endif

    if (lst->data [FICT].prev == NO_TAIL) {

        return NO_TAIL_TO_DELETE;
    }

    del_tail (lst);

    return TAIL_DELETED;
}

DEL_OPER_CODE list_delete (list_t *lst, ssize_t pos) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return DEL_VER_FAILED;
    }

#endif

    if (pos == FICT) {

        printf ("\nDeletion failed: *pos* argument is pointing at a base fictive node while trying to delete \
                an element on position %lld, in function list_delete ()\n", 
                pos);
        return DEL_WRONG_INPUT;
    }

    if (pos > lst->cap) {

        printf ("\nDeletion failed: *pos* argument exceeds lists capacity value while trying to delete \
                an element on position %lld, in function list_delete ()\n",
                pos);
        return DEL_WRONG_INPUT;
    }

    if (pos < 0) {

        printf ("\nDeletion failed: *pos* argument ran below zero while trying to delete \
                an element on position %lld, in function list_delete ()\n",
                pos);
        return DEL_WRONG_INPUT;
    }

    if (lst->data [pos].prev == FREE_NODE_MARKER) {

        printf ("\nDeletion failed: *pos* argument is pointing at a free node while trying to delete \
                an element on position %lld, in function list_delete ()\n",
                pos);
        return DEL_WRONG_INPUT;
    }

    del (lst, pos);

    lst->quick_mode = false;
    return DELETED;
}

ssize_t list_take (list_t *lst, ssize_t nseq) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (nseq < 0) {

        printf ("\nTake failed: *nseq* argument ran below zero while trying to take \
                an element in node %lld, in function list_take ()\n",
                nseq);
        return OPER_ERROR_INP;
    }

    if (lst->quick_mode) {

        if (lst->free) {

            if (nseq >= lst->free) {

                printf ("\nTake failed: *nseq* argument exceeds sequence's real size (%lld) \
                        while trying to take node %lld, in function list_take ()\n",
                        lst->free - 1, nseq);
                return OPER_ERROR_INP;
            }
        } else {

            if (nseq > lst->cap) {

                printf ("\nTake failed: *nseq* argument exceeds sequence's real size (%lld) \
                        while trying to take node %lld, in function list_take ()\n",
                        lst->cap, nseq);
                return OPER_ERROR_INP;
            }
        }

        return nseq;
    }

    ssize_t idx = FICT, nodes_handled = 0;
    do {

        if (nodes_handled == nseq) {

            return idx;
        }

        idx = lst->data [idx].next;
        nodes_handled += 1;

    } while (idx != FICT);

    printf ("\nTake failed: *nseq* argument exceeds sequence's real size (%lld) \
            while trying to take node %lld, in function list_take ()\n",
            nodes_handled, nseq);
    return OPER_ERROR_INP;
}

ssize_t list_seq_insert_before (list_t *lst, elem_t val, ssize_t nseq) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (nseq < 0) {

        printf ("\nInsertion failed: *nseq* argument ran below zero while trying to insert \
                node before node %lld, in function list_seq_insert_before ()\n",
                nseq);
        return OPER_ERROR_INP;
    }
    
    if (lst->free == FICT) {
        
        if (list_resize_up (lst) == RSZ_MEM_ERROR) {

            printf ("\nResize failed: memory error while trying to resize up \
                    from capacity %lld to capacity %lld, in function list_seq_insert_before ()\n",
                    lst->cap, lst->cap * 2 + 1);
            return OPER_ERROR_MEM;
        }
    }

    if (lst->quick_mode) {

        if (nseq > lst->free - 1) {

            printf ("\nInsertion failed: *nseq* argument exceeds sequence's real size (%lld) \
                    while trying to insert node before node %lld, in function list_seq_insert_before ()\n",
                    lst->free - 1, nseq);
            return OPER_ERROR_INP;
        }

        ins_before (lst, nseq, val);

        lst->quick_mode = false;
        return lst->data [nseq].prev;
    }

    ssize_t idx = FICT, nodes_handled = 0;
    do {

        if (nodes_handled == nseq) {

            ins_before (lst, idx, val);
            return lst->data [idx].prev;
        }

        idx = lst->data [idx].next;
        nodes_handled += 1;

    } while (idx != FICT);

    printf ("\nInsertion failed: *nseq* argument exceeds sequence's real size (%lld) \
            while trying to insert node before node %lld, in function list_seq_insert_before ()\n",
            nodes_handled - 1, nseq);
    return OPER_ERROR_INP;
}

ssize_t list_seq_insert_after (list_t *lst, elem_t val, ssize_t nseq) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return OPER_ERROR_VER;
    }

#endif

    if (nseq < 0) {

        printf ("\nInsertion failed: *nseq* argument ran below zero while trying to insert \
                node after node %lld, in function list_seq_insert_after ()\n",
                nseq);
        return OPER_ERROR_INP;
    }

    if (lst->free == FICT) {
        
        if (list_resize_up (lst) == RSZ_MEM_ERROR) {

            printf ("\nResize failed: memory error while trying to resize up \
                    from capacity %lld to capacity %lld, in function list_seq_insert_after ()\n",
                    lst->cap, lst->cap * 2 + 1);
            return OPER_ERROR_MEM;
        }
    }

    if (lst->quick_mode) {

        if (nseq > lst->free - 1) {

            printf ("\nInsertion failed: *nseq* argument exceeds sequence's real size (%lld) \
                    while trying to insert node after node %lld, in function list_seq_insert_after ()\n",
                    lst->free - 1, nseq);
            return OPER_ERROR_INP;
        }

        ins_after (lst, nseq, val);

        lst->quick_mode = false;
        return lst->data [nseq].next;
    }

    ssize_t idx = FICT, nodes_handled = 0;
    do {

        if (nodes_handled == nseq) {

            ins_after (lst, idx, val);

            return lst->data [idx].next;
        }

        idx = lst->data [idx].next;
        nodes_handled += 1;

    } while (idx != FICT);

    printf ("\nInsertion failed: *nseq* argument exceeds sequence's real size (%lld) \
            while trying to insert node before node %lld, in function list_seq_insert_after ()\n",
            nodes_handled - 1, nseq);
    return OPER_ERROR_INP;
}

DEL_SQ_OPER_CODE list_seq_delete (list_t *lst, ssize_t nseq) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return DEL_SQ_VER_FAILED;
    }

#endif

    if (nseq < 0) {

        printf ("\nDeletion failed: *nseq* argument ran below zero while trying to delete \
                node %lld, in function list_seq_delete ()\n",
                nseq);
        return DEL_SQ_WRONG_INPUT;
    }

    if (nseq == FICT) {

        printf ("\nDeletion failed: *nseq* argument is pointing at a base fictive node \
                while trying to delete node %lld, in function list_seq_delete ()\n", 
                nseq);
        return DEL_SQ_WRONG_INPUT;
    }

    if (lst->quick_mode) {

        if (lst->free) {

            if (nseq >= lst->free) {

                printf ("\nDeletion failed: *nseq* argument exceeds sequence's real size (%lld) \
                        while trying to delete node %lld, in function list_seq_delete ()\n",
                        lst->free - 1, nseq);
                return DEL_SQ_WRONG_INPUT;
            }
        } else {

            if (nseq > lst->cap) {

                printf ("\nDeletion failed: *nseq* argument exceeds sequence's real size (%lld) \
                        while trying to delete node %lld, in function list_seq_delete ()\n",
                        lst->cap, nseq);
                return DEL_SQ_WRONG_INPUT;
            }
        }

        del (lst, nseq);

        lst->quick_mode = false;
        return SQ_DELETED;
    }

    ssize_t idx = FICT, nodes_handled = 0;
    do {

        if (nodes_handled == nseq) {

            del (lst, idx);

            return SQ_DELETED;
        }

        idx = lst->data [idx].next;
        nodes_handled += 1;

    } while (idx != FICT);

    printf ("\nDeletion failed: *nseq* argument exceeds sequence's real size (%lld) \
            while trying to delete node %lld, in function list_seq_delete ()\n",
            nodes_handled, nseq);
    return DEL_SQ_WRONG_INPUT;
}

VERIFICATION_CODE list_verify (list_t *lst) {

    assert (lst);

    if (lst->data == NULL) {

        printf ("\nVerification failed: list's *data* pointer is NULL\n");
        return DATA_FLAW;
    }

    if (lst->cap < 0) {

        printf ("\nVerification failed: list's *capacity* parameter ran below zero (%lld)\n", lst->cap);
        return CAP_FLAW;
    }

    if (lst->free < 0) {

        printf ("\nVerification failed: list's *free* index ran below zero (%lld)\n", lst->free);
        return FREE_FLAW;
    }

    if (!((lst->data [FICT].next > 0 && lst->data [FICT].next <= lst->cap &&
        lst->data [FICT].prev > 0 && lst->data [FICT].prev <= lst->cap) ||
        (lst->data [FICT].next == 0 && lst->data [FICT].prev == 0))) {

        printf ("\nVerification failed: list's fictional node has an impossible \
                parameters combination (next: %lld; prev: %lld; list's capacity: %lld)\n",
                lst->data [FICT].next, lst->data [FICT].prev, lst->cap);
        return FICT_FLAW;
    }

    ssize_t nodes_handled = 0, idx = FICT;
    do {

        if (lst->data [idx].next > lst->cap || lst->data [idx].next < 0) {

            printf ("\nVerification failed: the node next to the one \
                    on position %lld has an impossible index: %lld (number %lld in the order of the list)\n",
                    idx, lst->data [idx].next, nodes_handled + 1);
            return LST_IDX_FLAW;
        }

        if (lst->data [lst->data [idx].next].prev != idx) {

            printf ("\nVerification failed: incongruity of next and prev parameters \
                    detected during the transition from the node on position %lld to the node \
                    on position %lld (number %lld and %lld in the order of the list)\n",
                    idx, lst->data [idx].next, nodes_handled, nodes_handled + 1);
            return LST_SEQUENCE_FLAW;
        }

        idx = lst->data [idx].next;
        nodes_handled += 1;

    } while (idx != FICT);

    idx = lst->free;
    for (ssize_t free_nodes_handled = 0; idx != FICT;
        ++ nodes_handled, ++ free_nodes_handled, idx = lst->data [idx].next) {

        if (lst->data [idx].prev != FREE_NODE_MARKER) {

            printf ("\nVerification failed: the free node on position %lld has \
                    no *free node* marker (prev: %lld; number %lld in the order of the free list)\n",
                    idx, lst->data [idx].prev, free_nodes_handled + 1);
            return FREE_MARKER_FLAW;
        }

        if (lst->data [idx].next > lst->cap || lst->data [idx].next < 0) {

            printf ("\nVerification failed: the free node next to the one on position %lld has \
                    an impossible index: %lld (number %lld in the order of the free list)\n",
                    idx, lst->data [idx].next, free_nodes_handled + 1);
            return FREE_IDX_FLAW;
        }
    }

    if (nodes_handled != lst->cap + 1) {

        printf ("\nVerification failed: number of nodes in main and free \
                sequences doesn't match list's capacity (%lld against %lld)\n",
                nodes_handled, lst->cap);
        return INCOMPLETENESS_FLAW;
    }

    return NO_FLAWS;
}

DUMP_OPER_CODE list_dump (list_t *lst, const char *file_name) {

    assert (lst);
    assert (file_name);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return DMP_VER_FAILED;
    }

#endif

    if (strlen (file_name) > 30) {

        printf ("\nDump failed: impossible dump file name\n");
        return COMMON_DMP_ERROR;
    }

    FILE *dump_file = fopen ("list_dump_instr.gv", "w");

    if (dump_file == NULL) {

        printf ("\nDump failed: failed to open the dump instruction file\n");
        return COMMON_DMP_ERROR;
    }

    fputs ("digraph dump{\n\trankdir=TB;\n\t{\n\t\tnode[shape=plaintext];\
            \n\t\tedge[color=white];\n\t\t\"cell 0 (FICT)\"", dump_file);
    for (ssize_t i = 1; i <= lst->cap; ++ i) {

        fprintf (dump_file, " -> \"cell %lld\"", i);
    }
    fprintf (dump_file, ";\n\t}\n\t");
    for (ssize_t i = 0; i <= lst->cap; ++ i) {

        fprintf (dump_file, "%lld [shape=record,label=\" elem %d | <next> next %lld | <prev> prev %lld\"];\n\t",
                 i, lst->data [i].elem, lst->data [i].next, lst->data [i].prev);
    }
    fprintf (dump_file, "{ rank = same; \"cell 0 (FICT)\"; 0; free; }\n\t");
    for (ssize_t i = 1; i <= lst->cap; ++ i) {

        fprintf (dump_file, "{ rank = same; \"cell %lld\"; %lld; }\n\t", i, i);
    }
    fputs ("{\n\t\tedge[color=orange];\n\t\t0:<next> -> ", dump_file);
    for (ssize_t idx = lst->data [FICT].next; idx != FICT; idx = lst->data [idx].next) {

        fprintf (dump_file, "%lld:<next> -> ", idx);
    }
    fputs ("0:<next>;\n\t}\n\t{\n\t\tedge[color=purple];\n\t\t0:<prev> -> ", dump_file);
    for (ssize_t idx = lst->data [FICT].prev; idx != FICT; idx = lst->data [idx].prev) {

        fprintf (dump_file, "%lld:<prev> -> ", idx);
    }
    fputs ("0:<prev>;\n\t}\n\t", dump_file);
    if (lst->free) {
        
        fprintf (dump_file, "{\n\t\tedge[color=lightgreen];\n\t\t%lld:<next> -> ", lst->free);
        for (ssize_t idx = lst->data [lst->free].next; idx != FICT; idx = lst->data [idx].next) {

            fprintf (dump_file, "%lld:<next> -> ", idx);
        }
        fputs("0:<next>;\n\t}\n\t", dump_file);
    }
    fprintf (dump_file, "{\n\t\tedge[color=darkgreen]\n\t\tfree -> %lld;\n\t}\n}\n", lst->free);

    fclose (dump_file);

    char *cmd = (char *) calloc (64, sizeof (char));
    sprintf (cmd, "dot -Tpng list_dump_instr.gv -o %s", file_name);
    system (cmd);
    free (cmd);

    return DUMPED;
}

SORT_OPER_CODE list_sort (list_t *lst) {

    assert (lst);

#ifdef AUTO_VERIFICATION_ON

    if (list_verify (lst) != NO_FLAWS) {

        DUMP_POSITION();
        return SRT_VER_FAILED;
    }

#endif

    ssize_t idx = lst->data [FICT].next, nseq = 1;
    for ( ; idx != FICT; ++ nseq) {

        node_swap (lst, idx, nseq);
        idx = lst->data [nseq].next;
    }

    lst->free = nseq;
    for ( ; nseq < lst->cap; ++ nseq) {

        lst->data [nseq].prev = FREE_NODE_MARKER;
        lst->data [nseq].next = nseq + 1;
    }
    if (nseq == lst->cap) {

        lst->data [nseq].prev = FREE_NODE_MARKER;
        lst->data [nseq].next = FICT;
    } else {

        lst->free = FICT;
    }

    lst->quick_mode = true;
    return SORTED;
}

static void node_swap (list_t *lst, ssize_t idx1, ssize_t idx2) {

    assert (lst);
    assert (idx1 > 0);
    assert (idx2 > 0);
    assert (idx1 <= lst->cap);
    assert (idx2 <= lst->cap);

    lst->data [lst->data [idx1].prev].next = idx2;
    lst->data [lst->data [idx1].next].prev = idx2;
    if (lst->data [idx2].prev != -1) {              // Same verification for idx1 is unnecessary, because sorting func uses swap only for idx1 = idx and idx2 = nseq and idx cannot point at a free node
        
        lst->data [lst->data [idx2].prev].next = idx1;
        lst->data [lst->data [idx2].next].prev = idx1;
    }

    node_t temp_nd = {};
    temp_nd = lst->data [idx1];
    lst->data [idx1] = lst->data [idx2];
    lst->data [idx2] = temp_nd;
}

static void ins_before (list_t *lst, ssize_t idx, elem_t val) {

    lst->data [lst->data [idx].prev].next = lst->free;
    lst->free = lst->data [lst->free].next;
    lst->data [lst->data [lst->data [idx].prev].next].prev = lst->data [idx].prev;
    lst->data [lst->data [lst->data [idx].prev].next].next = idx;
    lst->data [lst->data [lst->data [idx].prev].next].elem = val;
    lst->data [idx].prev = lst->data [lst->data [idx].prev].next;
}

static void ins_after (list_t *lst, ssize_t idx, elem_t val) {

    lst->data [lst->data [idx].next].prev = lst->free;
    lst->free = lst->data [lst->free].next;
    lst->data [lst->data [lst->data [idx].next].prev].next = lst->data [idx].next;
    lst->data [lst->data [lst->data [idx].next].prev].prev = idx;
    lst->data [lst->data [lst->data [idx].next].prev].elem = val;
    lst->data [idx].next = lst->data [lst->data [idx].next].prev;
}

static void del (list_t *lst, ssize_t idx) {

    lst->data [idx].elem = FREE_NODE_ELEM;
    lst->data [lst->data [idx].next].prev = lst->data [idx].prev;
    lst->data [lst->data [idx].prev].next = lst->data [idx].next;
    lst->data [idx].prev = FREE_NODE_MARKER;
    lst->data [idx].next = lst->free;
    lst->free = idx;
}

static void del_head (list_t *lst) {

    lst->data [FICT].next = lst->data [lst->data [FICT].next].next;
    lst->data [lst->data [lst->data [FICT].next].prev].next = lst->free;
    lst->data [lst->data [lst->data [FICT].next].prev].prev = FREE_NODE_MARKER;
    lst->free = lst->data [lst->data [FICT].next].prev;
    lst->data [lst->data [lst->data [FICT].next].prev].elem = FREE_NODE_ELEM;
    lst->data [lst->data [FICT].next].prev = FICT;
}

static void del_tail (list_t *lst) {

    lst->data [FICT].prev = lst->data [lst->data [FICT].prev].prev;
    lst->data [lst->data [lst->data [FICT].prev].next].prev = FREE_NODE_MARKER;
    lst->data [lst->data [lst->data [FICT].prev].next].next = lst->free;
    lst->free = lst->data [lst->data [FICT].prev].next;
    lst->data [lst->data [lst->data [FICT].prev].next].elem = FREE_NODE_ELEM;
    lst->data [lst->data [FICT].prev].next = FICT;
}

static RESIZE_OPER_CODE list_resize_up (list_t *lst) {

    ssize_t old_cap = lst->cap;
    node_t *buffer = (node_t *) realloc (lst->data, (old_cap * 2 + 2) * sizeof (node_t));
    if (buffer) {
        
        lst->cap = old_cap * 2 + 1;

    } else {
        
        return RSZ_MEM_ERROR;
    }

    lst->free = old_cap + 1;
    ssize_t idx = lst->free;
    for ( ; idx < lst->cap; ++ idx) {

        lst->data [idx].elem = FREE_NODE_ELEM;
        lst->data [idx].prev = FREE_NODE_MARKER;
        lst->data [idx].next = idx + 1;
    }
    lst->data [idx].elem = FREE_NODE_ELEM;
    lst->data [idx].prev = FREE_NODE_MARKER;
    lst->data [idx].next = FICT;
    
    return RESIZED;
}
