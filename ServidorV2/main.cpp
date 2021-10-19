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
#include <cstdio>

const int BUFFERSIZE = 4096;


using namespace std;

int countLog;
int Intentos;
unsigned t0, t1;
double times;
int nameLen = 0;
int asLen = 0;


int leerArchivoUsuarios(string RecvBuff);
void log(string archivo,string msg);
string menu();
string menuBusquedas();
string menu(){
    string menu = "MBIENVENIDO AL SISTEMA;1- Alta Servicio;2- Gestionar Pasajes;3- Ver Registro de Actividades;4- Cerrar Sesion";
    return menu;
}
string menuGestionarServicios(){
    string menu = "Gestionar Servicios;1- Reservar Asiento;2- Liberar Asiento;3- Elegir Otro Servicio;4- Volver al menu Anterior";
    return menu;
}
string menuBusquedas(){
    string menu = "MBusqueda Servicios;1- Origen;2- Fecha;3- Turno;4- Origen y Fecha;5- Origen y Turno;6- Fecha y Turno;7- Origen ,Fecha y Turno;8- Volver al Menu Anterior";
    return menu;
}

class Servicio {
public:
    string servicio;
    string asientos;


    Servicio(string serv){

        vector<string> a = generarAsientos();
//        strcpy(asientos,serializarAsientos(a).c_str());
        this->servicio = serv;
        this->asientos = serializarAsientos(a);
    }

    Servicio(string serv,string as){
        this->servicio = serv;
        this->asientos = as;
    }



    vector<string> generarAsientos(){
//        char letras[3] = {'a','b','c'}
        vector<string> asientos;
            for(int i = 0;i<3;i++){
                for(int j= 0;j<20;j++){
                    string ass;
                    ass+=to_string(i+1)+";"+to_string(j+1)+";"+to_string(0)+";";
                    asientos.push_back(ass);
                }
            }
        return asientos;
    }

    string serializarAsientos(vector<string> asientos){
        string todosAsientos;
            for(int i = 0;i<asientos.size();i++){
                todosAsientos+=asientos.at(i);
            }
        return todosAsientos;
    }
    //deseraliza la string y pone el estado del asiento en el string



    void mostrar(){
        cout<<"Mostrando Servicio :"<<this->servicio<<endl;
        cout<<"Mostrando Asientos :"<<this->asientos<<endl;
    }


    void setAsientos(string asient){
        this->asientos=asient;
    }
};





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
       servidor.sin_port = htons(1);
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


void gestionarServicios(Servidor *server ,std::string usuario);
void reservarAsiento(Servidor* server,Servicio* servicio,string usuario);
void generarViajes(Servidor *server,string usuario);
void CerrarSesion(Servidor *server,std::string usuario);
void mostrarMenuGestionarPasajes(Servidor *server,Servicio* servicio,string usuario);
void liberarAsientoOcupado(Servicio *servicio,string ass);
void ocuparAsiento(Servicio *servicio,string asiento);
int verificarAsientoLibre(Servicio* servicio,string asiento);
void verRegistroActividades(Servidor *server,std::string usuario);
string serializar(Servicio *servicio);
string reemplazar(string asientos,string asientoLiberado,string asientoOcupado);
void liberarAsiento(Servidor* server,Servicio *servicio,string usuario);
void generarOpciones(std::string opt,Servidor *server,std::string usuario);
int verificarServicio(Servicio * s);
void escribirArchivoServicio(Servicio *s);
std::vector <Servicio*> traerVectorServicios();
Servicio* deserializar(string msg);
void buscarPorOrigen(Servidor *server, std::string usuario);
void deserializar(Servicio *serv,string msg);
void buscarPorFecha(Servidor *server, std::string usuario);
void buscarPorTurno(Servidor *server, std::string usuario);
void buscarPorOrigenYFecha(Servidor *server, std::string usuario);
void buscarPorOrigenYTurno(Servidor *server, std::string usuario);
void buscarPorFechaYTurno(Servidor *server, std::string usuario);
void buscarPorOrigenFechaYTurno(Servidor *server, std::string usuario);
void inicioSesion(string sesion);


int main(int argc, char *argv[]){
    //todo
    //cout<<"Ingrese puerto"<<endl;
    int puerto = 1;
    //cin >> puerto;
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
                server->enviar("NUsuario o Contraseña incorrectos");
            }else{
                encontrado = 1;
                inicioSesion(usuario);
            }

            if (Intentos == 3 ){
                //hizo 3 intentos
                server->enviar("x - Se supero la cantidad maxima de intentos de ingreso, intente en otro momento");
                server->Reiniciar();
                usuario ="";
                Pass="";
                Intentos=0;
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
void  gestionarServicios(Servidor *server, std::string usuario){
    log(usuario,"Realiza consulta de servicio ");
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
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba el Origen: BA - MP");
    string msg = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;


    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
             if(aux[0]==msg[0]&&aux[1]==msg[1]){
                request+=aux;
                request+=";";
                vectorPosServicios.push_back(vectorServicios.at(i));
            }
        }
    }
    if(vectorPosServicios.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios.at(num),usuario);
    }
}
void buscarPorFecha(Servidor *server, std::string usuario){

    std::string request =  "M";
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba la fecha: DD-MM-AAAA");
    string msg = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;

    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
                request+=aux;
                request+=";";
                vectorPosServicios.push_back(vectorServicios.at(i));
        }
    }
    if(vectorPosServicios.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios.at(num),usuario);
    }
}
void buscarPorTurno(Servidor *server, std::string usuario){

    std::string request =  "M";
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba El Turno: TM - TT - TN");
    string msg = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;

    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
                request+=aux;
                request+=";";
                vectorPosServicios.push_back(vectorServicios.at(i));
        }
    }
    if(vectorPosServicios.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios.at(num),usuario);
    }
}
void buscarPorOrigenYFecha(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba el Origen: BA - MP");
    string msg = server->NewRecibir();
    server->enviar("MEscriba la fecha: DD-MM-AAAA");
    string msg2 = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;
    std::vector <Servicio*> vectorPosServicios2;

    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
             if(aux[0]==msg[0]&&aux[1]==msg[1]){
                vectorPosServicios.push_back(vectorServicios.at(i));
            }
        }
    }
    for(int i = 0;i< vectorPosServicios.size();i++){
        string aux2(vectorPosServicios.at(i)->servicio);
        bool exists = aux2.find(msg2) != std::string::npos;
        if(exists){
                request+=aux2;
                request+=";";
                vectorPosServicios2.push_back(vectorPosServicios.at(i));
            }
        }
    if(vectorPosServicios2.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
    }
}
void buscarPorOrigenYTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba el Origen: BA - MP");
    string msg = server->NewRecibir();
    server->enviar("MEscriba El Turno: TM - TT - TN");
    string msg2 = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;
    std::vector <Servicio*> vectorPosServicios2;

    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
             if(aux[0]==msg[0]&&aux[1]==msg[1]){
                vectorPosServicios.push_back(vectorServicios.at(i));
            }
        }
    }
    for(int i = 0;i< vectorPosServicios.size();i++){
        string aux2(vectorPosServicios.at(i)->servicio);
        bool exists = aux2.find(msg2) != std::string::npos;
        if(exists){
                request+=aux2;
                request+=";";
                vectorPosServicios2.push_back(vectorPosServicios.at(i));
            }
        }
    if(vectorPosServicios2.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
    }
}
void buscarPorFechaYTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba la fecha: DD-MM-AAAA");
    string msg = server->NewRecibir();
    server->enviar("MEscriba El Turno: TM - TT - TN");
    string msg2 = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;
    std::vector <Servicio*> vectorPosServicios2;

    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
                vectorPosServicios.push_back(vectorServicios.at(i));
            }
    }
    for(int i = 0;i< vectorPosServicios.size();i++){
        string aux2(vectorPosServicios.at(i)->servicio);
        bool exists = aux2.find(msg2) != std::string::npos;
        if(exists){
                request+=aux2;
                request+=";";
                vectorPosServicios2.push_back(vectorPosServicios.at(i));
            }
        }
    if(vectorPosServicios2.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
    }
}
void buscarPorOrigenFechaYTurno(Servidor *server, std::string usuario){
    std::string request =  "M";
    std::vector <Servicio*> vectorServicios = traerVectorServicios();
    server->enviar("MEscriba el Origen: BA - MP");
    string msg = server->NewRecibir();
    server->enviar("MEscriba la fecha: DD-MM-AAAA");
    string msg2 = server->NewRecibir();
    server->enviar("MEscriba el Turno: TM - TT -TN");
    string msg3 = server->NewRecibir();
    std::vector <Servicio*> vectorPosServicios;
    std::vector <Servicio*> vectorPosServicios2;

    for(int i = 0;i< vectorServicios.size();i++){
        string aux(vectorServicios.at(i)->servicio);
        bool exists = aux.find(msg) != std::string::npos;
        if(exists){
             if(aux[0]==msg[0]&&aux[1]==msg[1]){
                vectorPosServicios.push_back(vectorServicios.at(i));
            }
        }
    }
    for(int i = 0;i< vectorPosServicios.size();i++){
        string aux2(vectorPosServicios.at(i)->servicio);
        bool exists = aux2.find(msg2) != std::string::npos;
        if(exists){
            bool exists2 = aux2.find(msg3)!=std::string::npos;{
                request+=aux2;
                request+=";";
                vectorPosServicios2.push_back(vectorPosServicios.at(i));

            }
        }
    }
    if(vectorPosServicios2.size()==0){
        server->enviar("NNo se encuentran servicios disponibles");
    }else{
        server->enviar(request);
        string opt = server->NewRecibir();
        stringstream opcion(opt);
        int num;
        opcion>>num;
        num--;
        mostrarMenuGestionarPasajes(server,vectorPosServicios2.at(num),usuario);
    }
}
void mostrarMenuGestionarPasajes(Servidor *server,Servicio* servicio,string usuario){
    log("server","Usuario :"+usuario+"realiza consulta a servicio "+servicio->servicio);
    bool seguir = 1;
    while(seguir){

    string aux1="B";
    string aux2(servicio->asientos);

    server->enviar(aux1+aux2);
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


void reservarAsiento(Servidor* server,Servicio* servicio,string usuario){

    string fil,col;

    server->enviar("MIngrese fila");
    fil = server->NewRecibir();

    server->enviar("MIngrese columna");
    col = server->NewRecibir();

    switch(fil[0]){
        case 'a':
            fil = to_string(1);
            break;
        case 'b':
            fil = to_string(2);
            break;
        case 'c':
            fil = to_string(3);
            break;
        default:
            break;
    }

    string asiento= fil+";"+col;
    //verifica que el asiento este libre
    if(verificarAsientoLibre(servicio,asiento)== 1){
        ocuparAsiento(servicio,asiento);
        log(usuario,"Reserva un asiento "+fil+":"+col+";");
        log("server",usuario+ " Reserva un asiento "+fil+":"+col+";");

    }else{
    server->enviar("NAsiento ocupado");
    log(usuario,"Falla en reservar un asiento "+fil+":"+col+";en servicio "+servicio->servicio);
    log("server",usuario+ " Falla en reservar un asiento "+fil+":"+col+"; en servicio "+servicio->servicio);
    }

}


int verificarAsientoLibre(Servicio* servicio,string asiento){

    string asientoOcupado = asiento+";X;";
    cout<<endl;
    //verifica que exista el asiento libre
    //y envia no disponible si lo encuentra
    bool found = servicio->asientos.find(asientoOcupado) != string::npos;
        if(found){
            return 0;
        }
    return 1;
}


void ocuparAsiento(Servicio *servicio,string ass){

    string asientoLiberado = ass+";0;";
    string asientoOcupado = ass +";X;";
    //reemplazo en los asientos el liberado por ocupado
    string asientoReemplazado = reemplazar(servicio->asientos,asientoLiberado,asientoOcupado);
    ifstream file("servicios.bin",ios::binary | ios::in | ios::app);
    ofstream temp("temp.bin",ios::binary |ios::out|ios::app);

    servicio->setAsientos(asientoReemplazado);

    char buffer[409];
    Servicio *temporal;

    if(!file.is_open()){
        cout<<"no se puede abrir archivo"<<endl;
    }else{
        file.seekg(0,ios::end);
        int tam = file.tellg();
        //me muevo al principio del archibo
        file.seekg(0,ios::beg);
        //loopeo mientras no este en el final y no lo haya encontrado
        while(file.tellg()<tam)
        {

            file.read((char*)&buffer,sizeof(buffer));
            buffer[sizeof(buffer)] = '\0';
            temporal = deserializar(buffer);

            if(strcmp(temporal->servicio.c_str(),servicio->servicio.c_str())==0){
                    char tempServ[16];
                    char tempAs[393];
                    strcpy(tempServ,servicio->servicio.c_str());
                    strcpy(tempAs,asientoReemplazado.c_str());
                    cout<<"entro if"<<endl;
                    temp.write((char*)&tempServ,sizeof(tempServ));
                    temp.write((char*)&tempAs,sizeof(tempAs));
            }else{
                    char tempServ[16];
                    char tempAs[393];
                    strcpy(tempServ,servicio->servicio.c_str());
                    strcpy(tempAs,temporal->asientos.c_str());
                    cout<<"entro if"<<endl;
                    temp.write((char*)&tempServ,sizeof(tempServ));
                    temp.write((char*)&tempAs,sizeof(tempAs));
            }
        }
    }

    file.close();
    temp.close();
    remove("servicios.bin");
    rename("temp.bin","servicios.bin");
}


void liberarAsiento(Servidor* server,Servicio *servicio,string usuario){
    string fil,col;
    server->enviar("MIngrese fila");
    fil = server->NewRecibir();

    server->enviar("MIngrese columna");
    col = server->NewRecibir();

    switch(fil[0]){
        case 'a':
            fil = to_string(1);
            break;
        case 'b':
            fil = to_string(2);
            break;
        case 'c':
            fil = to_string(3);
            break;
        default:
            break;
    }
    string asiento= fil+";"+col;
       //verifica que el asiento este libre
    if(verificarAsientoLibre(servicio,asiento)==1){
        server->enviar("NEl Asiento no esta ocupado");
        log(usuario,"Falla en liberar un asiento "+fil+":"+col+";"+"del servicio :"+servicio->servicio);
        log("server",usuario+ " Falla en liberar un asiento "+fil+":"+col+";");
    }else{
        liberarAsientoOcupado(servicio,asiento);
        server->enviar("NEl Asiento Liberado");
        log(usuario,"Libera un asiento "+fil+":"+col+";");
        log("server",usuario+ " Libera un asiento "+fil+":"+col+";"+"del servicio :"+servicio->servicio);

    }
}

void liberarAsientoOcupado(Servicio *servicio,string ass){
    string asientoLiberado = ass+";0;";
    string asientoOcupado = ass +";X;";
    //reemplazo en los asientos el liberado por ocupado
    string asientoReemplazado = reemplazar(servicio->asientos,asientoOcupado,asientoLiberado);
    ifstream file("servicios.bin",ios::binary | ios::in | ios::app);
    ofstream temp("temp.bin",ios::binary |ios::out|ios::app);

    servicio->setAsientos(asientoReemplazado);

    char buffer[409];
    Servicio *temporal;

    if(!file.is_open()){
        cout<<"no se puede abrir archivo"<<endl;
    }else{
        file.seekg(0,ios::end);
        int tam = file.tellg();
        //me muevo al principio del archibo
        file.seekg(0,ios::beg);
        //loopeo mientras no este en el final y no lo haya encontrado
        while(file.tellg()<tam)
        {

            file.read((char*)&buffer,sizeof(buffer));
            buffer[sizeof(buffer)] = '\0';
            temporal = deserializar(buffer);

            if(strcmp(temporal->servicio.c_str(),servicio->servicio.c_str())==0){
                    char tempServ[16];
                    char tempAs[393];
                    strcpy(tempServ,servicio->servicio.c_str());
                    strcpy(tempAs,asientoReemplazado.c_str());
                    cout<<"entro if"<<endl;
                    temp.write((char*)&tempServ,sizeof(tempServ));
                    temp.write((char*)&tempAs,sizeof(tempAs));
            }else{
                    char tempServ[16];
                    char tempAs[393];
                    strcpy(tempServ,servicio->servicio.c_str());
                    strcpy(tempAs,temporal->asientos.c_str());
                    cout<<"entro if"<<endl;
                    temp.write((char*)&tempServ,sizeof(tempServ));
                    temp.write((char*)&tempAs,sizeof(tempAs));
            }
        }
    }

    file.close();
    temp.close();
    remove("servicios.bin");
    rename("temp.bin","servicios.bin");

}




void generarViajes(Servidor *server,string usuario){

    server->enviar("MIngrese Origen: BA(Buenos Aires) o MP(Mar del Plata) ");
    string origen = server->NewRecibir();

    server->enviar("MIngrese Destino: BA(Buenos Aires) o MP(Mar del Plata) ");
    string destino = server->NewRecibir();

    server->enviar("MIngrese Fecha con el formato DD-MM-AAAA: ");
    string fecha = server->NewRecibir();

    server->enviar("MIngrese Turno: TM(Turno Mañana) - TT(Turno Tarde) - TN(Turno Noche) ");
    string turno =  server->NewRecibir();


    string serv = origen+destino+fecha+turno;

    Servicio *s = new Servicio(serv);
    escribirArchivoServicio(s);
    server->enviar("NSe genero un nuevo servicio");
    log(usuario,"Ingresa un nuevo servicio a "+s->servicio);
    log("server","Usuario : "+usuario+" Ingresa un nuevo servicio "+s->servicio);

}



//escribe servicio en archivo
void escribirArchivoServicio(Servicio *s){
    //para guardar en el archivo servicios y tener un espacio no dinamico
    char tempServ[16];
    char tempAs[393];
    strcpy(tempServ,s->servicio.c_str());
    strcpy(tempAs,s->asientos.c_str());

    ofstream file("servicios.bin",ios::binary |ios::app);
    if(!file.is_open()){
        cout<<"no se puede abrir archivo"<<endl;
    }else{

        file.write((char*)&tempServ,sizeof(tempServ));
        file.write((char*)&tempAs,sizeof(tempAs));
    }
    file.close();
}

std::vector <Servicio*> traerVectorServicios(){
    string tmp;
    char buffer[409];
    vector<Servicio*> lista;
    Servicio *temp;

    ifstream file("servicios.bin",ios::binary | ios::in);
    if(!file.is_open()){
        cout<<"no se puede abrir archivo"<<endl;
    }else{
            file.seekg(0,ios::end);
            int tam = file.tellg();
            //me muevo al principio del archibo
            file.seekg(0,ios::beg);
        //loopeo mientras no este en el final y no lo haya encontrado
        while(file.tellg()<tam)
        {

            file.read((char*)&buffer,sizeof(buffer));
            buffer[sizeof(buffer)] = '\0';
            temp = deserializar(buffer);

            lista.push_back(temp);
            cout<<buffer<<endl;
        }
    }

    file.close();
    return lista;
}


void gestionarPasajes(Servidor *server , std::string usuario){
    menuBusquedas();

}

void CerrarSesion(Servidor *server,std::string usuario){
    server->enviar("x - Cierra Sesion");
    log (usuario, "Cierra Sesion");
    log (usuario, "===========================");
    server->Reiniciar();
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

Servicio* deserializar(string msg){
        string servicio = msg.substr(0,16);
        string asientos = msg.substr(16,393);
        Servicio* serv = new Servicio(servicio,asientos);
        return serv;
//}
}

string serializar(Servicio *servicio){
    return servicio->servicio+servicio->asientos;
}

string reemplazar(string asientos,string asientoLiberado,string asientoOcupado) {

    size_t start_pos = asientos.find(asientoLiberado);
    return asientos.replace(start_pos, asientoLiberado.length(), asientoOcupado);

}

