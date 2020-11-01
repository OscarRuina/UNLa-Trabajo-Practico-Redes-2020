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

       //Obtenemos la IP que usar� nuestro servidor...
       // en este caso localhost indica nuestra propia m�quina...
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
       servidor.sin_port = htons(5000);

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
          cout<<"Error al aceptar conexi�n entrante "<<WSAGetLastError()<<endl;
          closesocket(conexion_socket);
          WSACleanup();
          getchar();
       }
       cout<<"Conexion entrante desde: "<<inet_ntoa(cliente.sin_addr)<<endl;

       // Como no vamos a aceptar m�s conexiones cerramos el socket escucha
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
            t1 = clock();
            times = (double(t1-t0)/CLOCKS_PER_SEC);
            cout << "Tiempo de respuesta del cliente: " << times << endl;

            t0 = clock();
            t1 = clock();
            //cout<<"El cliente dice: "<<RecvBuff<<endl;
            string msglog = " El usuario ingreso: " + string(RecvBuff);
            log("server",msglog);
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
        if ( times < 180 ) {             //tiempo en segundos
            strcpy(SendBuff, msg.c_str());
        }else{
           strcpy(SendBuff, "x - Tiempo de inactividad superado, se cerrara la conexion");
        }
        //cout<<"Envia un mensaje: "<< msg << endl;;
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

class Servicio{
public:

    string origen;
    string destino;
    string fecha;
    string turno;


    string getOrigen(){
        return this->origen;
    }

    void setOrigen(string origen){
        this->origen=origen;
    }

    string getDestino(){
        return this->destino;
    }

    void setDestino(string destino){
        this->destino=destino;
    }
    string getFecha(){
        return this->fecha;
    }

    void setFecha(string fecha){
        this->fecha=fecha;
    }

    string getTurno(){
        return this->turno;
    }

    void setTurno(string turno){
        this->turno=turno;
    }

    Servicio(){
    }


    Servicio(string origen,string destino,string fecha,string turno){
        this->origen = origen;
        this->destino = destino;
        this->fecha = fecha;
        this->turno = turno;
    }

    void mostrar(){
        cout<<"Origen: " << origen << "\nDestino: " << destino << "\nFecha: " << fecha << "\nTurno: " << turno<<endl;
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


int main(int argc, char *argv[]){
    while (true){
        //system("cls");
        Servidor *server = new Servidor();
        t0 = clock();
        t1 = clock();
        times = 0;

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
                server->enviar("k");
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
            if ( opt == "4" || opt == "x"){
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

void inicioSesion(string usuario){
    log(usuario,"==================");
    log(usuario,"INICIO SESION");
    log(usuario,"==================");
    log("server","Inicio Sesion el usuario: " + usuario);
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

void CerrarSesion(Servidor *server,std::string usuario){
    server->enviar("x - Cierra Sesion");
    log (usuario, "Cierra Sesion");
    server->Reiniciar();
}

void generarViajes(Servidor *server){
    server->enviar("Ingrese Origen: BA(Buenos Aires) o MP(Mar del Plata) ");
    string origen = server->NewRecibir();

    server->enviar("Ingrese Destino: BA(Buenos Aires) o MP(Mar del Plata) ");
    string destino = server->NewRecibir();

    server->enviar("Ingrese Fecha con el formato DD/MM/AAAA: ");
    string fecha = server->NewRecibir();

    server->enviar("Ingrese Turno: TM(Turno Ma�ana) - TT(Turno Tarde) - TN(Turno Noche) ");
    string turno =  server->NewRecibir();

    string serv = origen+destino+fecha+turno;
    char servicio[40];

    strcpy(servicio,serv.c_str());

    //envia mensaje segun si escribio o no
    string msg;

    system("cls");
            //server->enviar(msg);

    int estado ;
    string msgSer;
    estado = verificarServicio(servicio);
    if(estado = 0){
        msgSer = "Servicio creado";
    }else{
        msgSer= "Servicio ya existe";
    }

    //system("cls");
    server->enviar(msg);
}

int verificarServicio(char serv[40]){
    int encontrado = 0;
    encontrado = leerArchivoServicios(serv);
    if(encontrado == 0){
        escribirArchivoServicio(serv);
    }
    return encontrado;
}

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

    //verifico el tama�o del archivo
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

void escribirArchivoServicio(char servicio[40]){


    ofstream servicios("servicios.bin", ios::out |ios::binary |ios::app);
    size_t len = strlen(servicio);

    servicios.write((char*)&len, sizeof(len));
    servicios.write(servicio, len);

    servicios.close();
    escribirAsientos(servicio);
}



std::vector <std::string> traerServicios(char servicio[40]){
    int tam = 0;
    //variable auxiliar para leer archivo
    char aux[sizeof(servicio)];
    std::vector<std::string> arrayServicios;

    ifstream servicios("servicios.bin", ios::in |ios::binary);
    if(!servicios.is_open()){
        cout<<"No se pudo abrir el archivo"<<endl;
        log("server","No se pudo abrir el archivo servicios");
    }else{
    //verifico el tama�o del archivo
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

void escribirAsientos(char servicio[40]){
    //el id es el tama�o del vector +1
    std::vector<std::string> arrayServicios = traerServicios(servicio);
    ofstream file;
    int idServicio = arrayServicios.size();

    file.open(to_string(idServicio),std::ofstream::out);
    //escribo en el archivo fila / col / estado / ocupante
    for(int i = 0 ; i < 3; i++){
        for(int j = 0;j<20;j++){
            file << i+1 <<";"<< j+1 << ";" << "0"<< ";"<< ";"<<"\n"<< endl;
        }
    }
    file.close();
}
