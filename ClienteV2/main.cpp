#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>


using namespace std;

int countLog;
int RespLogin;
string UsuarioLogin;
string OK;

void menu();
void log(string msg);
string& str_replace(const string &search, const string &replace, string &subject);

class Cliente{
public:
    WSADATA wsaData;
    SOCKET conexion_socket;
    struct sockaddr_in servidor;
    struct hostent *hp;
    int resp;
    char SendBuff[102400],RecvBuff[102400],user[1024],password[1024];

    Cliente(int puerto){
        countLog = 1;
        RespLogin = 0;
       //Inicializamos la libreria winsock2
       cout<<"Inicializando Winsock..."<<endl;
       resp=WSAStartup(MAKEWORD(1,0),&wsaData);
       if(resp){
            cout<<"Error al inicializar socket"<<WSAGetLastError()<<endl;
            getchar();
       }
       cout<<"Winsock Inicializado"<<endl;

       //Obtenemos la IP del servidor... en este caso
       // localhost indica nuestra propia m�quina...
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
       servidor.sin_port = htons(puerto);

       // Nos conectamos con el servidor...
       if(connect(conexion_socket,(struct sockaddr *)&servidor,sizeof(servidor))==SOCKET_ERROR){
          cout<<"Fallo al conectarse con el servidor "<<WSAGetLastError()<<endl;
          closesocket(conexion_socket);
          WSACleanup();
          getchar();
          system("PAUSE");
          exit(0);

       }
       cout<<"Conexion establecida con: "<<inet_ntoa(servidor.sin_addr)<<endl;

    }

    //metodo comun para enviar mensajes
    void enviar(){
        cout <<"Escriba respuesta: ";
        cin>>this->SendBuff;
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Mensaje enviado!" << endl;
    }

    void enviarInicio(string msg){
        strcpy(SendBuff, msg.c_str());
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
    }

    //metodo comun para recibir mensajes
    void recibir(){
        recv(conexion_socket, RecvBuff, sizeof(RecvBuff), 0);
        cout << "El servidor dice: " << RecvBuff << endl;
        if (RecvBuff[0] == 'x'){
           system("PAUSE");
           exit(0);
        }

        if (RecvBuff[0] == 'B'){
            system("cls");
            string menu = string(RecvBuff);
            cout << str_replace(";", "\n", menu) << endl;
        }

        if (RecvBuff[0] == 'k'){
            system("cls");
            OK = string(RecvBuff);

        }
        memset(RecvBuff, 0, sizeof(RecvBuff));
    } //ejemplo


    void cerrar(){
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
        system("PAUSE");
    }

};

void menu();

int main(int argc, char *argv[])
{
    OK = "";
    int puerto = 0;
    cout<<"Ingrese Puerto 5000:"<<endl;
    cin>>puerto;
    Cliente *cliente = new Cliente(puerto);

    cliente->enviarInicio("InicioSesion");

    while(true){
        cliente->recibir();
        if (OK == "k"){         //ingreso bien el usuario
            break;
        }
        cliente->enviar();
    }

     while(OK == "k"){
        cliente->recibir();
        cliente->enviar();
    }

    return 0;
}

void menu(){
    cout<<"BIENVENIDO AL SISTEMA"<<endl;
    cout<<"1- Alta Servicio"<<endl;
    cout<<"2- Gestionar Pasajes"<<endl;
    cout<<"3- Ver Registro de Actividades"<<endl;
    cout<<"4- Cerrar Sesion"<<endl;
}

string& str_replace(const string &search, const string &replace, string &subject)
{
    string buffer;

    int sealeng = search.length();
    int strleng = subject.length();

    if (sealeng==0)
        return subject;//no change

    for(int i=0, j=0; i<strleng; j=0 )
    {
        while (i+j<strleng && j<sealeng && subject[i+j]==search[j])
            j++;
        if (j==sealeng)//found 'search'
        {
            buffer.append(replace);
            i+=sealeng;
        }
        else
        {
            buffer.append( &subject[i++], 1);
        }
    }
    subject = buffer;
    return subject;
}
