#ifndef ATRESPONSES_H_ _
#define ATRESPONSES_H_ _

#define AT_CONNECT "CONNECT"
#define AT_OK "OK"

#define AT_ENTER string("\r\n")

#define AT_AT string("AT")

// Make call
#define AT_ATD string("ATD")

// Hang up call
#define AT_ATH string("ATH")
#define AT_ATCHLD string("AT+CHLD")

// Mute Microphone
#define AT_ATCMUT string("AT+CMUT")

// Set Message mode
#define AT_ATCMGF string("AT+CMGF")
// Create message
#define AT_ATCMGS string("AT+CMGS")
#endif
