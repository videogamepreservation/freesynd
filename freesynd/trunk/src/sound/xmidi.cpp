/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005  Stuart Binge  <skbinge@gmail.com>              *
 *   Copyright (C) 2005  Joost Peters  <joostp@users.sourceforge.net>   *
 *   Copyright (C) 2006  Trent Waddington <qg@biodome.org>              *
 *   Copyright (C) 2006  Tarjei Knapstad <tarjei.knapstad@gmail.com>    *
 *   Copyright (C) 2007  Mike Nordell ("proper" IFF parsing)            *
 *                                                                      *
 *    This program is free software;  you can redistribute it and / or  *
 *  modify it  under the  terms of the  GNU General  Public License as  *
 *  published by the Free Software Foundation; either version 2 of the  *
 *  License, or (at your option) any later version.                     *
 *                                                                      *
 *    This program is  distributed in the hope that it will be useful,  *
 *  but WITHOUT  ANY WARRANTY;  without even  the implied  warranty of  *
 *  MERCHANTABILITY  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *
 *  General Public License for more details.                            *
 *                                                                      *
 *    You can view the GNU  General Public License, online, at the GNU  *
 *  project's  web  site;  see <http://www.gnu.org/licenses/gpl.html>.  *
 *  The full text of the license is also included in the file COPYING.  *
 *                                                                      *
 ************************************************************************/

#include "xmidi.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define XMIDI_CONTROLLER_NEXT_BREAK     117


struct IFFchunk
{
\tbool isType(const char szId[4]) const {
\t\treturn memcmp(id, szId, 4) == 0;
\t}

\t// getActualSize takes IFF rounding rules into account,
\t// keeping stream sizes 2-byte aligned
\tuint32 getActualSize() const {
\t\treturn size + (size&1);
\t}

\tchar   id[4];
\tuint32 size;
};


static uint32 get_IFF_uint32(const unsigned char* stream)
{
\treturn (stream[0] << 24) |
\t       (stream[1] << 16) |
\t       (stream[2] <<  8) |
\t       (stream[3]);
}

static void getIFFchunkHeader(IFFchunk& chunk, const unsigned char* stream)
{
\tmemcpy(&chunk.id, stream, 4);
\tchunk.size = get_IFF_uint32(stream + 4);
}


std::vector<XMidi::Midi> XMidi::convertXMidi(uint8* buf, int size)
{
    std::vector<Midi> midi;
    XMidiFile* xmi = loadXMidi(buf, size);
    if (xmi)
    {
\tmidi.resize(xmi->tracks);
\tfor(unsigned int i = 0; i < xmi->tracks; ++i)
\t{
\t    midi[i].size_ = retrieveMidi(xmi, i, &(midi[i].data_));
\t}
\tdelete xmi;
    }
    return midi;
}

XMidi::XMidiFile* XMidi::loadXMidi(uint8* buf, int size)
{
    XMidiFile* xmidi = new XMidiFile;

    if (!readFile (xmidi, buf, size))
    {
\tdelete xmidi;
\txmidi = NULL;
    }
    return xmidi;
}

int XMidi::retrieveMidi(const XMidiFile *xmidi, uint32 track, uint8 **buffer)
{
    if (!xmidi->events)
    {
\tfprintf (stderr, "No midi data in loaded\n");
\treturn 0;
    }

    if (track >= xmidi->tracks)
    {
\tfprintf (stderr, "Can't retrieve MIDI data, track out of range\n");
\treturn 0;
    }

    int len = 14 + convertListToMTrk (NULL, xmidi->events[track]);

    if (len == 14)
    {
\tlen = 0;
\treturn 0;
    }

    uint8* buf = new unsigned char[len];
    *buffer = buf;

    if (!buf)
    {
\tfprintf (stderr, "Unable to allocate output buffer\n");
\treturn 0;
    }

    buf[0] = 'M';
    buf[1] = 'T';
    buf[2] = 'h';
    buf[3] = 'd';

    buf[4] = 0;
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 6;

    buf[8] = 0;
    buf[9] = 0;

    buf[10] = 0;
    buf[11] = 1;

    buf[12] = (unsigned char) (xmidi->timing[track] >> 8);
    buf[13] = (unsigned char) (xmidi->timing[track] & 0xFF);

    convertListToMTrk (buf+14, xmidi->events[track]);

    return len;
}



XMidi::midi_event* XMidi::allocateEvent(int time)
{
    midi_event *event = new midi_event;
    event->status = 0;
    event->next = NULL;
    event->time = time;
    event->stream = NULL;
    event->len = 0;

    return event;
}

/* Sets current to the new event and updates list */
void XMidi::createNewEvent (XMidiFile *xmidi, int time)
{
\tif (!xmidi->list)
\t{
\t\txmidi->list = xmidi->current = allocateEvent(time);
\t\treturn;
\t}

\tif (xmidi->current->time > time)
\t\txmidi->current = xmidi->list;

\twhile (xmidi->current->next)
\t{
\t\tif (xmidi->current->next->time > time)
\t\t{
\t\t\tmidi_event *event = allocateEvent(time);
\t\t\t
\t\t\tevent->next = xmidi->current->next;
\t\t\txmidi->current->next = event;
\t\t\txmidi->current = event;
\t\t\treturn;
\t\t}
\t\t
\t\txmidi->current = xmidi->current->next;
\t}

\txmidi->current->next = allocateEvent(time);
\txmidi->current = xmidi->current->next;
}


//
// Get the MIDI variable-length quantity. A string of 7-bits/byte,
// terminated by a byte not having MSB set..
// Most significant byte first.
// Returns # of bytes extracted from stream.
//
int XMidi::getVLQ (const unsigned char *stream, uint32 *quant)
{
\tint i;
\t*quant = 0;

\tfor (i = 0; i < 4; i++)
\t{
\t\t*quant <<= 7;
\t\t*quant |= stream[i] & 0x7F;

\t\tif (!(stream[i] & 0x80))
\t\t{
\t\t\ti++;
\t\t\tbreak;
\t\t}
\t}

\treturn i;
}

//
// Instead of treating consecutive delta/interval counts as separate counts,
// just sum them up until we hit a MIDI event.
// Returns # of bytes extracted from stream.
//
int XMidi::getVLQ2 (const unsigned char *stream, uint32 *quant)
{
\tint i;
\t*quant = 0;

\tfor (i = 0; i < 4 && !(stream[i] & 0x80); i++)
\t\t*quant += stream[i];

\treturn i;
}

//
// Write a MIDI variable-length quantity (see getVlc) into 'stream'.
// Returns # of bytes used to store 'value'.
// Note: stream can be NULL (useful to count how much space a value would need)
//
int XMidi::putVLQ(unsigned char *stream, uint32 value)
{
\tint buffer;
\tint i = 1;
\tbuffer = value & 0x7F;
\twhile (value >>= 7)
\t{
\t\tbuffer <<= 8;
\t\tbuffer |= ((value & 0x7F) | 0x80);
\t\ti++;
\t}
\tif (!stream) return i;
\twhile (1)
\t{
\t\t*stream++ = (buffer & 0xFF);
\t\tif (buffer & 0x80)
\t\t\tbuffer >>= 8;
\t\telse
\t\t\tbreak;
\t}
\t
\treturn i;
}

/*
// Converts Events
//
// Pointer to stream is at the status byte
// size 1 is single data byte
// size 2 is dual data byte
// size 3 is XMI Note on
// Returns bytes converted
*/

int XMidi::convertEvent (XMidiFile *xmidi, const int time, const unsigned char status, const unsigned char *stream, const int size)
{
\tint i;
\tuint32 delta = 0;
\tmidi_event *prev;

\tcreateNewEvent (xmidi, time);
\txmidi->current->status = status;
\txmidi->current->data[0] = stream[0];

\tif (size == 1) return 1;

\txmidi->current->data[1] = stream[1];

\tif (size == 2) return 2;

\t/* Save old */
\tprev = xmidi->current;

\ti = getVLQ (stream+2, &delta);

\tcreateNewEvent (xmidi, time+delta*3);

\txmidi->current->status = status;
\txmidi->current->data[0] = stream[0];
\txmidi->current->data[1] = 0;
\t
\t/* Change the xmidi->current to the prev */
\txmidi->current = prev;

\treturn i + 2;
}

/* Simple routine to convert system messages */
int XMidi::convertSystemMessage (XMidiFile *xmidi, const int time, const unsigned char *stream)
{
\tint i=1;

\tcreateNewEvent (xmidi, time);
\txmidi->current->status = stream[0];

\t/* Handling of Meta events */
\tif (stream[0] == EV_META)
\t{
\t\txmidi->current->data[0] = stream[1];
\t\ti++;
\t}

\ti += getVLQ (stream+i, &xmidi->current->len);

\tif (!xmidi->current->len) return i;

\txmidi->current->stream = new unsigned char[xmidi->current->len];

\tmemcpy (xmidi->current->stream, stream+i, xmidi->current->len);

\treturn i+xmidi->current->len;
}

/*
// XMidiFile to List
// Returns PPQN 
*/
int XMidi::readEventList (XMidiFile *xmidi, const unsigned char *stream)
{
\tint\ttime = 0;
\tuint32 delta;
\tint\tend = 0;
\tint\ttempo = 500000;
\tint\ttempo_set = 0;

\tint i = 0;

\tfor (i = 0; !end; )
\t{
\t\tswitch (stream[i] >> 4)
\t\t{
\t\t\t/* Note Off */
\t\t\tcase EV_NOTE_OFF >> 4:
\t\t\tfprintf (stderr, "ERROR: Note off not valid in XMidiFile\n");
\t\t\treturn 0;

\t\t\t/* Note On */
\t\t\tcase EV_NOTE_ON >> 4:
\t\t\ti+= 1+convertEvent (xmidi, time, stream[i], stream+i+1, 3);
\t\t\tbreak;

\t\t\t/* 2 byte data */
\t\t\t/* Aftertouch, Controller and Pitch Wheel */
\t\t\tcase EV_POLY_PRESS >> 4:
\t\t\tcase EV_CONTROL >> 4:
\t\t\tcase EV_PITCH >> 4:
\t\t\ti+= 1+convertEvent (xmidi, time, stream[i], stream+i+1, 2);
\t\t\tbreak;

\t\t\t/* 1 byte data */
\t\t\t/* Program Change and Channel Pressure */
\t\t\tcase EV_PROGRAM >> 4:
\t\t\tcase EV_CHAN_PRESS >> 4:
\t\t\ti+= 1+convertEvent (xmidi, time, stream[i], stream+i+1, 1);
\t\t\tbreak;

\t\t\t/* SysEx */
\t\t\tcase EV_SYSEX >> 4:
\t\t\tif (stream[i] == EV_META)
\t\t\t{
\t\t\t\tconst MetaEvent evt = static_cast<MetaEvent>(stream[i+1]);
\t\t\t\tswitch (evt)
\t\t\t\t{
\t\t\t\t\tcase META_OUTPUT_CABLE:
\t\t\t\t\tbreak;
\t\t\t\t\tcase META_TRACK_END: /* End Of Track */
\t\t\t\t\tend = 1;
\t\t\t\t\tbreak;
\t\t\t\t\tcase META_SET_TEMPO:
\t\t\t\t\tif (!tempo_set) /* Tempo. Need it for PPQN */
\t\t\t\t\t{
\t\t\t\t\t\ttempo = stream[i+5];
\t\t\t\t\t\ttempo |= stream[i+4] << 8;
\t\t\t\t\t\ttempo |= stream[i+3] << 16;
\t\t\t\t\t\ttempo_set = 1;
\t\t\t\t\t}
\t\t\t\t\tbreak;
\t\t\t\t\tcase META_TIME_SIG:
\t\t\t\t\t{
\t\t\t\t\t\t//int num = stream[i+2];        // numerator of the time signature
\t\t\t\t\t\t//int denom = 1 << stream[i+2]; // denominator of the time signature
\t\t\t\t\t\t//int qnotes = 4 * num / denom; // number of quarter notes per beat
\t\t\t\t\t}
\t\t\t\t\tbreak;
\t\t\t\t\tcase META_KEYSIG:
\t\t\t\t\t{
\t\t\t\t\t\t// value=# of sharps/flats, major/minor
\t\t\t\t\t}
\t\t\t\t\tbreak;
\t\t\t\t\tdefault:
\t\t\t\t\t// log "unknown meta event"?
\t\t\t\t\tbreak;
\t\t\t\t}
\t\t\t}
\t\t\telse
\t\t\t{
\t\t\t\t// log "unknown sysex event"?
\t\t\t}

\t\t\ti+= convertSystemMessage (xmidi, time, stream+i);
\t\t\tbreak;

\t\t\t/* Delta T, also known as interval count */
\t\t\tdefault:
\t\t\t{
\t\t\tconst int size = getVLQ2 (stream+i, &delta);
\t\t\ti+= size;
\t\t\tdelta *= 3;
\t\t\ttime += delta;
\t\t\t}
\t\t\tbreak;
\t\t}
\t}

\treturn (tempo*9)/25000;
}

//
// stream points to the MIDI events.
// size is the size (in bytes) of this event chunk
//
int XMidi::extractEvents(XMidiFile *xmidi, const unsigned char *stream, const uint32 size)
{
    xmidi->list = NULL;

    /* Convert it */
    signed short ppqn = readEventList (xmidi, stream);
    if (!ppqn)
    {
\tfprintf (stderr, "Unable to convert data\n");
\treturn 0;
    }

    xmidi->timing[xmidi->curr_track] = ppqn;
    xmidi->events[xmidi->curr_track] = xmidi->list;

    ++xmidi->curr_track;

    /* Return how many tracks were converted */
    return 1;
}

int XMidi::readFile (XMidiFile *xmidi, const unsigned char *stream, const uint32 streamsize)
{
    // Partial format specifications at
    // http://rewiki.regengedanken.de/wiki/.IFF
    // http://siedler15.ra-doersch.de/formate/single.html
    //
    // See also the specification "IFF - Interchange File Format"
    // as defined by Electronic Arts (EA) 1985, available in
    // e.g. "AMIGA ROM KERNEL REFERENCE MANUAL".

    uint32 read_so_far = 0;
    IFFchunk chunk;

    xmidi->tracks = 1; // default to 1 track, in case there is no XDIR chunk

    do {
\tbool result;
\tgetIFFchunkHeader(chunk, stream);
\tstream += 8;
\tread_so_far += 8;

\tif (chunk.isType("FORM")) {
\t    result = handleChunkFORM(xmidi, stream, chunk.size);
\t} else
\tif (chunk.isType("CAT ")) {
\t    result = handleChunkCAT(xmidi, stream, chunk.size);
\t} else {
\t    return 0; // We don't know how to handle this
\t}
\tif (!result) {
\t    return 0; // something failed
\t}
\tstream      += chunk.getActualSize();
\tread_so_far += chunk.getActualSize();
    } while (read_so_far < streamsize);

    return 1; // done
}

/*
// Converts an event list to a MTrk
// Returns bytes of the array
// buf can be NULL
*/
uint32 XMidi::convertListToMTrk (unsigned char *buf, const midi_event *mlist)
{
\tint time = 0;
\tuint32\tdelta = 0;
\tunsigned char\tlast_status = 0;
\tuint32 \ti = 8;
\tuint32 \tj;
\tint i_start;

\t/* This is set true to make the song end when an XMidiFile break is hit. */
\tint\tsshock_break = 0;

\tif (buf)
\t{
\t\tbuf[0] = 'M';
\t\tbuf[1] = 'T';
\t\tbuf[2] = 'r';
\t\tbuf[3] = 'k';
\t}

\tfor (const midi_event* event = mlist; event; event=event->next)
\t{
\t\ti_start = i;

\t\t/* If sshock_break is set, the delta is only 0 */
\t\tdelta = sshock_break?0:event->time - time;
\t\ttime = event->time;

\t\tif (buf) i += putVLQ (buf+i, delta);
\t\telse i += putVLQ (NULL, delta);

\t\tif ((event->status != last_status) || (event->status >= EV_SYSEX))
\t\t{
\t\t\tif (buf) buf[i] = event->status;
\t\t\ti++;
\t\t}

\t\tlast_status = event->status;

\t\tswitch (event->status >> 4)
\t\t{
\t\t\t/* 2 bytes data */
\t\t\t/* Note off, Note on, Aftertouch and Pitch Wheel */
\t\t\tcase EV_NOTE_OFF >> 4: // invalid in XMID
\t\t\tcase EV_NOTE_ON >> 4:
\t\t\tcase EV_POLY_PRESS >> 4:
\t\t\tcase EV_PITCH >> 4:
\t\t\tif (buf)
\t\t\t{
\t\t\t\tbuf[i] = event->data[0];
\t\t\t\tbuf[i+1] = event->data[1];
\t\t\t}
\t\t\ti += 2;
\t\t\tbreak;

\t\t\t/* Controller, we need to catch XMIXI Breaks */
\t\t\tcase EV_CONTROL >> 4:
\t\t\tif (buf)
\t\t\t{
\t\t\t\tbuf[i] = event->data[0];
\t\t\t\tbuf[i+1] = event->data[1];
\t\t\t}
\t\t\t/* XMidiFile Break */
\t\t\tif (event->data[0] == XMIDI_CONTROLLER_NEXT_BREAK) sshock_break = 1;
\t\t\ti += 2;
\t\t\tbreak;

\t\t\t/* 1 bytes data */
\t\t\t/* Program Change and Channel Pressure */
\t\t\tcase EV_PROGRAM >> 4:
\t\t\tcase EV_CHAN_PRESS >> 4:
\t\t\tif (buf) buf[i] = event->data[0];
\t\t\ti++;
\t\t\tbreak;
\t\t\t

\t\t\t/* Variable length */
\t\t\t/* SysEx */
\t\t\tcase EV_SYSEX >> 4:
\t\t\tif (event->status == EV_META)
\t\t\t{
\t\t\t\tif (buf) buf[i] = event->data[0];
\t\t\t\ti++;
\t\t\t}
\t
\t\t\tif (buf) i += putVLQ (buf+i, event->len);
\t\t\telse i += putVLQ (NULL, event->len);
\t\t\t
\t\t\tif (event->len)
\t\t\t{
\t\t\t\tfor (j = 0; j < event->len; j++)
\t\t\t\t{
\t\t\t\t\tif (buf) buf[i] = event->stream[j]; 
\t\t\t\t\ti++;
\t\t\t\t}
\t\t\t}

\t\t\tbreak;
\t\t\t

\t\t\t/* Never occur */
\t\t\tdefault:
\t\t\tfprintf (stdout, "Not supposed to see this\n");
\t\t\tbreak;
\t\t}
\t}

\tif (buf)
\t{
\t\tuint32\tsize = i -8;
\t\tbuf[4] = (unsigned char) ((size >> 24) & 0xFF);
\t\tbuf[5] = (unsigned char) ((size >> 16) & 0xFF);
\t\tbuf[6] = (unsigned char) ((size >> 8) & 0xFF);
\t\tbuf[7] = (unsigned char) (size & 0xFF);
\t}

\treturn i;
}

bool XMidi::handleChunkFORM(XMidiFile* xmidi, const unsigned char* stream, uint32 chunksize)
{
    char formType[4];
    memcpy(formType, stream, 4);
    stream += 4;

    if (!memcmp(formType, "XDIR", 4)) {
\treturn handleChunkXDIR(xmidi, stream, chunksize - 4);
    }

    if (!memcmp(formType, "XMID", 4)) {
\treturn handleChunkXMID(xmidi, stream, chunksize - 4);
    }

    return false; // We don't know how to handle this
}

// A "CAT " chunk is a conCATenation of other chunks.
bool XMidi::handleChunkCAT(XMidiFile* xmidi, const unsigned char* stream, uint32 chunksize)
{
    IFFchunk chunk;
    uint32 read_so_far = 0;

    do {
\tgetIFFchunkHeader(chunk, stream);
\t// do NOT increment stream by sizeof(chunk) here, as
\t// e.g. XMID isn't really a chunk but only a data identifier
\t// with no trailing size designator.
\tstream += 4;
\tread_so_far += 4;

\tbool result;

\tif (chunk.isType("FORM")) {
\t    stream += 4;      // adjust for "real" IFF chunk header size
\t    read_so_far += 4; // ditto
\t    result = handleChunkFORM(xmidi, stream, chunk.size);
\t} else
\tif (chunk.isType("XMID")) {
\t    const uint32 xmid_data_len = chunksize - read_so_far;
\t    result = handleChunkXMID(xmidi, stream, xmid_data_len);
\t    chunk.size = xmid_data_len; // "fake" it, for calculation below
\t} else {
\t    return false; // We don't know how to handle this
\t}
\tif (!result) {
\t    return false; // something failed
\t}
\tstream      += chunk.getActualSize();
\tread_so_far += chunk.getActualSize();
    } while (read_so_far < chunksize);

    return true;
}

bool XMidi::handleChunkXDIR(XMidiFile* xmidi, const unsigned char* stream, uint32 chunksize)
{
    if (chunksize != 10 /* "INFO" + 32-bit_size + UWORD */) {
\treturn false;
    }

    IFFchunk chunk;

    getIFFchunkHeader(chunk, stream);
    stream += 8;

    if (!chunk.isType("INFO")) {
\treturn false; // bugger
    }

    if (chunk.size != 2) {
\treturn false; // malformed chunk
    }
    // # of tracks (implies # of FORMs) is an IFF abomination, as
    // it's a little-endian 16-bit value.
    xmidi->tracks = READ_LE_UINT16(stream);

    xmidi->allocData();

    return true;
}

//
// The XMID chunk contains the actual midi data
//
bool XMidi::handleChunkXMID(XMidiFile* xmidi, const unsigned char* stream, uint32 chunksize)
{
    IFFchunk chunk;

    uint32 read_so_far = 0;

    do {
\tbool result;

\tgetIFFchunkHeader(chunk, stream);
\tstream += 8;
\tread_so_far += 8;

\tif (chunk.isType("FORM")) {
\t    result = handleChunkFORM(xmidi, stream, chunk.size);
\t} else
\t    if (chunk.isType("TIMB")) {
\t\tresult = handleChunkTIMB(xmidi, stream, chunk.size);
\t    } else
\t\tif (chunk.isType("EVNT")) {
\t\t    result = handleChunkEVNT(xmidi, stream, chunk.size);
\t\t} else {
\t\t    return false; // We don't know how to handle this
\t\t}
\t\tif (!result) {
\t\t    return 0; // something failed
\t\t}
\t\tstream      += chunk.getActualSize();;
\t\tread_so_far += chunk.getActualSize();
    } while (read_so_far < chunksize);

    return true;
}

//
// Data in the (optional) TIMB chunk is as follow:
//   TIMB<len>
//       UWORD # of timbre list entries, 0-16384
//     { UBYTE patch number 0-127
//       UBYTE timbre bank 0-127 } ...
//
bool XMidi::handleChunkTIMB(XMidiFile* xmidi, const unsigned char* stream, uint32 chunksize)
{
    // The TIMBre chunk contains a list of required timbres.
    // Believe it or not, but the size actually is little-endian
    // (how's that for IFF and compatibility)
    const uint16 timbre_count = READ_LE_UINT16(stream);
    stream += 2;

    // sanity check
    if ((uint32)timbre_count * 2 + 2 != chunksize) {
\t// sanity failure. Log "TIMBre chunk insanity"?
\treturn false;
    }

    typedef XMidiFile::TIMB_ timb_t;
    assert(sizeof(timb_t) == 2);

    timb_t*& t = xmidi->timbres[xmidi->curr_track];

    t = new timb_t[timbre_count];
    memcpy(t, stream, timbre_count * sizeof(*t));
    xmidi->timbre_sizes[xmidi->curr_track] = timbre_count;

    return true;
}

//
// The EVNT chunk. The actual MIDI events.
//
// Data in the EVNT chunk is as follow:
//  EVNT<len>
//    { UBYTE interval count (if < 128) 
//      UBYTE <MIDI event> (if > 127) } ...
//
// <MIDI event> consists of any MIDI Channel Voice, System Exclusive,
// or Meta-event except Note Off.  The MIDI 'running status' convention
// is not used.  Note On events are followed by their duration in intervals,
// expressed as a MIDI variable-length quantity (i.e., a string of 7-bit
// bytes stored most significant byte first, with the high bit set in
// all bytes except the last).
//
bool XMidi::handleChunkEVNT(XMidiFile* xmidi, const unsigned char* stream, uint32 chunksize)
{
    // Set things up and pass the events data to extractEvents

    xmidi->allocData(); // precaution, in case XDIR wasn't found

    int count = extractEvents(xmidi, stream, chunksize);

    if (count != 1)
    {
\tfprintf (stderr, "Error: unable to extract track from XMidiFile.\n");
\treturn false;
    }

    return true;
}

XMidi::XMidiFile::XMidiFile()
:   tracks(1),
    events(0),
    timing(0),
    list(0),
    current(0),
    curr_track(0),
    timbres(0),
    timbre_sizes(0)
{
}


XMidi::XMidiFile::~XMidiFile()
{
    if (events)
    {
\t\tfor (int i=0; i < tracks; i++)
\t\t{
\t\t\tdeleteEventList (events[i]);
\t\t\tdelete[] timbres[i];
\t\t}
\t\tdelete[] events;
    }
    delete[] timing;
    delete[] timbres;
    delete[] timbre_sizes;
}

void XMidi::XMidiFile::allocData()
{
    if (events) {
\treturn;
    }

    events = new midi_event*[tracks];
    timing = new int16[tracks];
    timbres = new TIMB_*[tracks];
    timbre_sizes = new uint16[tracks];

    for (int track = 0; track < tracks; ++track)
    {
\ttiming[track] = 0;
\tevents[track] = NULL;
\ttimbres[track] = 0;
\ttimbre_sizes[track] = 0;
    }
}

void XMidi::XMidiFile::deleteEventList (midi_event *mlist)
{
    midi_event* next = mlist;
    midi_event* event = mlist;

    while ((event = next))
    {
\tnext = event->next;
\tdelete [] event->stream;
\tdelete event;
    }
}

