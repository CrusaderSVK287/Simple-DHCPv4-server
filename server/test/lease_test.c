#include <fcntl.h>
#include <lease.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
/* tests.h include redefinition of LEASE_PATH_PREFIX */
#include "address_pool.h"
#include "allocator.h"
#include "cJSON.h"
#include "dhcp_server.h"
#include "tests.h"
#include "greatest.h"
#include "utils/llist.h"
#include "utils/xtoy.h"
#include <unistd.h>

/*
 * This function is not exposed by API but defined in dhcp_server.c 
 * priv is supposed to be dhcp_server_t 
 */
extern int check_lease_expirations(uint32_t check_time, void *priv);

static int lease_path_ok = -1;
TEST test_undef_lease_path_for_testing()
{
        if (strcmp("./test/test_leases/", LEASE_PATH_PREFIX) != 0) {
                PASS();
        }
        mkdir(LEASE_PATH_PREFIX, 0777);
        lease_path_ok = 0;
        PASS();
}

TEST test_lease_file_init()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        if (stat(LEASE_PATH_PREFIX "test_pool.lease", &st) >= 0) {
                remove(LEASE_PATH_PREFIX "test_pool.lease");
        }

        ASSERT_EQ(-1, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        lease_t l = {
                .address = ipv4_address_to_uint32("192.168.1.10"),
                .subnet  = ipv4_address_to_uint32("255.255.255.0"),
                .xid     = 0x80706050,
                .lease_start = 100,
                .lease_expire = 1000,
                .pool_name = "test_pool",
                .flags = 0,
        };
        uint8_t mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        memcpy(l.client_mac_address, mac, 6);

        ASSERT_EQ(LEASE_OK, lease_add(&l));
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        PASS();
}

// this test also has a role to setup the .lease file for future tests
TEST test_add_leases()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        lease_t l1 = {
                .address = ipv4_address_to_uint32("192.168.1.186"),
                .subnet  = ipv4_address_to_uint32("255.255.255.128"),
                .xid     = 0x616ab16f,
                .lease_start = 454148657,
                .lease_expire = 547784223,
                .pool_name = "test_pool",
                .flags = 0,
        };
        lease_t l2 = {
                .address = ipv4_address_to_uint32("192.168.1.33"),
                .subnet  = ipv4_address_to_uint32("255.255.255.128"),
                .xid     = 0xa156bf84,
                .lease_start = 60,
                .lease_expire = 60000,
                .pool_name = "test_pool",
                .flags = 0,
        };
        uint8_t mac1[] = {0xA1, 0xB1, 0xC1, 0xD1, 0xE1, 0xF1};
        memcpy(l1.client_mac_address, mac1, 6);
        uint8_t mac2[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
        memcpy(l2.client_mac_address, mac2, 6);

        ASSERT_EQ(LEASE_OK, lease_add(&l1));
        ASSERT_EQ(LEASE_OK, lease_add(&l2));

        PASS();
}

TEST test_are_3_leases_present()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        int fd = open(LEASE_PATH_PREFIX "test_pool.lease", O_RDONLY);
        if (fd < 0)
                FAIL();

        char buf[8000];
        memset(buf, 0, 8000);
        if (read(fd, buf, 8000) < 0)
                FAIL();

        close(fd);

        cJSON *root = cJSON_Parse(buf);
        ASSERT_NEQ(NULL, root);
        cJSON *array = cJSON_GetObjectItem(root, "leases");
        ASSERT_NEQ(NULL, array);
        ASSERT_EQ(3, cJSON_GetArraySize(array));

        cJSON_Delete(root);

        PASS();
}

// lease_t l1 = {
//         .address = ipv4_address_to_uint32("192.168.1.186"),
//         .subnet  = ipv4_address_to_uint32("255.255.255.128"),
//         .xid     = 0x616ab16f,
//         .lease_start = 454148657,
//         .lease_expire = 547784223,
//         .pool_name = "test_pool",
//         .flags = 0,
// };
TEST test_retrieve_lease()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        lease_t l = {0};
        ASSERT_EQ(LEASE_OK, lease_retrieve(&l, ipv4_address_to_uint32("192.168.1.186"), "test_pool"));

        ASSERT_EQ(l.address, ipv4_address_to_uint32("192.168.1.186"));
        ASSERT_EQ(l.subnet, ipv4_address_to_uint32("255.255.255.128"));
        ASSERT_EQ(l.xid, 0x616ab16f);
        ASSERT_EQ(l.lease_start, 454148657);
        ASSERT_EQ(l.lease_expire, 547784223);
        ASSERT_EQ(l.flags, 0);

        PASS();
}

TEST test_retrieve_lease_address()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        llist_t *pools = llist_new();
        address_pool_t *p = address_pool_new_str("test_pool", "192.168.1.1", "192.168.1.254", "255.255.255.0");
        ASSERT_EQ(0, llist_append(pools, p, false));



        lease_t l = {0};
        ASSERT_EQ(LEASE_OK, lease_retrieve_address(&l, ipv4_address_to_uint32("192.168.1.186"), pools));

        ASSERT_EQ(l.address, ipv4_address_to_uint32("192.168.1.186"));
        ASSERT_EQ(l.subnet, ipv4_address_to_uint32("255.255.255.128"));
        ASSERT_EQ(l.xid, 0x616ab16f);
        ASSERT_EQ(l.lease_start, 454148657);
        ASSERT_EQ(l.lease_expire, 547784223);
        ASSERT_EQ(l.flags, 0);
        ASSERT_STR_EQ(l.pool_name, "test_pool");

        llist_destroy(&pools);
        address_pool_destroy(&p);

        PASS();
}

TEST test_retrieve_non_existent()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        lease_t l = {0};
        ASSERT_EQ(LEASE_DOESNT_EXITS, lease_retrieve(&l, ipv4_address_to_uint32("192.168.1.222"), "test_pool"));

        PASS();
}

TEST test_remove_lease()
{
        if (lease_path_ok < 0)
                SKIP();

        struct stat st;
        ASSERT_EQ(0, stat(LEASE_PATH_PREFIX "test_pool.lease", &st));

        lease_t l = {0};
        ASSERT_EQ(LEASE_OK, lease_remove_address_pool(ipv4_address_to_uint32("192.168.1.186"), "test_pool"));
        ASSERT_EQ(LEASE_DOESNT_EXITS, lease_retrieve(&l, ipv4_address_to_uint32("192.168.1.186"), "test_pool"));

        PASS();
}

TEST test_lease_expiration()
{
        if (lease_path_ok < 0)
                SKIP();

        /* This assert is only used to verify the compiler can see the function */
        ASSERT_EQ(-1, check_lease_expirations(50, NULL));

        /* Since we dont have a dhcp server structure in theese tests, we need to create one */
        dhcp_server_t server;
        ASSERT_NEQ(NULL, (server.allocator = address_allocator_new()));
        ASSERT_EQ(0, allocator_add_pool(server.allocator, address_pool_new_str("test_pool", "192.168.1.1", "192.168.1.254", "255.255.255.0")));
        /* At this point in tests, we have allocated these addresses */
        uint32_t adr_buf;
        ASSERT_EQ(0, allocator_request_this_address_str(server.allocator, "192.168.1.10", &adr_buf));
        ASSERT_EQ(0, allocator_request_this_address_str(server.allocator, "192.168.1.33", &adr_buf));
        ASSERT_EQ(0, allocator_request_this_address_str(server.allocator, "192.168.1.186", &adr_buf));
        ASSERT_EQ(251, ((address_pool_t*)server.allocator->address_pools->first->data)->available_addresses);
        /* We have rady trimmed down */

        /* 
         * We expect 2 leases to be removed,
         * since we have leases with following expirations:
         * 1000, 60000 and 547784223
         */
        ASSERT_EQ(2, check_lease_expirations(100000, &server));
        ASSERT_EQ(253, ((address_pool_t*)server.allocator->address_pools->first->data)->available_addresses);

        allocator_destroy(&server.allocator);

        PASS();
}

TEST test_load_leases_from_persistant_database_one_pool()
{
        if (lease_path_ok < 0)
                SKIP();
        
        /* Since we dont have a dhcp server structure in theese tests, we need to create one */
        dhcp_server_t server;
        ASSERT_NEQ(NULL, (server.allocator = address_allocator_new()));
        ASSERT_EQ(0, allocator_add_pool(server.allocator, address_pool_new_str("persist1", "192.168.1.1", "192.168.1.254", "255.255.255.0")));
        /* At this point in tests, we have allocated these addresses */

        ASSERT_EQ(0, init_load_persisten_leases(&server));
        ASSERT_EQ(false, allocator_is_address_available_str(server.allocator, "192.168.1.1"));
        ASSERT_EQ(true, allocator_is_address_available_str(server.allocator, "192.168.1.2"));

        lease_t lease_buff = {0};
        ASSERT_EQ(0, lease_retrieve_address(&lease_buff, ipv4_address_to_uint32("192.168.1.1"), server.allocator->address_pools));
        ASSERT_EQ(1708786696, lease_buff.lease_start);
        ASSERT_EQ(845229166, lease_buff.xid);
        ASSERT_EQ(LEASE_DOESNT_EXITS, lease_retrieve_address(&lease_buff, ipv4_address_to_uint32("192.168.1.2"), server.allocator->address_pools));
        
        allocator_destroy(&server.allocator);

        PASS();
}

TEST test_load_leases_from_persistant_database_multiple_pools()
{
        if (lease_path_ok < 0)
                SKIP();
        
        /* Since we dont have a dhcp server structure in theese tests, we need to create one */
        dhcp_server_t server;
        ASSERT_NEQ(NULL, (server.allocator = address_allocator_new()));
        ASSERT_EQ(0, allocator_add_pool(server.allocator, address_pool_new_str("persist1", "192.168.1.1", "192.168.1.254", "255.255.255.0")));
        ASSERT_EQ(0, allocator_add_pool(server.allocator, address_pool_new_str("persist2", "192.168.2.1", "192.168.2.254", "255.255.255.0")));
        /* At this point in tests, we have allocated these addresses */

        ASSERT_EQ(0, init_load_persisten_leases(&server));
        ASSERT_EQ(false, allocator_is_address_available_str(server.allocator, "192.168.2.1"));
        ASSERT_EQ(true, allocator_is_address_available_str(server.allocator, "192.168.2.2"));
        ASSERT_EQ(false, allocator_is_address_available_str(server.allocator, "192.168.2.3"));

        lease_t lease_buff = {0};
        ASSERT_EQ(0, lease_retrieve_address(&lease_buff, ipv4_address_to_uint32("192.168.2.3"), server.allocator->address_pools));
        ASSERT_EQ(1708786696, lease_buff.lease_start);
        ASSERT_EQ(845229188, lease_buff.xid);
        ASSERT_EQ(0, lease_retrieve_address(&lease_buff, ipv4_address_to_uint32("192.168.2.1"), server.allocator->address_pools));
        ASSERT_EQ(1000000000, lease_buff.lease_start);
        ASSERT_EQ(845229166, lease_buff.xid);
        ASSERT_EQ(LEASE_DOESNT_EXITS, lease_retrieve_address(&lease_buff, ipv4_address_to_uint32("192.168.1.2"), server.allocator->address_pools));

        allocator_destroy(&server.allocator);

        PASS();
}

SUITE(lease)
{
        RUN_TEST(test_undef_lease_path_for_testing);

        RUN_TEST(test_lease_file_init);
        RUN_TEST(test_add_leases);
        RUN_TEST(test_are_3_leases_present);
        RUN_TEST(test_retrieve_lease);
        RUN_TEST(test_retrieve_lease_address);
        RUN_TEST(test_retrieve_non_existent);
        RUN_TEST(test_lease_expiration);
        RUN_TEST(test_remove_lease);
        RUN_TEST(test_load_leases_from_persistant_database_one_pool);
        RUN_TEST(test_load_leases_from_persistant_database_multiple_pools);
}

