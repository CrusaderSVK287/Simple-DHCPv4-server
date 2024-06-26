
#ifndef __DHCPNAK_H__
#define __DHCPNAK_H__

#include "../dhcp_server.h"
#include "../dhcp_packet.h"

/* Handle DHCPNAK messages */
int message_dhcpnak_send(dhcp_server_t *server, dhcp_message_t *message);
int message_dhcpnak_build(dhcp_server_t *server, dhcp_message_t *request);

#endif // !__DHCPNAK_H__

