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
#include <sstream>
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
    string toString()    const;
    
    void   setCount(int newCount) {count = newCount;}
    
    
private:
    string sentence;
    int    count;
};
typedef vector<Sentence> SentenceBucket;
//
class SimilarSentences
{
public:
    SimilarSentences(string filename);
    
    void writeToFileNoDupliData();
    void writeToFileLengthBucket();
    
    void findAndProcessDuplicates();
    void hashToLengthBuckets();
    
private:
    string                 filename;
    vector<string>         wholeData;
    SentenceBucket         noDupliData;
    vector<SentenceBucket> lengthBucket;
    
};

#endif /* defined(__MiningMassiveDataSetsC__SimilarSentences__) */
