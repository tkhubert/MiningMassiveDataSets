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
}
//
Sentence::Sentence(string s, int c)
    : str(s), count(c)
{
    stringstream stream(str);
    vector<string> vec{istream_iterator<string>(stream), istream_iterator<string>()};
    length =  int(vec.size());
    
    hash<string> hashFunc;
    string half1, half2;
    
    if (length<10)
        throw "Problem";
    
    for (int i=0; i<NB_WORDS; ++i)
        half1 += vec[i] + " ";
    for (int i=length-NB_WORDS; i<length; ++i)
        half2 += vec[i] + " ";
    
    hash1 = hashFunc(half1) % NB_BUCKETS;
    hash2 = hashFunc(half2) % NB_BUCKETS;
}
//
string Sentence::toString() const
{
    stringstream ss;
    ss << str << " " << count;
    return ss.str();
}
//
bool Sentence::isEditDistInf1(const Sentence& s) const
{
    const Sentence& sent1 = (s.getLength()< getLength()) ? s : *this;
    const Sentence& sent2 = (s.getLength()< getLength()) ? *this : s;
    size_t n1 = sent1.getLength();
    size_t n2 = sent2.getLength();
    
    if (n2-n1>1)
        return false;
    
    stringstream ss1(sent1.getStr());
    stringstream ss2(sent2.getStr());
    
    int nbEdit = 0;
    if (n1==n2)
    {
        for (int i=0; i<n1; ++i)
        {
            string s1, s2;
            ss1 >> s1;
            ss2 >> s2;
            
            if (s1!=s2)
                nbEdit++;
        }
        
        return nbEdit<=1;
    }
    else if (n1+1==n2)
    {
        for (int i=0; i<n1; ++i)
        {
            string s1, s2;
            ss1 >> s1;
            ss2 >> s2;
            
            if (s1!=s2)
            {
                ss2 >> s2;
                nbEdit++;
            }
        }
        return nbEdit<=1;
    }
    else
        return false;
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
SentencePair::SentencePair(Sentence s1, Sentence s2) : sPair(make_pair(s1,s2))
{
    int s1Count = s1.getCount();
    int s2Count = s2.getCount();
    
    if (s1==s2)
        count = s1Count*(s1Count-1)/2;
    else
        count = s1Count*s2Count;
}
//
string SentencePair::toString() const
{
    stringstream ss;
    if (sPair.first==sPair.second)
        ss << sPair.first.toString() << endl;
    else
    {
        ss << sPair.first.toString()  << endl;
        ss << sPair.second.toString() << endl;
    }
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
void SimilarSentences::findSmilarSentences()
{
    findAndProcessDuplicates();
    hashToLengthBuckets();
    hashToStringBuckets();
    countSimilarSentences();
    //writeToFilePairBucket();
}
//
void SimilarSentences::findAndProcessDuplicates()
{
    sort(wholeData.begin(), wholeData.end());
    
    string currentString = wholeData[0];
    int    currentCount  = 0;
    
    for (vector<string>::const_iterator str=wholeData.begin(); str!=wholeData.end(); ++str)
    {
        if (*str==currentString)
            currentCount++;
        else
        {
            Sentence sent = Sentence(currentString, currentCount);
            noDupliData.push_back(sent);
            
            if (currentCount>1)
                pairBucket.insert(SentencePair(sent, sent));
            
            currentCount  = 1;
            currentString = *str;
        }
    }
    
    noDupliDataSize = noDupliData.size();
    wholeData.clear();
}
//
void SimilarSentences::hashToLengthBuckets()
{
    FuncLength f;
    
    int    bucketCount = 0;
    int    currentIdx  = 0;
    
    lengthBucket.resize(noDupliDataSize);

    sort(noDupliData.begin(), noDupliData.end(), FuncCompare<FuncLength>(f));
    
    int currentVal = f(noDupliData[0]);
    SentenceBucket   tmpBucket;
    for (SentenceBucket::iterator sentence=noDupliData.begin(); sentence!=noDupliData.end(); ++sentence)
    {
        SentenceBucket& currentBucket = lengthBucket[currentIdx];
        
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
                SentenceBucket& nextBucket = lengthBucket[currentIdx];
                nextBucket.insert(nextBucket.end(), tmpBucket.begin(), tmpBucket.end());
            }
            
            tmpBucket.clear();
            currentVal = thisVal;
        }
        tmpBucket.push_back(*sentence);
    }
    
    if (tmpBucket.size()>1)
    {
        SentenceBucket& currentBucket = lengthBucket[currentIdx];
        currentBucket.insert(currentBucket.end(), tmpBucket.begin(), tmpBucket.end());
        currentIdx++;
        bucketCount++;
    }

    lengthBucket.resize(bucketCount);
    noDupliData.clear();
}
//
void SimilarSentences::hashToStringBuckets()
{
    for (vector<SentenceBucket>::const_iterator bucket=lengthBucket.begin(); bucket!=lengthBucket.end(); ++bucket)
    {
        vector<SentenceBucket> hashTable1;
        vector<SentenceBucket> hashTable2;
        hashTable1.resize(NB_BUCKETS);
        hashTable2.resize(NB_BUCKETS);
        
        for (SentenceBucket::const_iterator sentence=bucket->begin(); sentence!=bucket->end(); ++sentence)
        {
            int hash1 = sentence->getHash1();
            int hash2 = sentence->getHash2();
            
            hashTable1[hash1].push_back(*sentence);
            hashTable2[hash2].push_back(*sentence);
        }
        
        for (int i=0; i<NB_BUCKETS; ++i)
        {
            const SentenceBucket& set1 = hashTable1[i];
            const SentenceBucket& set2 = hashTable2[i];
            
            bruteForce(set1);
            bruteForce(set2);
        }
    }
}
//
void SimilarSentences::bruteForce(const SentenceBucket& bucket)
{
    size_t bucketSize = bucket.size();
    
    for (int i=0; i<bucketSize; ++i)
    {
        const Sentence& s1=bucket[i];
        
        for (int j=i+1; j<bucketSize; ++j)
        {
            const Sentence& s2=bucket[j];
            
            if (s1.isEditDistInf1(s2))
                pairBucket.insert(SentencePair(s1,s2));
        }
    }
}
//
void SimilarSentences::countSimilarSentences()
{
    finalCount=0;
    
    for (set<SentencePair>::const_iterator pair=pairBucket.begin(); pair!=pairBucket.end(); ++pair)
        finalCount+=pair->getCount();
    
    cout << "nbSentences at 1 or 0 edit dist: " << finalCount << endl;
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
void SimilarSentences::writeToFilePairBucket()
{
    int count=0;
    ofstream file(filename+"PairBucket.txt");
    
    for (set<SentencePair>::const_iterator pair=pairBucket.begin(); pair!=pairBucket.end(); ++pair)
    {
        count+=2;
        file <<  pair->toString() << endl;
    }
    
    file.close();
    cout << "PairBucketSize: " << count << endl;
}

