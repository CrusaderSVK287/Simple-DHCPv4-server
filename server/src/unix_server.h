#ifndef __UNIX_SERVER__
#define __UNIX_SERVER__

#include "utils/llist.h"
#define UNIX_SOCKET_PATH "/run/dhcp.sock"

typedef struct unix_server {    
    int fd;
    llist_t *commands;
} unix_server_t;

enum unix_svr_status {
    UNIX_STATUS_BAD_COMMAND = -2,
    UNIX_STATUS_ERROR = -1,
    UNIX_STATUS_OK = 0,
    UNIX_STATUS_OK_NO_REQUEST = 1,
 };

int unix_server_init(unix_server_t *server);

/* We cannot recursively include, we need to use void pointer and then cast it */
int unix_server_handle(void *dhcp_server);

void unix_server_clean(unix_server_t *server);

#endif // !__UNIX_SERVER__

