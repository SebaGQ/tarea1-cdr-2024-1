#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>

using namespace std;

const int filas = 6;
const int columnas = 7;

// Clase para manejar el tablero del juego
class Tablero {
private:
    char tablero[filas][columnas];

public:
    Tablero() {
        inicializar(); // Inicializa el tablero al crear el objeto
    }

    void inicializar() {
        // Rellena el tablero con espacios en blanco
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j < columnas; j++) {
                tablero[i][j] = ' ';
            }
        }
    }

    // Imprime el tablero en pantalla
    void imprimir(int socket_cliente) {
        string tablero_str = "TABLERO\n";
        for (int i = 0; i < filas; i++) {
            tablero_str += to_string(filas - i) + "|";
            for (int j = 0; j < columnas; j++) {
                tablero_str += tablero[i][j];
                tablero_str += " ";
            }
            tablero_str += "\n";
        }
        tablero_str += "  -------------\n";
        tablero_str += "  1 2 3 4 5 6 7\n";
        // Envia el tablero al cliente
        send(socket_cliente, tablero_str.c_str(), tablero_str.size(), 0);
    }

    //Busca los 4 en linea
    bool comprobarGanador(char ficha) {
        // Revisar filas
        for (int i = 0; i < filas; i++) {
            for (int j = 0; j <= columnas - 4; j++) {
                if (tablero[i][j] == ficha && tablero[i][j + 1] == ficha && tablero[i][j + 2] == ficha && tablero[i][j + 3] == ficha) {
                    return true;
                }
            }
        }
        // Revisar columnas
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 0; j < columnas; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j] == ficha && tablero[i + 2][j] == ficha && tablero[i + 3][j] == ficha) {
                    return true;
                }
            }
        }
        // Revisar diagonales
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 0; j <= columnas - 4; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j + 1] == ficha && tablero[i + 2][j + 2] == ficha && tablero[i + 3][j + 3] == ficha) {
                    return true;
                }
            }
        }
        // Revisar diagonales inversas
        for (int i = 0; i <= filas - 4; i++) {
            for (int j = 3; j < columnas; j++) {
                if (tablero[i][j] == ficha && tablero[i + 1][j - 1] == ficha && tablero[i + 2][j - 2] == ficha && tablero[i + 3][j - 3] == ficha) {
                    return true;
                }
            }
        }
        return false;
    }

    bool tableroLleno() {
        // Verifica si el tablero esta lleno
        for (int i = 0; i < filas; ++i) {
            for (int j = 0; j < columnas; ++j) {
                if (tablero[i][j] == ' ') {
                    return false;
                }
            }
        }
        return true;
    }

    bool colocarFicha(int col, char ficha) {
        // Coloca una ficha en la columna especificada
        for (int i = filas - 1; i >= 0; i--) {
            if (tablero[i][col - 1] == ' ') {
                tablero[i][col - 1] = ficha;
                return true;
            }
        }
        return false;
    }

    bool columnaLlena(int col) {
        // Verifica si una columna esta llena
        for (int i = 0; i < filas; ++i) {
            if (tablero[i][col - 1] == ' ') {
                return false;
            }
        }
        return true;
    }
};

// Clase para manejar el juego
class Juego {
private:
    Tablero tablero; // Instancia del tablero

public:
    void jugar(int socket_cliente, struct sockaddr_in direccionCliente) {
        srand(time(NULL)); // Semilla para aleatoriedad
        char buffer[1024]; // Tamaño del buffer
        memset(buffer, 0, sizeof(buffer));
        int n_bytes = 0;
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
        cout << "Juego nuevo [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]" << endl;
        tablero.inicializar();
        char AlAzar = (rand() % 2 == 0) ? 'S' : 'C'; //Determina quien comienza de manera aleatoria
        string mensaje = "Presione cualquier tecla para iniciar el juego"; // Mensaje inicial
        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
        
        n_bytes = recv(socket_cliente, buffer, 1024, 0);
        if (n_bytes <= 0) {
           cout << "Error en la conexion, Saliendo del juego." << endl;
           return;
        }
        
        tablero.imprimir(socket_cliente);

        if (AlAzar == 'C') {
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: inicia juego el cliente" << endl;
        } else {
            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: inicia juego el servidor" << endl;
        }
        
        //Codigo del juego como tal
        while (true) {
                int columna;
                buffer[n_bytes] = '\0';
                
                //Turno del cliente
                if (AlAzar == 'C') {
                    mensaje = "Es su turno, ingrese columna. (1-7) ";
                    // Envia mensaje al cliente
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0);
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    buffer[n_bytes] = '\0';
                    if (strncmp(buffer, "exit", 4) == 0) {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna)) {
                        mensaje = "Columna llena, eliga otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0) {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    while (columna < 1 || columna > 7) {
                        mensaje = "Columna invalida, vuelva a ingresar: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0) {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.colocarFicha(columna, 'C');
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                    if (tablero.comprobarGanador('C')) {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Ganador Cliente, felicitaciones\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << endl;
                        break;
                    }
                    if (tablero.tableroLleno()) {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                        break;
                    }
                    mensaje = "\nEs el turno del Servidor...\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                    colocarFichaServidor();
                    if (tablero.comprobarGanador('S')) {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana Servidor." << endl;
                        break;
                    }
                    tablero.imprimir(socket_cliente);
                    //Fin turno cliente
                
                 //Turno del servidor
                } else if (AlAzar == 'S') {
                    mensaje = "Juega el servidor\nEspere su turno\n";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                    colocarFichaServidor();
                    tablero.imprimir(socket_cliente);
                    mensaje = "Es su turno, ingrese columna. (1-7): ";
                    send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                    n_bytes = recv(socket_cliente, buffer, 1024, 0);
                    if (strncmp(buffer, "exit", 4) == 0) {
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                        break;
                    }
                    buffer[n_bytes] = '\0';
                    columna = atoi(&buffer[0]);
                    while (tablero.columnaLlena(columna)) {
                        mensaje = "Columna llena, eliga otra: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0) {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    while (columna < 1 || columna > 7) {
                        mensaje = "Columna invalida, vuelva a ingresar: ";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        n_bytes = recv(socket_cliente, buffer, 1024, 0);
                        if (strncmp(buffer, "exit", 4) == 0) {
                            cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "] ha salido del juego." << endl;
                            break;
                        }
                        buffer[n_bytes] = '\0';
                        columna = atoi(&buffer[0]);
                    }
                    tablero.colocarFicha(columna, 'C');
                    cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: Cliente juega columna " << columna << endl;
                    
                    //Se verifica si existe ganador o fue empate
                    if (tablero.comprobarGanador('C')) {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Ganador Cliente, felicitaciones\nFin del Juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana cliente." << endl;
                        break;
                    }
                    if (tablero.comprobarGanador('S')) {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Gana el Servidor...\nFin del juego.\n";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: gana Servidor." << endl;
                        break;
                    }
                    if (tablero.tableroLleno()) {
                        tablero.imprimir(socket_cliente);
                        mensaje = "Tablero lleno, es un empate.";
                        send(socket_cliente, mensaje.c_str(), mensaje.size(), 0); // Envia mensaje al cliente
                        cout << "Juego [" << ip << ":" << ntohs(direccionCliente.sin_port) << "]: empate." << endl;
                        break;
                    }
                    tablero.imprimir(socket_cliente);
                    //Fin turno servidor
                }
            }
        }
    }

private:
    void colocarFichaServidor() {
        srand(time(NULL));
        bool colocado = false;
        while (!colocado) {
            int col = rand() % columnas;
            if (tablero.colocarFicha(col + 1, 'S')) {
                colocado = true;
            }
        }
    }
};

void *jugar_wrapper(void *arg) {
    pair<int, struct sockaddr_in> *data = (pair<int, struct sockaddr_in> *)arg; // Extrae socket y direccion de los args 
    
    int socket_cliente = data->first; // Asigna el socket del cliente
    
    struct sockaddr_in direccionCliente = data->second; // Asigna la direccion del cliente
    
    Juego juego; // Crea una instancia de la clase Juego
    
    juego.jugar(socket_cliente, direccionCliente); // Inicia el juego
    
    //Aqui ya termino el juego.
    
    close(socket_cliente); // Cierra el socket del cliente
    
    delete data; // Libera la memoria asignada para el par
    
    // Finaliza el hilo
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    
     // Verifica presencia de argumentos
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <puerto>" << endl;
        return 1;
    }

    int port = atoi(argv[1]);
    int socket_server = 0;
    struct sockaddr_in direccionServidor, direccionCliente;
    
    // Se crea el socker
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "No se pudo crear el socket." << endl;
        return 1;
    }
    
    //Configura direccion del servidor
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port = htons(port);

    // Asigna al socket el puerto y la direccion especificada
    if (bind(socket_server, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor)) < 0) {
        cerr << "Error en bind()." << endl;
        return 1;
    }

    if (listen(socket_server, 1024) < 0) {
        cerr << "Error en listen()." << endl;
        return 1;
    }

    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);

    cout << "Esperando conexiones..." << endl;
    //Se queda esperando conexiones
    while (true) {
        int socket_cliente;
        
        //Se acepta la conexion
        if ((socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cerr << "Error en accept()." << endl;
            continue;
        }
        //Crea un hilo para cada conexion con el cliente
        pthread_t thread;
        pair<int, struct sockaddr_in> *data = new pair<int, struct sockaddr_in>(socket_cliente, direccionCliente);
        // Cierra el socket y limpia memoria en caso de error
        if (pthread_create(&thread, NULL, jugar_wrapper, (void *)data) != 0) {
            cerr << "Error creando hilo." << endl;
            delete data;
            close(socket_cliente);
        } else {
            pthread_detach(thread); // Desconecta el hilo para evitar fugas de memoria
        }
    }

    return 0;
}
