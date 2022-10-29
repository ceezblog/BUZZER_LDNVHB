//////////////////////////////////////////////////////////////////////////
// bz_udp_helper.hr.30 

// DO NOT MODIFY
#define PANEL_BASE_ADDR 100
#define LOCAL_PORT 4444
#define REMOTE_PORT 4444

#define CHECK_BIT(x, n)   ((x) & (1<<(n)))

// for data-type consistency with arduino
#ifdef WIN32
typedef unsigned char byte; // use "byte" to unify with arduino code
typedef unsigned char uint8_t; // use "uint8_t" to unify with arduino code
typedef unsigned short uint16_t; // use "uint16_t" to unify with arduino code
#endif

// macro for zero memory
#ifndef memzero
#define memzero(x) memset((void*)& x, 0, sizeof x) // same way as ZeroMemory, just shorter name
#define zeromem(in_destination) memset((void*)& in_destination, 0, sizeof in_destination) // same way as ZeroMemory, just shorter name
#define copymem(in_destination, in_source, in_size) memcpy(& in_destination, & in_source, in_size) // same way as ZeroMemory, just shorter name
#endif



//////////////////////////////////////////////////////////////////////////
// BZ DATA STRUCTURE
#ifndef _BZ_UDP
#define _BZ_UDP


#define BZ_DATALENGTH 13 // text size 13, 1 extra null

// 64x32px, font 5x7 => 12 characters 
// Display layout
// 3 fonts:  default, large, extra large

// 0 = big center
// 1 = header, larger center
// 2 = large header, footer
// 3 = header, larger center, footer
// 4 = header, center, footer 
// 5 = 4 small lines // probably never use
enum BYTE_LAYOUT : uint8_t
{ LAY_BCENTER = 0, LAY_HEADER_BCENTER = 1, LAY_BHEADER_FOOTER = 2, LAY_HEADER_BCENTER_FOOTER = 3, LAY_HEADER_CENTER_FOOTER = 4, LAY_HEADER_CENTER_CENTER_FOOTER = 5,};

enum MULTICHOICE : uint8_t
{ MC_NONE = 0, MC_A = 1, MC_B = 2, MC_C = 3, MC_D = 4,};

enum TT_TYPE : uint8_t
{TT_TEAM = 0, TT_PLAYER1 = 1, TT_PLAYER2 = 2, TT_AUXTEXT1 = 3, TT_AUXTEXT2, TT_FULLSCREEN, TT_CENTER, TT_FOOTER, TT_FOOTER1, TT_FOOTER2,};

// MESSAGE ID
enum BYTE_MMSG_ID : uint8_t
{
  M_NULL = 0, M_PING = 6, M_ACK = 7, M_OK = 8, M_FAIL = 9, M_RESET = 10,
  M_HOST_IP = 15, // only use this to identify IP hOST
  M_CLEARSCREEN = 16,
  M_CLEARSCENTER = 17,
  M_CLEARHEADER = 18,
  M_SCREENSAVER_ON = 21,
  M_SCREENSAVER_OFF = 22,
  M_DISABLE = 33,
  M_ENABLE = 35,
  M_SET_TEXT = 90, 
  M_SHOW_TEXT = 91, 
  M_SHOW_INSTANT_TEXT = 99,
  M_TRIGGER_BZ = 120,
	M_TRIGGER_A = 121,
	M_TRIGGER_B = 122,
	M_TRIGGER_C = 123,
	M_TRIGGER_D = 124,
};


typedef uint16_t CREF16; // use this "cref16" custom data type to 

//#define isBig(exdata) CHECK_BIT(exdata, 1)
//#define isClear

// 4 bytes
// struct BZ_EX_DATA{ 
//   union
//   {
// 		uint32_t ul32; // unsigned long 32, double word, 4 byte
// 		struct {uint8_t b1, b2, b3, b4; } b32; // 4 bytes
// 		struct {CREF16 cref; uint8_t ex1; TT_TYPE type;} text;
// 		struct {CREF16 cref; bool isBig; bool isFullscreen;} insTextEx;
//   };
// };

// 4 bytes
struct BZ_EX_DATA{ 
	union
	{
		uint32_t ul32; // unsigned long 32, double word, 4 byte
		struct {uint8_t b1, b2, b3, b4; } b32; // 4 bytes
		struct {CREF16 cref; uint8_t size; TT_TYPE type;} cxtxt;
	};
};


//limit to 22 bytes per datagram
struct BZ_DATAGRAM{
  //uint32_t senderIP; // 4 bytes //obsolete, use M_HOST_IP instead
  BYTE_MMSG_ID mmsgid; // 1 byte
  BZ_EX_DATA extra;     // 4 bytes extra data
  byte data[BZ_DATALENGTH]; // size 12 + 1 extra for null terminated maybe
};

// PC IP : 2-99
// Switch box IP: 100
//static 
//static byte UDP_HOST_ADDRESS; // 2 - 99
//#define UDP_SWITCH_BOX_ADDRESS = 100;

// pre-define static address
// for host pc software only
// static byte UDP_ESP_IPADDRESS[20] = {101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};
// static byte UDP_ESP_STATUS[20]; // ON or OFF

#endif //end of _BZ_UDP



////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
// _udp_helper_
// Receive and send a struct
// you must define the struct as UDP_PACKAGE_STRUCT
//
// process of UDP
// step1: init -> setup socket, ip, port -> bind socket
// step2: receive datagram package, this is block function
// step3: repeat receive...
// step4: stop
//
// how to use:
// 
// UDP_HELPER udp;
// udp.begin(LISTEN PORT);
// udp.send(.....);
// udp.receive(.....); // block function
// udp.end(); // clean up

#ifdef WIN32
#ifndef _UDP_HELPER_
#define  _UDP_HELPER_

#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>


struct IPV4_ADDR{
  union
  {
    struct { uint8_t b1, b2, b3, b4; } b32;
    uint32_t ul32;
  };
};

// Include your header which define your struct
// the size of this struct must not exceed 256 bytes
// anyway, the bigger datagram, easier it gets lost in transport

//typedef struct  your_data_struct  UDP_PACKAGE_STRUCT 
typedef struct    bz_datagram       UDP_PACKAGE_STRUCT; //limit to 32 bytes

// Result enum of socket
enum WS_RESULT : uint8_t
{
  WS_OK = 0,
  WS_READY = 1,
  WS_NOT_READY,
  WS_INIT_FAIL,
  WS_SOCKET_FAIL,
  WS_GET_HOSTNAME_FAIL,
  WS_GET_LOCAL_IP_FAIL,
  WS_BIND_SOCKET_FAIL,
};

// UPD helper class
class UDP_HELPER
{
public:
  WS_RESULT status;
  IPV4_ADDR local_IPv4; //read only, change it doesn't affect anything
  uint16_t local_port;  //read only, change it doesn't affect anything
  WS_RESULT begin(uint16_t localPort);
  WS_RESULT end();
  uint8_t receive(UDP_PACKAGE_STRUCT &datagram, IPV4_ADDR &sender, uint16_t &port);
  uint8_t send(UDP_PACKAGE_STRUCT datagram, IPV4_ADDR receiver, uint16_t port);

protected:
private:
  WSADATA w;
  SOCKET sockDesc;
  struct sockaddr_in local_addr;
  //struct sockaddr_in remote_addr;
};

WS_RESULT UDP_HELPER::begin(uint16_t localPort)//::begin(uint16_t localPort)
{
  status = WS_NOT_READY;
  // Open windows connection 
  if (WSAStartup(0x0101, &w) != 0) return WS_INIT_FAIL;

  // Open a datagram socket 
  sockDesc = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockDesc == INVALID_SOCKET)
  {
    WSACleanup();
    return WS_SOCKET_FAIL;
  }

  //memzero(remote_addr);
  memzero(local_addr);

  //memset((void *)&remote_addr, '\0', sizeof(struct sockaddr_in));
  //memset((void *)&local_addr, '\0', sizeof(struct sockaddr_in));

  local_addr.sin_family = AF_INET;
  local_addr.sin_port = htons(localPort);

  char host_name[256];
  struct hostent *hp;
  gethostname(host_name, sizeof(host_name));
  hp = gethostbyname(host_name);
  if (hp == NULL)
  {
    end();
    return WS_GET_HOSTNAME_FAIL;
  }

  local_addr.sin_addr.S_un.S_un_b.s_b1 = hp->h_addr_list[0][0];
  local_addr.sin_addr.S_un.S_un_b.s_b2 = hp->h_addr_list[0][1];
  local_addr.sin_addr.S_un.S_un_b.s_b3 = hp->h_addr_list[0][2];
  local_addr.sin_addr.S_un.S_un_b.s_b4 = hp->h_addr_list[0][3];


  // Bind address to socket 
  if (bind(sockDesc, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in)) == -1)
  {
    end();
    return WS_BIND_SOCKET_FAIL;
  }

  local_IPv4.b32.b1 = hp->h_addr_list[0][0];
  local_IPv4.b32.b2 = hp->h_addr_list[0][1];
  local_IPv4.b32.b3 = hp->h_addr_list[0][2];
  local_IPv4.b32.b4 = hp->h_addr_list[0][3];
  local_port = localPort;

  status = WS_READY;
  return WS_OK;
}

WS_RESULT UDP_HELPER::end()
{
  status = WS_NOT_READY;
  closesocket(sockDesc);
  WSACleanup();
  return WS_OK;
}

//uint8_t UDP_HELPER::send(UDP_PACKAGE_STRUCT datagram, IPV4_ADDR receiver, uint16_t port)
uint8_t UDP_HELPER::send(UDP_PACKAGE_STRUCT datagram, IPV4_ADDR receiver_addr, uint16_t port)
{
  if (status == WS_NOT_READY) return 0;

  struct sockaddr_in remote_addr;
  memzero(remote_addr);
  remote_addr.sin_family = AF_INET;
  remote_addr.sin_port = htons(port);
  remote_addr.sin_addr.S_un.S_addr = receiver_addr.ul32;
  int addr_size = sizeof(sockaddr_in);

  int bytes_sent = sendto(sockDesc, (char *)&datagram, sizeof (UDP_PACKAGE_STRUCT), 0, (struct sockaddr *)&remote_addr, addr_size);

  return bytes_sent;
}

//uint8_t UDP_HELPER::receive(UDP_PACKAGE_STRUCT &datagram, IPV4_ADDR &sender, uint16_t &port)
uint8_t UDP_HELPER::receive(UDP_PACKAGE_STRUCT &datagram, IPV4_ADDR &sender_addr, uint16_t &port)
{
  if (status == WS_NOT_READY) return 0;
  int remote_addr_len = (int)sizeof(sockaddr_in);

  struct sockaddr_in remote_addr;
  memzero(remote_addr);

  // Block function
  int bytes_received = recvfrom(sockDesc, (char*)&datagram, sizeof(UDP_PACKAGE_STRUCT), 0, (struct sockaddr *)&remote_addr, &remote_addr_len);

  sender_addr.ul32 = remote_addr.sin_addr.S_un.S_addr;
  port = ntohs(remote_addr.sin_port);

  return bytes_received;
}

#endif
#endif
