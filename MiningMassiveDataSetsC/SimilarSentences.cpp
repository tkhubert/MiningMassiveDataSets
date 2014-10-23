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
    bool stringCompare(const Sentence& s1, const Sentence& s2)
    {
        return s1.getStr() < s2.getStr();
    }
    //
    bool freqCompare(const Sentence& s1, const Sentence& s2)
    {
        return s1.getFreq() > s2.getFreq();
    }
    //
    template<typename FuncType>
    struct FuncCompare
    {
        FuncType f;
        
        FuncCompare(FuncType func) : f(func) {}
        
        bool operator()(const Sentence&s1, const Sentence& s2)
        {
            return f(s1) < f(s2);
        }
    };
    //
    struct FuncLength
    {
        int operator()(const Sentence& s1) const {return s1.getLength();}
    };
    //
    struct FuncEditDist
    {
        int i;
        
        FuncEditDist(int idx) : i(idx) {}
        
        int operator()(const Sentence& s1) const {return s1.getEDist(i);}
    };
}
//
Sentence::Sentence(string s, int c)
    : str(s), count(c)
{
    freq     = 0;
    editDist.resize(NB_FREQ_WORDS);
    
    stringstream stream(str);
    length =  int(distance(istream_iterator<string>(stream), istream_iterator<string>()));
}
//
string Sentence::toString() const
{
    stringstream ss;
    ss << str << " " << count;
    return ss.str();
}
//
int Sentence::editDistTo(const Sentence& s) const
{
    size_t n =   getLength()+1;
    size_t m = s.getLength()+1;
    
    vector<int> data(n*m);
    
    for (int i=0; i<n; ++i)
        data[i*m] = i;
    for (int j=0; j<m; ++j)
        data[j] = j;
    
    stringstream ssi(str);
    for (int i=1; i<n; ++i)
    {
        string str_i;
        ssi >> str_i;
        
        stringstream ssj(s.str);
        for (int j=1; j<m; ++j)
        {
            string str_j;
            ssj >> str_j;
            int d1 = min(data[i*m+j-1], data[(i-1)*m+j])+1;
            int d2 = data[(i-1)*m+j-1] + (str_i!=str_j? 1 : 0);
            data[i*m+j] = min(d1,d2);
        }
    }
    return data[n*m-1];
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
    
    noDupliDataSize = noDupliData.size();
    wholeData.clear();
}
//
void SimilarSentences::writeToFileNoDupliData()
{
    ofstream file(filename+"noDupli.txt");
    int i=0;
    for (SentenceBucket::const_iterator itr=noDupliData.begin(); itr!=noDupliData.end(); ++itr)
        file << i++ << " " <<itr->toString() << endl;
    
    file.close();
    cout << "noDupliSize: " << noDupliData.size() << endl;
}
//
template<typename FuncType>
void SimilarSentences::generateBuckets(vector<SentenceBucket>& input, vector<SentenceBucket>& output, const FuncType& f) const
{
    int    bucketCount = 0;
    int    currentIdx  = 0;
    
    output.resize(noDupliDataSize);
    
    for (vector<SentenceBucket>::iterator bucket=input.begin(); bucket!=input.end(); ++bucket)
    {
        sort(bucket->begin(), bucket->end(), FuncCompare<FuncType>(f));
        
        int currentVal = f((*bucket)[0]);
        SentenceBucket   tmpBucket;
        for (SentenceBucket::iterator sentence=bucket->begin(); sentence!=bucket->end(); ++sentence)
        {
            SentenceBucket& currentBucket = output[currentIdx];
            
            int thisVal = f(*sentence);
            
            if (thisVal!=currentVal)
            {
                if (currentBucket.size()+tmpBucket.size()>1)
                {
                    currentBucket.insert(currentBucket.end(), tmpBucket.begin(), tmpBucket.end());
                    currentIdx++;
                    bucketCount++;
                }
                if (thisVal==currentVal+1)
                {
                    if (currentIdx>output.size())
                        output.resize(output.size()*2);
                    SentenceBucket& nextBucket = output[currentIdx];
                    nextBucket.insert(nextBucket.end(), tmpBucket.begin(), tmpBucket.end());
                }
                
                tmpBucket.clear();
                currentVal = thisVal;
            }
            tmpBucket.push_back(*sentence);
        }
        
        if (tmpBucket.size()>1)
        {
            SentenceBucket& currentBucket = output[currentIdx];
            currentBucket.insert(currentBucket.end(), tmpBucket.begin(), tmpBucket.end());
            currentIdx++;
            bucketCount++;
        }
    }
    output.resize(bucketCount);
}
//
void SimilarSentences::hashToLengthBuckets()
{
    FuncLength FLength;
    
    vector<SentenceBucket> noDupliDataV(1);
    noDupliDataV[0] = noDupliData;
    
    generateBuckets<FuncLength>(noDupliDataV, lengthBucket, FLength);
    noDupliData.clear();
}
//
void SimilarSentences::writeToFileLengthBucket()
{
    int count=0;
    ofstream file(filename+"LengthBucket.txt");
    
    for (vector<SentenceBucket>::const_iterator itr=lengthBucket.begin(); itr!=lengthBucket.end(); ++itr)
    {
        count+=itr->size();
        for(SentenceBucket::const_iterator itr2=itr->begin(); itr2!=itr->end(); ++itr2)
            file << itr2->getLength() << " " << itr2->toString() << endl;
        file <<endl;
    }
    
    file.close();
}
//
vector<string> SimilarSentences::findMostFrequentSentence(const SentenceBucket& input) const
{
    int bucketSize  = int(input.size());
    int length      = min(SIZE_OF_FREQ_WORD, input[0].getLength());
    
    vector<SentenceBucket> data    (length);
    vector<SentenceBucket> freqData(length);
    
    for (int i=0; i<length; ++i)
        data[i].reserve(bucketSize);
    
    for (SentenceBucket::const_iterator sentence=input.begin(); sentence!=input.end(); ++sentence)
    {
        stringstream ss(sentence->getStr());
        for (int i=0; i<length; ++i)
        {
            string word;
            ss >> word;
            data[i].push_back(word);
        }
    }
    
    int idx=0;
    for (vector<SentenceBucket>::iterator bucket=data.begin(); bucket!=data.end(); ++bucket)
    {
        sort(bucket->begin(), bucket->end(), stringCompare);

        SentenceBucket sentenceCount;
        
        int    currentCount = 0;
        string currentWord  = bucket->begin()->getStr();
        for (SentenceBucket::iterator sentence=bucket->begin(); sentence!=bucket->end(); ++sentence)
        {
            if (sentence->getStr()!=currentWord)
            {
                sentenceCount.push_back(Sentence(currentWord));
                sentenceCount.back().setFreq(currentCount);

                currentCount = 0;
                currentWord  = sentence->getStr();
            }
            currentCount++;
        }
        
        sort(sentenceCount.begin(), sentenceCount.end(), freqCompare);
        
        SentenceBucket tmpBucket;
        tmpBucket.insert(tmpBucket.end(), sentenceCount.begin(), sentenceCount.begin() + min(NB_FREQ_WORDS, int(sentenceCount.size())));
        freqData[idx]=tmpBucket;
        idx++;
    }
    
    vector<string> freqSentences(NB_FREQ_WORDS);
    for (int i=0; i<length; ++i)
    {
        const SentenceBucket& currentBucket = freqData[i];
        for (int j=0; j<currentBucket.size(); ++j)
            freqSentences[j] += currentBucket[j].getStr() + " ";
    }
    return freqSentences;
}
//
void SimilarSentences::hashToEditDistBuckets()
{
    vector<SentenceBucket> newInput;
    newInput.insert(newInput.begin(), lengthBucket.begin(), lengthBucket.end());
    
    eDistBucket.clear();
    eDistBucket.resize(noDupliDataSize);
    
    
    for (vector<SentenceBucket>::iterator bucket=newInput.begin(); bucket!=newInput.end(); ++bucket)
    {
        vector<string> freqStrings = findMostFrequentSentence(*bucket);
        SentenceBucket freqSentences;
        for (int i=0; i<freqStrings.size(); ++i)
            freqSentences.push_back(Sentence(freqStrings[i]));
        
        int freqSentencesSize = freqSentences.size();
        
        for (SentenceBucket::iterator sentence=bucket->begin(); sentence!=bucket->end(); ++sentence)
        {
            for (int i=0; i<freqSentencesSize; ++i)
            {
                const Sentence& pivot = freqSentences[i];
                int editDist = sentence->editDistTo(pivot);
                sentence->setEditDist(i, editDist);
            }
        }
    }
    
    vector<SentenceBucket>& input = newInput;
    vector<SentenceBucket>  output;
    for (int i=0; i<NB_FREQ_WORDS; ++i)
    {
        FuncEditDist FEditDist(i);
        
        output.clear();
        generateBuckets<FuncEditDist>(input, output, FEditDist);
        debugInfo(output);
        input = output;
    }

    eDistBucket.insert(eDistBucket.end(), output.begin(), output.end());
}
//
void SimilarSentences::writeToFileEditDistBucket()
{
    int count=0;
    ofstream file(filename+"EditDist.txt");
    
    for (vector<SentenceBucket>::const_iterator itr=eDistBucket.begin(); itr!=eDistBucket.end(); ++itr)
    {
        count+=itr->size();
        for(SentenceBucket::const_iterator itr2=itr->begin(); itr2!=itr->end(); ++itr2)
        {
            for (int i=0; i<NB_FREQ_WORDS; ++i)
                file << itr2->getEDist(i) << " ";
            file << itr2->getLength() << " " << itr2->toString() << endl;
        }
        file <<endl;
    }
    
    file.close();
    cout << "EditDistBucketSize: " << count << endl;
}
//
void SimilarSentences::debugInfo(const vector<SentenceBucket>& input) const
{
    size_t nbBuckets = input.size();
    vector<size_t> bucketsSize(nbBuckets);
    
    int i=0;
    for (vector<SentenceBucket>::const_iterator bucket=input.begin(); bucket!=input.end(); ++bucket)
    {
        bucketsSize[i] = bucket->size();
        i++;
    }
    int totalElements = accumulate(bucketsSize.begin(), bucketsSize.end(), 0.);
    cout << "nbBuckets: "<<nbBuckets << endl;
    cout << "nbElements: "<< totalElements << endl;
    cout << "Buckets max:"<< *max_element(bucketsSize.begin(), bucketsSize.end())<< endl;
    cout << "Buckets min:"<< *min_element(bucketsSize.begin(), bucketsSize.end())<< endl;
    cout << "Buckets avg:"<< totalElements/nbBuckets<< endl;
    cout << endl;
}
//
void SimilarSentences::bruteForce()
{
    SentenceBucket tmpBucket;
    for (vector<SentenceBucket>::const_iterator itr=eDistBucket.begin(); itr!=eDistBucket.end(); ++itr)
    {
        size_t n = itr->size();
        for (int i=0; i<n; ++i)
        {
            const Sentence& si = (*itr)[i];
            
            for (int j=i+1; j<n; ++j)
            {
                const Sentence& sj = (*itr)[j];
                int   editDist = si.editDistTo(sj);
                
                if (editDist<=1)
                {
                    tmpBucket.push_back(si);
                    tmpBucket.push_back(sj);
                    pairBucket.push_back(tmpBucket);
                    tmpBucket.clear();
                }
            }
        }
    }
}
//
void SimilarSentences::writeToFilePairBucket()
{
    int count=0;
    ofstream file(filename+"PairBucket.txt");
    
    for (vector<SentenceBucket>::const_iterator itr=pairBucket.begin(); itr!=pairBucket.end(); ++itr)
    {
        count+=itr->size();
        for(SentenceBucket::const_iterator itr2=itr->begin(); itr2!=itr->end(); ++itr2)
            file <<  itr2->toString() << endl;
        file <<endl;
    }
    
    file.close();
    cout << "PairBucketSize: " << count << endl;
}

