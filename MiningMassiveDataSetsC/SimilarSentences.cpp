//
//  SimilarSentences.cpp
//  MiningMassiveDataSetsC
//
//  Created by Thomas Hubert on 19/10/2014.
//  Copyright (c) 2014 Thomas Hubert. All rights reserved.
//

#include "SimilarSentences.h"

namespace
{
    bool lengthCompare(const Sentence& s1, const Sentence& s2)
    {
        return s1.getLength() < s2.getLength();
    }
}
//
string Sentence::toString() const
{
    stringstream ss;
    ss << sentence << " " << count;
    return ss.str();
}
//
SimilarSentences::SimilarSentences(string _filename) : filename(_filename)
{
    ifstream file(filename);
    
    string line;
    string sentence;
    int    i;
    
    if (file.is_open())
    {
        while (getline(file, line))
        {
            stringstream ss, oss;
            ss << line;
            ss >> i;
            oss << ss.rdbuf();

            wholeData.push_back(oss.str());
        }
        file.close();
    }
    else
        throw "Could not open file: " + filename;
}
//
void SimilarSentences::findAndProcessDuplicates()
{
    sort(wholeData.begin(), wholeData.end());
    
    string currentString = wholeData[0];
    int    currentCount  = 0;
    
    for (vector<string>::const_iterator itr=wholeData.begin(); itr!=wholeData.end(); ++itr)
    {
        if (*itr==currentString)
            currentCount++;
        else
        {
            noDupliData.push_back(Sentence(currentString, currentCount));
            
            currentCount  = 1;
            currentString = *itr;
        }
    }
}
//
void SimilarSentences::writeToFileNoDupliData()
{
    ofstream file(filename+"noDupli.txt");
    int i=0;
    for (SentenceBucket::const_iterator itr=noDupliData.begin(); itr!=noDupliData.end(); ++itr)
        file << i++ << " " <<itr->toString() << endl;
    
    file.close();
}
//
void SimilarSentences::hashToLengthBuckets()
{
    sort(noDupliData.begin(), noDupliData.end(), lengthCompare);
    
    int    currentIdx    = 0;
    size_t currentLength = noDupliData[0].getLength();
    
    lengthBucket.resize(noDupliData.size());
    
    SentenceBucket tmpBucket;
    
    int count = 0;
    for (SentenceBucket::const_iterator itr=noDupliData.begin(); itr!=noDupliData.end(); ++itr)
    {
        SentenceBucket& currentBucket = lengthBucket[currentIdx];
        size_t thisLength = itr->getLength();
        
        if (thisLength==currentLength)
            tmpBucket.push_back(*itr);
        else
        {
            if (currentBucket.size()+tmpBucket.size()>1)
            {
                currentBucket.insert(currentBucket.end(), tmpBucket.begin(), tmpBucket.end());
                currentIdx++;
                count++;
            }
            if (thisLength==currentLength+1)
            {
                SentenceBucket& nextBucket = lengthBucket[currentIdx];
                nextBucket.insert(nextBucket.end(), tmpBucket.begin(), tmpBucket.end());
            }
            
            tmpBucket.clear();
            tmpBucket.push_back(*itr);
            currentLength = thisLength;
        }
    }
    lengthBucket.resize(count);
}
//
void SimilarSentences::writeToFileLengthBucket()
{
    ofstream file(filename+"LengthBucket.txt");
    
    for (vector<SentenceBucket>::const_iterator itr=lengthBucket.begin(); itr!=lengthBucket.end(); ++itr)
    {
        for(SentenceBucket::const_iterator itr2=itr->begin(); itr2!=itr->end(); ++itr2)
            file << itr2->getLength() << " " << itr2->toString() << endl;
        file <<endl;
    }
    
    file.close();
}
