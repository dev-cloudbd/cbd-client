/*
 * Open connection for network block device
 *
 * Copyright 1997,1998 Pavel Machek, distribute under GPL
 *  <pavel@atrey.karlin.mff.cuni.cz>
 * Copyright (c) 2002 - 2011 Wouter Verhelst <w@uter.be>
 *
 * Version 1.0 - 64bit issues should be fixed, now
 * Version 1.1 - added bs (blocksize) option (Alexey Guzeev, aga@permonline.ru)
 * Version 1.2 - I added new option '-d' to send the disconnect request
 * Version 2.0 - Version synchronised with server
 * Version 2.1 - Check for disconnection before INIT_PASSWD is received
 * 	to make errormsg a bit more helpful in case the server can't
 * 	open the exported file.
 * 16/03/2010 - Add IPv6 support.
 * 	Kitt Tientanopajai <kitt@kitty.in.th>
 *	Neutron Soutmun <neo.neutron@gmail.com>
 *	Suriya Soutmun <darksolar@gmail.com>
 */

#define MY_NAME "cbd_client"

#include "config.h"
#include "cliserv.h"

#include <netinet/in.h>

#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#ifndef SYSCONFDIR
#define SYSCONFDIR "/etc"
#endif

#ifndef RUNSTATEDIR
#define RUNSTATEDIR "/var/run"
#endif

#define NBDC_DO_LIST 1

int check_conn(char* devname, int do_print)
{
    char buf[256];
    char* p;
    int fd;
    int len;

    if ((p = strrchr(devname, '/')))
    {
        devname = p + 1;
    }
    if ((p = strchr(devname, 'p')))
    {
        /* We can't do checks on partitions. */
        *p = '\0';
    }
    snprintf(buf, 256, "/sys/block/%s/pid", devname);
    if ((fd = open(buf, O_RDONLY)) < 0)
    {
        if (errno == ENOENT)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    len = read(fd, buf, 256);
    if (len < 0)
    {
        perror("could not read from server");
        close(fd);
        return 2;
    }
    buf[(len < 256) ? len : 255] = '\0';
    if (do_print)
        printf("%s\n", buf);
    close(fd);
    return 0;
}

int openunix(const char *device_name)
{
    int sock;
    struct sockaddr_un un_addr;
    memset(&un_addr, 0, sizeof(un_addr));

    un_addr.sun_family = AF_UNIX;

    // unix domain socket path = "RUNSTATEDIR/cloudbd/<remote_id:device_id>.socket"
    if (snprintf(un_addr.sun_path, sizeof un_addr.sun_path, RUNSTATEDIR "/cloudbd/%s.socket", device_name) >= sizeof un_addr.sun_path)
    {
        err_nonfatal("UNIX socket path too long");
        return -1;
    }

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        err_nonfatal("SOCKET failed");
        return -1;
    };

    if (connect(sock, &un_addr, sizeof(un_addr)) == -1)
    {
        err_nonfatal("CONNECT failed");
        close(sock);
        return -1;
    }
    return sock;
}

void ask_list(int sock)
{
    uint32_t opt;
    uint32_t opt_server;
    uint32_t len;
    uint32_t reptype;
    uint64_t magic;
    int rlen;
    const int BUF_SIZE = 1024;
    char buf[BUF_SIZE];

    magic = ntohll(opts_magic);
    if (write(sock, &magic, sizeof(magic)) < 0)
        err("Failed/2.2: %m");

    /* Ask for the list */
    opt = htonl(NBD_OPT_LIST);
    if (write(sock, &opt, sizeof(opt)) < 0)
    {
        err("writing list option failed: %m");
    }
    /* Send the length (zero) */
    len = htonl(0);
    if (write(sock, &len, sizeof(len)) < 0)
    {
        err("writing length failed: %m");
    }
    /* newline, move away from the "Negotiation:" line */
    printf("\n");
    do
    {
        memset(buf, 0, 1024);
        if (read(sock, &magic, sizeof(magic)) < 0)
        {
            err("Reading magic from server: %m");
        }
        if (read(sock, &opt_server, sizeof(opt_server)) < 0)
        {
            err("Reading option: %m");
        }
        if (read(sock, &reptype, sizeof(reptype)) < 0)
        {
            err("Reading reply from server: %m");
        }
        if (read(sock, &len, sizeof(len)) < 0)
        {
            err("Reading length from server: %m");
        }
        magic = ntohll(magic);
        len = ntohl(len);
        reptype = ntohl(reptype);
        if (magic != rep_magic)
        {
            err("Not enough magic from server");
        }
        if (reptype & NBD_REP_FLAG_ERROR)
        {
            switch (reptype)
            {
            case NBD_REP_ERR_POLICY:
                fprintf(stderr, "\nE: listing not allowed by server.\n");
                break;
            default:
                fprintf(stderr, "\nE: unexpected error from server.\n");
                break;
            }
            if (len > 0 && len < BUF_SIZE)
            {
                if ((rlen = read(sock, buf, len)) < 0)
                {
                    fprintf(stderr, "\nE: could not read error message from server\n");
                }
                else
                {
                    buf[rlen] = '\0';
                    fprintf(stderr, "Server said: %s\n", buf);
                }
            }
            exit(EXIT_FAILURE);
        }
        else
        {
            if (len)
            {
                if (reptype != NBD_REP_SERVER)
                {
                    err("Server sent us a reply we don't understand!");
                }
                if (read(sock, &len, sizeof(len)) < 0)
                {
                    fprintf(stderr, "\nE: could not read export name length from server\n");
                    exit(EXIT_FAILURE);
                }
                len = ntohl(len);
                if (len >= BUF_SIZE)
                {
                    fprintf(stderr, "\nE: export name on server too long\n");
                    exit(EXIT_FAILURE);
                }
                if (read(sock, buf, len) < 0)
                {
                    fprintf(stderr, "\nE: could not read export name from server\n");
                    exit(EXIT_FAILURE);
                }
                buf[len] = 0;
                printf("%s\n", buf);
            }
        }
    } while (reptype != NBD_REP_ACK);
    opt = htonl(NBD_OPT_ABORT);
    len = htonl(0);
    magic = htonll(opts_magic);
    if (write(sock, &magic, sizeof(magic)) < 0)
        err("Failed/2.2: %m");
    if (write(sock, &opt, sizeof(opt)) < 0)
        err("Failed writing abort");
    if (write(sock, &len, sizeof(len)) < 0)
        err("Failed writing length");
    if (read(sock, &magic, sizeof(magic)) < 0)
        err("Reading magic from server: %m");
    if (read(sock, &opt_server, sizeof(opt_server)) < 0)
        err("Reading option: %m");
    if (read(sock, &reptype, sizeof(reptype)) < 0)
        err("Reading reply from server: %m");
    if (read(sock, &len, sizeof(len)) < 0)
        err("Reading length from server: %m");
    magic = ntohll(magic);
    len = ntohl(len);
    reptype = ntohl(reptype);
    if (magic != rep_magic)
        err("Not enough magic from server");
}

void negotiate(int *sockp, uint64_t *rsize64, uint16_t *flags, char* name, uint32_t needed_flags,
        uint32_t client_flags, uint32_t do_opts)
{
    uint64_t magic, size64;
    uint16_t tmp;
    uint16_t global_flags;
    char buf[256] = "\0\0\0\0\0\0\0\0\0";
    uint32_t opt;
    uint32_t namesize;
    int sock = *sockp;

    printf("Negotiation: ");
    readit(sock, buf, 8);
    if (strcmp(buf, INIT_PASSWD))
        err("INIT_PASSWD bad");
    printf(".");
    readit(sock, &magic, sizeof(magic));
    magic = ntohll(magic);
    if (magic != opts_magic)
    {
        if (magic == cliserv_magic)
        {
            err(
                    "It looks like you're trying to connect to an oldstyle server. This is no longer supported since nbd 3.10.");
        }
    }
    printf(".");
    readit(sock, &tmp, sizeof(uint16_t));
    global_flags = ntohs(tmp);
    if ((needed_flags & global_flags) != needed_flags)
    {
        /* There's currently really only one reason why this
         * check could possibly fail, but we may need to change
         * this error message in the future... */
        fprintf(stderr, "\nE: Server does not support listing exports\n");
        exit(EXIT_FAILURE);
    }

    if (global_flags & NBD_FLAG_NO_ZEROES)
    {
        client_flags |= NBD_FLAG_C_NO_ZEROES;
    }
    client_flags = htonl(client_flags);
    if (write(sock, &client_flags, sizeof(client_flags)) < 0)
        err("Failed/2.1: %m");

    if (do_opts & NBDC_DO_LIST)
    {
        ask_list(sock);
        exit(EXIT_SUCCESS);
    }

    /* Write the export name that we're after */
    magic = htonll(opts_magic);
    if (write(sock, &magic, sizeof(magic)) < 0)
        err("Failed/2.2: %m");

    opt = ntohl(NBD_OPT_EXPORT_NAME);
    if (write(sock, &opt, sizeof(opt)) < 0)
        err("Failed/2.3: %m");
    namesize = (uint32_t) strlen(name);
    namesize = ntohl(namesize);
    if (write(sock, &namesize, sizeof(namesize)) < 0)
        err("Failed/2.4: %m");
    if (write(sock, name, strlen(name)) < 0)
        err("Failed/2.4: %m");

    readit(sock, &size64, sizeof(size64));
    size64 = ntohll(size64);

    if ((size64 >> 12) > (uint64_t) ~0UL)
    {
        printf("size = %luMB", (unsigned long) (size64 >> 20));
        err("Exported device is too big for me. Get 64-bit machine :-(\n");
    }
    else
        printf("size = %luMB", (unsigned long) (size64 >> 20));

    readit(sock, &tmp, sizeof(tmp));
    *flags = (uint32_t) ntohs(tmp);

    if (!(global_flags & NBD_FLAG_NO_ZEROES))
    {
        readit(sock, &buf, 124);
    }
    printf("\n");

    *rsize64 = size64;
}

bool get_from_config(char* cfgname, char** nbddev_ptr, char** device_name_ptr)
{
    bool retval = false;
    bool fail = false;
    char *data = NULL;
    char *nbd_ptr, *cbd_ptr, *id_ptr, *line, *nextline, *opt_ptr;
    off_t size;

    int fd = open(SYSCONFDIR "/cloudbd/cbdtab", O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "while opening " SYSCONFDIR "/cloudbd/cbdtab: ");
        perror("could not open config file");
        goto out;
    }

    size = lseek(fd, 0, SEEK_END);
    if (size < 0)
    {
        perror("E: reading cbdtab size");
        fail = true;
        goto out;
    }

    if (lseek(fd, 0, SEEK_SET) != 0)
    {
        perror("E: seeking start of cbdtab");
        fail = true;
        goto out;
    }

    data = malloc((size_t) size + 1);
    if (!data)
    {
        perror("E: allocating memory to read cbdtab");
        fail = true;
        goto out;
    }

    ssize_t bytes_read = read(fd, data, (size_t) size);
    if (bytes_read < 0)
    {
        perror("E: reading cbdtab");
        fail = true;
        goto out;
    }
    else if (bytes_read != size)
    {
        errno = ENODATA;
        perror("E: reading cbdtab");
        fail = true;
        goto out;
    }
    data[size] = '\0';

    if (!strncmp(cfgname, "/dev/", 5))
        cfgname += 5;

    nextline = data;
    while (nextline)
    {
        line = strsep(&nextline, "\n");
        line[strcspn(line, "#")] = '\0'; // truncate line at first '#', if any, for comments

        nbd_ptr = line;
        // get first field
        do
        {
            nbd_ptr = strsep(&line, " \t");
        } while (nbd_ptr && strlen(nbd_ptr) == 0);

        if (nbd_ptr && !strcmp(nbd_ptr, cfgname))
            break;
    }

    if (!nbd_ptr) // cfgname not found
        goto out;

    // get second field
    do
    {
        cbd_ptr = strsep(&line, " \t");
    } while (cbd_ptr && strlen(cbd_ptr) == 0);

    if (!cbd_ptr)
    {
        fprintf(stderr, "found %s in cbdtab with invalid config", cfgname);
        fail = true;
        goto out;
    }

    size_t l = strlen(nbd_ptr) + 6;
    *nbddev_ptr = malloc(l);
    snprintf(*nbddev_ptr, l, "/dev/%s", cfgname);

    const char *sep = strchr(cbd_ptr, ':');
    if (!sep || sep == cbd_ptr || strnlen(sep + 1, 1) == 0) // no remote or empty remote found or empty device found
    {
        fprintf(stderr, "found %s in cbdtab with invalid config", cfgname);
        fail = true;
        goto out;
    }

    *device_name_ptr = strdup(cbd_ptr);
    retval = true;

    /*
    // get third field
    do
    {
        opt_ptr = strsep(&line, " \t");
    } while(opt_ptr && strlen(opt_ptr) == 0);

    // third field is the options field, currently no options for client, a comma-separated field of options
    if (!opt_ptr)
        goto out; // not an error, options field is not required

    do
    {
        if (!strncmp(opt_ptr, "connections=", 12))
        {
            //
            strtol(opt_ptr + 12, &opt_ptr, 0);
            goto next;
        }
        if (!strncmp(opt_ptr, "block_bufferss=", 3))
        {
            *bs = (int) strtol(opt_ptr + 3, &opt_ptr, 0);
            goto next;
        }
        if (!strncmp(opt_ptr, "timeout=", 8))
        {
            *timeout = (int) strtol(opt_ptr + 8, &opt_ptr, 0);
            goto next;
        }
        if (!strncmp(opt_ptr, "noauto", 6))
        {
            opt_ptr += 6;
            goto next;
        }
        // skip unknown options, with a warning unless they start with a '_'
        l = strcspn(opt_ptr, ",");
        if (*opt_ptr != '_')
        {
            char *s = strndup(opt_ptr, l);
            fprintf(stderr, "Warning: unknown option '%s' found in cbdtab file", s);
            free(s);
        }
        opt_ptr += l;
        next: if (*opt_ptr == ',')
        {
            opt_ptr++;
        }
    } while (strlen(opt_ptr) > 0);
    */

    out:
    if (data != NULL)
    {
        free(data);
    }

    if (fd >= 0)
    {
        close(fd);
    }

    if (fail)
    {
        exit(EXIT_FAILURE);
    }

    return retval;
}

void setsizes(int nbd, uint64_t size64, int blocksize, uint32_t flags)
{
    unsigned long size;
    int read_only = (flags & NBD_FLAG_READ_ONLY) ? 1 : 0;

    if (size64 >> 12 > (uint64_t) ~0UL)
        err("Device too large.\n");
    else
    {
        int tmp_blocksize = 4096;
        if (size64 / (uint64_t) blocksize <= (uint64_t) ~0UL)
            tmp_blocksize = blocksize;
        if (ioctl(nbd, NBD_SET_BLKSIZE, tmp_blocksize) < 0)
        {
            fprintf(stderr, "Failed to set blocksize %d\n", tmp_blocksize);
            err("Ioctl/1.1a failed: %m\n");
        }
        size = (unsigned long) (size64 / (uint64_t) tmp_blocksize);
        if (ioctl(nbd, NBD_SET_SIZE_BLOCKS, size) < 0)
            err("Ioctl/1.1b failed: %m\n");
        if (tmp_blocksize != blocksize)
        {
            if (ioctl(nbd, NBD_SET_BLKSIZE, (unsigned long) blocksize) < 0)
            {
                fprintf(stderr, "Failed to set blocksize %d\n", blocksize);
                err("Ioctl/1.1c failed: %m\n");
            }
        }
        fprintf(stderr, "bs=%d, sz=%llu bytes\n", blocksize, (unsigned long long) tmp_blocksize * size);
    }

    /* ignore error as kernel may not support */
    ioctl(nbd, NBD_SET_FLAGS, (unsigned long) flags);

    if (ioctl(nbd, BLKROSET, (unsigned long) &read_only) < 0)
        err("Unable to set read-only attribute for device");
}

void set_timeout(int nbd, int timeout)
{
    if (timeout)
    {
        if (ioctl(nbd, NBD_SET_TIMEOUT, (unsigned long) timeout) < 0)
            err("Ioctl NBD_SET_TIMEOUT failed: %m\n");
        fprintf(stderr, "timeout=%d\n", timeout);
    }
}

void finish_sock(int sock, int nbd)
{
    if (ioctl(nbd, NBD_SET_SOCK, sock) < 0)
    {
        if (errno == EBUSY)
            err("Kernel doesn't support multiple connections\n");
        else
            err("Ioctl NBD_SET_SOCK failed: %m\n");
    }
}

void usage(char* errmsg, ...)
{
    if (errmsg)
    {
        char tmp[256];
        va_list ap;
        va_start(ap, errmsg);
        snprintf(tmp, 256, "ERROR: %s\n\n", errmsg);
        vfprintf(stderr, tmp, ap);
        va_end(ap);
    }
    else
    {
        fprintf(stderr, "cbd-client version %s\n", PACKAGE_VERSION);
    }
    fprintf(stderr, "Usage: cbd-client remote:disk nbd_device [-foreground|-f]\n");
    fprintf(stderr, "Or   : cbd-client nbdX\n");
    fprintf(stderr, "Or   : cbd-client -d|--disconnect nbd_device\n");
    fprintf(stderr, "Or   : cbd-client -c|--check nbd_device\n");
    fprintf(stderr, "Or   : cbd-client -l|--list [remote:]name\n");
    fprintf(stderr, "Or   : cbd-client -h|--help\n");
    fprintf(stderr, "Or   : cbd-client -V|--version\n");
}

void disconnect(char* device)
{
    int nbd = open(device, O_RDWR);

    if (nbd < 0)
        err("Cannot open NBD: %m\nPlease ensure the 'nbd' module is loaded.");
    printf("disconnect, ");
    if (ioctl(nbd, NBD_DISCONNECT) < 0)
        err("Ioctl failed: %m\n");
    printf("sock, ");
    if (ioctl(nbd, NBD_CLEAR_SOCK) < 0)
        err("Ioctl failed: %m\n");
    printf("done\n");
}

int main(int argc, char *argv[])
{
    int sock, nbd;
    int blocksize = 4096;
    char *device_name = NULL;
    const char *sep;
    char *nbddev = NULL;
    int timeout = 0;
    int nofork = 0; // if -f NOFORK
    pid_t main_pid;
    uint64_t size64;
    uint16_t flags = 0;
    int c;
    int nonspecial = 0;
    uint16_t needed_flags = 0;
    uint32_t cflags = NBD_FLAG_C_FIXED_NEWSTYLE;
    uint32_t opts = 0;
    sigset_t block, old;
    struct sigaction sa;
    int num_connections = 1;
    struct option long_options[] =
            {
                { "block-size", required_argument, NULL, 'b' },
                { "check", required_argument, NULL, 'c' },
                { "connections", required_argument, NULL, 'C'},
                { "disconnect", required_argument, NULL, 'd' },
                { "foreground", no_argument, NULL, 'f' },
                { "help", no_argument, NULL, 'h' },
                { "list", no_argument, NULL, 'l' },
                { "timeout", required_argument, NULL, 't' },
                { "version", no_argument, NULL, 'V'},
                { 0, 0, 0, 0 }
            };
    int i;

    logging(MY_NAME);

    while ((c = getopt_long_only(argc, argv, "-c:d:lhfb:t:C:V", long_options, NULL)) >= 0)
    {
        switch (c)
        {
        case 1:
            // non-option argument
            if (strchr(optarg, '='))
            {
                // old-style 'bs=' or 'timeout='
                // argument
                fprintf(stderr, "WARNING: old-style command-line argument encountered. This is deprecated.\n");
                if(!strncmp(optarg, "bs=", 3)) {
                    optarg+=3;
                    goto blocksize;
                }
                if(!strncmp(optarg, "timeout=", 8)) {
                    optarg+=8;
                    goto timeout;
                }
                usage("unknown option %s encountered", optarg);
                exit(EXIT_FAILURE);
            }
            switch (nonspecial++)
            {
            case 0:
                device_name = optarg;
                break;
            case 1:
                // nbd_device
                nbddev = optarg;
                break;
            default:
                usage("too many non-option arguments specified");
                exit(EXIT_FAILURE);
            }
            break;
        case 'c':
            return check_conn(optarg, 1);
        case 'd':
            disconnect(optarg);
            exit(EXIT_SUCCESS);
        case 'l':
            needed_flags |= NBD_FLAG_FIXED_NEWSTYLE;
            opts |= NBDC_DO_LIST;
            nbddev = "";
            break;
        case 'h':
            usage(NULL);
            exit(EXIT_SUCCESS);
        case 'f':
            nofork = 1;
            break;
        case 'b':
            blocksize:
            blocksize = (int)strtol(optarg, NULL, 0);
            break;
        case 't':
            timeout:
            timeout = (int)strtol(optarg, NULL, 0);
            break;
        case 'C':
            num_connections = (int)strtol(optarg, NULL, 0);
            break;
        case 'V':
            printf("This is cbd-client version %s\n", PACKAGE_STRING);
            return 0;
        default:
            fprintf(stderr, "E: option eaten by 42 mice\n");
            exit(EXIT_FAILURE);
        }
    }

    if (device_name)
    {
        if (!nbddev && !(opts & NBDC_DO_LIST))
        {
            if (!strncmp(device_name, "nbd", 3) || !strncmp(device_name, "/dev/nbd", 8))
            {
                if (!get_from_config(device_name, &nbddev, &device_name))
                {
                    usage("no valid configuration for specified device found", device_name);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                usage("not enough information specified, and argument didn't look like a cbd device");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
        usage("no information specified");
        exit(EXIT_FAILURE);
    }

    if (!(opts & NBDC_DO_LIST))
    {
        nbd = open(nbddev, O_RDWR);
        if (nbd < 0)
            err("Cannot open NBD: %m\nPlease ensure the 'nbd' module is loaded.");
    }

    ioctl(nbd, NBD_CLEAR_SOCK);

    for (i = 0; i < num_connections; i++)
    {
        sock = openunix(device_name);
        if (sock < 0)
            exit(EXIT_FAILURE);

        negotiate(&sock, &size64, &flags, nbddev + 5, needed_flags, cflags, opts);
        finish_sock(sock, nbd);

        if (i == 0)
        {
            setsizes(nbd, size64, blocksize, flags);
            set_timeout(nbd, timeout);
        }
    }

    /* Go daemon */
    if (!nofork)
    {
        if (daemon(0, 0) < 0)
            err("Cannot detach from terminal");
    }

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &sa, NULL);

    /* For child to check its parent */
    main_pid = getpid();

    sigfillset(&block);
    sigdelset(&block, SIGKILL);
    sigdelset(&block, SIGTERM);
    sigdelset(&block, SIGPIPE);
    sigprocmask(SIG_SETMASK, &block, &old);

    if (!fork())
    {
        /* Due to a race, the kernel NBD driver cannot
         * call for a reread of the partition table
         * in the handling of the NBD_DO_IT ioctl().
         * Therefore, this is done in the first open()
         * of the device. We therefore make sure that
         * the device is opened at least once after the
         * connection was made. This has to be done in a
         * separate process, since the NBD_DO_IT ioctl()
         * does not return until the NBD device has
         * disconnected.
         */
        struct timespec req =
        { .tv_sec = 0, .tv_nsec = 100000000, };
        while (check_conn(nbddev, 0))
        {
            if (main_pid != getppid())
            {
                /* check_conn() will not return 0 when nbd disconnected
                 * and parent exited during this loop. So the child has to
                 * explicitly check parent identity and exit if parent
                 * exited */
                exit(0);
            }
            nanosleep(&req, NULL);
        }
        if (open(nbddev, O_RDWR) < 0) // RDWR to trigger udev watch to fire change event
            perror("could not open device for updating partition table");
        exit(0);
    }

    if (ioctl(nbd, NBD_DO_IT) < 0)
    {
        int error = errno;
        fprintf(stderr, "nbd,%d: Kernel call returned: %d", main_pid, error);
    }
    else
    {
        /* We're on 2.4. It's not clearly defined what exactly
         * happened at this point. Probably best to quit, now
         */
        fprintf(stderr, "Kernel call returned.");
    }

    printf("sock, ");
    ioctl(nbd, NBD_CLEAR_SOCK);
    printf("done\n");

//    if (ioctl(nbd, NBD_SET_SIZE_BLOCKS, 0) < 0)
//        err("Ioctl/1.1b failed: %m\n");

    return 0;
}
