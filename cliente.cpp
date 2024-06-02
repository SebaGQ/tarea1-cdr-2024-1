#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

// Clase para manejar la conexion del cliente
class Cliente {
private:
    int socket_fd;
    struct sockaddr_in server;
    const int buffer_size = 1024;

public:
    Cliente(const string &ip, int puerto) {
        // Crea un socket TCP
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw runtime_error("No se pudo crear el socket.");
        }

        // Configura la direccion del servidor
        server.sin_addr.s_addr = inet_addr(ip.c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(puerto);

        // Conecta el socket al servidor
        if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
            throw runtime_error("Conexion fallida.");
        }

        cout << "Conectado al servidor en " << ip << ":" << puerto << "." << endl;
        cout << "Para salir, puedes escribir 'exit' en cualquier momento." << endl;
    }

    // Destructor de la clase Cliente
    ~Cliente() {
        close(socket_fd); // Cierra el socket
        cout << "Conexion cerrada." << endl;
    }

    void interactuarConServidor() {
         char buffer[buffer_size]; // Buffer para enviar y recibir datos
        fd_set readfds; // Conjunto de descriptores de archivo para la funcion select
        struct timeval tv;

        while (true) {
            FD_ZERO(&readfds); // Limpia el conjunto de descriptores de archivo
            FD_SET(socket_fd, &readfds); // Añade el socket del cliente al conjunto
            FD_SET(STDIN_FILENO, &readfds); // Añade la entrada estandar al conjunto

            tv.tv_sec = 5; // 5 segundos de tiempo de espera
            tv.tv_usec = 0;

            // Espera hasta que haya actividad en el socket o en la entrada estandar
            int activity = select(socket_fd + 1, &readfds, NULL, NULL, &tv);

            if (activity < 0) {
                cout << "Error en la seleccion." << endl;
                break;
            }
            
            // Verifica que el socket tenga datos
            if (FD_ISSET(socket_fd, &readfds)) {
                memset(buffer, 0, buffer_size); //limpia el bufer
                int len = recv(socket_fd, buffer, buffer_size - 1, 0); //Recbe respuesta servidor y la setea en el bufer
                if (len > 0) {
                    cout << buffer << endl; // Imprime la respuesta del servidor
                } else {
                    cout << "Conexion cerrada por el servidor." << endl;
                    break;
                }
            }

            // Verifica si hay datos para leer en la entrada estandar
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                cout << " ";
                string input;
                getline(cin, input); // Setea el input

                if (!input.empty()) {
                    send(socket_fd, input.c_str(), input.length(), 0); // Envia los datos al servidor
                    if (input == "exit") {
                        cout << "Cerrando la conexion..." << endl;
                        break;
                    }
                }
            }
            
        }
    }
};

int main(int argc, char *argv[]) {
    // Verifica los argumentos
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <IP> <puerto>" << endl;
        return 1;
    }

    try {
        // Crea un objeto Cliente y se conecta al servidor
        Cliente cliente(argv[1], atoi(argv[2]));
        cliente.interactuarConServidor();
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
