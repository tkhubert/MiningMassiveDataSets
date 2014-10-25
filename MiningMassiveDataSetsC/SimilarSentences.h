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
#include <set>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cmath>

using namespace std;

const int NB_WORDS   = 5;
const int NB_BUCKETS = 1000;
//
class Sentence
{
public:
    Sentence(string s, int c=1);
    
    string getStr   () const {return str;}
    int    getCount () const {return count;}
    int    getLength() const {return length;}
    int    getHash1 () const {return hash1;}
    int    getHash2 () const {return hash2;}
    string toString () const;

    bool isEditDistInf1(const Sentence& s) const;
    int  editDistTo    (const Sentence& s) const;
    
    bool operator< (const Sentence& s) const {return str< s.getStr(); }
    bool operator==(const Sentence& s) const {return str==s.getStr(); }
    
private:
    // members
    string str;
    int    count;
    int    length;
    int    hash1;
    int    hash2;
};
//
class SentencePair
{
public:
    SentencePair(Sentence s1, Sentence s2);
    
    const Sentence& first()    const {return sPair.first;}
    const Sentence& second()   const {return sPair.second;}
    int             getCount() const {return count;}
    string          toString() const;
    
    bool operator< (const SentencePair& sP) const { return sPair.first < sP.first();}
    
private:
    // members
    pair<Sentence, Sentence> sPair;
    int                      count;
};
//
typedef vector<Sentence> SentenceBucket;
//
class SimilarSentences
{
public:
    SimilarSentences(string filename);
    
    void findSmilarSentences();

private:
    // members
    string filename;
    size_t noDupliDataSize;
    int    finalCount;
    
    vector<string>         wholeData;
    SentenceBucket         noDupliData;
    vector<SentenceBucket> lengthBucket;
    set<SentencePair>      pairBucket;
    
    // member functions
    void writeToFileNoDupliData();
    void writeToFileLengthBucket();
    void writeToFileEditDistBucket();
    void debugInfo(const vector<SentenceBucket>& input) const;
    void writeToFilePairBucket();
    
    void findAndProcessDuplicates();
    void hashToLengthBuckets();
    void hashToStringBuckets();
    void countSimilarSentences();
    void bruteForce(const SentenceBucket& bucket);
};

#endif /* defined(__MiningMassiveDataSetsC__SimilarSentences__) */
