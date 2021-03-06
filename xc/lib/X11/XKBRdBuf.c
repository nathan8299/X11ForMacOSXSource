/* $Xorg: XKBRdBuf.c,v 1.3 2000/08/17 19:45:02 cpqbld Exp $ */
/************************************************************
Copyright (c) 1993 by Silicon Graphics Computer Systems, Inc.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Silicon Graphics not be 
used in advertising or publicity pertaining to distribution 
of the software without specific prior written permission.
Silicon Graphics makes no representation about the suitability 
of this software for any purpose. It is provided "as is"
without any express or implied warranty.

SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/
/* $XFree86: xc/lib/X11/XKBRdBuf.c,v 1.2 2001/10/28 03:32:33 tsi Exp $ */

#include <stdio.h>
#define NEED_REPLIES
#define NEED_EVENTS
#include "Xlibint.h"
#include "XKBlibint.h"
#include <X11/extensions/XKBproto.h>

/***====================================================================***/

int 
#if NeedFunctionPrototypes
_XkbInitReadBuffer(Display *dpy,XkbReadBufferPtr buf,int size)
#else
_XkbInitReadBuffer(dpy,buf,size)
    Display		*dpy;
    XkbReadBufferPtr	 buf;
    int			 size;
#endif
{
    if ((dpy!=NULL) && (buf!=NULL) && (size>0)) {
	buf->error=  0;
	buf->size=   size;
	buf->start= buf->data= _XkbAlloc(size);
	if (buf->start) {
	    _XRead(dpy, buf->start, size);
	    return 1;
	}
    }
    return 0;
}

#define	_XkbReadBufferDataLeft(b)	(((b)->size)-((b)->data-(b)->start))

int
#if NeedFunctionPrototypes
_XkbSkipReadBufferData(XkbReadBufferPtr	from,int size)
#else
_XkbSkipReadBufferData(from,size)
    XkbReadBufferPtr	 from;
    int			 size;
#endif
{
    if (size==0)
	return 1;
    if ((from==NULL)||(from->error)||(size<1)||
					(_XkbReadBufferDataLeft(from)<size))
	return 0;
    from->data+= size;
    return 1;
}

int
#if NeedFunctionPrototypes
_XkbCopyFromReadBuffer(XkbReadBufferPtr	from,char *to,int size)
#else
_XkbCopyFromReadBuffer(from,to,size)
    XkbReadBufferPtr	 from;
    char		*to;
    int			 size;
#endif
{
    if (size==0)
	return 1;
    if ((from==NULL)||(from->error)||(to==NULL)||(size<1)||
					(_XkbReadBufferDataLeft(from)<size))
	return 0;
    memcpy(to,from->data,size);
    from->data+= size;
    return 1;
}

#ifdef XKB_FORCE_INT_KEYSYM
int
#if NeedFunctionPrototypes
_XkbReadCopyKeySyms(int *wire,KeySym *to,int num_words)
#else
_XkbReadCopyKeySyms(wire,to,num_words)
    int *               wire;
    KeySym *            to;
    int                 num_words;
#endif
{
    while (num_words-->0) {
	*to++= *wire++;
    }
    return 1;
}

int
#if NeedFunctionPrototypes
_XkbReadBufferCopyKeySyms(XkbReadBufferPtr from,KeySym *to,int num_words)
#else
_XkbReadBufferCopyKeySyms(from,to,num_words)
    XkbReadBufferPtr    from;
    KeySym *            to;
    int                 num_words;
#endif
{
    if ((unsigned)(num_words*4)>_XkbReadBufferDataLeft(from))
        return 0;
    _XkbReadCopyKeySyms((int *)from->data,to,num_words);
    from->data+= (4*num_words);
    return True;
}

int
#if NeedFunctionPrototypes
_XkbWriteCopyKeySyms (register KeySym *from,CARD32 *to,int len)
#else
_XkbWriteCopyKeySyms (from,to,len)
    register KeySym *           from;
    CARD32 *                    to;
    int                         len;
#endif
{

    while (len-->0) {
        *to++= (CARD32)*from++;
    }
    return True;
}
#endif

#ifdef LONG64
int
#if NeedFunctionPrototypes
_XkbReadCopyData32(int *wire,long *to,int num_words)
#else
_XkbReadCopyData32(wire,to,num_words)
    int *		wire;
    long *		to;
    int			num_words;
#endif
{
    while (num_words-->0) {
	*to++= *wire++;
    }
    return 1;
}
#endif
#ifdef WORD64
int
#if NeedFunctionPrototypes
_XkbReadCopyData32(int *from,long *lp,int num_words)
#else
_XkbReadCopyData32(from,lp,num_words)
    int *		from;
    long *		lp;
    int			num_words;
#endif
{
long *lpack;
long mask32 = 0x00000000ffffffff;
long maskw, i, bits;

    lpack = (long *)from;
    bits = 32;

    for (i=0;i<num_words;i++) {
	maskw = mask32 << bits;
	*lp++ = (*lpack & maskw) >> bits;
	bits = bits ^ 32;
	if (bits)
	    lpack++;
    }
    return 1;
}
#endif

#if defined(LONG64) || defined(WORD64)
int
#if NeedFunctionPrototypes
_XkbReadBufferCopy32(XkbReadBufferPtr from,long *to,int num_words)
#else
_XkbReadBufferCopy32(from,to,num_words)
    XkbReadBufferPtr	from;
    long *		to;
    int			num_words;
#endif
{
    if ((unsigned)(num_words*4)>_XkbReadBufferDataLeft(from))
	return 0;
    _XkbReadCopyData32((int *)from->data,to,num_words);
    from->data+= (4*num_words);
    return True;
}
#endif

#ifdef LONG64
int
#if NeedFunctionPrototypes
_XkbWriteCopyData32 (register unsigned long *from,CARD32 *to,int len)
#else
_XkbWriteCopyData32 (from,to,len)
    register unsigned long *	from;
    CARD32 *			to;
    int 			len;
#endif
{

    while (len-->0) {
	*to++= (CARD32)*from++;
    }
    return True;
}
#endif /* LONG64 */

#ifdef WORD64
_XkbWriteCopyData32 Not Implemented Yet for sizeof(int)==8
#endif

char *
#if NeedFunctionPrototypes
_XkbPeekAtReadBuffer(XkbReadBufferPtr from,int size)
#else
_XkbPeekAtReadBuffer(from,size)
    XkbReadBufferPtr	 from;
    int			 size;
#endif
{
    if ((from==NULL)||(from->error)||(size<1)||
					(_XkbReadBufferDataLeft(from)<size))
	return 0;
    return from->data;
}

char *
#if NeedFunctionPrototypes
_XkbGetReadBufferPtr(XkbReadBufferPtr from,int size)
#else
_XkbGetReadBufferPtr(from,size)
    XkbReadBufferPtr	 from;
    int			 size;
#endif
{
char	*ptr;
    if ((from==NULL)||(from->error)||(size<1)||
					(_XkbReadBufferDataLeft(from)<size))
	return 0;
    ptr= from->data;
    from->data+= size;
    return ptr;
}


int
#if NeedFunctionPrototypes
_XkbFreeReadBuffer(XkbReadBufferPtr buf)
#else
_XkbFreeReadBuffer(buf)
    XkbReadBufferPtr	buf;
#endif
{
    if ((buf!=NULL) && (buf->start!=NULL)) {
	int left;
	left= (int)_XkbReadBufferDataLeft(buf);
	if (buf->start!=NULL)
	    Xfree(buf->start);
	buf->size= 0;
	buf->start= buf->data= NULL;
	return left;
    }
    return 0;
}

Bool
#if NeedFunctionPrototypes
_XkbGetReadBufferCountedString(XkbReadBufferPtr buf,char **rtrn)
#else
_XkbGetReadBufferCountedString(buf,rtrn)
    XkbReadBufferPtr	buf;
    char **		rtrn;
#endif
{
CARD16	len,*pLen;
int	left;
char *	str = NULL;

    if ((buf==NULL)||(buf->error)||((left=(int)_XkbReadBufferDataLeft(buf))<4))
	return False;
    pLen= (CARD16 *)buf->data;
    len= *pLen;
    if (len>0) {
	if (XkbPaddedSize(len+2)>left)
	    return False;
	str= _XkbAlloc(len+1);
	if (str) {
	    memcpy(str,&buf->data[2],len);
	    str[len]= '\0';
	}
    }
    buf->data+= XkbPaddedSize(len+2);
    *rtrn= str;
    return True;
}
