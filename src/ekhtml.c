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

/*
 * ekhtml: The El-Kabong HTML parser
 *         by Jon Travis (jtravis@p00p.org)
 *
 * El-Kabong: A speedy, yet forgiving, APR-centric, SAX-stylee HTML parser.  
 *
 * The idea behind this parser is for it to use very little memory, and still 
 * be very speedy, while forgiving poorly written HTML.

 * The internals of the parser consist of a small memory buffer which is able
 * to grow when not enough information is known to correctly parse a tag.
 * Given the typical layout of HTML, 4k should be plenty.  
 *
 * The main state engine loops through this internal buffer, determining what 
 * the next state should be.  Once this is known, it passes off a segment to 
 * the state handlers (starttag, endtag, etc.) to process.  The segment
 * handlers and the main state engine communicate via a few variables.  These
 * variables indicate whether or not the main engine should switch state,
 * or successfully remove some data, etc.  The segment handlers are 
 * guaranteed the same starting data (though not the same pointer) on each
 * invocation until the state is changed.  Thus, the segment handlers cannot
 * use pointers into the main buffer -- they must use offsets.
 *
 * Some of the speed is gained from using character map data found in
 * ekhtml_tables.h.  I don't have any empirical data for this yet --
 * it only sounds like it would be faster.. ;-)  
 *
 * I'm always looking for ways to clean && speed up this code.  Feel free
 * to give feedback -- JMT
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "apr.h"
#include "apr_lib.h"
#include "apr_hash.h"
#include "apr_general.h"
#include "apr_strings.h"

#include "ekhtml.h"
#define EKHTML_USE_TABLES
#include "ekhtml_tables.h"
#define EKHTML_USE_PRIVATE
#include "ekhtml_private.h"

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif


/*
 * ekhtml_buffer_grow:  Grow the parser's internal buffer by a blocksize.  
 *                      NOTE:  Calling the function has the potential to
 *                             change the data buffer location.  Do
 *                             not rely on it's location!
 *
 * Arguments:           Parser = Parser to grow
 */

static void ekhtml_buffer_grow(ekhtml_parser_t *parser){
    apr_abortfunc_t afunc;
    apr_size_t newsize;
    char *newbuf;
    
    newsize = parser->nalloced + EKHTML_BLOCKSIZE;
    
    if((newbuf = realloc(parser->buf, newsize)) == NULL){
        fprintf(stderr, "BAD! Can't allocate %d bytes in ekhtml_buffer_grow\n",
                newsize);
        fflush(stderr); /* Just in case someone changes the buffering scheme */
        afunc = apr_pool_get_abort(parser->pool);
        afunc(APR_ENOMEM);
    }

    parser->buf      = newbuf;
    parser->nalloced = newsize;
}

/*
 * parser_state_determine:  Determine the next state that the main parser 
 *                          should have, by investigating up to the first
 *                          4 characters in the buffer.
 *
 * Arguments:      startp = Starting data pointer
 *                 endp   = Pointer to first byte of 'out of range' data
 *
 * Return values:  Returns one of EKHTML_STATE_* indicating the state that
 *                 was found.
 *
 */

static APR_INLINE 
int parser_state_determine(const char *startp, const char *endp){
    const char *firstchar;
    int newstate;
    
    assert(startp != endp);
    
    if(*startp != '<')
        return EKHTML_STATE_INDATA;
    
    firstchar = startp + 1;
    if(firstchar == endp)
        return EKHTML_STATE_NONE;

    newstate = EKCMap_EKState[(unsigned char)*firstchar];
    if(newstate == EKHTML_STATE_NONE){
        if(firstchar + 2 >= endp) /* Not enough data to evaluate */
            return EKHTML_STATE_NONE;
        if(*(firstchar + 1) == '-' && *(firstchar + 2) == '-')
            return EKHTML_STATE_COMMENT;
        else
            return EKHTML_STATE_SPECIAL;
    } else 
        return newstate;
}


/*
 * ekhtml_parser_flush:  Flush the parser innards.  The internal buffer is
 *                       processed, and any remaining state is saved.  All
 *                       data which is processed is removed from the parser
 *                       and the internal buffer is reshuffled.  
 *
 * Arguments:            parser   = Parser to flush
 *                       flushall = Flush all data, even if some tags
 *                                  weren't finished, etc.
 *
 * Return values:        Returns 1 if action was taken -- i.e. bytes were
 *                       processed, and the internal buffer was reshuffled
 *                       else 0
 */

int ekhtml_parser_flush(ekhtml_parser_t *parser, int flushall){
    void **state_data = &parser->state.state_data;
    char *buf = parser->buf, *curp = buf, *endp = buf + parser->nbuf;
    int badp = -1, tmpstate = parser->state.state, didsomething = 0;
    
    while(curp != endp){
        char *workp = curp;
        
        if(tmpstate == EKHTML_STATE_NONE){
            tmpstate = parser_state_determine(workp, endp);
            if(tmpstate == EKHTML_STATE_NONE)  /* Not enough data yet */
                break;
        }
        
        if(tmpstate == EKHTML_STATE_INDATA || tmpstate == EKHTML_STATE_BADDATA)
            curp = ekhtml_parse_data(parser, workp, endp, tmpstate);
        else if(endp - workp > 2){  /* All tags fall under this catagory */
            switch(tmpstate){
            case EKHTML_STATE_ENDTAG:
                curp = ekhtml_parse_endtag(parser, state_data, 
                                           workp, endp, &badp);
                break;
            case EKHTML_STATE_STARTTAG:
                curp = ekhtml_parse_starttag(parser, state_data, 
                                             workp, endp, &badp);
                break;
            case EKHTML_STATE_COMMENT:
                curp = ekhtml_parse_comment(parser, state_data, 
                                            workp, endp, &badp);
                break;
            case EKHTML_STATE_SPECIAL:
                curp = ekhtml_parse_special(parser, state_data, 
                                            workp, endp, &badp);
                break;
            default:
                assert(!"Unimplemented state");
            }
        } else {
            curp = NULL; /* Not enough data, keep going */
        }
        
        /* If one of the parsers said the data was bad, reset the state */
        if(badp != -1){
            tmpstate = badp;
            badp = -1;
        }

        if(curp == NULL){ /* State needed more data, so break out */
            curp = workp;
            break;
        }

        if(workp != curp){  /* state backend cleared up some data */
            didsomething = 1;
            tmpstate = EKHTML_STATE_NONE;
            assert(*state_data == NULL);
        }
    }

    if(flushall){
        /* Flush whatever we didn't use */
        if(parser->datacb)
            parser->datacb(parser->cbdata, curp, endp - curp);
        curp = endp;
        didsomething = 1;
        tmpstate = EKHTML_STATE_NONE;   /* Clean up to an unknown state */
        *state_data = NULL;
    }

    parser->state.state = tmpstate;

    if(didsomething){
        /* Shuffle the data back, based on where we ended up */
        parser->nbuf -= curp - buf;
        if(endp - curp){  /* If there's still any data to move */
            memmove(buf, curp, endp - curp);
        }
    }
    return didsomething;
}

/*
 * ekhtml_parser_feed:  Feed data into the HTML parser.  This routine will
 *                      fill up the internal buffer until it can go no
 *                      more, then flush the data and refill.  If there is
 *                      more data that is required than the internal buffer
 *                      can hold, it will be resized
 *
 * Arguments:           parser = Parser to feed the data into 
 *                      data   = Bytes to feed the parser
 *                      ndata  = # of bytes in `data`
 */

void ekhtml_parser_feed(ekhtml_parser_t *parser, const char *data,
			apr_size_t ndata)
{
    apr_size_t nfed = 0;
    
    while(nfed != ndata){
        apr_size_t tocopy;
        
        /* First see how much we can fill up our internal buffer */
        tocopy = MIN(parser->nalloced - parser->nbuf, ndata - nfed);
        memcpy(parser->buf + parser->nbuf, data + nfed, tocopy);
        nfed         += tocopy;
        parser->nbuf += tocopy;
        if(parser->nalloced == parser->nbuf){
            /* Process the buffer */
            if(!ekhtml_parser_flush(parser, 0)){
                /* If we didn't actually process anything, grow our buffer */
                ekhtml_buffer_grow(parser);
            }
        }
    }
}

static apr_status_t ekhtml_parser_cleanup(void *cbdata){
    ekhtml_parser_t *ekparser = cbdata;
    
    free(ekparser->buf);
    return APR_SUCCESS;
}

/*
 * ekhtml_parser_new:  Create a new parser instance.
 *
 * Arguments:          pool   = Pool out of which to allocate the parser.
 *                     cbdata = Callback data to use when calling back 
 *                              into various ... callbacks ... such as the
 *                              data handler, and tag callbacks.
 *
 * Return values:      Returns a new parser instance.
 */

ekhtml_parser_t *ekhtml_parser_new(apr_pool_t *pool, void *cbdata){
    ekhtml_parser_t *res;
    
    res = apr_palloc(pool, sizeof(*res));
    res->pool               = pool;
    res->datacb             = NULL;
    res->startcb            = NULL;
    res->endcb              = NULL;
    res->cbdata             = cbdata;
    res->startcb_unk        = NULL;
    res->endcb_unk          = NULL;
    res->commentcb          = NULL;
    res->buf                = NULL;
    res->nalloced           = 0;
    res->nbuf               = 0;
    res->freeattrs          = NULL;
    res->state.state        = EKHTML_STATE_NONE;
    res->state.state_data   = NULL;
    
    /* Start out with a buffer of 1 block size */
    ekhtml_buffer_grow(res);
    apr_pool_cleanup_register(pool, res, ekhtml_parser_cleanup, 
                              apr_pool_cleanup_null);
    return res;
}

