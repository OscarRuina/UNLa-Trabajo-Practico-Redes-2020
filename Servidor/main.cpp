#include <iostream>
#include <stdio.h>
#include <winsock2.h>

using namespace std;

class Servidor{
public:
    WSADATA wsaData;
    SOCKET conn_socket,comm_socket;
    SOCKET comunicacion;
    struct sockaddr_in server;
    struct sockaddr_in client;
    struct hostent *hp;
    int resp,stsize;
    char SendBuff[1024],RecvBuff[1024];

    Servidor(){
       //Inicializamos la DLL de sockets
       resp=WSAStartup(MAKEWORD(1,0),&wsaData);
       if(resp){
        cout<<"Error al inicializar socket"<<endl;
        getchar();

       }

       //Obtenemos la IP que usará nuestro servidor...
       // en este caso localhost indica nuestra propia máquina...
       hp=(struct hostent *)gethostbyname("localhost");
       if(!hp){
        cout<<"No se ha encontrado servidor..."<<endl;
        getchar();
        WSACleanup();
       }
       // Creamos el socket...
       conn_socket=socket(AF_INET,SOCK_STREAM, 0);
       if(conn_socket==INVALID_SOCKET){
        cout<<"Error al crear socket"<<endl;
        getchar();
        WSACleanup();
       }
       //asociamos la address al socket
       memset(&server, 0, sizeof(server)) ;
       memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
       server.sin_family = hp->h_addrtype;
       server.sin_port = htons(6000);

       // Asociamos ip y puerto al socket
       resp=bind(conn_socket, (struct sockaddr *)&server, sizeof(server));
       if(resp==SOCKET_ERROR){
        cout<<"Error al asociar puerto e ip al socket"<<endl;
        closesocket(conn_socket);
        WSACleanup();
        getchar();
       }
       if(listen(conn_socket, 1)==SOCKET_ERROR){
        cout<<"Error al habilitar conexiones entrantes"<<endl;
        closesocket(conn_socket);
        WSACleanup();
        getchar();
       }

       // Aceptamos conexiones entrantes
       printf("Esperando conexiones entrantes... \n");
       stsize=sizeof(struct sockaddr);
       comm_socket=accept(conn_socket,(struct sockaddr *)&client,&stsize);
       if(comm_socket==INVALID_SOCKET){
          cout<<"Error al aceptar conexión entrante"<<endl;
          closesocket(conn_socket);
          WSACleanup();
          getchar();
       }
       printf("Conexión entrante desde: %s\n", inet_ntoa(client.sin_addr));

       // Como no vamos a aceptar más conexiones cerramos el socket escucha
       closesocket(conn_socket);
    }
    void recibir(){
        /*printf("Recibiendo Mensaje... \n");
        recv (comm_socket, RecvBuff, sizeof(RecvBuff), 0);
        printf("Datos recibidos: %s \n", RecvBuff);*/
        recv (comm_socket, RecvBuff, sizeof(RecvBuff), 0);
        cout<<"El cliente dice "<< RecvBuff <<endl;
        memset(RecvBuff,0,sizeof(RecvBuff));
    }

    void enviar(){
        /*printf("Enviando Mensaje... \n");
        send (comm_socket, SendBuff, sizeof(SendBuff), 0);
        printf("Datos enviados: %s \n", SendBuff);*/
        cout<<"Escribe el mensaje a enviar: ";
        cin>>this->SendBuff;
        send(comm_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Mensaje enviado!" << endl;
    }

    void cerrar(){
        // Cerramos el socket de la comunicacion
        closesocket(comm_socket);
        WSACleanup();
    }




};

int main(int argc, char *argv[])
{
    Servidor *server = new Servidor();
    while(true){
        server->recibir();
        server->enviar();
    }
    return 0;
}
