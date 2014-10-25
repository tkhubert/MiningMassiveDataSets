//
//  main.cpp
//  MiningMassiveDataSetsC
//
//  Created by Thomas Hubert on 19/10/2014.
//  Copyright (c) 2014 Thomas Hubert. All rights reserved.
//
#include <iostream>
#include "time.h"
#include "SimilarSentences.h"

int main(int argc, const char * argv[])
{
    clock_t t = clock();
    
    // insert code here...
    string filename = "/Users/tkhubert/Documents/Etude/5.MiningMassiveDataSets/HW/sentences.txt";
    
    SimilarSentences simSent(filename);
    simSent.findSmilarSentences();
    
    t = clock() - t;
    cout << "time to compute: "<< float(t)/CLOCKS_PER_SEC << endl;
    
    return 0;
}

