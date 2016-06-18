#ifndef LIBDVDCSS_CONFIG_H
#define LIBDVDCSS_CONFIG_H 1

#define VERSION "1.4.0"
#define PACKAGE "libdvdcss"

#define PACKAGE_VERSION VERSION
#define PACKAGE_NAME    PACKAGE
#define PACKAGE_TARNAME PACKAGE
#define PACKAGE_STRING  PACKAGE_NAME " " PACKAGE_VERSION

#define PACKAGE_URL "https://www.videolan.org/developers/libdvdcss.html"
#define PACKAGE_BUGREPORT ""

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
#define _DARWIN_USE_64_BIT_INODE 1
#endif

#if defined(__linux)

/* Define if <linux/cdrom.h> defines DVD_STRUCT. */
#define DVD_STRUCT_IN_LINUX_CDROM_H 1
/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1
/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1
/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1
/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1
/* Define to 1 if you have the <linux/cdrom.h> header file. */
#define HAVE_LINUX_CDROM_H 1
/* Define if Linux-like dvd_struct is defined. */
#define HAVE_LINUX_DVD_STRUCT 1
/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1
/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1
/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1
/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1
/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1
/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1
/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1
/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1
/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1
/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1
/* Define to 1 if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1
/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1
/* Define O_BINARY if missing */
#define O_BINARY 0
/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
/* Define this if the compiler supports __attribute__((visibility("default"))) */
#define SUPPORT_ATTRIBUTE_VISIBILITY_DEFAULT 1
/* Define this if the compiler supports the -fvisibility flag */
#define SUPPORT_FLAG_VISIBILITY 1

#elif defined(__WIN32) || defined(_WIN32)

/* Define if you have a broken mkdir */
#define HAVE_BROKEN_MKDIR 1
/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1
/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1
/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1
/* Define to 1 if you have the <io.h> header file. */
#define HAVE_IO_H 1
/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1
/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1
/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1
/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1
/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1
/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1
/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1
/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1
/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1
/* Define to 1 if you have the <windows.h> header file. */
#define HAVE_WINDOWS_H 1
/* Define to 1 if you have the <winioctl.h> header file. */
#define HAVE_WINIOCTL_H 1
/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
/* Define this if the compiler supports __attribute__((visibility("default"))) */
#define SUPPORT_ATTRIBUTE_VISIBILITY_DEFAULT 1
/* Define this if the compiler supports the -fvisibility flag */
#define SUPPORT_FLAG_VISIBILITY 1
/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64
/* Define to '0x0501' for IE 5.01 (and shell) APIs. */
#define _WIN32_IE 0x0501

#elif defined(__APPLE__) && defined(__MACH__)

/* Have IOKit DVD IOCTL headers */
#define DARWIN_DVD_IOCTL 1
/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1
/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1
/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1
/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1
/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1
/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1
/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1
/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1
/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1
/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1
/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1
/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1
/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1
/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1
/* Define to 1 if you have the <sys/uio.h> header file. */
#define HAVE_SYS_UIO_H 1
/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1
/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"
/* Define O_BINARY if missing */
#define O_BINARY 0
/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1
/* Define this if the compiler supports __attribute__((visibility("default"))) */
#define SUPPORT_ATTRIBUTE_VISIBILITY_DEFAULT 1
/* Define this if the compiler supports the -fvisibility flag */
#define SUPPORT_FLAG_VISIBILITY 1

#endif /* Linux / Windows / Mac OS X */
#endif /* LIBDVDCSS_CONFIG_H */
