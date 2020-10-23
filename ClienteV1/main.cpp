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

void menu();
void log(string msg);


class Cliente{
public:
    WSADATA wsaData;
    SOCKET conexion_socket;
    struct sockaddr_in servidor;
    struct hostent *hp;
    int resp;
    char SendBuff[1024],RecvBuff[1024],user[1024],password[1024];

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

     int recibirResp(){
        int OK = 0;
        recv(conexion_socket, RecvBuff, sizeof(RecvBuff), 0);
        if ( RecvBuff[0] == '4' ) {cerrarPorIntentos();}        //Si se superaron los 3 intentos cierra.
        if ( RecvBuff[0] == '1'){OK = 1;}                       //Si no se encontro el usuario pone un 1
        memset(RecvBuff, 0, sizeof(RecvBuff));
        return OK;
    }
    //case 1
    void envioAltaServicio(string Origen, int fecha, string Turno){
        //aca hace algo
        cout<<"Entro a Envio Alta Servicio"<<endl;
    }

    //case 2
    void envioGestionPasajes(){}

    //case 3
    void envioVerRegistroDeActividades(){}

    //case 4
    void envioCerrarSesion(){
        //Cerrar servidor
        closesocket(conexion_socket);
        SendBuff[0] = '9'       ;                //Cierra servidor
        send(conexion_socket, SendBuff, sizeof(SendBuff), 0);
        cout<<"Pulse cualquier tecla para salir"<<endl;
        system("PAUSE");
        exit(0);
    }

    void cerrarPorIntentos(){
        // Cerramos el socket y liberamos la DLL de sockets
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Se supero la cantidad maxima de intentos de ingreso"<<endl;
        cout<<"Pulse cualquier tecla para salir"<<endl;
        system("PAUSE");
        exit(0);
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
    int opcion = -1;
    while(true){
        cout<<"Ingrese Usuario"<<endl;
        cin>>usuario;
        cout<<"Ingrese Contraseña"<<endl;
        cin>>pass;
        // msg = "1~" + usuario + "~" + pass;
        msg = usuario + ";" + pass;
        cliente->enviarUserPassword(msg);
        //cliente->enviar();
        if(cliente->recibirResp() == 0){
            UsuarioLogin = usuario;
            //aca va el menu despues si responde "OK" el server.
            //si es incorrecto el usuario llega al menu igual
            system("cls");
            log("INICIA SECCION");

            while(true){
                menu();
                cin>>opcion;
                switch(opcion)
                {
                    case 1:{ //debe ingresar el Origen, la fecha y el turno
                            string Origen;
                            Origen = "";
                            while (Origen != "BA" && Origen != "MP"){
                                cout<<"Ingrese Origen: BA (Buenos Aires) o MP (Mar del Plata)"<<endl;
                                cin>>Origen;
                            }
                            int fecha;
                            cout<<"Ingrese fecha con formato DDMMAAAA"<<endl;           //QUEDA VALIDAR
                            cin>>fecha;

                            string Turno;
                            while (Turno != "M" && Turno != "T" && Turno != "N"){
                                cout<<"Ingrese Turno: M (Mañana) T (Tarde) N (Noche)"<<endl;           //QUEDA VALIDAR
                                cin>>Turno;
                            }
                            cliente->envioAltaServicio(Origen,fecha,Turno);
                            break;
                        }
                    case 2: cliente->envioGestionPasajes();
                        break;
                    case 3: cliente->envioVerRegistroDeActividades();
                        break;
                    case 4: cliente->envioCerrarSesion();
                        break;
                    //default: return -1;
                }
            }
        }
    }

    return 0;
}

//funciones del main
void menu(){
    cout<<"BIENVENIDO AL SISTEMA"<<endl;
    cout<<"1- Alta Servicio."<<endl;
    cout<<"2- Gestionar Pasajes."<<endl;
    cout<<"3- Ver Registro de Actividades."<<endl;
    cout<<"4- Cerrar Sesion."<<endl;
}


//LOG va al servidor
void log(string msg){
        // Declaramos las variables
        std::ofstream log;
        string log_file;
        string FileName;
        FileName = UsuarioLogin + ".log";
        //Abrir el archivo
        log_file.assign(FileName);
        log.open(log_file.c_str(),ios::app);

        //Declaramos la fecha/hora del dia.
        time_t now = time(0);
        tm* time = localtime(&now);
        int dia = time-> tm_mday;
        int mes = time-> tm_mon + 1;
        int anio = 1900 + time-> tm_year;
        int hora = time-> tm_hour;
        int minu = time-> tm_min;

        // Escribimos inicio del log
        if (countLog == 1) {
            log << anio <<"-"<< mes <<"-"<< dia <<"_"<< hora <<":"<< minu << ": ==================" << std::endl;
            log << anio <<"-"<< mes <<"-"<< dia <<"_"<< hora <<":"<< minu << ": " << msg << std::endl;
            log << anio <<"-"<< mes <<"-"<< dia <<"_"<< hora <<":"<< minu << ": ==================" << std::endl;
            countLog += 1;
        }else{
            // Escribimos en el log
            log << anio <<"-"<< mes <<"-"<< dia <<"_"<< hora <<":"<< minu << ": " << msg << std::endl;
        }

        // Cerramos el archivo
        log.close();
    }
