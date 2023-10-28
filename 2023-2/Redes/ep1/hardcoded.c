#include "hardcoded.h"

const unsigned char DEFAULT_PROTOCOL_HEADER[] = "\x41\x4d\x51\x50\x00\x00\x09\x01";

const unsigned char CONNECTION_START_FRAME[] = {
"\x01\x00\x00\x00\x00\x01\x93\x00\x0a\x00\x0a\x00\x09\x00\x00\x01\x6e\x0c\x63" 
"\x61\x70\x61\x62\x69\x6c\x69\x74\x69\x65\x73\x46\x00\x00\x00\xc7\x12\x70\x75"
"\x62\x6c\x69\x73\x68\x65\x72\x5f\x63\x6f\x6e\x66\x69\x72\x6d\x73\x74\x01\x1a"
"\x65\x78\x63\x68\x61\x6e\x67\x65\x5f\x65\x78\x63\x68\x61\x6e\x67\x65\x5f\x62"
"\x69\x6e\x64\x69\x6e\x67\x73\x74\x01\x0a\x62\x61\x73\x69\x63\x2e\x6e\x61\x63"
"\x6b\x74\x01\x16\x63\x6f\x6e\x73\x75\x6d\x65\x72\x5f\x63\x61\x6e\x63\x65\x6c"
"\x5f\x6e\x6f\x74\x69\x66\x79\x74\x01\x12\x63\x6f\x6e\x6e\x65\x63\x74\x69\x6f"
"\x6e\x2e\x62\x6c\x6f\x63\x6b\x65\x64\x74\x01\x13\x63\x6f\x6e\x73\x75\x6d\x65"
"\x72\x2e\x70\x72\x69\x6f\x72\x69\x74\x69\x65\x73\x74\x01\x1c\x61\x75\x74\x68"
"\x65\x6e\x74\x69\x63\x61\x74\x69\x6f\x6e\x5f\x66\x61\x69\x6c\x75\x72\x65\x5f"
"\x63\x6c\x6f\x73\x65\x74\x01\x10\x70\x65\x72\x5f\x63\x6f\x6e\x73\x75\x6d\x65"
"\x72\x5f\x71\x6f\x73\x74\x01\x0f\x64\x69\x72\x65\x63\x74\x5f\x72\x65\x70\x6c"
"\x79\x5f\x74\x6f\x74\x01\x0c\x63\x6c\x75\x73\x74\x65\x72\x5f\x6e\x61\x6d\x65"
"\x53\x00\x00\x00\x18\x73\x65\x72\x76\x69\x64\x6f\x72\x41\x4d\x51\x50\x40\x6c"
"\x61\x75\x72\x61\x50\x61\x6c\x6d\x65\x72\x09\x63\x6f\x70\x79\x72\x69\x67\x68"
"\x74\x53\x00\x00\x00\x00\x0b\x69\x6e\x66\x6f\x72\x6d\x61\x74\x69\x6f\x6e\x53"
"\x00\x00\x00\x00\x08\x70\x6c\x61\x74\x66\x6f\x72\x6d\x53\x00\x00\x00\x00\x07"
"\x70\x72\x6f\x64\x75\x63\x74\x53\x00\x00\x00\x18\x73\x65\x72\x76\x69\x64\x6f"
"\x72\x41\x4d\x51\x50\x73\x69\x6d\x70\x6c\x69\x66\x69\x63\x61\x64\x6f\x07\x76"
"\x65\x72\x73\x69\x6f\x6e\x53\x00\x00\x00\x0b\x30\x2e\x30\x2e\x30\x2e\x30\x2e"
"\x30\x2e\x31\x00\x00\x00\x0e\x50\x4c\x41\x49\x4e\x20\x41\x4d\x51\x50\x4c\x41"
"\x49\x4e\x00\x00\x00\x05\x65\x6e\x5f\x55\x53\xce"
};

const unsigned char CONNECTION_TUNE_FRAME [] = {
"\x01\x00\x00\x00\x00\x00\x0c\x00\x0a\x00\x1e\x07\xff\x00\x02\x00\x00\x00\x3c"
"\xce"
};

const unsigned char CONNECTION_OPEN_OK_FRAME [] = {
"\x01\x00\x00\x00\x00\x00\x05\x00\x0a\x00\x29\x00\xce"
};

const unsigned char CONNECTION_CLOSE_OK_FRAME [] = {
"\x01\x00\x00\x00\x00\x00\x04\x00\x0a\x00\x33\xce"
};

const unsigned char CHANNEL_OPEN_OK_FRAME [] = {
"\x01\x00\x01\x00\x00\x00\x08\x00\x14\x00\x0b\x00\x00\x00\x00\xce"
};

const unsigned char CHANNEL_CLOSE_OK [] = {
"\x01\x00\x01\x00\x00\x00\x04\x00\x14\x00\x29\xce"
};