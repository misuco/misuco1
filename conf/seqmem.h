#ifndef SEQMEM_H
#define SEQMEM_H

#include "sequence.h"

class SeqMem
{
public:
    SeqMem();
    ~SeqMem();

    static const int seqmem_max=200;
    Sequence * seqmem[seqmem_max];

    void readSeqmem(char * filename);
    void writeSeqmem(char * filename);

};

#endif // SEQMEM_H
