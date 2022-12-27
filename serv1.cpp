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
    for(;;){
        int res;
        recv(cl, &res, sizeof(int), 0);
        if(res == 3){
            std::cout << "Exit client\n";
            close(cl);
            break;
        }
        if(res == 1){
            system("lsblk -o NAME,FSTYPE > inf1_1.txt");
            std::ifstream fin;
            fin.open("inf1_1.txt");
            std::string str;
            char symb;
            while(fin.get(symb))
                str.push_back(symb);
            fin.close();
            int len = str.size();
            write(cl, &len, sizeof(int));
            const char* cstr = str.c_str();
            write(cl, cstr, str.size());
        }

        if(res == 2){
            system("nproc --all > inf1_2.txt");
            std::ifstream fin;
            fin.open("inf1_2.txt");
            char count;
            fin.get(count);
            fin.close();
            send(cl, &count, 1, 0);
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
    addr.sin_port = htons(18656);
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
