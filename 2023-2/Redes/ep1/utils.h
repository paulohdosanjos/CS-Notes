int read_frame (int connfd, unsigned char* buf);

int read_header (int connfd, unsigned char* buf);

unsigned char* to_longstr(char* value);

unsigned char* to_shortstr(char* value);

void write_short_int(unsigned char* frame, int i, unsigned short int n);

void write_long_int(unsigned char* frame, int i, unsigned long int n);

void write_stream(unsigned char* frame, unsigned char* buffer, int i, int n);

unsigned char* concatenate(unsigned char* buffer1, int n1, unsigned char* buffer2, int n2);

int atribute_S_size(char* name, char* value);

void get_S_atribute(unsigned char* a_buffer, char* name, char* value);

int atribute_t_size(char* name);

void get_t_atribute(unsigned char* a_buffer, char* name, int value);

int atribute_s_size(char* name, char* value);

void get_s_atribute(unsigned char* a_buffer, char* name, char* value);

int write_S_atribute(unsigned char* table, char* a_name, char* a_value, int offset);

int write_t_atribute(unsigned char* table, char* a_name, int a_value, int offset);

int write_s_atribute(unsigned char* table, char* a_name, char* a_value, int offset);

int connection_start (unsigned char* frame);

int connection_tune (unsigned char* frame);

int connection_open_ok (unsigned char* frame);

int connection_close_ok (unsigned char* frame);

int channel_open_ok (unsigned char* frame);

int channel_close_ok (unsigned char* frame);

int basic_publish (unsigned char* frame, char* _exchange_name, char* _routing_key);

int queue_declare_ok(unsigned char* frame, char* _queue_name);





