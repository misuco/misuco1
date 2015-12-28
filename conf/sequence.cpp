#include "sequence.h"

Sequence::Sequence(int nsteps, int nbars)
{
    steps=new std::vector<int>[nsteps];
    this->nsteps=nsteps;
    this->nbars=nbars;
/*
    for(int i=0;i<nsteps;i++) {
        steps[i].push_back(i);
    }
*/
}

Sequence::~Sequence()
{
    delete steps;
}

void Sequence::addNote(int s, int n)
{
    steps[s].push_back(n);
}

void Sequence::removeNote(int s, int n)
{
    steps[s].erase(steps[s].begin()+n);
}

void Sequence::clearStep(int s)
{
    steps[s].clear();
}

