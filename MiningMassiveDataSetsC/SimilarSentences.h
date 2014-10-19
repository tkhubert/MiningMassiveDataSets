//
//  SimilarSentences.h
//  MiningMassiveDataSetsC
//
//  Created by Thomas Hubert on 19/10/2014.
//  Copyright (c) 2014 Thomas Hubert. All rights reserved.
//

#ifndef __MiningMassiveDataSetsC__SimilarSentences__
#define __MiningMassiveDataSetsC__SimilarSentences__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

class Sentence
{
public:
    Sentence(string s, int c=1) : sentence(s), count(c) {}
    
    string getSentence() const {return sentence;}
    int    getCount()    const {return count;}
    size_t getLength()   const {return sentence.length();}
    
    void   setCount(int newCount) {count = newCount;}
    
private:
    string sentence;
    int    count;
    
    
};
//
class SimilarSentences
{
public:
    SimilarSentences(string filename);
    
private:
    vector<string>   wholeData;
    vector<Sentence> noDuplicatesData;
    
    void findAndProcessDuplicates();
    void hashToLengthBuckets();
    
};

#endif /* defined(__MiningMassiveDataSetsC__SimilarSentences__) */
