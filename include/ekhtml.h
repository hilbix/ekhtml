/*
 * Copyright (c) 2002, Jon Travis
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef EKHTML_DOT_H
#define EKHTML_DOT_H

#include "apr.h"
#include "apr_general.h"

typedef struct ekhtml_attr_t {
    const char *name;           /* Name of the attribute                  */
    apr_size_t namelen;         /* Length of 'name'                       */
    
    /* Value data is NULL and unused if the attribute is a boolean value  */
    const char *val;            /* Value of the attribute (if !boolean)   */
    apr_size_t vallen;          /* Value length                           */
    struct ekhtml_attr_t *next; /* Pointer to next attribute in the list  */
} ekhtml_attr_t;

/*
 * Typedefs for function callback types
 */

typedef struct ekhtml_parser_t ekhtml_parser_t;

typedef void (*ekhtml_data_cb_t)(void *cbdata, const char *data, 
				 apr_size_t ndata);
typedef void (*ekhtml_starttag_cb_t)(void *cbdata, const char *tag, 
				     apr_size_t ntag, ekhtml_attr_t *attrs);
typedef void (*ekhtml_endtag_cb_t)(void *cbdata, const char *tag, 
				   apr_size_t ntag);

/*
 * Protoize
 */

extern ekhtml_parser_t *ekhtml_parser_new(apr_pool_t *, void *);
extern void ekhtml_parser_cbdata_set(ekhtml_parser_t *, void *);
extern void ekhtml_parser_datacb_set(ekhtml_parser_t *, ekhtml_data_cb_t );
extern void ekhtml_parser_commentcb_set(ekhtml_parser_t *, ekhtml_data_cb_t );
extern void ekhtml_parser_feed(ekhtml_parser_t *, const char *, apr_size_t);
extern int ekhtml_parser_flush(ekhtml_parser_t *, int);
extern void ekhtml_parser_startcb_add(ekhtml_parser_t *, const char *,
				      ekhtml_starttag_cb_t );
extern void ekhtml_parser_endcb_add(ekhtml_parser_t *, const char *,
				    ekhtml_endtag_cb_t);

/* EKHTML_BLOCKSIZE = # of blocks to allocate per chunk */
#define EKHTML_BLOCKSIZE (1024 * 4)  

#endif
