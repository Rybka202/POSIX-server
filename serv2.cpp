#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
 
void* func(void* arg){
    int cl = *(int*) arg;
    std::string lastRes1[3];
    std::string lastRes2[3];
    for(;;){
        int res;
        recv(cl, &res, sizeof(int), 0);
        if(res != 1 && res != 2){
            std::cout << "Exit client\n";
            close(cl);
            break;
        }
        if(res == 1){
            char buf[50];
            char unit [2];
            read(cl, unit, 2);
            int i;
            if(unit[1] == 'b')
                i = 0;
            if(unit[1] == 'm')
                i = 1;
            if(unit[1] == 'g')
                i = 2;
            sprintf(buf,"%s %s %s", "free", unit, "| awk 'NR == 2{print $2}' > inf2_1.txt");
            system(buf);
            std::ifstream fin;
            fin.open("inf2_1.txt");
            std::string str;
            while(!fin.eof())
                str.push_back(fin.get());
            str.erase(str.size()-1);
            str.pop_back();
            fin.close();
            int len[2];
            len[0] = str.size(); 
            len[1] = lastRes1[i] == str ? 0 : 1;
            if(lastRes1[i].size() == 0)
                len[1] = 2;
            lastRes1[i] = str;
            write(cl, len, sizeof(int)*2);
            if(len[1] == 1 || len[1] == 2){
                const char* cstr = str.c_str();
                write(cl, cstr, str.size());
            }
        }
 
        if(res == 2){
            char buf[50];
            char unit [2];
            read(cl, unit, 2);
            int i;
            if(unit[1] == 'b')
                i = 0;
            if(unit[1] == 'm')
                i = 1;
            if(unit[1] == 'g')
                i = 2;
            sprintf(buf,"%s %s %s", "free", unit, "| awk 'NR == 2{print $4}' > inf2_2.txt");
            system(buf);
            std::ifstream fin;
            fin.open("inf2_2.txt");
            std::string str;
            while(!fin.eof())
                str.push_back(fin.get());
            str.erase(str.size()-1);
            str.pop_back();
            fin.close();
            int len[2];
            len[0] = str.size();
            len[1] = lastRes2[i] == str ? 0 : 1;
            if(lastRes2[i].size() == 0)
                len[1] = 2;
            lastRes2[i] = str;
            write(cl, len, sizeof(int)*2);
            if(len[1] == 1 || len[1] == 2){
                const char* cstr = str.c_str();
                write(cl, cstr, str.size());
            }

        }
    } 
}

int main(){
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
        std::cerr << "Error calling socket\n";
            return 1;
    }
 
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(19556);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    if(bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0){
         std::cerr << "Error calling bind\n";
         return 1;
    }
 
    if(listen(s, 5)){
        std::cerr << "Error calling listen\n";
        return 1;
    }
    for(;;){
        std::cout << "Waiting accepting...\n";
        int cl = accept(s, NULL, NULL);
        if(cl < 0){
            std::cerr << "Error calling accept\n";
            return 1;
        }
        std::cout << "Yes, it accept me\n";
        pthread_t thread;
        pthread_create(&thread, NULL, func, &cl);
    }
    return 0;
}
