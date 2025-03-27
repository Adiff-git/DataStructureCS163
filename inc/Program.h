#ifndef PROGRAM_H
#define PROGRAM_H

#include "raylib.h"
#include "HashTable.h"
class Program {
public:
    Program();
    ~Program();
    void Run();

private:
    HashTable hashTable;
};

#endif