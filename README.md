# Trabajo-Practico-Redes-2020
Trabajo Practico de la materia de Redes y Comunicaciones 2020
IDE Codeblocks, Lenguaje C++, Sistema Operativo Windows.
Cliente Servidor Sockets en C++.

Los Sockets son una interfaz que permiten que las aplicaciones puedan acceder a los servicios que brinda el software TCP/IP, permitiendo la comunicación entre procesos en el mismo equipo o en equipos diferentes. 

La Interfaz Socket proporciona funciones generalizadas que dan soporte a las comunicaciones en red empleando para ello muchos de los protocolos disponibles hoy en día. Los llamados sockets hacen referencia a todos los protocolos TCP/IP como una única familia. Las llamadas permiten al programador especificar el tipo de servicio requerido, en vez de el nombre de un protocolo específico.

Los Sockets se caracterizan por un dominio, un tipo y un protocolo de comunicación.
El dominio de comunicación nos dice como ser va a realizar la comunicación de los procesos que se van a intercomunicar, o mejor dicho, en que ambiente.
Si los procesos se comunicarán bajo la forma de un único sistema (tipo root de Unix), el dominio de comunicación será AF_UNIX, si los procesos se comunicarán como sistemas independientes y estos se hallan unidos mediante una red TCP/IP, el dominio de comunicación será AF_INET.

Algunos dominios: 
  . AF_INET (mediante una red TCP/IP).
  .AF_UNIX (en el mismo sistema).
  .Otros dominios.

Los tipos más comunes son:
Sockets Stream: son los más utilizados, hacen uso del protocolo TCP, el cual nos provee un flujo de datos bidireccional, secuenciado, sin duplicación de paquetes y libre de errores.
Sockets Datagram: hacen uso del protocolo UDP, el cual nos provee un flujo de datos bidireccional, pero los paquetes pueden llegar fuera de secuencia, pueden no llegar o contener errores. 
Por lo tanto el proceso que recibe los datos debe realizar re-secuenciamiento, eliminar duplicados y asegurar la confiabilidad. 
Se llaman también sockets sin conexión, porque no hay que mantener una conexión activa, como en el caso de sockets stream. 
Son utilizados para transferencia de información paquete por paquete. Ejemplo: dns, tftp, bootp, etc. 
Entones podríamos preguntar, Cómo hacen estos programas para funcionar si pueden perder datos ? 
Ellos implementan un protocolo encima de UDP que realiza control de errores.
Sockets SeqPacket: Establece una conexión fiable bidireccional con un tamaño de mensaje máximo definido. (Este tipo puede no estar habilitado en algunos sistemas.)
Sockets Raw: no son para el usuario común, son provistos principalmente para aquellos interesados en desarrollar nuevos protocolos de comunicación o para hacer uso de facilidades ocultas de un protocolo existente.

Cada tipo de Sockets tiene uno o más protocolos:
TCP/IP (Streams) 
UDP (Datagram)

Uso de Sockets:
Los sockets basados en la conexión son cliente-servidor: el servidor espera por una conexión del cliente.
Los sockets Independientes de la conexión son de igual a igual (peer-to-peer): cada proceso es simétrico.

Uso de puertos

Note que para iniciar un tipo de comunicación mediante un socket se requiere designar un puerto de comunicaciones. 
Esto significa que algunos puertos deben reservarse para éstos usos "bien conocidos". 

0-1023: Estos puertos pueden ser sólo enlazados por el sistema
1024-5000: Puertos designados a aplicaciones conocidas
5001-64K-1: Puertos disponibles para usos particulares

Modelo Simple Cliente-Servidor: Conexión Stream
Teoría de conexión:

En el servidor:

Se crea un socket de comunicación mediante la función socket(). 
El socket está formado por una dirección ip + un puerto de conexión, para ello hay que asociar esa información al socket mediante la función bind(). 
La parte servidor de la aplicación debe establecer una cola de conexiones entrantes para atender a los clientes según el orden de llegada, esto se hace mediante la función listen(). 
Ahora el servidor deberá atender las conexiones entrantes por el puerto que establecimos mediante la función accept(). 
Tras establecer una comunicación entre un cliente y un servidor se enviarán datos mediante la función send() y se recibirán mediante la función recv(). 
Al finalizar la comunicación deberemos cerrar el socket de comunicación.

En el cliente:

Se crea un socket de comunicación mediante la función socket(). 
El servidor está aceptando conexiones entrantes, así que nos conectamos al servidor mediante la función connect(). 
Recibimos y enviamos datos, pues ya estamos en contacto con el equipo remoto, mediante las funciones recv() y send(). 
Al finalizar la comunicación deberemos cerrar el socket de comunicación.

Funciones:

Función socket
SOCKET socket (int af, int type, int protocol); 

af: Dominio de comunicación (AF_INET en nuestro caso).

type: Tipo de dominio (SOCK_STREAM al ser conexión TCP/IP).

protocol: Protocolo de comunicación (dejaremos 0 para que se el sistema lo escoja por defecto, en este caso el sistema escogerá el protocolo TCP).

Valor de retorno: La función socket devolverá un descriptor del socket, o en caso de error INVALID_SOCKET y el código de error es retornado por WSAGetLastError.

Función bind
Cuando se crea un socket con socket(), se crea en un espacio de nombres (familia de direcciones), pero no tiene ningún nombre asignado. bind establece la asociación local del socket asignando un nombre local al socket anónimo.

int bind (SOCKET sock, const struct sockaddr FAR* name, int namelen); 

sock: Un descriptor que identifica el socket de comunicación.

name: La dirección para asignar al socket. La estructura sockaddr se define como:
struct sockaddr {
    u_short sa_family;
    char sa_data[14];
}; 

Excepto el campo sa_family, los demás campos de la estructura sockaddr se expresan en network byte order.

namelen: Tamaño de la estructura sockaddr.

Valor de retorno: La función bind devolverá 0 en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Función listen

int listen (SOCKET sock, int backlog);

sock: Un descriptor que identifica el socket de comunicación.

backlog: La longitud máxima que la cola de conexiones pendientes puede alcanzar. Si este valor es SOMAXCONN, entonces el proveedor del servicio subyacente responsable del socket pondrá backlog a un valor máximo razonable.

Valor de retorno: La función listen devolverá 0 en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Función accept

SOCKET accept (SOCKET sock, struct sockaddr FAR* addr, int FAR* addrlen);

sock: Un descriptor que identifica el socket de comunicación.

addr: Un puntero opcional a un buffer que recibe la dirección de la entidad que une, conocido como "capa de comunicaciones". El formato exacto del argumento addr es determinado por la familia de direcciónes establecida cuando el socket fue creado.

addrlen: Un puntero opcional a un entero que contiene la longitud de la dirección addr.

Valor de retorno: La función accept devolverá un descriptor de socket que será el que usaremos para enviar y recibir información con la aplicación cliente en caso se ejecutarse correctamente, en caso de error INVALID_SOCKET y el código de error es retornado por WSAGetLastError.

Función connect

int connect (SOCKET sock, const struct sockaddr FAR* name, int namelen);

sock: Un descriptor que identifica el socket de comunicación.

name: Zona de memoria de una estructura tipo sockaddr con los datos de conexión del cliente hacia el servidor.

namelen: Tamaño de la estructura sockaddr.

Valor de retorno: La función accept devolverá 0 en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Función send

int send (SOCKET sock, const char FAR * buf, int len, int flags);

sock: Un descriptor que identifica el socket de comunicación.

buf: Buffer donde se encuentra el mensaje a enviar.

len: Tamaño del buffer que contiene el mensaje a enviar.

flags: Banderas que especifican la forma en que se enviarán los datos (por defecto 0). Otros posibles flags son:

MSG_DONTROUTE: Especifica que los datos no deben estar sujetos a una ruta. Un Servidor de Sockets de Windows puede ignorar esta bandera.

MSG_OOB: Envía los datos fuera de banda (solo para sockets del tipo SOCK_STREAM).

Valor de retorno: La función send devolverá el número de bytes que se han enviado en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Esta función es bloqueante, es decir, hasta que se envíen los datos no se continuará con la ejecución del programa.

Función recv

int recv (SOCKET sock, char FAR* buf, int len, int flags); 


sock: Un descriptor que identifica el socket de comunicación.

buf: Puntero a un buffer donde se almacenará el mensaje enviado.

len: Tamaño del buffer que contendrá el mensaje enviado.

flags: Banderas que especifican la forma en que se recibirán los datos (por defecto 0). Otros posibles flags son:

MSG_PEEK: Acumula a los datos entrantes. Los datos se copian en el buffer pero no son removidos de la cola de la entrada.
MSG_OOB: Proceso fuera de banda.

Valor de retorno: La función recv devolverá el número de bytes que se han recibido en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Esta función es bloqueante, es decir, hasta que se reciban datos no se continuará con la ejecución del programa.



Función shutdown

La función shutdown desactiva el envío y/o la recepción de mensajes en un socket.

int shutdown (SOCKET sock, int how); 


sock: Un descriptor que identifica el socket de comunicación.

how: Bandera que especifica qué tipos de funcionamiento ya no se permitirán. Estas pueden ser:

SD_RECEIVE: Se bloquearán los mensajes entrantes subsiguientes. Esto no tiene efecto en las capas protocolares. Para los sockets TCP, si todavía hay datos en la cola del socket en espera de ser recibidos, o llegan datos de respuestas, la conexión se restablece hasta que los datos sean entregados al usuario. Para los sockets UDP, se aceptan los datagramas entrantes que se encuentren en la cola. En ningún caso se generará un paquete de error ICMP. 

SD_SEND: Se bloquearán los subsiguientes mensajes enviados . Para TCP sockets, un mensaje FIN será enviado.

SD_BOTH: Deshabilita tanto el envío como la recepción de mensajes subsiguientes.

Valor de retorno: La función shutdown devolverá 0 caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.



Función sendto

int sendto (SOCKET sock, const char FAR * buf, int len, int flags, const 
struct sockaddr FAR * to, int tolen); 


sock: Un descriptor que identifica el socket de comunicación.

buf: Buffer donde se encuentra el mensaje a enviar.

len: Tamaño del buffer que contiene el mensaje a enviar.

flags: Banderas que especifican la forma en que se enviarán los datos (por defecto 0). Otros posibles flags son:

MSG_DONTROUTE: Especifica que los datos no deben estar sujetos a una ruta. Un Servidor de Sockets de Windows puede ignorar esta bandera.

MSG_OOB: Envía los datos fuera de banda (solo para sockets del tipo SOCK_STREAM).

to: Puntero opcional a una estructura sockaddr con la dirección del socket al que se conectará.

tolen: Tamaño de la estructura ingresada en to.

Valor de retorno: La función sendto devolverá el número de bytes que se han enviado en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Esta función es bloqueante, es decir, hasta que se envíen los datos no se continuará con la ejecución del programa.



Función recvfrom

int recvfrom (SOCKET sock, char FAR* buf, int len, int flags, 
struct sockaddr FAR* from, int FAR* fromlen); 


sock: Un descriptor que identifica el socket de comunicación.

buf: Puntero a un buffer donde se almacenará el mensaje enviado.

len: Tamaño del buffer que contendrá el mensaje enviado.

flags: Banderas que especifican la forma en que se recibirán los datos (por defecto 0). Otros posibles flags son:

MSG_PEEK: Acumula a los datos entrantes. Los datos se copian en el buffer pero no son removidos de la cola de la entrada.
MSG_OOB: Proceso fuera de banda.

from: Puntero opcional a un buffer en el que se almacenará la dirección de la fuente conectada.

fromlen: Puntero a una variable interna donde se especifica el tamaño de la estructura pasada en from y donde se almacenará la cantidad de datos guardados.

Valor de retorno: La función recvfrom devolverá el número de bytes que se han recibido en caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Esta función es bloqueante, es decir, hasta que se reciban datos no se continuará con la ejecución del programa.

Función closesocket

int closesocket (SOCKET sock); 

sock: Un descriptor que identifica el socket de comunicación.

Valor de retorno: La función closesocket devolverá 0 caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Nota: Todas estas funciones se hallan implementadas en la librería libwsock32.a o wsock32.lib.

Otras funciones para el manejo de Sockets

Función getprotobyname

Retorna un puntero a una estructura la cual contiene el nombre y el número del protocolo correspondiente con el nombre ingresado.

struct protoent FAR * getprotobyname (const char FAR * name ); 

name: Buffer donde se encuentra el nombre del protocolo.

struct protoent {
    char FAR * p_name;
    char FAR * FAR * p_aliases;
    short p_proto;
}; 

Función getsockname

Devuelve la dirección del socket.

int getsockname (SOCKET sock, struct sockaddr FAR* name, int FAR* namelen ); 

sock: Un descriptor que identifica el socket de comunicación.

name: Buffer donde se almacenará la dirección (nombre) del socket.

namelen: Tamaño del buffer name.

Función gethostbyaddr

Obtiene información del Host a través de su dirección.

struct hostent FAR * gethostbyaddr (const char FAR * addr, int len, int type); 

addr: Puntero a una dirección en network byte order.

len: Largo de la dirección

type: Tipo de dirección.

Función gethostbyname

Obtiene información del Host a través de su nombre.

struct hostent FAR * gethostbyname (const char FAR * name); 

name: Buffer que contendrá el nombre del Host.

Función gethostname

Obtiene información del Host Local Predeterminado.

int gethostname (char FAR * name, int namelen ); 

name: Buffer donde se almacenará el nombre del Host.

namelen: Tamaño del buffer.

Función inet_addr

Convierte una dirección IP en notación números y puntos, en un unsigned long, retorna la dirección en network byte order.

unsigned long inet_addr (const char FAR * cp ); 

cp: Bufer que contiene la dirección IP.

Retorna -1 si hubo error.

Función inet_ntoa

Convierte una direccion IP unsigned long en network byte order, a un string en números y puntos.

char FAR * inet_ntoa (struct in_addr in ); 

in: Estructura que representa una dirección de IP de Internet.

Retorna NULL si hubo error.

APIS de Windows para el manejo de sockets

Función WSAStartup

Inicializa la Dll encargada de los sockets de Windows.

int WSAStartup (WORD wVersionRequested, LPWSADATA lpWSAData); 

wVersionRequested: Versión de Windows Sockets mínima necesitada. El byte de mayor orden especifica la versión menor (la revisión); el byte de menor orden especifica la versión mayor.

lpWSAData: Puntero a una estructura WSADATA que recibirá los detalles de la implementación de Windows Sockets.

Valor de retorno: La función WSAStartup devolverá 0 caso se ejecutarse correctamente, distinto de 0 en caso contrario.

Función WSACleanup

Libera la Dll encargada de los sockets de Windows.

int WSACleanup (void); 

Valor de retorno: La función WSACleanup devolverá 0 caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

Nota: Existen implementaciones de todas las funciones de sockets sobre WSA, por ejemplo WSASocket, WSAConnect, etc. pero éstas solo agregan más opciones a la forma en que la conexión se realizará.

Configurando los Sockets

Modos de funcionamiento del socket: Supongamos un ejemplo, creamos un socket y nos conectamos a un servidor, sabemos que el socket nos provee una comunicación bidireccional, podemos enviar y recibir datos simultáneamente. 

Llamamos a la función recv() para recibir datos, pero el servidor en ese momento no tiene nada para enviarnos, entonces la función recv() no retorna. Justo en ese momento queremos enviar datos hacia el servidor, pero no podemos porque la función recv() no retornó y nos bloqueó el programa. 

Debemos encontrar alguna forma para que la función recv() retorne aunque el servidor no envie nada. Esto se realiza estableciendo al socket como no bloqueante. 

Cuando creamos un socket, se establece como bloqueante, al llamar a ciertas funciones como accept(), recv(), send(), etc, se bloquea el programa. 

Para establecer al socket como no bloqueante utilizamos la función ioctlsocket() de la siguiente manera : 

int ioctlsocket (SOCKET sock, long cmd, u_long FAR* argp); 

sock: Un descriptor que identifica el socket de comunicación.

cmd: Comando que se va a aplicar al socket sock.

argp: Puntero a los parámetros para cmd.

Comandos soportados por esta función:

FIONBIO: Activa o desactiva el modo del no bloqueante del socket sock. argp apunta a un unsigned long que es distinto de cero si el modo no bloqueante será activado y a cero si será desactivado.

Nota: Las funciones WSAAsyncSelect o WSAEventSelect setean automáticamente el socket en modo no bloqueante. Si se omite WSAAsyncSelect o WSAEventSelect en un socket, entonces cualquier intento de usar ioctlsocket para setear el socket en modo no bloqueante fallará y devolverá el error WSAEINVAL. Para setear un socket en modo no bloqueante, se debe desactivar WSAAsyncSelect primero llamando WSAAsyncSelect con el parámetro de lEvent igual a cero, o desactivar WSAEventSelect llamando WSAEventSelect con el parámetro lNetworkEvents igual a cero.


Ejemplo:

WSAAsyncSelect (mysocket, NULL, 0 ,0); 

WSAEventSelect (mysocket, NULL, 0); 

Nota 2: Estas funciones se hallan incluidas en la librería libws2_32.a o ws2_32.lib. y requieren la versión de 2.0 de Winsocks.

FIONREAD: Determine la cantidad de datos que pueden leerse atómicamente del socket sock. argp apunta a un unsigned long donde ioctlsocket guarda el resultado. Si sock es un stream orientado (por ejemplo, tipo SOCK_STREAM), FIONREAD devuelve la cantidad de datos que pueden leerse en un solo recv; esto puede o no puede ser igual a la cantidad total de datos en la cola del socket. Si sock es un mensaje orientado (por ejemplo, tipo SOCK_DGRAM), FIONREAD devuelve el tamaño de los primeros datagram (el mensaje) en la cola del socket.

SIOCATMARK: Determina si se ha leído todo o parte de los datos fuera de banda. Esto sólo aplica a sockets de tipo stream (por ejemplo, tipo SOCK_STREAM) que se han configurado para la recepción en línea de cualquier dato fuera de banda (SO_OOBINLINE). Si ningún dato fuera de banda está esperando ser leído, la operación retorna TRUE. En caso contrario, retornará FALSE, y los próximos recv o recvfrom realizados en el socket recuperarán algunos o todos los datos que preceden la marca.

Valor de retorno: La función ioctlsocket devolverá 0 caso se ejecutarse correctamente, en caso de error SOCKET_ERROR y el código de error es retornado por WSAGetLastError.

unsigned long mod=0;
mysocket=socket(AF_INET, SOCK_STREAM, 0);
WSAAsyncSelect (mysocket, NULL, 0 ,0); //Modo no bloqueante...
ioctlsocket (mysocket, FIONBIO, &mod); //Modo bloqueante nuevamente... 

Una vez establecido el socket como no bloqueante, se puede llamar a la funciones bloqueantes como recv() para recibir datos, si no hay datos disponibles recv() devuelve SOCKET_ERROR y WSAGetLastError devuelve WSAEWOULDBLOCK. 

Se puede ir consultando (polling) el socket para saber si hay datos disponibles, pero esta no es una solución muy buena, se consume tiempo de CPU consultando al socket si tiene datos, se verá más adelante una solución más elegante. 

Función select

Nos permite monitorear un conjunto de descriptores de sockets y nos avisa cuales tienen datos para leer, cuáles están listos para escribir, y cuáles produjeron excepciones

int select (int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, 
const struct timeval FAR * timeout); 

nfds: Se ignora y sólo incluyó por compatibilidad.

readfds: Un puntero opcional a un conjunto de sockets que ejecutan lecturas.

writefds: Un puntero opcional a un conjunto de sockets que ejecutan escrituras.

exceptfds: Un puntero opcional a un conjunto de sockets que han cometido errores.

timeout: Tiempo máximo de espera, NULL por bloquear el funcionamiento.

Monitorea 3 conjuntos distintos de sockets, aquellos agregados al conjunto readfds los monitorea para ver si hay caracteres disponibles para leer, al conjunto writefds los monitorea para ver si están listos para ser escritos y al conjunto exceptfds los monitorea para ver si se producen excepciones.

Se definen cuatro macros en el archivo WINSOCK2.H para manipular y verificar los conjuntos de descriptores. La constante FD_SETSIZE determina el número máximo de descriptores en un conjunto. (El valor predefinido de FD_SETSIZE es de 64 pero puede modificarse definiendo FD_SETSIZE a otro valor antes incluir WINSOCK2.H)

Los macros para manipular y verificar los elementos de fd_set son:

FD_CLR(SOCKET sock, fd_set *set) - Quita el descriptor sock del conjunto.

FD_ISSET(SOCKET sock, fd_set *set) - Devuelve distinto de cero si sock es un miembro del conjunto. Si no, cero.

FD_SET(SOCKET sock, fd_set *set) - Agrega el descriptor sock al conjunto.

FD_ZERO(fd_set *set) - Inicializa el conjunto en NULO. 

timeout determina cuánto tiempo select puede esperar para retornar. Si timeout es NULL, select se bloqueará indefinidamente hasta por lo menos un descriptor cumpla con alguno de los criterios especificados. La otra opción es que timeout apunte a una structura timeval que especifique el tiempo máximo que select debe esperar antes de retornar. Cuando se produzca un evento el valor de timeval será alterado. Si timeval se inicializa con {0, 0}, select retornará inmediatamente.

Estructura timeval:

struct timeval{
    int tv_sec; // segundos
    int tv_usec; // micro segundos
}; 


Valor de retorno: Devuelve el número descriptores que están listos en las estructuras fd_set, 0 si expiró el límite de tiempo, o SOCKET_ERROR si ocurre un error y el código de error es retornado por WSAGetLastError. 

Ejemplo:

struct timeval timeout;
fd_set readfds;

timeout.tv_sec = 2;
timeout.tv_usec = 500000 ;

FD_ZERO(&readfds);
FD_SET(mysocket,&readfds);

select(0, &readfds, NULL, NULL, &timeout);

if (FD_ISSET(mysocket, &readfds ))
    printf ("mysocket se comunicó\n");
else
    printf(" Se Acabó el tiempo\n"); 

