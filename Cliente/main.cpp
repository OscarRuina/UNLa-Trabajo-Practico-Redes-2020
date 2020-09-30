#include <iostream>
#include <stdio.h>
#include <windows.h>
using namespace std;

class Cliente{
public:
    WSADATA wsaData;
    SOCKET conn_socket;
    struct sockaddr_in server;
    struct hostent *hp;
    int resp;
    char SendBuff[1024],RecvBuff[1024];
    Cliente(){
       //Inicializamos la DLL de sockets
       resp=WSAStartup(MAKEWORD(1,0),&wsaData);
       if(resp){
       cout<<"Error al inicializar socket"<<endl;
       getchar();
       }
       //Obtenemos la IP del servidor... en este caso
       // localhost indica nuestra propia máquina...
       hp=(struct hostent *)gethostbyname("localhost");
       if(!hp){
          cout<<"No se ha encontrado servidor..."<<endl;
          getchar();WSACleanup();
       }
       // Creamos el socket...
       conn_socket=socket(AF_INET,SOCK_STREAM, 0);
       if(conn_socket==INVALID_SOCKET) {
          cout<<"Error al crear socket"<<endl;
          getchar();WSACleanup();
       }
       memset(&server, 0, sizeof(server)) ;
       memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
       server.sin_family = hp->h_addrtype;
       server.sin_port = htons(6000);
       // Nos conectamos con el servidor...
       if(connect(conn_socket,(struct sockaddr *)&server,sizeof(server))==SOCKET_ERROR){
          cout<<"Fallo al conectarse con el servidor"<<endl;
          closesocket(conn_socket);
          WSACleanup();getchar();
       }
       printf("Conexión establecida con: %s\n", inet_ntoa(server.sin_addr));
    }
    void enviar(){
        /*printf("Enviando Mensaje... \n");
        send(conn_socket,SendBuff,sizeof(SendBuff),0);
        printf("Datos enviados: %s \n", SendBuff);*/
        cout <<"Escribe el mensaje a enviar: "<<endl;
        cin>>this->SendBuff;
        send(conn_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Mensaje enviado!" << endl;
    }
    void recibir(){
        /*printf("Recibiendo Mensaje... \n");
        recv(conn_socket,RecvBuff, sizeof(RecvBuff), 0);
        printf("Datos recibidos: %s \n", RecvBuff);*/
        recv(conn_socket, RecvBuff, sizeof(RecvBuff), 0);
        cout << "El servidor dice: " << RecvBuff << endl;
        memset(RecvBuff, 0, sizeof(RecvBuff));
    }
    void cerrar(){
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conn_socket);
        WSACleanup();
    }

};


int main(int argc, char *argv[])
{
    Cliente *cliente = new Cliente();
    while(true){
        cliente->enviar();
        cliente->recibir();
    }
    return 0;
}
