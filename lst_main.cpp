#include "lst.hpp"
#include <stdio.h>

int main () {

    list_t l = {};
    list_t *lp = &l;

    list_ctor (lp);

    list_insert_front (lp, 1);
    list_insert_front (lp, 2);
    list_insert_front (lp, 3);
    list_insert_front (lp, 4);
    list_insert_front (lp, 5);
    list_dump (lp, "dump0");

    list_delete (lp, 4);
    list_delete_back (lp);
    list_delete_front (lp);
    list_insert_back (lp, 117);
    list_insert_after (lp, 888, 2);

    list_dump (lp, "dump1");
    list_sort (lp);
    list_verify (lp);
    list_dump (lp, "dump2");
    
    list_take (lp, 3);
    // list_seq_delete (lp, 3);
    list_seq_insert_after (lp, 941, 4);
    list_dump (lp, "dump3");

    list_dtor (lp);
    return 0;
}