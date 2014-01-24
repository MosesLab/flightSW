/************************************************
 *Author: Roy Smart				*
 *1/21/14					*
 *MOSES LAB					*
 *						*
 *queue.h declares functions to be used by queues*
 *namely push and pop				*
 ************************************************/


typedef struct {
    packet *first;
    packet *last;
} packetQueue;

void enqueue(packetQueue queue, packet p) {
    if (queue.first == NULL) {
        queue.first = &p;
        queue.last = queue.first;
    } else {
        packet * temp = queue.last;
        temp->next = &p;
        queue.last = &p;
}
}

packet dequeue(packetQueue queue) {
    packet p = *(queue.first);
    queue.first = p.next;
}
