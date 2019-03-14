/* This header file is shared by client & server. They really have
 * something to share...
 * */

/* Client/server protocol is as follows:
   Send INIT_PASSWD
   Send 64-bit cliserv_magic
   Send 64-bit size of exported device
   Send 128 bytes of zeros (reserved for future use)
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef UINT64_MAX
#error I need at least some 64-bit type
#endif

#define __be32 uint32_t
#define __be64 uint64_t
#include "nbd.h"

#ifndef G_GNUC_NORETURN
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define G_GNUC_NORETURN __attribute__((__noreturn__))
#define G_GNUC_UNUSED __attribute__((unused))
#else
#define G_GNUC_NORETURN
#define G_GNUC_UNUSED
#endif
#endif

extern const uint64_t cliserv_magic;
extern const uint64_t opts_magic;
extern const uint64_t rep_magic;

#define INIT_PASSWD "NBDMAGIC"

#define INFO(a) do { } while(0)

int set_nonblocking(int fd, int nb);
void err_nonfatal(const char *s);

void err(const char *s) G_GNUC_NORETURN;

void logging(const char* name);

#ifndef ntohll
uint64_t ntohll(uint64_t a);
#endif
#ifndef htonll
#define htonll ntohll
#endif

void readit(int f, void *buf, size_t len);
int writeit(int f, void *buf, size_t len);

/* Options that the client can select to the server */
#define NBD_OPT_EXPORT_NAME	(1)	/** Client wants to select a named export (is followed by name of export) */
#define NBD_OPT_ABORT		(2)	/** Client wishes to abort negotiation */
#define NBD_OPT_LIST		(3)	/** Client request list of supported exports (not followed by data) */

/* Replies the server can send during negotiation */
#define NBD_REP_ACK		(1)	/** ACK a request. Data: option number to be acked */
#define NBD_REP_SERVER		(2)	/** Reply to NBD_OPT_LIST (one of these per server; must be followed by NBD_REP_ACK to signal the end of the list */
#define NBD_REP_FLAG_ERROR	(1 << 31)	/** If the high bit is set, the reply is an error */
#define NBD_REP_ERR_UNSUP	(1 | NBD_REP_FLAG_ERROR)	/** Client requested an option not understood by this version of the server */
#define NBD_REP_ERR_POLICY	(2 | NBD_REP_FLAG_ERROR)	/** Client requested an option not allowed by server configuration. (e.g., the option was disabled) */
#define NBD_REP_ERR_INVALID	(3 | NBD_REP_FLAG_ERROR)	/** Client issued an invalid request */
#define NBD_REP_ERR_PLATFORM	(4 | NBD_REP_FLAG_ERROR)	/** Option not supported on this platform */
#define NBD_REP_ERR_TLS_REQD	(5 | NBD_REP_FLAG_ERROR)	/** TLS required */

/* Global flags */
#define NBD_FLAG_FIXED_NEWSTYLE (1 << 0)	/* new-style export that actually supports extending */
#define NBD_FLAG_NO_ZEROES	(1 << 1)	/* we won't send the 128 bits of zeroes if the client sends NBD_FLAG_C_NO_ZEROES */
/* Flags from client to server. Only one such option currently. */
#define NBD_FLAG_C_FIXED_NEWSTYLE NBD_FLAG_FIXED_NEWSTYLE
#define NBD_FLAG_C_NO_ZEROES	NBD_FLAG_NO_ZEROES
