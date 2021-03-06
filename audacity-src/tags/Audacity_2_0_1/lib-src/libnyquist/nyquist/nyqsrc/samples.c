/* samples.c -- various functions for the Nyquist sound data type */

/* CHANGE LOG
 * --------------------------------------------------------------------
 * 28Apr03  dm  min->MIN, max->MAX
 */

#include <stdio.h>
#ifndef mips
#include "stdlib.h"
#endif
#include "xlisp.h"
#include "sound.h"
#include "falloc.h"
#include "samples.h"


LVAL s_next = NULL;
LVAL s_send;

void samples_symbols()
{
    s_next = xlenter(":NEXT");
    s_send = xlenter("SEND");
}

/* snd_from_array -- convert lisp array to sound type */
/**/
sound_type snd_from_array(double t0, double sr, LVAL array)
{
    sound_type result;
    snd_list_type snd_list;
    long total = 0;

    if (!vectorp(array)) xlerror("array expected", array);

    result = sound_create(NULL, t0, sr, 1.0);
    snd_list = result->list;
    while (total < getsize(array)) {
        long togo = MIN(getsize(array) - total, max_sample_block_len);
        sample_block_type block;
        int i;
        falloc_sample_block(block, "snd_from_array");
        snd_list->block = block;
        for (i = 0; i < togo; i++) {
            LVAL elem = getelement(array, total + i);
            sample_type *ptr = block->samples + i;
            if (fixp(elem)) *ptr = (sample_type) getfixnum(elem);
            else if (floatp(elem)) *ptr = (sample_type) getflonum(elem);
            else xlerror("expecting array elem to be number", elem);
        }
        total += togo;
        snd_list->block_len = (short) togo;
        snd_list->u.next = snd_list_create(NULL);
        snd_list = snd_list->u.next;
    }
    snd_list->block_len = max_sample_block_len;
    snd_list->block = zero_block;
    snd_list->logically_stopped = true;
    snd_list->u.next = zero_snd_list;
    return result;
}


/* snd_length -- count how many samples are in a sound */
/**/
long snd_length(sound_type s, long len)
{
    long total = 0;
    long blocklen;

    s = sound_copy(s);
    if (len > s->stop) len = s->stop;
    while (total < len) {
        sample_block_type sampblock = sound_get_next(s, &blocklen);
        if (sampblock == zero_block) break;
        total += blocklen;
    }
    if (total > len) total = len;
    sound_unref(s);
    return total;
}


/* snd_maxsamp -- compute the maximum value of samples in s */
/**/
double snd_maxsamp(sound_type s)
{
    sample_type result = 0.0F;
    long blocklen;
    s = sound_copy(s);

    while (true) {
        sample_block_type sampblock = sound_get_next(s, &blocklen);
        long i;
        sample_block_values_type sbufp = sampblock->samples;
        if (sampblock == zero_block || blocklen == 0) {
            break;
        }
        for (i = 0; i < blocklen; i++) {
            register sample_type samp = *sbufp++;
            if (result < samp) result = samp;
            else if (result < -samp) result = -samp;
        }
    }
    return (double) (s->scale * result);
}


/* snd_samples -- convert sound (prefix) to lisp array */
/**/
LVAL snd_samples(sound_type s, long len)
{
    LVAL v;
    long vx = 0;
    long blocklen;
    register double scale_factor = s->scale;
    len = snd_length(s, len);
    s = sound_copy(s);

    xlsave1(v);
    v = newvector(len);

    while (len > 0) {
        sample_block_type sampblock = sound_get_next(s, &blocklen);
        long togo = MIN(blocklen, len);
        long i;
        sample_block_values_type sbufp = sampblock->samples;
        for (i = 0; i < togo; i++) {
            setelement(v, vx++, cvflonum(*sbufp++ * scale_factor));
        }
        len -= togo;
    }
    sound_unref(s);

    /* restore the stack */
    xlpop();
    return v;
}


/* NOTE: this code does not properly handle start times that do not
 * correspond to the time of the first actual sample
 */

/* NOTE: we need some addtional state to keep track of where we are.
 *  We'll use the extra field of sound_type; first long is length,
 *  so second field will be the count of how many samples we've read.
 */
#define CNT extra[1]
#define INDEX extra[2]
#define FIELDS 3
#define SAMPLES list->block->samples

LVAL snd_fetch(sound_type s)
{
    if (!s->extra) { /* this is the first call, so fix up s */
        s->extra = (long *) malloc(sizeof(long) * FIELDS);
        s->extra[0] = sizeof(long) * FIELDS;
        s->CNT = s->INDEX = 0;
    } else if (s->extra[0] != sizeof(long) * FIELDS) {
        xlfail("sound in use by another iterator");
    }
    if (s->CNT == s->INDEX) {
        sound_get_next(s, &(s->CNT));
        s->INDEX = 0;
    }
    if (s->SAMPLES == zero_block->samples) {
        return NULL;
    }	

    /* logical stop time is ignored by this code -- to fix this,
     * you would need a way to return the logical stop time to 
     * the caller.
     */

    return cvflonum(s->SAMPLES[s->INDEX++] * s->scale);
} /* snd_fetch */


/* snd_fetch_array -- fetch a lisp array of samples */
/*
 * storage layout: the extra field points to extra state that we'll use
 * extra[0] -> length of extra storage
 * extra[1] -> CNT (number of samples in current block)
 * extra[2] -> INDEX (current sample index in current block)
 * extra[3] -> FILLCNT (how many samples in buffer)
 * extra[4] -> TERMCNT (how many samples until termination)
 * extra[4 .. 4+len-1] -> samples (stored as floats)
 * 
 * Termination details:
 *   Return NIL when the sound terminates.
 *   Termination is defined as the point where all original
 * signal samples have been shifted out of the samples buffer
 * so that all that's left are zeros from beyond the termination
 * point.
 *   Implementation: when termination is discovered, set TERMCNT
 * to the number of samples to be shifted out. TERMCNT is initially
 * -1 as a flag that we haven't seen the termination yet. 
 * Each time samples are shifted, decrement TERMCNT by the shift amount.
 * When TERMCNT goes to zero, return NULL.
 */
/* these macros define entries in extra, more macros are defined above */
#define FILLCNT extra[3]
#define TERMCNT extra[4]
#define OFFSET 5

LVAL snd_fetch_array(sound_type s, long len, long step)
{
    long i, maxlen, skip, fillptr;
    float *samples;
    LVAL result;
    
    if (len < 1) xlfail("len < 1");

    if (!s->extra) { /* this is the first call, so fix up s */
        s->extra = (long *) malloc(sizeof(long) * (len + OFFSET));
        s->extra[0] = sizeof(long) * (len + OFFSET);
        s->CNT = s->INDEX = s->FILLCNT = 0;
        s->TERMCNT = -1;
        maxlen = len;
    } else {
        maxlen = (s->extra[0] / sizeof(long)) - OFFSET;
        if (maxlen < 1) xlfail("sound in use by another iterator");
        if (maxlen < len) xlfail("len grew");
    }
    samples = (float *) &(s->extra[OFFSET]);
    
    /* step 1: refill buffer with samples */
    fillptr = s->FILLCNT;
    while (fillptr < maxlen) {
        if (s->INDEX == s->CNT) {
            sound_get_next(s, &(s->CNT));
            if (s->SAMPLES == zero_block->samples) {
                if (s->TERMCNT < 0) s->TERMCNT = fillptr;
            }	
            s->INDEX = 0;
        }
        samples[fillptr++] = s->SAMPLES[s->INDEX++] * s->scale;
    }
    s->FILLCNT = fillptr;

    /* it is important to test here AFTER filling the buffer, because
     * if fillptr WAS 0 when we hit the zero_block, then filling the 
     * buffer will set TERMCNT to 0.
     */
    if (s->TERMCNT == 0) return NULL;
    
    /* logical stop time is ignored by this code -- to fix this,
     * you would need a way to return the logical stop time to 
     * the caller.
     */

    /* step 2: construct an array and return it */
    xlsave1(result);
    result = newvector(len);

    for (i = 0; i < len; i++) {
        setelement(result, i, cvflonum(samples[i]));
    }

    /* step 3: shift samples by step */
    if (step < 0) xlfail("step < 0");
    s->FILLCNT -= step;
    if (s->FILLCNT < 0) s->FILLCNT = 0;
    for (i = 0; i < s->FILLCNT; i++) {
        samples[i] = samples[i + step];
    }
    

    if (s->TERMCNT >= 0) {
        s->TERMCNT -= step;
        if (s->TERMCNT < 0) s->TERMCNT = 0;
    }


    /* step 4: advance in sound to next sample we need
     *   (only does work if step > size of buffer)
     */
    skip = step - maxlen;
    while (skip > 0) {
        long remaining = s->CNT - s->INDEX;
        if (remaining >= skip) {
            s->INDEX += skip;
            skip = 0;
        } else {
            skip -= remaining;
            sound_get_next(s, &(s->CNT));
            s->INDEX = 0;
        }
    }
    
    /* restore the stack */
    xlpop();
    return result;
} /* snd_fetch_array */




