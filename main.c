/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface */
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#define BUFFSIZE 1518
#define ETHER_TYPE_IPv4 0x0800 /**< IPv4 Protocol. */
#define ETHER_TYPE_IPv6 0x86DD /**< IPv6 Protocol. */
#define ETHER_TYPE_ARP  0x0806 /**< Arp Protocol. */

enum tipos {
    ipv4 = 0x0800,
    ipv6 = 0x86DD,
    arp = 0x0806
};

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

unsigned char buff1[BUFFSIZE]; // buffer de recepcao

int sockd;
int on;
struct ifreq ifr;

int main(int argc, char *argv[]) {
    /* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
    /* De um "man" para ver os parametros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
    if ((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        printf("Erro na criacao do socket.\n");
        exit(1);
    }

    int min = -1;
    int max = -1;
    int med = -1;

    // O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
    strcpy(ifr.ifr_name, "wlo1");
    if (ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
        printf("erro no ioctl!");
    ioctl(sockd, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags |= IFF_PROMISC;
    ioctl(sockd, SIOCSIFFLAGS, &ifr);

    // recepcao de pacotes
    while (1) {
        recv(sockd, (char *) &buff1, sizeof(buff1), 0x0);

        unsigned int tipo = (buff1[12] << 8) + buff1[13];
        printf("Tipo: %04x - ", tipo);
        if (tipo == ipv4) {
            printf("IPv4\n");
        } else if (tipo == ipv6) {
            printf("IPv6\n");
        } else if (tipo == arp) {
            printf("ARP\n");
        } else {
            printf("Outro\n");
        }

        // impress�o do conteudo - exemplo Endereco Destino e Endereco Origem
        printf("MAC Destino: %02x:%02x:%02x:%02x:%02x:%02x \n", buff1[0], buff1[1], buff1[2], buff1[3], buff1[4],
               buff1[5]);
        printf("MAC Origem:  %02x:%02x:%02x:%02x:%02x:%02x \n\n", buff1[6], buff1[7], buff1[8], buff1[9], buff1[10],
               buff1[11]);
    }
}