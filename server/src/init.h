#ifndef __INIT_H__
#define __INIT_H__

#include "dhcp_server.h"
#include "unix_server.h"
int init_allocator(dhcp_server_t *server);
int init_dhcp_options(dhcp_server_t *server);
int init_cache(dhcp_server_t *server);
int init_ACL(dhcp_server_t *server);
int init_dynamic_ACL(dhcp_server_t *server);
int init_unix_commands(unix_server_t *server);

#endif // !__INIT_H__

