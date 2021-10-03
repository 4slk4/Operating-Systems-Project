#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "mlfq_final.h"

using namespace std;

const int SIZE = 4096;
const char *name = "OS";


int main(int argc, char *argv[])
{
    if (argc != 2){
        cout << "Usage: ./ipc filename.txt" << endl;
        return -1;
    }
    else{

        pid_t pid;
        int fd;
        char *ptr;

        ifstream inFile(argv[1]);
        
        //Turn the file into a single string
        string content( (istreambuf_iterator<char>(inFile) ),
                       (istreambuf_iterator<char>()) );
        
        inFile.close();

        //Convert to a c-string
        const char* txt = content.data();


        //Create a shared memory object
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);    
        //Configure the size of the SMO
        ftruncate(fd,SIZE);
        //Map the SMO
        ptr = (char *)
            mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        //Create process
        pid = fork();
        
        if (pid < 0){
            fprintf(stderr,"Fork Failed!");
            return 1;
        }
        else if (pid == 0){ 
            /*Child process */
            
            //Producer
            sprintf(ptr, "%s", txt);
            ptr += strlen(txt);
            exit(0);
        }
        else { 
            /* Parent process */
            wait(NULL);
            
            //Consumer
            string receiver((char*) ptr);
            
            //cout << "This is what I got " << receiver << endl;

            /* remove the shared memory object */
            shm_unlink(name);

            vector<process> ps; // A vector of all input processes

            /* Parsing the string and initialize vector ps */
            stringstream sstream(receiver);            
            string line;
            int id = 0;
            string lim = ",";
            int lim_len = lim.length();
            
            while (getline(sstream,line)){
                string word = line;

                //Initialize the process px object
                process px;
                px.id += id;
                px.name = word.substr(0, word.find(lim));
                word.erase(0, word.find(lim) + lim_len);
                px.AT = stoi(word.substr(0, word.find(lim)));
                word.erase(0, word.find(lim) + lim_len);
                px.BT = stoi(word);
                px.RT = px.BT;
                
                //Add to the vector ps
                ps.push_back(px);
                id++; 
            }

        ML_feedback_queue(ps); //Run Multi Level Feedback Queue
            
        }
    }

    return 0;
}
