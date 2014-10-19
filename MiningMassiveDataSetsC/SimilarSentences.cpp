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
SimilarSentences::SimilarSentences(string filename)
{
    ifstream file(filename);
    
    copy(istream_iterator<string>(file), istream_iterator<string>(), wholeData);
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
        {
            currentCount++;
            continue;
        }
        
        noDuplicatesData.push_back(Sentence(currentString, currentCount));
        
        currentCount  = 1;
        currentString = *itr;
    }
}
//
void SimilarSentences::hashToLengthBuckets()
{
    sort(noDuplicatesData.begin(), noDuplicatesData.end(), lengthCompare);
    
    
}