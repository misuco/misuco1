#include <QDebug>
#include "seqmem.h"
#include <stdio.h>
#include <stdlib.h>

SeqMem::SeqMem() {
    for(int i=0;i<seqmem_max;i++) {
        seqmem[i]=new Sequence(16,4);
    }
}

SeqMem::~SeqMem() {
    for(int i=0;i<seqmem_max;i++) {
        delete(seqmem[i]);
    }
}

void SeqMem::readSeqmem(char * filename) {
    int counter;
    FILE *ptr_myfile;

    ptr_myfile=fopen(filename,"rb");
    if (!ptr_myfile) {
        printf("Unable to open file!");
    } else {
        int nseqs,nbars,nsteps,nnotes,note;
        //fread(&nseqs,sizeof(int),1,ptr_myfile);
        for (int i=0;i<seqmem_max;i++) {
            fread(&nbars,sizeof(int),1,ptr_myfile);
            seqmem[i]->nbars=nbars;
            fread(&nsteps,sizeof(int),1,ptr_myfile);
            seqmem[i]->setNsteps(nsteps);
            qDebug() << "read seq " << i << " nsteps " << nsteps << " nbars " << nbars;
            for(int j=0;j<seqmem[i]->getNsteps();j++) {
                fread(&nnotes,sizeof(int),1,ptr_myfile);
                seqmem[i]->clearStep(j);
                for(int k=0;k<nnotes;k++) {
                    fread(&note,sizeof(int),1,ptr_myfile);
                    seqmem[i]->addNote(j,note);
                }
            }

        }
        fclose(ptr_myfile);
    }
}

void SeqMem::writeSeqmem(char * filename) {
    FILE *ptr_myfile;

    ptr_myfile=fopen(filename,"wb");
    if (!ptr_myfile) {
        printf("Unable to open file!");
    } else {
        for(int i=0;i<seqmem_max;i++) {
            int x=seqmem[i]->getNbars();
            fwrite(&x, sizeof(int), 1, ptr_myfile);
            x=seqmem[i]->getNsteps();
            fwrite(&x, sizeof(int), 1, ptr_myfile);
            for(int j=0;j<seqmem[i]->getNsteps();j++) {
                int nn=seqmem[i]->getStepNNotes(j);
                fwrite(&nn, sizeof(int), 1, ptr_myfile);
                for(int k=0;k<nn;k++) {
                    x=seqmem[i]->getStepNote(j,k);
                    fwrite(&x, sizeof(int), 1, ptr_myfile);
                }
            }
        }
        fclose(ptr_myfile);
    }
}

