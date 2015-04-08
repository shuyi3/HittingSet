//
//  main.cpp
//  HittingSet
//
//  Created by shuyi on 15/3/18.
//  Copyright (c) 2015年 shuyi. All rights reserved.
//
#include <iostream>
#include <map>
#include <cassert>
#include "HittingSet.h"
#include <fstream>
#include <string>
#include <sstream>
#include <tr1/unordered_map>
#include <tr1/random>
//#include <unordered_map>
//#include <random>
#include <time.h>
#include<dirent.h>


typedef uint64_t Move;
//typedef pair<int, vector<uint64_t>> scoreStats;

unsigned long long rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

//template<typename Iter, typename RandomGenerator>
//Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
//    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
//    std::advance(start, dis(g));
//    return start;
//}
//
//template<typename Iter>
//Iter select_randomly(Iter start, Iter end) {
//    static std::random_device rd;
//    static std::mt19937 gen(rd());
//    return select_randomly(start, end, gen);
//}



template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    uniform_int<> dis(0, std::distance(start, end) - 1);
    advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static random_device rd;
    static mt19937 gen;
    gen.seed(rdtsc());
    return select_randomly(start, end, gen);
}

HSState play(HSState p, Move element)
{
    assert(p.elementCount > 0);
    HSState after = HSState(p);
    after.Hit(element);
    return after;
}

// random playout from position p.
int sample(HSState p, list<Move>& playedMoves)
{
    while (!p.IsAllHit()) {
        
//        srand ( time(NULL) );
//        int RandIndex = rand() % p.elementArray.size();
//        unordered_set<Move>::iterator item = p.elementArray.begin();
//        std::advance(item, RandIndex);
//        Move element = *item;
        
        Move element = *select_randomly(p.elementArray.begin(), p.elementArray.end());
//        cout<<"element: "<<element<<endl;
        p.Hit(element);
        playedMoves.push_back(element);
    }
    return p.score;
}

int nested (HSState p, int level, list<Move>& Moves)
{
    list<Move> playedMoves;
    
    unordered_map<uint64_t, AMAFStats> AMAFtable;
    for ( auto it = p.elementArray.begin(); it != p.elementArray.end(); ++it ){
        uint64_t key = *it;
        AMAFtable.insert(make_pair(key, AMAFStats()));
    }
    
    int bestGlobalScore = std::numeric_limits<int>::max();
    if (p.IsAllHit())
        bestGlobalScore = p.score;
    
    while (!p.IsAllHit())
    {
        int bestScore = std::numeric_limits<int>::max();
//        Move bestMove = *p.elementArray.begin();
        list<Move> bestMoves;
//        cout<<"begin: "<<level<<endl;
//        for (Move move : p.elementArray) // try move
        for (unordered_map<uint64_t, AMAFStats>::iterator it = AMAFtable.begin(); it != AMAFtable.end(); ++it) // try move
        {
            if (it->second.numVisit != 0) {
                continue;
            }
//            if (level == 2)
//                cout<<"move: "<<it->first<<" level="<<level<<endl;
            Move move = it->first;
            int score;
            list<Move> sampledMove = playedMoves;
            if (level == 1){
                score = sample(play(p, move), sampledMove);
            }
            else{
                score = nested(play(p, move), level - 1, sampledMove);
            }
            
            sampledMove.push_front(move);
            if (score < bestScore)
            {
//                bestMove = move;
                bestScore = score;                //update move seq
                bestMoves = sampledMove;
//                cout<<endl;
            }
            
            for (Move move : sampledMove){
                if (score < AMAFtable[move].bestScore){
                    AMAFtable[move].bestScore = score;
                }
                AMAFtable[move].numVisit++;
                AMAFtable[move].totalScore += score;
            }
        }
        
//        if (level == 2){
//            cout<<"table"<<endl;
//            for (unordered_map<uint64_t, AMAFStats>::iterator it = AMAFtable.begin(); it != AMAFtable.end(); ++it)
//            {
//                cout<<"m:"<<it->first<<","<<it->second.bestScore<<",v:"<<it->second.numVisit<<endl;
//            }
//        }
        
//        if (level == 2)
//            cout<<"array"<<endl;
        AMAFStats bestMoveStats = AMAFtable[*p.elementArray.begin()];
        Move bestMove = *p.elementArray.begin();
        for (Move element : p.elementArray){
//        for (unordered_map<uint64_t, AMAFStats>::iterator it = AMAFtable.begin(); it != AMAFtable.end(); ++it){
            
//            cout<<"m:"<<it->first<<","<<it->second.bestScore<<",v:"<<it->second.numVisit<<endl;
            AMAFStats moveStats = AMAFtable[element];
//            if (level == 2)
//                cout<<"m:"<<element<<","<<moveStats.bestScore<<",v:"<<moveStats.numVisit<<endl;
            
            if (moveStats.bestScore < bestMoveStats.bestScore){
                bestMove = element;
                bestMoveStats = moveStats;
//                cout<<"best:"<<bestMove<<endl;
            }else if (moveStats.bestScore == bestMoveStats.bestScore){
                if ((float)(moveStats.totalScore/moveStats.numVisit) < (float)(bestMoveStats.totalScore/bestMoveStats.numVisit)){
                    bestMove = element;
                    bestMoveStats = moveStats;
//                    cout<<"best:"<<bestMove<<endl;
                }
            }
        }
//        if (level == 2)
//            cout<<"best:"<<bestMove<<endl;
        if (bestScore < bestGlobalScore)
        {
            bestGlobalScore = bestScore;
            Moves = bestMoves;
        }
//        cout<<"final: ";
        p = play (p, bestMove);
        playedMoves.push_back(bestMove);
//        cout<<"play:"<<bestMove<<" hit:"<<p.setList.size()<<endl;
    }
    assert(bestGlobalScore < std::numeric_limits<int>::max());
    return bestGlobalScore;
}

//int iterativeSampleStatistics()
//{
//    const int DEPTH = 60;
//    const int TRIES = 10000;
//    std::map<int, int> m;
//    const position p(DEPTH, 0);
//    
//    for (int n = 0; n < TRIES; ++n)
//    {
//        int count = sample(p);
//        ++m[count];
//    }
//    
//    for (auto p : m)
//        std::cout << p.first << " generated " << p.second << " times\n";
//    
//    return 0;
//}

int nmcStatistics(int level, int tries, HSState& initState)
{
    
    for (int n = 0; n < tries; ++n)
    {
        HSState p = HSState(initState);
        list<Move> playedMoves;
        int count = nested(p, level, playedMoves);
//        ++m[count];
        cout<<"score: "<<count<<endl;
        cout<<"move: ";
        for (Move move : playedMoves){
            cout<<move<<"->";
        }
        cout<<endl;
    }
    
    return 0;
}

void printState(HSState& state){
    for (set<uint64_t> mSet : state.setList){
        for (uint64_t element : mSet){
            cout<<element<<" ";
        }
        cout<<endl;
    }
}

int main()
{
    list<set<uint64_t> > mSetList;
    
    //test data
    /*
    10 5
    5 7 8 9 10
    5 7
    1 3 6 10
    4 6 7 9 10
    1 2 8 10
     */
    
    
    std::ifstream file("/Users/shuyi/Documents/CMPUT657/HittingSet/output/stepanov_vector.idemGA.report.txt");
//    std::ifstream file("/Users/shuyi/Documents/CMPUT657/HittingSet/output/fftbench.idemGA.report.txt");
//    std::ifstream file("/Users/shuyi/Documents/CMPUT657/HittingSet/output/lpbench.idemGA.report.txt");

    std::string str;
    bool beginParse = false;
    while (std::getline(file, str))
    {
        if (std::strncmp("===", str.c_str(), 3) == 0){
            break;
        }
        
        if (beginParse){
            
            set<uint64_t> mSet;
            
            stringstream ssin(str);
            while (ssin.good()){
                uint64_t element;
                ssin >> element;
                if (element == 0) break;
                mSet.insert(element);
            }
            
            if (!mSet.empty())
                mSetList.push_back(mSet);

        }
        if (std::strncmp("Sets:", str.c_str(), 4) == 0){
            //begin
            beginParse = true;
        }
    }
    

    
//    set<uint64_t> s1,s2,s3,s4,s5;
    
//    uint64_t a1[] = {5, 7, 8, 9, 10};
//    uint64_t a2[] = {5, 7};
//    uint64_t a3[] = {1, 3, 6, 10};
//    uint64_t a4[] = {4, 6, 7, 9, 10};
//    uint64_t a5[] = {1, 2, 8, 10};
//
//    set<uint64_t> s1(a1, a1+5);
//    set<uint64_t> s2(a2, a2+2);
//    set<uint64_t> s3(a3, a3+4);
//    set<uint64_t> s4(a4, a4+5);
//    set<uint64_t> s5(a5, a5+4);
//
//    mSetList.push_back(s1);
//    mSetList.push_back(s2);
//    mSetList.push_back(s3);
//    mSetList.push_back(s4);
//    mSetList.push_back(s5);

//    mSetList = {s1, s2, s3, s4, s5};
    
    cout<<"sets:"<<endl;
    
    int i = 1;
    for (set<uint64_t> mSet : mSetList){
        cout<<i<<": ";
        for (uint64_t element : mSet){
            cout<<element<<" ";
        }
        cout<<endl;
        i++;
    }
    
    cout<<"end sets"<<endl;
    
    HSState newState;
    newState.InitSetList(mSetList);
    
    cout<<"number of elements:"<<newState.elementArray.size()<<endl;
    
    
    DIR *dir;
    struct dirent *ent;
    string dirName = "/Users/shuyi/Documents/CMPUT657/HittingSet/output/";
    if ((dir = opendir (dirName.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            
            list<set<uint64_t> > mSetList;
            
            string fileName(ent->d_name);
            string dirName = "/Users/shuyi/Documents/CMPUT657/HittingSet/output/";
            dirName.append(fileName);
            
            std::ifstream file(dirName.c_str());
            //    std::ifstream file("/Users/shuyi/Documents/CMPUT657/HittingSet/output/fftbench.idemGA.report.txt");
            //    std::ifstream file("/Users/shuyi/Documents/CMPUT657/HittingSet/output/lpbench.idemGA.report.txt");
            
            std::string str;
            bool beginParse = false;
            while (std::getline(file, str))
            {
                if (std::strncmp("===", str.c_str(), 3) == 0){
                    break;
                }
                
                if (beginParse){
                    
                    set<uint64_t> mSet;
                    
                    stringstream ssin(str);
                    while (ssin.good()){
                        uint64_t element;
                        ssin >> element;
                        if (element == 0) break;
                        mSet.insert(element);
                    }
                    
                    if (!mSet.empty())
                        mSetList.push_back(mSet);
                    
                }
                if (std::strncmp("Sets:", str.c_str(), 4) == 0){
                    //begin
                    beginParse = true;
                }
            }
            
            
            printf ("%s\n", ent->d_name);
            
            cout<<"size: "<<mSetList.size()<<endl;
            
            HSState newState;
            newState.InitSetList(mSetList);
            nmcStatistics(1, 1, newState);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }
    
//    newState.Hit(5);
//    
//    
//    
//    printState(newState);
//    
//    newState.Hit(10);simple_types_constant_folding.idemGA.report
//    
//    printState(newState);
//    
//    if (newState.IsAllHit()){
//        cout<<"all hit"<<endl;
//    }


    
//    for (set<uint64_t> mSet : newState.setList){
//        for (uint64_t element : mSet){
//            cout<<element<<" ";
//        }
//        cout<<endl;
//    }
//    
//    for (uint64_t element : newState.elementArray){
//        cout<<element<<" ";
//    }
//    cout<<endl;
//    
//    HSState copyState = HSState(newState);
//
//    for (set<uint64_t> mSet : copyState.setList){
//        for (uint64_t element : mSet){
//            cout<<element<<" ";
//        }
//        cout<<endl;
//    }
//    
//    for (uint64_t element : copyState.elementArray){
//        cout<<element<<" ";
//    }
//    cout<<endl;
    
    
    
    
    //iterativeSampleStatistics();
//    nmcStatistics(1, 50, newState);
//    nmcStatistics(2, 1000);
    //nmcStatistics(3, 10);
    return 0;
}