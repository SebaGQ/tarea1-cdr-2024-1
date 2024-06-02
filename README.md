# tarea1-cdr-2024-1
Primera entrega Comunicación de Datos y Redes 2024-1. Juego 4 en linea desarrollado en C++ con cliente/servidor.

## Requisitos

- `g++` (compilador de C++)
- `make` (herramienta de automatización de compilación)
- `pthread` (biblioteca de hilos POSIX)

## Compilación

Para compilar se debe ejecutar el comando `make` en el directorio raiz del proyecto , esto genera dos ejecutables  **cliente** y **servidor** .

## Ejecución

### Ejecutar el Servidor

Para iniciar el servidor en el puerto `7777`:

`make run_servidor`


### Ejecutar el Cliente

En otra terminal, para iniciar el cliente y conectarse al servidor en `127.0.0.1` y el puerto `7777`:

`make run_cliente`
