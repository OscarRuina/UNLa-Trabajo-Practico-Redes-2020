#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <fstream>

using namespace std;

class Servidor{
public:
    WSADATA wsaData; //variable para inicializar la libreria winsock2
    //socket de conexion y socket de comunicacion con el cliente
    SOCKET conexion_socket,comunicacion_socket;
    struct sockaddr_in servidor;//direccion del socket servidor
    struct sockaddr_in cliente;//direccion del socket cliente
    struct hostent *hp;
    int resp,stsize;
    char SendBuff[1024],RecvBuff[1024];//enviar y recibir mensajes

    Servidor(){

       //Inicializamos la libreria winsock2
       cout<<"Inicializando Winsock..."<<endl;
       resp=WSAStartup(MAKEWORD(1,0),&wsaData);
       if(resp){
        cout<<"Error al inicializar socket"<<WSAGetLastError()<<endl;
        getchar();
       }
       cout<<"Winsock Inicializado"<<endl;

       //Obtenemos la IP que usará nuestro servidor...
       // en este caso localhost indica nuestra propia máquina...
       hp=(struct hostent *)gethostbyname("localhost");
       if(!hp){
        cout<<"No se ha encontrado servidor..."<<endl;
        getchar();
        WSACleanup();
       }

       // Creamos el socket...
       conexion_socket=socket(AF_INET,SOCK_STREAM, 0);
       if(conexion_socket==INVALID_SOCKET){
        cout<<"Error al crear socket"<<WSAGetLastError()<<endl;
        getchar();
        WSACleanup();
       }
       cout<<"Socket creado."<<endl;

       //asociamos la address al socket
       memset(&servidor, 0, sizeof(servidor)) ;
       memcpy(&servidor.sin_addr, hp->h_addr, hp->h_length);
       servidor.sin_family = hp->h_addrtype;
       servidor.sin_port = htons(6000);

       // Asociamos ip y puerto al socket
       resp=bind(conexion_socket, (struct sockaddr *)&servidor, sizeof(servidor));
       if(resp==SOCKET_ERROR){
        cout<<"Error al asociar puerto e ip al socket"<<WSAGetLastError()<<endl;
        closesocket(conexion_socket);
        WSACleanup();
        getchar();
       }
       cout<<"Bind Success"<<endl;

       //ponemos a escuchar al servidor
       if(listen(conexion_socket, 1)==SOCKET_ERROR){
        cout<<"Error al habilitar conexiones entrantes"<<WSAGetLastError()<<endl;
        closesocket(conexion_socket);
        WSACleanup();
        getchar();
       }

       // Aceptamos conexiones entrantes
       cout<<"Esperando conexiones entrantes..."<<endl;
       stsize=sizeof(struct sockaddr);
       comunicacion_socket=accept(conexion_socket,(struct sockaddr *)&cliente,&stsize);
       if(comunicacion_socket==INVALID_SOCKET){
          cout<<"Error al aceptar conexión entrante"<<WSAGetLastError()<<endl;
          closesocket(conexion_socket);
          WSACleanup();
          getchar();
       }
       cout<<"Conexion entrante desde: "<<inet_ntoa(cliente.sin_addr)<<endl;

       // Como no vamos a aceptar más conexiones cerramos el socket escucha
       //closesocket(conexion_socket);
    }
    void recibir(){
        recv (comunicacion_socket, RecvBuff, sizeof(RecvBuff), 0);
        cout<<"El cliente dice: "<<RecvBuff<<endl;
        log(RecvBuff);
        memset(RecvBuff,0,sizeof(RecvBuff));
    }

    void enviar(){
        cout<<"Escribe el mensaje a enviar: ";
        cin>>this->SendBuff;
        log(SendBuff);
        send(comunicacion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Mensaje enviado!" <<endl;
    }

    void cerrar(){
        // Cerramos el socket de la conexion
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
        //log.close();
    }

    void log(string msg){
        // Declaramos las variables
        ofstream log;
        string log_file;

        // Creamos el archivo de log
        log_file.assign("sever.log");
        log.open(log_file.c_str());

        // Escribimos una línea con el nombre del archivo
        log << "2020-10-15: " << msg << std::endl;

        // Escribimos en el log
        //log << "Esta es una linea del log" << std::endl;

        // Cerramos el archivo
        //log.close();
    }


};

int main(int argc, char *argv[])
{
    Servidor *server = new Servidor();

    while(true){
        server->recibir();
        server->enviar();
    }
    server->cerrar();
    return 0;
}
