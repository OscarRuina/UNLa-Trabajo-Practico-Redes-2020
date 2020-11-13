#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <vector>
#include <istream>
#include <algorithm>    // std::find


using namespace std;


int countLog;
int Intentos;
unsigned t0, t1;
double times;

int leerArchivoUsuarios(string RecvBuff);
void log(string archivo,string msg);
string menu();
string menuBusquedas();


class Servidor{
public:
    WSADATA wsaData; //variable para inicializar la libreria winsock2
    //socket de conexion y socket de comunicacion con el cliente
    SOCKET conexion_socket,comunicacion_socket;
    struct sockaddr_in servidor;//direccion del socket servidor
    struct sockaddr_in cliente;//direccion del socket cliente
    struct hostent *hp;
    int resp,stsize;
    char SendBuff[102400],RecvBuff[102400];//enviar y recibir mensajes
    int RespLogin;

    Servidor(int puerto){
        countLog = 1;
        RespLogin = 0;
        log("server","INICIA SERVIDOR");
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
       servidor.sin_port = htons(puerto);
       //todo
        log("server", "Escucha puerto :"+to_string(puerto));
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
          cout<<"Error al aceptar conexión entrante "<<WSAGetLastError()<<endl;
          closesocket(conexion_socket);
          WSACleanup();
          getchar();
       }
       cout<<"Conexion entrante desde: "<<inet_ntoa(cliente.sin_addr)<<endl;

       // Como no vamos a aceptar más conexiones cerramos el socket escucha
       //closesocket(conexion_socket);


    }   //FIN SERVIDOR

    //metodo comun para recibir mensajes
    void recibir(){
        recv (comunicacion_socket, RecvBuff, sizeof(RecvBuff), 0);
        //cout<<"El cliente dice: "<<RecvBuff<<endl;
        memset(RecvBuff,0,sizeof(RecvBuff));
    }

    std::string NewRecibir(){
        int receiveCode = recv(comunicacion_socket, RecvBuff, sizeof(RecvBuff),0);
        //recv (comunicacion_socket, RecvBuff, sizeof(RecvBuff), 0);
        if (receiveCode == SOCKET_ERROR || receiveCode == 0){
            Reiniciar();
            RecvBuff[0] = 'x';
        }else{

            //cout<<"El cliente dice: "<<RecvBuff<<endl;

            t1 = clock();
            times = (double(t1-t0)/CLOCKS_PER_SEC);
            cout << "Tiempo de respuesta del cliente: " << times << endl;

            t0 = clock();
            t1 = clock();
            //cout<<"El cliente dice: "<<RecvBuff<<endl;
            //------------------------------------
        }
        return std::string(RecvBuff);
    }
    //metodo comun para enviar mensajes
    void enviar(){
        cin>>this->SendBuff;
        //log(SendBuff);
        send(comunicacion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        //cout << "Mensaje enviado!" <<endl;
    }

    void enviar(string msg){
        if ( times < 120 ) {             //tiempo en segundos
            strcpy(SendBuff, msg.c_str());
        }else{
           strcpy(SendBuff, "x - Tiempo de inactividad superado, se cerrara la conexion");
        }
        //cout<<"Envia un mensaje"<<endl;;
        //log(SendBuff);
        send(comunicacion_socket, SendBuff, sizeof(SendBuff), 0);
        memset(SendBuff, 0, sizeof(SendBuff));
        //cout << "Mensaje enviado!" <<endl;

    }


    void cerrarConexion(){
        // Cerramos el socket de la conexion
        closesocket(conexion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
    }

    void cerrarComunicacion(){
        // Cerramos el socket de la comunicacion
        closesocket(comunicacion_socket);
        WSACleanup();
        cout<<"Socket cerrado"<<endl;
    }

    void Reiniciar(){
        times = 0;
        closesocket(comunicacion_socket);
        closesocket(conexion_socket);
        //conexion_socket=socket(AF_INET,SOCK_STREAM, 0);
        //Servidor();
    }
};


void inicioSesion(string sesion);
void generarOpciones(std::string opt,Servidor *server,std::string usuario);
int guardarServicio(char servicio[40]);
void verRegistroActividades(Servidor *server,std::string usuario);
void CerrarSesion(Servidor *server,std::string usuario);
int verificarServicio(char servicio[40]);
void escribirArchivoServicio(char servicio[40]);
int leerArchivoServicios(char serv[40]);
void escribirAsientos(char servicio[40]);
void gestionarServicios(Servidor *server ,std::string usuario);
void buscarPorOrigen(Servidor *server, std::string usuario);
std::vector <std::string> traerServicios();
std::vector <std::string> generarVectorServicios(string target);
void mostrarMenuGestionarPasajes(Servidor *server,string servicio,string usuario);
void buscarAsientosServicios(string servicio);
void reservarAsiento(Servidor *server,string servicio,string usuario);
void mostrarAsientosServicios(Servidor* server,string servicio);
int verificarAsientoLibre(string servicio,string asiento);
void ocuparAsiento(string servicio,string asiento,string usuario);
void liberarAsiento(Servidor* server, string servicio,string usuario);
void liberarAsientoOcupado(string servicio,string asiento,string usuario);
void buscarPorOrigen(Servidor *server, std::string usuario);
void buscarPorFecha(Servidor *server, std::string usuario);
void buscarPorTurno(Servidor *server, std::string usuario);
void buscarPorOrigenYFecha(Servidor *server, std::string usuario);
void buscarPorOrigenYTurno(Servidor *server, std::string usuario);
void buscarPorFechaYTurno(Servidor *server, std::string usuario);
void buscarPorOrigenFechaYTurno(Servidor *server, std::string usuario);
void generarViajes(Servidor *server,string usuario);

int main(int argc, char *argv[]){
    //todo
    cout<<"Ingrese puerto"<<endl;
    int puerto = 0;
    cin >> puerto;
    while (true){
        //system("cls");
        Servidor *server = new Servidor(puerto);

        times = 0;
        t0 = clock();
        t1 = clock();
        Intentos = 0;
        server->recibir();
        int encontrado = 0;
        string usuario; //la declare aca para mandarla por parametro para el menu, opcion 3
        string Pass;
        string Respuesta;
        Respuesta = "";
        while (encontrado == 0){
            server->enviar("MIngrese Usuario");
            Respuesta = server->NewRecibir();
            usuario = Respuesta;

            if ( Respuesta != "x") {
                usuario = Respuesta;
            }else{
                break;
            }

            server->enviar("MIngrese Contrasenia");
            Respuesta = server->NewRecibir();
            Pass = Respuesta;

            if ( Respuesta != "x") {
                Pass = Respuesta;
            }else{
                break;
            }

            string UsuPass = usuario + ';' + Pass + ';' ;

            if ( leerArchivoUsuarios(UsuPass) == 0 ){  //No lo encontro, intento++
                Intentos++;               //encontrado seguira en 0 para repetirse
            }else{
                encontrado = 1;
                inicioSesion(usuario);
            }

            if (Intentos == 3 ){
                //hizo 3 intentos
                server->enviar("x - Se supero la cantidad maxima de intentos de ingreso, intente en otro momento");
                server->Reiniciar();
                break;
            }
        }


        //envio menu de opciones
        while (encontrado == 1 && Respuesta != "x"){
            log("server","Usuario abre sesion "+usuario);
            server->enviar(menu());
            //recibo respuesta y entro a las subopciones
            string opt = server->NewRecibir();
            generarOpciones(opt,server,usuario);
            if(opt[0]=='x'){
                encontrado = 0;
                server->Reiniciar();
            }

            if ( opt == "4" ){
                log("server","Usuario cierra sesion "+usuario);
                server->Reiniciar();
                break;
            }
        }
    }
    return 0;
}

//funciones del main
string menu(){
    string menu = "MBIENVENIDO AL SISTEMA;1- Alta Servicio;2- Gestionar Pasajes;3- Ver Registro de Actividades;4- Cerrar Sesion";
    return menu;
}


void gestionarPasajes(Servidor *server , std::string usuario){
    menuBusquedas();

}

string menuGestionarServicios(){
    string menu = "Gestionar Servicios;1- Reservar Asiento;2- Liberar Asiento;3- Elegir Otro Servicio;4- Volver al menu Anterior";
    return menu;
}
string menuBusquedas(){
    string menu = "MBusqueda Servicios;1- Origen;2- Fecha;3- Turno;4- Origen y Fecha;5- Origen y Turno;6- Fecha y Turno;7- Origen ,Fecha y Turno;8- Volver al Menu Anterior";
    return menu;
}

void inicioSesion(string usuario){
    log(usuario,"==================");
    log(usuario,"INICIO SESION");
    log(usuario,"==================");
}
//Funcion Log
void log(string archivo, string msg){
        // Declaramos las variables
        ofstream log;
        string log_file;
        string nombre = archivo;
        nombre = archivo + ".log";
        //Abrir el archivo
        log_file.assign(nombre);
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

//funcion para leer el archivo
int leerArchivoUsuarios(string RecvBuff){
   ifstream usuarios;
   string linea;
   string usuario;

   usuario = RecvBuff;
   int encontrado  = 0;
   usuarios.open("usuarios.txt",ios::in); // abro el archivo en modo lectura
   if(usuarios.fail()){
    cout<<"No se pudo abrir el archivo"<<endl;
    log("server","No se pudo abrir el archivo");
   }
   while(getline(usuarios,linea)){
        if(linea.find(usuario) != string::npos){
            //cout<<linea<<endl;                         //aca tendria el usuario y pass encontrado.
            //log("El Usuario: " + linea + " se logeo");
            encontrado = 1;
        }
   }
   if(encontrado == 0){
    cout<<"Usuario o contrasena no encontrado: "<<usuario<<endl;
    log("server","Usuario o contrasena no encontrado");
   }
   usuarios.close();
   return encontrado;
}

void verRegistroActividades(Servidor *server,std::string usuario){
    log(usuario,"Vio registro de actividades");
    log("server",usuario+" Vio registro de actividades");

    //leo el archivo usuario entrante
    ifstream archivo;
    string linea;
    string mensaje ="N";
    string nombre = usuario + ".log";
    archivo.open(nombre.c_str(),ios::in);//
    if(archivo.fail()){
        cout<<"No se pudo abrir el archivo del usuario: "<<usuario<<endl;
        log("server","No se pudo abrir el archivo del usuario " + usuario);
    }
    while(getline(archivo,linea)){
        mensaje = mensaje + "\n" + linea ;
    }
    archivo.close();
    server->enviar(mensaje);
}

void generarOpciones(std::string opt,Servidor *server,std::string usuario){
    switch(opt[0]){
        case '1':
            generarViajes(server,usuario);
            break;
        case '2':
            gestionarServicios(server,usuario);
            break;
        case '3':
            verRegistroActividades(server,usuario);
            break;
        case '4':
            CerrarSesion(server,usuario);
            break;
        default:
            break;
    }
}

void gestionarServicios(Servidor *server, std::string usuario){
    log(usuario,"Realiza consulta de servicio");
    log("server",usuario+" Realiza consulta de servicio");

    int seguir = 1;
    while(seguir){
    server->enviar(menuBusquedas());
    string opt = server->NewRecibir();

    switch(opt[0]){
        case '1':
            buscarPorOrigen(server,usuario);
            break;
        case '2':
            buscarPorFecha(server,usuario);
            break;
        case '3':
            buscarPorTurno(server,usuario);
            break;
        case '4':
            buscarPorOrigenYFecha(server,usuario);
            break;
        case '5':
            buscarPorOrigenYTurno(server,usuario);
            break;
        case '6':
            buscarPorFechaYTurno(server,usuario);
            break;
        case '7':
            buscarPorOrigenFechaYTurno(server,usuario);
            break;
        case '8':
            seguir = 0;
            break;
        default:
            break;
        }
    }
}

void buscarPorOrigen(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba el Origen: BA - MP");
    string msg = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;

    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;

        if(exists){
            if(aux[0]==msg[0]&&aux[1]==msg[1]){
                 request+=aux;
            request+=";";
            vectorPosServicios.push_back(aux);
            cout<<request<<endl;
            }
        }
    }
    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios.at(num),usuario);
}


void buscarPorFecha(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba la fecha: DD-MM-AAAA");
    string msg = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;

    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
            request+=aux;
            request+=";";
            vectorPosServicios.push_back(aux);
            cout<<request<<endl;
        }
    }
    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios.at(num),usuario);
}

void buscarPorTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba El Turno: TM - TT - TN");
    string msg = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;

    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
            request+=aux;
            request+=";";
            vectorPosServicios.push_back(aux);
            cout<<request<<endl;
        }
    }

    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios.at(num),usuario);
}

void buscarPorOrigenYFecha(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba el Origen: BA - MP");
    string msg = server->NewRecibir();
    server->enviar("MEscriba la fecha: DD-MM-AAAA");
    string msg2 = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;
    std::vector<std::string> vectorPosServicios2;



    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
            if(aux[0]==msg[0]&&aux[1]==msg[1]){
            vectorPosServicios.push_back(aux);
            }
        }
    }
    for (vector<std::string>::iterator serv=vectorPosServicios.begin(); serv!=vectorPosServicios.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg2) != std::string::npos;
        if(exists){
            request+=aux;
            request+=";";
            vectorPosServicios2.push_back(aux);
            cout<<request<<endl;
            }
        }
    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
}

void buscarPorOrigenYTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba Origen: BA - MP");
    string msg = server->NewRecibir();
    server->enviar("MEscriba el Turno: TM - TT -TN");
    string msg2 = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;
    std::vector<std::string> vectorPosServicios2;

    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
            if(aux[0]==msg[0]&&aux[1]==msg[1]){
            vectorPosServicios.push_back(aux);
        }
        }
    }
    for (vector<std::string>::iterator serv=vectorPosServicios.begin(); serv!=vectorPosServicios.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg2) != std::string::npos;
        if(exists){
            request+=aux;
            request+=";";
            vectorPosServicios2.push_back(aux);
            cout<<request<<endl;
            }
        }
    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
}

void buscarPorFechaYTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba la Fecha: DD-MM-AAAA");
    string msg = server->NewRecibir();
    server->enviar("MEscriba el Turno: TM - TT -TN");
    string msg2 = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;
    std::vector<std::string> vectorPosServicios2;

    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){

            vectorPosServicios.push_back(aux);
        }
    }
    for (vector<std::string>::iterator serv=vectorPosServicios.begin(); serv!=vectorPosServicios.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg2) != std::string::npos;
        if(exists){
            request+=aux;
            request+=";";
            vectorPosServicios2.push_back(aux);
            cout<<request<<endl;
            }
        }
    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
}

void buscarPorOrigenFechaYTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("MEscriba Origen: BA - MP");
    string msg = server->NewRecibir();
    server->enviar("MEscriba la Fecha: DD-MM-AAAA");
    string msg2 = server->NewRecibir();
    server->enviar("MEscriba el Turno: TM - TT -TN");
    string msg3 = server->NewRecibir();
    std::vector<std::string> vectorPosServicios;
    std::vector<std::string> vectorPosServicios2;
    std::vector<std::string> vectorPosServicios3;


    for (vector<std::string>::iterator serv=busqueda.begin(); serv!=busqueda.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
        if(aux[0]==msg[0]&&aux[1]==msg[1]){
            vectorPosServicios.push_back(aux);

            }
        }
    }
    for (vector<std::string>::iterator serv=vectorPosServicios.begin(); serv!=vectorPosServicios.end(); ++serv)
    {
        aux =  serv->c_str();
        bool exists = aux.find(msg2) != std::string::npos;
        if(exists){
            vectorPosServicios2.push_back(aux);
            }
        }
    for (vector<std::string>::iterator serv=vectorPosServicios2.begin(); serv!=vectorPosServicios2.end(); ++serv)
        {
        aux =  serv->c_str();
        bool exists = aux.find(msg3) != std::string::npos;
        if(exists){
            request+=aux;
            request+=";";
            vectorPosServicios3.push_back(aux);
            cout<<request<<endl;
            }
        }
    server->enviar(request);
    string opt = server->NewRecibir();
    stringstream opcion(opt);
    int num;
    opcion>>num;
    num--;
    mostrarMenuGestionarPasajes(server,vectorPosServicios3.at(num),usuario);
}


void mostrarMenuGestionarPasajes(Servidor *server,string servicio,string usuario){
    bool seguir = 1;
    while(seguir){
    mostrarAsientosServicios(server,servicio);
    server->enviar("R"+menuGestionarServicios());
    string opt = server->NewRecibir();
    //muestra los asientos del servicio
    //revela las opciones

        switch(opt[0]){
        case '1':
            reservarAsiento(server,servicio,usuario);
            break;
        case '2':
            liberarAsiento(server,servicio,usuario);
            break;
        case '3':
            seguir = 0;
            break;
        case '4':
            seguir = 0;
            break;
        default:
            break;
        }
    }
}



void mostrarAsientosServicios(Servidor* server,string servicio){
   ifstream file;
   string linea;
   string asiento = "B";

   int encontrado  = 0;
   file.open(servicio,ios::in); // abro el archivo en modo lectura
   if(file.fail()){
        cout<<"No se pudo abrir el archivo"<<endl;
        log("server","No se pudo abrir el archivo de servicio asientos");
   }
   while(getline(file,linea)){
        asiento+=linea;
   }
    file.close();
    server->enviar(asiento);
}


void reservarAsiento(Servidor* server,string servicio,string usuario){

    string fil,col;
    server->enviar("MIngrese fila");
    fil = server->NewRecibir();
    server->enviar("MIngrese columna");
    col = server->NewRecibir();
    //fopen(servicio+".txt");
    string asiento= fil+";"+col;
    //verifica que el asiento este libre
    if(verificarAsientoLibre(servicio,asiento)==1){
        ocuparAsiento(servicio,asiento,usuario);
        log(usuario,"Reserva un asiento "+fil+":"+col+";");
        log("server",usuario+ " Reserva un asiento "+fil+":"+col+";");


    }else{
    server->enviar("NAsiento ocupado");
    log(usuario,"Falla en reservar un asiento "+fil+":"+col+";");
    log("server",usuario+ " Falla en reservar un asiento "+fil+":"+col+";");

    }
}

void liberarAsiento(Servidor* server,string servicio,string usuario){

    string fil,col;
    server->enviar("MIngrese fila");
    fil = server->NewRecibir();
    server->enviar("MIngrese columna");
    col = server->NewRecibir();
    //fopen(servicio+".txt");
    string asiento= fil+";"+col;
    //verifica que el asiento este libre
    if(verificarAsientoLibre(servicio,asiento)==1){
        server->enviar("NEl Asiento no esta ocupado");
        log(usuario,"Falla en liberar un asiento "+fil+":"+col+";");
        log("server",usuario+ " Falla en liberar un asiento "+fil+":"+col+";");
    }else{
        liberarAsientoOcupado(servicio,asiento,usuario);
        server->enviar("NEl Asiento Liberado");
        log(usuario,"Libera un asiento "+fil+":"+col+";");
        log("server",usuario+ " Libera un asiento "+fil+":"+col+";");

    }
}


void ocuparAsiento(string servicio,string asiento,string usuario){
    string asientoAOcupar=asiento+";"+";";
    string asientoUsuario=asiento+";"+usuario+";";
    string linea;
    string servicio2 = "servicio2";
    ifstream file;
    file.open(servicio, ios::in);
    ofstream temp;
    temp.open(servicio2,ios::out);
    while(getline(file,linea)){
        if(linea.find(asientoAOcupar) != string::npos){
            temp<<asientoUsuario<<endl;
            cout<<linea<<endl;
        }else{
            temp<<linea<<endl;
        }
    }
    file.close();
    temp.close();
    remove(servicio.c_str());
    rename(servicio2.c_str(),servicio.c_str());
}

void liberarAsientoOcupado(string servicio,string asiento,string usuario){
    string asientoUsuarioALiberar=asiento+";"+usuario+";";
    string asientoLiberado=asiento+";"+";";
    string linea;
    string servicio2 = "servicio2";
    ifstream file;
    file.open(servicio, ios::in);
    ofstream temp;
    temp.open(servicio2,ios::out);
    while(getline(file,linea)){
        if(linea.find(asientoUsuarioALiberar) != string::npos){
            temp<<asientoLiberado<<endl;
            cout<<linea<<endl;
        }else{
            temp<<linea<<endl;
        }
    }
    file.close();
    temp.close();
    remove(servicio.c_str());
    rename(servicio2.c_str(),servicio.c_str());
}



int verificarAsientoLibre(string servicio,string asiento){

    asiento=asiento+";"+";";
    ifstream asientosServicio;
    string linea;

    int libre  = 0;
    asientosServicio.open(servicio,ios::in); // abro el archivo en modo lectura
    if(asientosServicio.fail()){
    log("server","No se pudo abrir el archivo");
    }
    while(getline(asientosServicio,linea)){
        if(linea.find(asiento) != string::npos){
            libre = 1;
        }
    }
    asientosServicio.close();
    return libre;
}

void generarViajes(Servidor *server,string usuario){
    log(usuario,"Ingresa un nuevo servicio");
    log("server",usuario+" Ingresa un nuevo servicio");

    server->enviar("MIngrese Origen: BA(Buenos Aires) o MP(Mar del Plata) ");
    string origen = server->NewRecibir();

    server->enviar("MIngrese Destino: BA(Buenos Aires) o MP(Mar del Plata) ");
    string destino = server->NewRecibir();

    server->enviar("MIngrese Fecha con el formato DD-MM-AAAA: ");
    string fecha = server->NewRecibir();

    server->enviar("MIngrese Turno: TM(Turno Mañana) - TT(Turno Tarde) - TN(Turno Noche) ");
    string turno =  server->NewRecibir();

    string serv = origen+destino+fecha+turno;
    char servicio[40];

    strcpy(servicio,serv.c_str());
    if(verificarServicio(servicio)==0){
        server->enviar("NSe genero un nuevo servicio");
    }else{
        server->enviar("NServicio ya existe");
        }
    //envia mensaje segun si escribio o no
    //TODO
}


//verifica si existe el servicio , si existe vuelve atras
//sino crea un servicio nuevo
int verificarServicio(char serv[40]){
    int encontrado = 0;
    encontrado = leerArchivoServicios(serv);
    if(encontrado == 0){
        escribirArchivoServicio(serv);
    }
    return encontrado;
}


//lee archivo para verificar la existencia de servicio
int leerArchivoServicios(char serv[40]){
    int encontrado = 0;
    int tam = 0;
    //variable auxiliar para leer archivo
    char aux[sizeof(serv)];

    ifstream servicios("servicios.bin", ios::in |ios::binary);
    if(!servicios.is_open()){
        log("server","No se pudo abrir el archivo servicios");
    }else{

    //verifico el tamaño del archivo
    servicios.seekg(0,ios::end);
    tam = servicios.tellg();
    //me muevo al principio del archibo
    servicios.seekg(0,ios::beg);

    //loopeo mientras no este en el final y no lo haya encontrado
    while(servicios.tellg()<tam&&!encontrado)
    {
        size_t len = 0;
        servicios.read((char*)&len, sizeof(len));
        servicios.read(aux, len);
        //saco la ultima posicion para borrar basura
        aux[len] = '\0';
        //comparo para verificar que sean iguales
        if(strcmp(serv,aux)==0){
            cout<<"encontrado"<<endl;
            encontrado = 1;
        }
    }
    servicios.close();
    }
    return encontrado;
}


//escribe servicio en archivo
//y genera asientos para estos
void escribirArchivoServicio(char servicio[40]){

    ofstream servicios("servicios.bin", ios::out |ios::binary |ios::app);
    size_t len = strlen(servicio);

    servicios.write((char*)&len, sizeof(len));
    servicios.write(servicio, len);

    servicios.close();
    escribirAsientos(servicio);
}


std::vector <std::string> traerServicios(){
    char servicio[40];
    int tam = 0;
    //variable auxiliar para leer archivo
    char aux[sizeof(servicio)];
    std::vector<std::string> arrayServicios;

    ifstream servicios("servicios.bin", ios::in |ios::binary);
    if(!servicios.is_open()){
        log("server","No se pudo abrir el archivo servicios");
    }else{
    //verifico el tamaño del archivo
    servicios.seekg(0,ios::end);
    tam = servicios.tellg();
    //me muevo al principio del archibo
    servicios.seekg(0,ios::beg);

    //loopeo mientras no este en el final y no lo haya encontrado
    while(servicios.tellg()<tam)
    {
        size_t len = 0;
        servicios.read((char*)&len, sizeof(len));
        servicios.read(aux, len);
        //saco la ultima posicion para borrar basura
        aux[len] = '\0';
        //agrego los elementos al vector
        arrayServicios.push_back(aux);
    }
    servicios.close();
    }
    return arrayServicios;
}


//escribo el archivo de asientos como el nombre del servicio, contiene el estado del asiento y el ocupante
void escribirAsientos(char servicio[40]){
    char letrasAsientos[3] = {'a','b','c'};
    ofstream file;
    string serv(servicio);
    file.open(serv,std::ofstream::out);
    //escribo en el archivo estado / ocupante
    for(int i = 0 ; i < 3; i++){
        for(int j = 0;j<20;j++){
                //guarda el estado y quien lo ocupa
            file << letrasAsientos[i]<<";"<<j+1<<";"<<";"<<"\n"<< endl;
        }
    }
    file.close();
}



 std::vector <std::string> generarVectorServicios(string target){
    string delim = ";";
    vector<string> v;
    if (!target.empty()) {
        string::size_type start = 0;
        do {
            size_t x = target.find(delim, start);
            if (x == string::npos)
                break;

            v.push_back(target.substr(start, x-start));
            start += delim.size();
        }
        while (true);

        v.push_back(target.substr(start));
    }
    return v;
    }


void CerrarSesion(Servidor *server,std::string usuario){
    server->enviar("x - Cierra Sesion");
    log (usuario, "Cierra Sesion");
    server->Reiniciar();
}
