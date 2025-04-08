#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int32_t gi = 0;
int32_t ngi() {
    int32_t r = gi++;
    return r;
}

int main() {
    /*
    // WIP code to support the authentication protocol
    const char* xa = getenv("XAUTHORITY");
    bool fxa = false;
    if(!xa) {
        fxa = true;

        const char* h = getenv("HOME");
        xa = malloc(strlen(h) + strlen("/.Xauthority") + 1);
        strcpy(xa, h);
        strcat(xa, "/.Xauthority");
    }
    */

    int sk = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sk <= 0) {
        fprintf(stderr, "Couldn't open socket\n");
        return 13;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/.X11-unix/X0", sizeof(addr.sun_path) - 1);

    int r = connect(sk, (struct sockaddr *)&addr, sizeof(addr));
    if(r) {
        perror("Couldn't connect to socket\n");
        return 13;
    }

    unsigned char ir[12];
    memset(ir, 0, 12);
    ir[0] = 'l';
    ir[2] = 11;

    char wb[16 * 1024];
    char rb[16 * 1024];

    int br = read(sk, rb, 8);

    // TODO: Handle possible errors

    br = read(sk, rb + 8, (16 * 1024) - 8);

    // create window

    uint16_t lov = *((uint16_t*)(&(rb[24])));
    int32_t sso = 40 + lov + ((4 - (lov % 4)) % 4) + (8 * (*((uint16_t*)(&(rb[29])))));
    uint32_t rw = *((uint32_t*)(&(rb[sso])));
    uint32_t rvi = *((uint32_t*)(&(rb[sso + 32])));
    uint32_t ridb = *((uint32_t*)(&(rb[12])));
    uint32_t ridm = *((uint32_t*)(&(rb[16])));

    int32_t wid = ngi();
    int32_t cwfc = 2;
    int rl = 8 + cwfc;

    wb[0] = 1;
    wb[1] = 0; // depth
    *((int16_t *)(&(wb[2]))) = rl;
    *((int32_t *)(&(wb[4]))) = wid;
    *((int32_t *)(&(wb[8]))) = rw;
    *((int16_t *)(&(wb[12]))) = 100; // x
    *((int16_t *)(&(wb[14]))) = 100; // y
    *((int16_t *)(&(wb[16]))) = 512; // width
    *((int16_t *)(&(wb[18]))) = 512; // height
    *((int16_t *)(&(wb[20]))) = 1; // border width
    *((int16_t *)(&(wb[22]))) = 1; // class
    *((int32_t *)(&(wb[24]))) = rvi;
    *((int32_t *)(&(wb[28]))) = 0x800 | 2;
    *((int32_t *)(&(wb[32]))) = 0xff000000;
    *((int32_t *)(&(wb[36]))) = 0x8000 | 1;

    int bw = write(sk, wb, rl * 4);

    // map window

    wb[0] = 8;
    wb[1] = 0;
    *((int16_t *)(&(wb[2]))) = rl;
    *((int32_t *)(&(wb[4]))) = wid;

    bw = write(sk, wb, 8);
}