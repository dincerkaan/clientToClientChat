#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_SIZE 1024
#define CLIENT1_PORT 12345
#define CLIENT2_PORT 54321

void error(const char *message) {
    perror(message);
    exit(1);
}

int main() {
    int sock;
    struct sockaddr_in client1_addr, client2_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        error("İstemci 2 için soket oluşturulamadı.");
    }

    memset((char *)&client1_addr, 0, sizeof(client1_addr));
    client1_addr.sin_family = AF_INET;
    client1_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client1_addr.sin_port = htons(CLIENT1_PORT);

    memset((char *)&client2_addr, 0, sizeof(client2_addr));
    client2_addr.sin_family = AF_INET;
    client2_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client2_addr.sin_port = htons(CLIENT2_PORT);

    if (bind(sock, (struct sockaddr *)&client2_addr, sizeof(client2_addr)) < 0) {
        error("İstemci 2 için soket bağlanamadı.");
    }

    int client2_message_count = 0;
    printf("İstemci 1 ile mesajlaşmaya başlayın...\n");

    char message[MAX_MESSAGE_SIZE];

    while (1) {
        socklen_t client1_len = sizeof(client1_addr);
        memset(message, 0, MAX_MESSAGE_SIZE);
        if (recvfrom(sock, message, MAX_MESSAGE_SIZE, 0, (struct sockaddr *)&client1_addr, &client1_len) < 0) {
            error("Mesaj alınamadı.");
        }
        
	if (strcmp(message, "exit\n") == 0 || strcmp(message, "EXIT\n") == 0){
		printf("İstemci1 konuşmadan ayrıldı..\n");
	}
	else{
        	printf("İstemci 1'den gelen mesaj: %s", message);
        }

        printf("Mesajınızı girin ya da sohbeti sonlandırmak için 'exit' yazın: \n");
        fgets(message, MAX_MESSAGE_SIZE, stdin);

        if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&client1_addr, sizeof(client1_addr)) < 0) {
            error("Mesaj gönderilemedi.");
        }
        
	client2_message_count++;
	
         if (strcmp(message, "exit\n") == 0 || strcmp(message, "EXIT\n") == 0) {
       		printf("Çıkışı onaylıyorsanız Y'ye basın..");
		fgets(message, MAX_MESSAGE_SIZE, stdin);
		if(strcmp(message, "y\n") == 0 || strcmp(message, "Y\n") == 0){
			break;
		}
         } 
    }

    printf("Bu sohbet boyunca toplamda %d mesaj gönderdiniz..\n", client2_message_count);
    close(sock);
    
    return 0;
}