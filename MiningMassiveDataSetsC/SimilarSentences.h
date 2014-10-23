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
#include <numeric>
#include <functional>

using namespace std;

const int NB_FREQ_WORDS     = 5;
const int SIZE_OF_FREQ_WORD = 10;
//
class Sentence
{
public:
    Sentence(string s, int c=1);
    
    string getStr   () const {return str;}
    int    getCount () const {return count;}
    int    getLength() const {return length;}
    int    getFreq  () const {return freq;}
    string toString () const;
    
    int    getEDist  (int i) const {return editDist[i];}
    int    editDistTo(const Sentence& s) const;

    void   setFreq    (int f)            {freq = f;}
    void   setEditDist(int i, int eDist) {editDist[i] = eDist;}
    
private:
    string str;
    int    count;
    int    length;
    int    freq;
    
    vector<int> editDist;
};
typedef vector<Sentence> SentenceBucket;
//
class SimilarSentences
{
public:
    SimilarSentences(string filename);
    
    void writeToFileNoDupliData();
    void writeToFileLengthBucket();
    void writeToFileEditDistBucket();
    void debugInfo(const vector<SentenceBucket>& input) const;
    void writeToFilePairBucket();
    
    void findAndProcessDuplicates();
    
    template<typename FuncType>
    void generateBuckets(vector<SentenceBucket>& input, vector<SentenceBucket>& output, const FuncType& f) const;

    
    vector<string> findMostFrequentSentence(const SentenceBucket& input) const;
    
    void hashToLengthBuckets();
    void hashToEditDistBuckets();
    void bruteForce();
    
    string                 filename;
    size_t                 noDupliDataSize;
    vector<string>         wholeData;
    SentenceBucket         noDupliData;
    vector<SentenceBucket> lengthBucket;
    vector<SentenceBucket> eDistBucket;
    vector<SentenceBucket> pairBucket;
};

#endif /* defined(__MiningMassiveDataSetsC__SimilarSentences__) */
