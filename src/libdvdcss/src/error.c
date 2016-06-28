/*****************************************************************************
 * error.c: error management functions
 *****************************************************************************
 * Copyright (C) 1998-2002 VideoLAN
 *
 * Author: Sam Hocevar <sam@zoy.org>
 *
 * libdvdcss is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * libdvdcss is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with libdvdcss; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *****************************************************************************/

#include <stdarg.h>
#include <stdio.h>

#include "libdvdcss.h"

static void print_message( const char *prefix, const char *psz_string,
                           va_list args )
{
    fprintf( stderr, "libdvdcss %s: ", prefix );
    vfprintf( stderr, psz_string, args );
    fprintf( stderr, "\n" );
}

/* TL: Extended error log */
static void log_message( dvdcss_log p_log, void *p_log_param,
                         int b_debug, const char *psz_string, va_list args )
{
    char buffer [1024];
    vsnprintf( buffer, sizeof(buffer), psz_string, args );
    buffer [sizeof(buffer) - 1] = '\0';
    p_log( p_log_param, b_debug, buffer );
}

/*****************************************************************************
 * Error messages
 *****************************************************************************/
void print_error( dvdcss_t dvdcss, const char *psz_string, ... )
{
    if( dvdcss->p_log )
    {
        va_list args;

        va_start( args, psz_string );
        log_message(dvdcss->p_log, dvdcss->p_log_param, 0, psz_string, args);
        va_end( args );
    }
    else if( dvdcss->b_errors )
    {
        va_list args;

        va_start( args, psz_string );
        print_message("error", psz_string, args);
        va_end( args );
    }

    dvdcss->psz_error = psz_string;
}

/*****************************************************************************
 * Debug messages
 *****************************************************************************/
void print_debug( const dvdcss_t dvdcss, const char *psz_string, ... )
{
    if( dvdcss->p_log )
    {
        va_list args;

        va_start( args, psz_string );
        log_message(dvdcss->p_log, dvdcss->p_log_param, 1, psz_string, args);
        va_end( args );
    }
    else if( dvdcss->b_debug )
    {
        va_list args;

        va_start( args, psz_string );
        print_message("debug", psz_string, args );
        va_end( args );
    }
}
