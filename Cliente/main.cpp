#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <string.h>

using namespace std;

class Cliente{
public:
    WSADATA wsaData;
    SOCKET conexion_socket;
    struct sockaddr_in servidor;
    struct hostent *hp;
    int resp;
    char SendBuff[1024],RecvBuff[1024],user[1024],password[1024];

    Cliente(){

       //Inicializamos la libreria winsock2
       cout<<"Inicializando Winsock..."<<endl;
       resp=WSAStartup(MAKEWORD(1,0),&wsaData);
       if(resp){
            cout<<"Error al inicializar socket"<<WSAGetLastError()<<endl;
            getchar();
       }
       cout<<"Winsock Inicializado"<<endl;

       //Obtenemos la IP del servidor... en este caso
       // localhost indica nuestra propia máquina...
       hp=(struct hostent *)gethostbyname("localhost");
       if(!hp){
          cout<<"No se ha encontrado servidor..."<<endl;
          getchar();
          WSACleanup();
       }

       // Creamos el socket...
       conexion_socket=socket(AF_INET,SOCK_STREAM, 0);
       if(conexion_socket==INVALID_SOCKET) {
          cout<<"Error al crear socket "<<WSAGetLastError()<<endl;
          getchar();
          WSACleanup();
       }
       cout<<"Socket creado."<<endl;

       //asociamos la address al socket
       memset(&servidor, 0, sizeof(servidor)) ;
       memcpy(&servidor.sin_addr, hp->h_addr, hp->h_length);
       servidor.sin_family = hp->h_addrtype;
       servidor.sin_port = htons(6000);

       // Nos conectamos con el servidor...
       if(connect(conexion_socket,(struct sockaddr *)&servidor,sizeof(servidor))==SOCKET_ERROR){
          cout<<"Fallo al conectarse con el servidor "<<WSAGetLastError()<<endl;
          closesocket(conexion_socket);
          WSACleanup();
          getchar();

       }
       cout<<"Conexion establecida con: "<<inet_ntoa(servidor.sin_addr)<<endl;

    }
    //metodo que envia el usuario y la contraseña al servidor
    void enviarUserPassword(string msg){
        strcpy(SendBuff, msg.c_str());
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Usuario y Contraseña enviados!" << endl;
    }
    //metodo comun para enviar mensajes
    void enviar(){
        cout <<"Escribe el mensaje a enviar: ";
        cin>>this->SendBuff;
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Mensaje enviado!" << endl;
    }
    //metodo comun para recibir mensajes
    void recibir(){
        recv(conexion_socket, RecvBuff, sizeof(RecvBuff), 0);
        cout << "El servidor dice: " << RecvBuff << endl;
        memset(RecvBuff, 0, sizeof(RecvBuff));
    } ///ejemplo

     void recibirResp(){
        recv(conexion_socket, RecvBuff, sizeof(RecvBuff), 0);
        if ( RecvBuff[0] == '4' ) {
            cerrarPorIntentos();                 //explota aca
        }

        memset(RecvBuff, 0, sizeof(RecvBuff));
    }

    void cerrarPorIntentos(){
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Se supero la cantidad maxima de intentos de ingreso"<<endl;
        system("PAUSE");
    }

    void cerrar(){
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
        system("PAUSE");
    }

};


int main(int argc, char *argv[])
{
    Cliente *cliente = new Cliente();
    string usuario;
    string pass;
    string msg;

    while(true){
        cout<<"Ingrese Usuario"<<endl;
        cin>>usuario;
        cout<<"Ingrese Contraseña"<<endl;
        cin>>pass;
        // msg = "1~" + usuario + "~" + pass;
        msg = usuario + ";" + pass;
        cliente->enviarUserPassword(msg);
        //cliente->enviar();
        cliente->recibirResp();
        //aca va el menu despues si responde "OK" el server.
    }

    return 0;
}

//funciones del main
