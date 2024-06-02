# Definición de los compiladores y las banderas
CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lpthread

# Nombres de los ejecutables
CLIENTE_EXEC = cliente
SERVIDOR_EXEC = servidor

# Regla para compilar todo
all: $(CLIENTE_EXEC) $(SERVIDOR_EXEC)

# Regla para compilar el cliente
$(CLIENTE_EXEC): cliente.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regla para compilar el servidor
$(SERVIDOR_EXEC): servidor.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regla para limpiar los archivos generados
clean:
	rm -f $(CLIENTE_EXEC) $(SERVIDOR_EXEC)

# Regla para ejecutar el cliente
run_cliente:
	./cliente 127.0.0.1 7777

# Regla para ejecutar el servidor
run_servidor:
	./servidor 7777

# Phony targets
.PHONY: all clean run_cliente run_servidor