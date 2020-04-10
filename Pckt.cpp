
/***********************************************************************
 rawtracert.cpp - Contains all of the functions essential to sending "ping"
	packets using Winsock 2 raw sockets.  Depends on ip_checksum.cpp for
	calculating IP-style checksums on blocks of data, however.
***********************************************************************/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "Pckt.h"
//#include "ip_checksum.h"




void init_ping_packet(ICMPHeader* icmp_hdr, int packet_size, int seq_no)
{
	// Set up the packet's fields
	icmp_hdr->type = ICMP_ECHO_REQUEST;
	icmp_hdr->code = 0;
	icmp_hdr->checksum = 0;
	icmp_hdr->id = (USHORT)GetCurrentProcessId();
	icmp_hdr->seq = seq_no;
	icmp_hdr->timestamp = GetTickCount();

	// "You're dead meat now, packet!"
	const unsigned long int deadmeat = 0xDEADBEEF;
	char* datapart = (char*)icmp_hdr + sizeof(ICMPHeader);
	int bytes_left = packet_size - sizeof(ICMPHeader);
	while (bytes_left > 0) {
		memcpy(datapart, &deadmeat, min(int(sizeof(deadmeat)), bytes_left));
		bytes_left -= sizeof(deadmeat);
		datapart += sizeof(deadmeat);
	}

	//	memset(&icmp_hdr[sizeof(icmp_hdr)], '?', 32);

		// Calculate a checksum on the result
	icmp_hdr->checksum = 0;
}


int send_ping(SOCKET sd, const sockaddr_in& dest, ICMPHeader* send_buf, int packet_size)
{
	// Send the ping packet in send_buf as-is
	printf("Sending %d bytes to %s...\n", packet_size, inet_ntoa(dest.sin_addr));
	int bwrote = sendto(sd, (char*)send_buf, packet_size, 0,
		(sockaddr*)& dest, sizeof(dest));
	if (bwrote == SOCKET_ERROR) {
		printf("Sending failed\n");
		return -1;
	}
	else if (bwrote < packet_size) {
		printf("Sent %d bytes\n", bwrote);
	}

	return 0;
}
