#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

int countLog;
int RespLogin;
string UsuarioLogin;


void menu();
void log(string msg);
string& str_replace(const string &search, const string &replace, string &subject);
int verificarRecibir(string msg);
void mostrarAsientos(string asientos);
vector<string> procesarAsientos(string asientos);

class Cliente{
public:
    WSADATA wsaData;
    SOCKET conexion_socket;
    struct sockaddr_in servidor;
    struct hostent *hp;
    int resp;
    char SendBuff[102400],RecvBuff[102400],user[1024],password[1024];

    Cliente(){
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

    //metodo comun para enviar mensajes
    void enviar(){
        cout <<"Escriba respuesta: ";
        cin>>this->SendBuff;
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        cout << "Mensaje enviado!" << endl;
    }

    void newEnviar(string msg){
        strcpy(SendBuff, msg.c_str());
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
    }

    void enviarInicio(string msg){
        strcpy(SendBuff, msg.c_str());
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
    }

    //metodo comun para recibir mensajes
    string recibir(){
        string msg;
        recv(conexion_socket, RecvBuff, sizeof(RecvBuff), 0);
        msg = RecvBuff;
        memset(RecvBuff, 0, sizeof(RecvBuff));
        return msg;
    }


    void cerrar(){
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
        system("PAUSE");
    }



};


//verifica los estados del mensaje
int verificarRecibir(string msg){
    int responder = 1;
    cout<<"el servidor dice:\n"<<endl;
        if(msg[0]=='Z'){
            responder = 0;
            //borro el primer char
            msg.erase(0,1);
        }

        if (msg[0] == 'x'){
            cout<<msg<<endl;
           system("PAUSE");
           exit(0);
        }else if(msg[0] == 'L'){
            responder = 0;
            msg.erase(0,1);
            string asientos = string(msg);
            mostrarAsientos(asientos);
        }else {
            msg.erase(0,1);
            string menu = string(msg);
            cout << str_replace(";", "\n", msg) << endl;
        }
    return responder;
}

void mostrarAsientos(string asientos){
    char letrasAsientos[3] = {'A','B','C'};
    int counter = 0;

    vector<string> vectorAsientos = procesarAsientos(asientos);
    string encabezado1 = "  |                   1 1 1 1 1 1 1 1 1 1 2";
    string encabezado2 = "  | 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0";
    string encabezado3 = "  | ---------------------------------------";


    string split       = "  | =======================================";

    cout<<encabezado1<<endl;
    cout<<encabezado2<<endl;
    cout<<encabezado3<<endl;


    for(int i=0; i < vectorAsientos.size(); i++){
        if(i%20==0&&i>0){
            cout<<"\n";
        }
        if(i==40){
                cout<<split<<endl;
        }
        if(i==0||i==20||i==40){
            cout<<letrasAsientos[counter]<<" | ";
            counter++;
        }
        if(vectorAsientos.at(i).size()==0){
            cout<<"O ";
        }else{
            cout<<"X ";
        }
    }
            cout<<"\n";

}

vector<string> procesarAsientos(string asientos){

    vector<string> vectorAsientos;
    int counter= 0;
    std::string delimiter = ";";
    size_t pos = 0;
    std::string token;

    while ((pos = asientos.find(delimiter)) != std::string::npos) {
        counter++;
        token = asientos.substr(0, pos);
        if(counter%3==0){
            vectorAsientos.push_back(token);
        }
        asientos.erase(0, pos + delimiter.length());
    }


    return vectorAsientos;
}

int main(int argc, char *argv[])
{
    Cliente *cliente = new Cliente();
    cliente->enviarInicio("InicioSeccion");
    int responder = 1;
    while(true){
        //string str = cliente->recibir();
        string msg = cliente->recibir();
        responder = verificarRecibir(msg);
        if(responder!=0){
            cliente->enviar();
        }
}
    return 0;
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


