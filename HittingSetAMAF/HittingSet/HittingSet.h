//
//  HittingSet.h
//  HittingSet
//
//  Created by shuyi on 15/3/18.
//  Copyright (c) 2015年 shuyi. All rights reserved.
//

#ifndef __HittingSet__HittingSet__
#define __HittingSet__HittingSet__

#include <stdio.h>
#include <list>
#include <set>
//#include <unordered_set>
#include <tr1/unordered_set>
#include <vector>

#include <iostream>


using namespace std;
using namespace tr1;

class AMAFStats{
    
public:
    int bestScore;
    int numVisit;
    int totalScore;

    AMAFStats():bestScore(std::numeric_limits<int>::max()), numVisit(0), totalScore(0){}
    
};

class HSState{

public:
    HSState():score(0){}
    ~HSState(){}
    
    HSState(const HSState& mHSState){
        elementCount = mHSState.elementCount;
        setList = mHSState.setList;
        elementArray = mHSState.elementArray;
        score = mHSState.score;
    }
    
    list<set<uint64_t> > setList;
    unordered_set<uint64_t> elementArray;
    int elementCount;
    int score;
    
    void InitSetList(list<set<uint64_t> >& _setList){
        elementCount = 0;
        setList = _setList;
        for (set<uint64_t> mSet : setList){
            for (uint64_t element : mSet){
                unordered_set<uint64_t>::const_iterator got = elementArray.find(element);
                if ( got == elementArray.end() ){
                    elementArray.insert(element);
                    ++elementCount;
                }//else find
            }
        }
    }
    
    void Hit(uint64_t element){
        for(auto i = setList.begin(); i != setList.end();)
        {
            std::set<uint64_t>::iterator got = i->find(element);
            if(got != i->end()){//hit
                i = setList.erase(i);
            }
            else{
                ++i;
            }
        }
        elementArray.erase(element);
        for (uint64_t e : elementArray){
            bool valid = false;
            for(auto i = setList.begin(); i != setList.end();)
            {
                std::set<uint64_t>::iterator got = i->find(e);
                if(got != i->end()){//hit
                    valid = true;
                    break;
                }
                else{
                    ++i;
                }
            }
            if (!valid) elementArray.erase(e);
        }
        ++score;
    }
    
    bool IsAllHit(){
        return setList.empty();
    }

};

#endif /* defined(__HittingSet__HittingSet__) */
