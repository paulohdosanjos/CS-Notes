#include "server_config.h"

// Estrutura que guarda informações específicas de cada conexão, que é gerida por uma thread
struct client_thread {
  int connfd; // socket da conexão
  state current_state;
  pthread_t thread_id; // também é a consumer_tag desse cliente
  unsigned char buf[MAX_FRAME_SIZE]; // buffer para guardar dados lidos/escritos do socket
  int bytes_read;
  int client_queue; // índice da fila na qual o cliente está inscrito, se ele for um consumidor
};

struct server_data {
  queue* queue_list[MAX_NUM_QUEUE]; // lista de filas
  int queue_list_size;
  queue* client_queue[MAX_NUM_QUEUE]; // lista de filas de clientes conectados em cada fila. Esse array é alinhado com queue_list, ou seja, client_queue[i] guarda a fila de consumidores conectados na fila queue_list[i]
};

extern server_data _server_data; // Informações globais do servidor compartilhadas por todas as threads 
extern pthread_mutex_t _server_data_mutex; // Mutex para controlar acesso às informações globais do servidor. Toda thread que acessa a estrutura _server_data obtém o esse mutex antes.
