#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>




using namespace std;

class Servicio {
public:
    string servicio;
    string asientos;

    Servicio(string serv,string as){
        this->servicio = serv;
        this->asientos = as;
    }

};

std::vector <Servicio*> traerVectorServicios();
Servicio* deserializar(string msg);
void mostrarAsientos(string asientos);
vector<string> procesarAsientos(string asientos);

int main()
{
    std::vector <Servicio*> servicios;
    servicios = traerVectorServicios();
    for(int i = 0; i < servicios.size(); i++){
        cout<<servicios[i]->servicio<<endl;
        mostrarAsientos(servicios[i]->asientos);
        system("PAUSE"); // para ver uno a uno en vez de todo
    }



    return 0;
}



Servicio* deserializar(string msg){
        string servicio = msg.substr(0,16);
        string asientos = msg.substr(16,393);
        Servicio* serv = new Servicio(servicio,asientos);
        return serv;
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
            //cout<<buffer<<endl;
        }
    }

    file.close();
    return lista;
}

void mostrarAsientos(string asientos){
    //system("cls");
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
        if(vectorAsientos.at(i)=="0"){
            cout<<"O ";
        }else{
            cout<<"X ";
        }
    }
            cout<<"\n"<<endl;
            cout<<"\n"<<endl;

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


