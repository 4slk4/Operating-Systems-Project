#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using namespace std;

const int QT0 = 8;
const int QT1 = 16;

struct process{
    string name;
    int id=0; 
    int AT=0,BT=0,WT=0,TAT=0,RT=0,CT= 0; /* Arrival time, Burst Time, Wait Time,
                                            Turn Around Time, Run Time, Complete Time */
    int interupt = 0; //number of interuption
    int t0=0, t1=0, t2=0; //Time spent in each queue
    int q_complete = 0;
};


void ML_feedback_queue(vector<process> px){
    //sort by time arrival
    sort(px.begin(),px.end(), [](const process &lhs, const process &rhs){
                                return lhs.AT < rhs.AT;});
    vector<process> complete;
    queue<process> q0, q1, q2;

    //Initialize q0
    for (auto p : px) {
        p.RT = p.BT;
        q0.push(p);
    }

    int time = q0.front().AT; //To count the time passes.
    
    //Iterate over queue 0 with quantum time QT0=8
    while (!q0.empty()){
        process temp = q0.front();
        
        if (temp.BT <= QT0){
            temp.t0 = temp.BT;
            time += temp.BT;/*from arrival time of first process to completion of this process*/
            temp.RT = 0;
            temp.CT = temp.RT;
            temp.WT = time - temp.AT - temp.BT;/*amount of time process has been waiting in the first queue*/
            temp.TAT = time - temp.AT;/*amount of time to execute the process*/
            temp.interupt = 0;
            temp.q_complete = 0;
            complete.push_back(temp);
        }
        else {
            temp.t0 = QT0;
            time += QT0;
            temp.RT -= QT0;
            temp.CT = time;
            temp.interupt++;
            q1.push(temp); //Move to q1
        }
        
        q0.pop();
    }

    //Iterate over queue 1 with quantum time QT1=16
    while (!q1.empty()){
        process temp = q1.front();
        if (temp.RT <= QT1){
            temp.t1 = temp.RT;
            time += temp.RT; /*from arrival time of first process to completion of this process*/
            temp.CT = time;
            temp.RT = 0;
            temp.TAT = temp.CT - temp.AT; /*amount of time to execute the process*/
            temp.WT = temp.TAT - temp.BT;
            temp.q_complete = 1;
            complete.push_back(temp);
        }
        else {            
            temp.t1 = QT1;
            time += QT1;
            temp.RT -= QT1;
            temp.CT = time;
            temp.interupt++;
            q2.push(temp); //Move to q2
        }
        q1.pop();
    }

    //Iterate over q2 using FCFS
    while (!q2.empty()){
        process temp = q2.front();
        temp.t2 = temp.RT;
        time += temp.RT;
        temp.RT = 0;
        temp.CT = time;
        temp.TAT = temp.CT - temp.AT;
        temp.WT = temp.TAT - temp.AT;
        temp.q_complete = 2;
        complete.push_back(temp);
        q2.pop();
    }

    //Sort the complete result in order
    sort(complete.begin(), complete.end(), [](const process &lhs, const process &rhs)
                                            { return lhs.id < rhs.id; });

    for (auto p : complete){
        cout << p.name << " at Queue 0 it is executed for " << p.t0 << endl;
        cout << p.name << " at Queue 1 it is executed for " << p.t1 << endl;
        cout << p.name << " at Queue 2 it is executed for " << p.t2 << endl;
        cout << p.name << " is interrupted " << p.interupt 
            << " times,  completes on queue " << p.q_complete 
            << ", and TAT is " << p.TAT << endl;

    }

}

/* Test cases */
// int main(){
//     vector<process> q0,q1;
//     queue<process> q2;
//     process a;
//     a.name = "A";
//     a.id = 1;
//     a.AT = 0;
//     a.BT = 40;

//     process b;
//     b.name = "B";
//     b.id = 2;
//     b.AT = 2;
//     b.BT = 10;

//     process c;
//     c.name = "C";
//     c.id = 3;
//     c.AT = 4;
//     c.BT = 15;

//     q0.push_back(a);
//     q0.push_back(b);
//     q0.push_back(c);

//     ML_feedback_queue(q0);
// }