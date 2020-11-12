#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <vector>

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

    Servidor(){
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
        log("server",RecvBuff);
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
            log("server",RecvBuff);
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
        closesocket(comunicacion_socket);
        closesocket(conexion_socket);
        //conexion_socket=socket(AF_INET,SOCK_STREAM, 0);
        //Servidor();
    }
};


void inicioSesion(string sesion);
void generarOpciones(std::string opt,Servidor *server,std::string usuario);
void generarViajes(Servidor *server);
//int guardarServicio(string viaje);
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
void reservarAsiento(string servicio,string usuario);
void mostrarAsientosServicios(Servidor* server,string servicio);

int main(int argc, char *argv[]){
    while (true){
        //system("cls");
        Servidor *server = new Servidor();

        Intentos = 0;
        server->recibir();
        int encontrado = 0;
        string usuario; //la declare aca para mandarla por parametro para el menu, opcion 3
        string Pass;
        string Respuesta;
        Respuesta = "";
        while (encontrado == 0){
            server->enviar("Ingrese Usuario");
            Respuesta = server->NewRecibir();
            usuario = Respuesta;

            if ( Respuesta != "x") {
                usuario = Respuesta;
            }else{
                break;
            }

            server->enviar("Ingrese Contrasenia");
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
            server->enviar(menu());
            //recibo respuesta y entro a las subopciones
            string opt = server->NewRecibir();
            generarOpciones(opt,server,usuario);
            if ( opt == "4" ){
                break;
            }
        }
    }
    return 0;
}

//funciones del main
string menu(){
    string menu = "BIENVENIDO AL SISTEMA;1- Alta Servicio;2- Gestionar Pasajes;3- Ver Registro de Actividades;4- Cerrar Sesion";
    return menu;
}


void gestionarPasajes(Servidor *server , std::string usuario){
menuBusquedas();

}

string menuGestionarServicios(){
    string menu = "CGestionar Servicios;1- Reservar Asiento;2- Liberar Asiento;3- Elegir Otro Servicio;4- Volver al Menu Principal";
    return menu;
}
string menuBusquedas(){
    string menu = "CBusqueda Servicios;1- Origen;2- Fecha;3- Turno;4- Origen y Fecha;5- Origen y Turno;6-Fecha y Turno;7- Origen ,Fecha y Turno;8- Volver al Menu Anterior";
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
    log(usuario,"Pulso la Opcion 3");
    //leo el archivo usuario entrante
    ifstream archivo;
    string linea;
    string mensaje;
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
            generarViajes(server);
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
    server->enviar(menuBusquedas());
    string opt = server->NewRecibir();

    switch(opt[0]){
        case '1':
            buscarPorOrigen(server,usuario);
            break;
        case '2':
            break;
        case '3':
            break;
        case '4':
            break;
        case '5':
            break;
        case '6':
            break;
        case '7':
            break;
          default:
            break;
    }
}

void buscarPorOrigen(Servidor *server, std::string usuario){
    std::string request =  "D";
    std::string aux;
    std::vector <std::string> busqueda = traerServicios();
    server->enviar("Escriba el Origen");
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

void mostrarMenuGestionarPasajes(Servidor *server,string servicio,string usuario){
    mostrarAsientosServicios(server,servicio);
    server->enviar(menuGestionarServicios());
    string opt = server->NewRecibir();
    //muestra los asientos del servicio
    //revela las opciones
    switch(opt[0]){
        case 'a':
            reservarAsiento(servicio,usuario);
            break;
        case 'b':
            break;
        case 'c':
            break;
        case 'd':
            break;
        default:
            break;
    }
}

void mostrarAsientosServicios(Servidor* server,string servicio){
    string asientos;






    server->enviar(asientos);
}


void reservarAsiento(string servicio,string usuario){
    //fopen(servicio+".txt");
}

void generarViajes(Servidor *server){
    server->enviar("Ingrese Origen: BA(Buenos Aires) o MP(Mar del Plata) ");
    string origen = server->NewRecibir();

    server->enviar("Ingrese Destino: BA(Buenos Aires) o MP(Mar del Plata) ");
    string destino = server->NewRecibir();

    server->enviar("Ingrese Fecha con el formato DD/MM/AAAA: ");
    string fecha = server->NewRecibir();

    server->enviar("Ingrese Turno: TM(Turno Mañana) - TT(Turno Tarde) - TN(Turno Noche) ");
    string turno =  server->NewRecibir();

    string serv = origen+destino+fecha+turno;
    char servicio[40];

    strcpy(servicio,serv.c_str());
    verificarServicio(servicio);
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
        cout<<"No se pudo abrir el archivo"<<endl;
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
        cout<<"No se pudo abrir el archivo"<<endl;
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
    cout<<serv<<endl;
    //escribo en el archivo estado / ocupante
    for(int i = 0 ; i < 3; i++){
        for(int j = 0;j<20;j++){
                //guarda el estado y quien lo ocupa
            file << letrasAsientos[i]<<j+1<<";"<<";"<<"\n"<< endl;
        }
    }
    file.close();
}

void buscarAsientosServicios(string servicio){
    string asientos;




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
