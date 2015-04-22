#include <libnet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// sudo gcc ipblock_arp.c -lnet

int main() {

  libnet_t *l;
  struct libnet_ether_addr *src_mac_addr; 

  u_int32_t src_ip_addr,target_ip_addr;
  u_int8_t mac_broadcast_addr[6] = {0xff, 0xff, 0xff, 0xff,0xff, 0xff},mac_zero_addr[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; 
  char target_ip_addr_str[16],src_ip_addr_str[16],error_bufmsg[LIBNET_ERRBUF_SIZE];
  
  int write_buf;  

  l = libnet_init(LIBNET_LINK, "eth0", error_bufmsg);
  if ( l == NULL ) {
    fprintf(stderr, "libnet_init() failed: %s\n", error_bufmsg);
    exit(EXIT_FAILURE);
  }

  // Getting our  IP addresses 

  src_ip_addr = libnet_get_ipaddr4(l);
  if ( src_ip_addr != -1 ) 
   printf("My IP address: %s\n", libnet_addr2name4(src_ip_addr,LIBNET_DONT_RESOLVE));
 else {
  fprintf(stderr, "Get own IP address failed: %s\n",libnet_geterror(l));
  libnet_destroy(l);
  exit(EXIT_FAILURE);
}

  // Getting our  MAC address

src_mac_addr = libnet_get_hwaddr(l);
if ( src_mac_addr == NULL ) {
  fprintf(stderr, "Get own MAC address failed: %s\n",libnet_geterror(l));
  libnet_destroy(l);
  exit(EXIT_FAILURE);
}

   //Getting target IP address

printf("Enter Target IP address: ");
scanf("%15s",target_ip_addr_str);

target_ip_addr = libnet_name2addr4(l, target_ip_addr_str,LIBNET_DONT_RESOLVE);

if ( target_ip_addr == -1 ) {
  fprintf(stderr, "Error converting IP address.\n");
  libnet_destroy(l);
  exit(EXIT_FAILURE);
}

  // Building ARP header

if ( libnet_autobuild_arp (ARPOP_REQUEST,src_mac_addr->ether_addr_octet,(u_int8_t*)(&src_ip_addr), mac_zero_addr,(u_int8_t*)(&target_ip_addr), l) == -1){
  fprintf(stderr, "Error building ARP header: %s\n",libnet_geterror(l));
  libnet_destroy(l);
  exit(EXIT_FAILURE);
}

  // Building Ethernet header

if ( libnet_autobuild_ethernet (mac_broadcast_addr,ETHERTYPE_ARP, l) == -1 ){
  fprintf(stderr, "Error building Ethernet header: %s\n",libnet_geterror(l));
  libnet_destroy(l);
  exit(EXIT_FAILURE);
}

  // Writing packet

while(1){
  write_buf = libnet_write(l);
  if ( write_buf != -1 )
    printf("%d bytes written.\n", write_buf);
  else
    fprintf(stderr, "Error writing packet: %s\n",\
  libnet_geterror(l));
  sleep(3);
}

libnet_destroy(l);
return 0;
}
