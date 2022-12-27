#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>
#include <cstdio>

int main(){
    while(1){
        std::cout << "К какому серверу хотите подключться: 1 или 2\n" 
        << "Введите цифру: ";
        int serv;
        std::cin >> serv;
        serv = serv == 1? 18656: serv == 2? 19556: 0;
        if (serv == 0)
            break; 
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if(s < 0){
            std::cerr << "Error calling socket\n";
            return 1;
        }
     
        struct sockaddr_in peer;
        peer.sin_family = AF_INET;
        peer.sin_port = htons(serv);
        peer.sin_addr.s_addr = inet_addr("127.0.0.1");

        int result = connect(s, (struct sockaddr*) &peer, sizeof(peer));
        if(result){
            std::cerr << "Error calling connect\n";
            return 1;
        }
        if(serv == 18656){
        while(1){
            int otv;
            std::cout << "Введите цифру информации, которую хотите узнать:\n" 
            << "\t1 - Количество жестких дисков и их файловые системы;\n"
            << "\t2 - Количество логических процессоров;\n" 
            << "\t3 - Выход.\n"
            << "Введите число: ";
        
            std::cin >> otv;
            result = send(s, &otv, sizeof(int), 0);
 
            if(otv != 1 && otv != 2)
                break; 
            int len = 1; 
            if(otv == 1){ 
                recv(s, &len, sizeof(int), 0);
            }
            char* buf = new char[len];
            recv(s, buf, len, 0);
       
            if(otv == 1){ 
                for(int i = 0; i < len; ++i)
                    std::cout << buf[i];
            } else if(otv == 2)
                std::cout << "Количество логических процессоров: " << buf[0] << "\n";
            delete[] buf;
        }
        } else{
            while(1){
                int otv;
                std::cout << "Введите цифру информации, которую хотите узнать:\n"
                << "\t1 - Объём физической памяти;\n"
                << "\t2 - Объём свободной физической памяти;\n"
                << "\t3 - Выход.\n"
                << "Введите число: ";
                std::cin >> otv;
                send(s, &otv, sizeof(int), 0);
 
                if(otv != 1 && otv != 2)
                    break;
 
                std::cout << "Выберете величину, в которой вы хотите получить ответ:    \n"
                << "\t1 - В Байтах;\n" << "\t2 - В Мегабайтах\n"
                << "\t3 - В Гигабайтах\n" << "Введите число: ";
                char unit [2];
                int otv1;
                std::cin >> otv1;
 
                unit[0] = '-';
                switch(otv1){
                    case 1:
                        unit[1] = 'b';
                        break;
                    case 2:
                        unit[1] = 'm';
                        break; 
                    case 3:
                        unit[1] = 'g';
                        break;
                }
                send(s, unit, 2, 0);
 
                int len[2];
 
                recv(s, len, sizeof(int)*2, 0);
                
                if(len[1] == 1){
                    char time1[18];
                    time_t timer;
                    time(&timer);
                    struct tm* localTime = localtime(&timer);
                    strftime(time1, 18, "%D %H:%M:%S", localTime);
                    for(int i = 0; i < 18; ++i)
                        std::cout << time1[i];
                    std::cout << " ";
                } else if(len[1] == 0){
                    std::cout << "Изменений не произошло\n";
                    continue;
                }
                
                char* buf = new char[len[0]];
 
                recv(s, buf, len[0], 0);
                
                if(otv == 1){
                    switch(unit[1]){
                        case 'b':
                            std::cout << "Объём физической памяти: "; 
                            for(int i = 0; i < len[0]; ++i)
                                std::cout << buf[i];
                            std::cout << " Байт\n";
                            break;
                        case 'm':
                            std::cout << "Объём физической памяти: "; 
                            for(int i = 0; i < len[0]; ++i)
                                std::cout << buf[i];
                            std::cout << " Мегабайт\n";
                            break;
                        case 'g':
                            std::cout << "Объём физической памяти: "; 
                            for(int i = 0; i < len[0]; ++i)
                                std::cout << buf[i];
                            std::cout << " Гигабайт\n";
                            break;
                    }

                } else if(otv == 2){
                    switch(unit[1]){
                        case 'b':
                            std::cout << "Объём свободной физической памяти: ";
                            for(int i = 0; i < len[0]; ++i)
                                std::cout << buf[i];
                            std::cout << " Байт\n";
                            break;
                        case 'm':
                            std::cout << "Объём свободной физической памяти: ";
                            for(int i = 0; i < len[0]; ++i)
                                std::cout << buf[i];
                            std::cout << " Мегабайт\n";
                            break;
                        case 'g':
                            std::cout << "Объём свободной физической памяти: ";
                            for(int i = 0; i < len[0]; ++i)
                                std::cout << buf[i];
                            std::cout << " Гигабайт\n";
                            break;
 
                    } 
                } 
                delete[] buf;
            }
        }    
        if(shutdown(s, 1) < 0){
            std::cerr << "Error calling shutdown\n";
            return 1;
        }
    }
    return 0;
}
