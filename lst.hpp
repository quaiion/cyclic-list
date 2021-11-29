#ifndef LIST_ACTIVE
#define LIST_ACTIVE

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

constexpr int OS_RESERVED_ADDR = 13;

enum OPERATION_CODE {RESIZED = 1, DELETED = 1, NO_OBJ_TO_DELETE = 2, DUMPED = 3, ERROR = 0};            // ERROR не -1, чтобы в лишний раз не рисковать выходом за выделенную память
enum VERIFICATION_CODE {NO_FLAWS, DATA_FLAW, CAP_FLAW, FREE_FLAW, FICT_FLAW, LST_IDX_FLAW,
                        LST_SEQUENCE_FLAW, FREE_MARKER_FLAW, FREE_IDX_FLAW, INCOMPLETENESS_FLAW};
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
    ssize_t free;                // Не стираем, в отличие от head и tail, т.к. экономией одного поля сильно ухудшим читаемость и скорость кода (с головой и хвостом это не так работает)
    ssize_t cap;
    bool quick_mode;
};

void list_ctor (list_t *lst, ssize_t cap = 8);
void list_dtor (list_t *lst);

ssize_t list_insert_front (list_t *lst, elem_t val);
ssize_t list_insert_back (list_t *lst, elem_t val);
ssize_t list_insert_before (list_t *lst, elem_t val, size_t pos);       // Не просто так тут pos беззнаковый........ Если серьезно, надо бы уточнить, работает ли такой метод перекладывания ответственности на пользователя, или нужно париться, ставить ssize_t и вписывать верификатор на >= 0
ssize_t list_insert_after (list_t *lst, elem_t val, size_t pos);        // Что ж, верятно есть смысл во всех сомнительных сравнениях прописать явное приведение к беззнаковым

/*

Here and wherever *nseq* parameter is used or
"in the order of the list" is printed it is supposed
that nodes' sequential numbers are starting from 1 (head)
and ending on *cap* (tail).

*cap* + 1 and 0 both imply the fictional node's
sequantial number, but TO USE FICTIONAL NODE'S
NUMBER as a function argument always use 0

*/

elem_t list_take (list_t *lst, size_t nseq);

OPERATION_CODE list_delete_front (list_t *lst);            // Возвращают ERROR, если все плохо, DELETED, если все ок (ну или NO_OBJ_TO_DELETE в особых случаях)
OPERATION_CODE list_delete_back (list_t *lst);
OPERATION_CODE list_delete (list_t *lst, size_t pos);

VERIFICATION_CODE list_verify (list_t *lst);
OPERATION_CODE list_dump (list_t *lst, const char *file_name);

void list_sort (list_t *lst);
ssize_t list_seq_insert_before (list_t *lst, elem_t val, size_t nseq);          // Same про перекладывание ответственности
ssize_t list_seq_insert_after (list_t *lst, elem_t val, size_t nseq);           // Стоит ли в подобных функциях жертвовать немного скоростью в пользу простоты кода и вызывать общие функции через частный обсчет аргументов (=> избыточная верификация)
OPERATION_CODE list_seq_delete (list_t *lst, size_t nseq);

#endif

/*

Это нормально, что прцентов 40 или больше всего времени написания проекта у меня уходит на верификацию?
Добавить в дамповку вывод дополнительных параметров узла и общую информационную подпись дампа

*/
