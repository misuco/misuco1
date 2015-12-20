#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <vector>

class Sequence
{
public:
    Sequence(int nsteps, int nbars);
    ~Sequence();
    int getNsteps() {return nsteps;}
    int getNbars() {return nbars;}
    int getStepNNotes(int i) {return steps[i].size();}
    int getStepNote(int s, int n) {return steps[s].at(n);}

    void addNote(int s, int n);
    void removeNote(int s, int n);

    int nbars;

private:
    int nsteps;
    std::vector<int> * steps;
};

#endif // SEQUENCE_H
