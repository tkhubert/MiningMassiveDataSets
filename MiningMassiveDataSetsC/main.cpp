//
//  main.cpp
//  MiningMassiveDataSetsC
//
//  Created by Thomas Hubert on 19/10/2014.
//  Copyright (c) 2014 Thomas Hubert. All rights reserved.
//

#include <iostream>
#include "SimilarSentences.h"

int main(int argc, const char * argv[])
{
    // insert code here...
    string filename = "/Users/tkhubert/Documents/Etude/5.MiningMassiveDataSets/HW/sentences50000.txt";
    
    SimilarSentences simSent(filename);
    simSent.findAndProcessDuplicates();
    simSent.hashToLengthBuckets();
    simSent.debugInfo(simSent.lengthBucket);
    simSent.hashToBuckets();
    simSent.writeToFilePairBucket();
    
    return 0;
}

