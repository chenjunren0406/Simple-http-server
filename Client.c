//
//  main.c
//  comp429Project01
//
//  Created by Junren Chen on 13-9-10.
//  Copyright (c) 2013年 Junren Chen. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>

int main(int argc, const char * argv[])
{

    // insert code here...
    
    int sock;
    
    int current_count = 1, i;
    
    struct sockaddr_in sin;
    
    struct hostent *host = gethostbyname(argv[1]);
    
    unsigned int server_addr = *(unsigned int *) host->h_addr_list[0]; // Ip address of server
    
    unsigned short server_port = atoi(argv[2]);  // port of server
    
    char *buffer, *user_buffer, *recvbuffer;
    
    int user_size = atoi(argv[3]); // the size of message which user want to input
    
    int size = atoi(argv[3]) + 10; // the size of ping-pong message
    
    int count = atoi(argv[4]); // how many times the program will run
    
    int recv_count = 0 ; // use to message received from server
    
    struct timeval start;  // use this struct to get the system time(_sec and _usec)
    
    struct timeval end; // this is used to get timezone, we wiil not use that into the ping-pong message
    
    int recv_sec;
    int recv_usec ; // use to store lantence of each ping-pong messages
    
    float current_latence;
    float latence = 0;
    
    
    
    // allocate space 
    buffer = (char *) malloc(size + 1);
    
    if(!buffer){
        perror("fail to allocated buffer");
        abort();
    }
    
    user_buffer = (char *) malloc(user_size + 1);
    
    if(!user_buffer){
        perror("fail to allocated user_buffer");
        abort();
    }
    memset(user_buffer,'c',user_size);// to fill the user data with "c"
    user_buffer[user_size] = '\0';
    
    recvbuffer = (char *) malloc(500);
    
    if(!recvbuffer){
        perror("fail to allocated frist_recvbuffer");
        abort();
    }
    
    // create a socket
    
    if((sock = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP)) < 0){ //fail to create a socket
        perror("opening TCP Socket");
        abort();
    }
    
    // set the information of server
    memset(&sin, 0 , sizeof(sin)); // initialize the space of sin with 0
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = server_addr;
    sin.sin_port = htons(server_port);
    
    
    
    // connect to server

    if (connect(sock, (struct sockaddr *) &sin, sizeof (sin) ) < 0) {
        perror("failed to connect to server");
        abort();
    }
    // connect has been established.
    
    printf("connecting to %s \n", *(argv+1));
    
    
    
    while(current_count < count + 1){
        
        *(unsigned short *)buffer = (unsigned short)htons(size);
            
        gettimeofday(&start,NULL);
        
        *(int *)(buffer+2) = htonl(start.tv_sec);
        
        *(int *)(buffer+6) = htonl(start.tv_usec);
        
        for (i=0; i<user_size; i++)
            buffer[10 + i] = user_buffer[i];
        
        send(sock,buffer,size,0);
        
        
        sleep(1);
        
        recv_count = (int)recv(sock,recvbuffer, 400, 0);
        
        if(recv_count < 0){
            perror("recieve error");
            abort();
        }
            gettimeofday(&end, NULL);
            recv_sec = (int) ntohl(*(int *)(recvbuffer+2));
            recv_usec = (int) ntohl(*(int *)(recvbuffer+6));
        
            current_latence = (end.tv_sec - recv_sec - 1) + (end.tv_usec - recv_usec) * pow(10,-6);
        
            printf("seqence: %d,latence is : %fs\n",current_count,current_latence);
        
            latence = current_latence + latence;
        
            current_count++;
        
            sleep(1);
    }
    
    printf("********************************\n");
    printf("Total Ping times:%d  Average latence:%fs\n\n",count,latence/count);
    
    free(buffer);
    free(recvbuffer);
    free(user_buffer);
    return 0;
}
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


