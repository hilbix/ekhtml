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

#include "apr_lib.h"
#include "apr_strings.h"

#include "ekhtml.h"
#include "ekhtml_tables.h"
#define EKHTML_USE_PRIVATE
#include "ekhtml_private.h"


/*
 * ekhtml_make_upperstr:  Make a new bytestring based on the old one .. 
 *                        only uppercase.  
 *
 * Arguments:             buf  = Buffer containing bytes to 'upper'
 *                        len  = Length of bytes in buf
 *
 * Return values:  Capitalizes the string pointed at by 'buf', and returns
 *                 'buf'
 */

char *ekhtml_make_upperstr(char *buf, int len){
    char *endp = buf + len, *cp;
    
    for(cp = buf; cp < endp; cp++)
        *cp = apr_toupper(*cp);
    return buf;
}


/*
 * ekhtml_parser_datacb_set:  Set the data handler callback for the parser.
 *
 * Arguments:       parser = Parser to set the callback for
 *                  cb     = New callback
 */

void ekhtml_parser_datacb_set(ekhtml_parser_t *parser, ekhtml_data_cb_t cb){
    parser->datacb = cb;
}

/*
 * ekhtml_parser_commentcb_set:  Set the comment handler callback.
 *
 * Arguments:       parser = Parser to set the callback for
 *                  cb     = New callback
 */

void ekhtml_parser_commentcb_set(ekhtml_parser_t *parser, ekhtml_data_cb_t cb){
    parser->commentcb = cb;
}

/*
 * ekhtml_parser_cbdata_set:  Set the callback data for the parser
 *
 * Arguments:       parser = Parser to set the callback for
 *                  cb     = New callback data.
 */

void ekhtml_parser_cbdata_set(ekhtml_parser_t *parser, void *cbdata){
    parser->cbdata = cbdata;
}

/*
 * ekhtml_parser_startcb_add:  Add a new start-tag callback to the parser.
 *                             The tag information is case insensitive.
 *
 * Arguments:    parser = Parser to add a new starttag callback for
 *               tag    = NULL for any unknown tag, else a string
 *                        containing the tag for which the callback 
 *                        should be made.
 *               cback  = Callback to invoke when the tag is found.
 */

void ekhtml_parser_startcb_add(ekhtml_parser_t *parser, const char *tag,
			       ekhtml_starttag_cb_t cback)
{
    if(tag){
        ekhtml_tag_container *newcont;
        char *newtag, *cp;
        
        if(!parser->startcb)
            parser->startcb = apr_hash_make(parser->pool);
        
        newcont = apr_palloc(parser->pool, sizeof(*newcont));
        newcont->startfunc = cback;
        newtag = apr_pstrdup(parser->pool, tag);
        for(cp=newtag; *cp; cp++)
            *cp = apr_toupper(*cp);
        apr_hash_set(parser->startcb, newtag, cp - newtag, newcont);
    } else {
        parser->startcb_unk = cback;
    }
}

/*
 * ekhtml_parser_endcb_add:  Add a new end-tag callback to the parser.
 *                           The tag information is case insensitive.
 *
 * Arguments:    parser = Parser to add a new endtag callback for
 *               tag    = NULL for any unknown tag, else a string
 *                        containing the tag for which the callback 
 *                        should be made.
 *               cback  = Callback to invoke when the tag is found.
 */

void ekhtml_parser_endcb_add(ekhtml_parser_t *parser, const char *tag,
			     ekhtml_endtag_cb_t cback)
{
    if(tag){
        ekhtml_tag_container *newcont;
        char *newtag, *cp;
        
        if(!parser->endcb)
            parser->endcb = apr_hash_make(parser->pool);
        
        newcont = apr_palloc(parser->pool, sizeof(*newcont));
        newcont->endfunc = cback;
        newtag = apr_pstrdup(parser->pool, tag);
        for(cp=newtag; *cp; cp++)
            *cp = apr_toupper(*cp);
        apr_hash_set(parser->endcb, newtag, cp - newtag, newcont);
    } else {
        parser->endcb_unk = cback;
    }
}

