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
#include <tr1/unordered_map>
#include <vector>

#include <iostream>


using namespace std;
using namespace tr1;

class AMAFStats{
    
public:
    int bestScore;
    int numVisit;
    int totalScore;
    int hitScore;

    AMAFStats():bestScore(std::numeric_limits<int>::max()), numVisit(0), totalScore(0), hitScore(std::numeric_limits<int>::max()){}
    
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
    unordered_map<uint64_t,int> elementArray;
    int elementCount;
    int score;
    
    void InitSetList(list<set<uint64_t> >& _setList){
        elementCount = 0;
        setList = _setList;
        for (list<set<uint64_t> >::iterator listIter = _setList.begin(); listIter != _setList.end(); ++listIter){
            set<uint64_t> mSet = *listIter;
            for (set<uint64_t>::iterator setIter = mSet.begin(); setIter != mSet.end(); ++setIter){
                uint64_t element = *setIter;
                unordered_map<uint64_t,int>::const_iterator got = elementArray.find(element);
                if ( got == elementArray.end() ){
                    elementArray.insert(make_pair(element,1));
                    ++elementCount;
                }else{//else find
                    elementArray[element]++;
                }
            }
        }
    }
    
    int Hit(uint64_t element){
//        cout<<"hit:"<<element<<endl;
        int hitScore = 0;
        for(list<set<uint64_t> >::iterator listIter = setList.begin(); listIter != setList.end();)
        {
            std::set<uint64_t>::iterator got = listIter->find(element);
            if(got != listIter->end()){//hit
                set<uint64_t> mSet = *listIter;
                for (set<uint64_t>::iterator elementIter = mSet.begin(); elementIter != mSet.end(); ++elementIter){
                    elementArray[*elementIter]--;
                    if (elementArray[*elementIter] == 0){
//                        cout<<"erase:"<<*elementIter<<endl;
                        elementArray.erase(*elementIter);
                    }
                }

                listIter = setList.erase(listIter);
                hitScore++;
            }
            else{
                ++listIter;
            }
        }
        elementArray.erase(element);
//        for (uint64_t e : elementArray){
//            bool valid = false;
//            for(list<set<uint64_t> >::iterator listIter = setList.begin(); listIter != setList.end();)
//            {
//                std::set<uint64_t>::iterator got = listIter->find(e);
//                if(got != listIter->end()){//hit
//                    valid = true;
//                    break;
//                }
//                else{
//                    ++listIter;
//                }
//            }
//            if (!valid) elementArray.erase(e);
//        }
        ++score;
        return hitScore;
    }
    
    bool IsAllHit(){
        return setList.empty();
    }

};

#endif /* defined(__HittingSet__HittingSet__) */
