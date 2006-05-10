<?php

/************************************************************************
 *                                                                      *
 *  FreeSynd - a remake of the classic Bullfrog game "Syndicate".       *
 *                                                                      *
 *   Copyright (C) 2005,2006                                            *
 *      Stuart Bingë  <skbinge@gmail.com>                               *
 *      Joost Peters  <joostp@users.sourceforge.net>                    *
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

// $Id$

$title = 'File Formats'; require_once 'include/common.php'; ?>

<h2>Contents</h2>

<ol>
<li><a href="#synd">Syndicate File Formats</a>
 <ol>
 <li><a href="#synd_tile">Map Tile Graphics</a></li>
 <li><a href="#synd_spr">Sprite Graphics</a></li>
 </ol>
</li>
<li><a href="#swars">Syndicate Wars File Formats</a>
 <ol>
 <li><a href="#swars_spr">Sprite Graphics</a></li>
 </ol>
</li>
</ol>

<hr />

<h2><a name="synd">Syndicate File Formats</a></h2>

<h3><a name="synd_tile">Map Tile Graphics</a></h3>

<h4>Occurrences</h4>
<p>hblk01.dat</p>

<h4>Format</h4>
<p>First, an overview of the tiles. Each map tile in Syndicate is 64 pixels
wide and 48 pixels high. Here are a couple of examples so you can get an idea
of what they look like (the white area is transparent in-game):</p>

<p class="cent_algn"><img src="images/tiles/example1.png"
    alt="Example Tiles (1)" style="width:212px;height:58px;" />
</p>

<p>Of course tiles need not look like those diamond-shaped blocks. Here are a
few more examples of things like piping and fencing:</p>

<p class="cent_algn"><img src="images/tiles/example2.png"
    alt="Example Tiles (2)" style="width:281px;height:58px;" />
</p>

<p>As you can see, tiles in Syndicate are slightly different to those of other
tile-based games, in that they have "height". This means the tiles can be
stacked on top of each other to form taller structures such as walls, guard
posts, lights and even buildings, similar to building bricks.</p>

<p>The tiles are not however stored as 64&times;48 pixel blocks within hblk01.dat;
instead each tile is broken down into six subtiles, each being 32 pixels wide
and 16 pixels high. This was probably done to save space; separate tiles can
reuse common subtiles without duplicating the data within the data file. Here's
the first tile of the above examples split into its six subtiles:</p>

<p class="cent_algn"><img src="images/tiles/subtiles.png"
    alt="Subtile Illustration" style="width:79px;height:67px;" />
</p>

<p>Now on to the data file format. hblk01.dat stores 256 complete tiles where
each tile is composed of 6 subtiles, which works out to 256 * 6 = 1536 subtiles
in total. As I mentioned above, subtiles are reused among several tiles meaning
hblk01.dat does not store all 1536 subtiles, but rather a set of a couple of
hundred unique subtiles.</p>

<p>The first 6144 bytes of hblk01.dat is the offset array. This is a vector of
1536 dwords (6 per tile, one for each subtile, for all of the 256 tiles, stored
in little endian format) where each offset points to the absolute position
within hblk01.dat where the corresponding subtile is stored. If a subtile
offset points to a position below the first 6144 bytes then that subtile is
simply taken to be blank.</p>

<p>The six subtile offsets per tile are taken in order from bottom to top, left
to right, as they lie within the tile. The following picture illustrates this
ordering:</p>

<p class="cent_algn"><img src="images/tiles/subtile-order.png"
    alt="Subtile Ordering" style="width:89px;height:67px;" />
</p>

<p><a name="pixel_blocks">As Syndicate is a 16 color game, only four bits are
needed to represent a pixel. There's the added complexity of transparency
however, meaning an extra bit is needed for every pixel which indicates whether
the pixel is transparent or not. Pixels are usually stored in blocks, grouped
eight per block. This means each block is stored in 5 bytes (8 pixels per block
&times; 5 bits per pixel = 40 bits per block). Pixel data in hblk01.dat is
slightly different though; the blocks instead contain 32 pixels (which works
out to 20 bytes per pixel block); this is also (not coincidently) the width of
a subtile.</a></p>

<p>The way pixel information is stored within these blocks is the only mildy
complicated issue. I suspect the format has a lot to do with the way pixel data
needed to be stored for use with the particular VGA mode that Syndicate used,
although that is (thankfully) ancient history now so I'm not entirely sure.
Perhaps a graphics programmer of yore can inform us as to the accuracy of this
statement.</p>

<p>I'll start with describing the format of the eight pixel blocks. As I said
these are five bytes long; we'll number the bytes from 0 to 4 going from left
to right (where the left position corresponds to the memory position with the
lowest address), and the bits within each byte from 0 to 7, going from right
to left. This numbering scheme should be familiar to anyone who has studied
even rudimentary computer architecture. The scheme looks as follows:</p>

<p class="cent_algn"><img src="images/tiles/bytes.png"
    alt="Byte Ordering" style="width:395px;height:33px;" />
</p>

<p>We'll also number the pixels from 0 to 7 going from left to right. Bit
ordering is the same as the byte scheme, although each pixel is only comprised
of four bits. The transparency bit is "separate" from the color bits. An
illustration should make this more clear:</p>

<p class="cent_algn"><img src="images/tiles/pixels.png"
    alt="Pixel Ordering" style="width:446;height:33px;" />
</p>

<p>Note: The meaning of the transparency bit is opposite to that which one
commonly understands when discussing the subject of transparency as it
relates to such concepts as alpha channels; if a transparency bit is set (i.e.
has the value "1") then the corresponding pixel is in fact transparent; if the
bit is cleared (i.e. has the value "0") the pixel is opaque.</p>

<p>The relationship of the five data bytes to the eight pixels is as follows.
Byte 0 is exclusively transparency information; each bit within this byte
controls whether the corresponding pixel is transparent or not. Note however
that the bit ordering is opposite the numbering scheme here; bit 0 does not
control the transparency of pixel 0, but rather pixel 7. The easiest way to
think of this is to read the bits from left to right in the same way you would
read the pixels from left to right, then simply match the two up.</p>

<p>The remaining four bytes (1 through 4) store the color data. Again, the bit
ordering is reversed, however there's an added twist. Instead of storing each
pixel as a contiguous 4 bit block within each byte (i.e. a byte would hold two
complete pixels), the 4 bits that make up a pixel are instead spread out over
all 4 bytes. So for example, to obtain the color bits for pixel 0 you'd take
bit 7 from each of the 4 color bytes. Bit 7 from byte 1 gives bit 0 of pixel 0,
bit 7 from byte 2 gives bit 1 of pixel 0, etc. I think a picture will
illustrate this better than text; this example is for pixel 2 but it should be
easy to see how the scheme works for the other pixels (note that the coloring
is purely to highlight which line goes where):</p>

<p class="cent_algn"><img src="images/tiles/pixel2.png"
    alt="Bit Ordering for Pixel 2" style="width:395px;height:112px;" />
</p>

<p>On to the 32 pixel blocks. As I've said these take up 20 bytes (32
transparency bits + 32 * 4 color bits), however instead of storing these 32
pixel blocks as four distinct 8-pixel blocks in the format described above, all
32 pixels are instead represented using the same scheme as above. This means
the first 4 bytes give the transparency information for all 32 pixels, and each
block of 4 bytes after that holds a single bit from each of the 32 pixels. So
for example, to obtain pixel 0 (assuming the same byte and pixel numbering
scheme as above) you'd take bit 7 of byte 0 for its transparency bit, bit 7 of
byte 4 for its bit 0, bit 7 of byte 8 for its bit 1, etc. To obtain pixel 31
you'd take bit 0 of byte 3 for its transparency, bit 0 of byte 7 for its bit 0,
bit 0 of byte 11 for its bit 1, etc.</p>

<p>Back to the tile format. Each subtile within the data file is simply stored
as sixteen contiguous 32-pixel blocks in the format I've just described, with
one block per line of the subtile. Note that the lines of a subtile are in
bottom-up order (i.e. the first line read is the last line of the subtile,
assuming the lines of the subtile are numbered from 0 to 15, top to bottom).
And that's all there is to hblk01.dat.</p>

<h3><a name="synd_spr">Sprite Graphics</a></h3>

<h4>Occurrences</h4>
<p>hpointer.dat/hpointer.tab; hspr-?-d.dat/hspr-?-d.tab;
mfnt-0.dat/mfnt-0.tab; mspr-0-d.dat/mspr-0-d.tab</p>

<h4>Format</h4>
<p>The various sprite files make heavy use of the <a
href="#pixel_blocks">eight-pixel blocks</a> as described in the tile section.
If you haven't read that yet then you should do so now or else you won't be
able to understand the following sections.</p>

<p>Each sprite group is comprised of two files; a .dat file that contains the
actual pixel data of the sprites and a .tab file that is used to index into
the .dat file in order to find the relevant pixel data.</p>

<p>The .tab files are simply a continuous collection of the following
structures, which we'll refer to as tab entries:</p>

<pre>struct TabEntry {
    Uint32  offset;
    Uint8   width;
    Uint8   height;
};</pre>

<br />

<p>The offset field is in little endian format, and gives the absolute position
in the .dat file where the pixel data for the sprite is located. The width
and height fields obviously give the dimensions of the relevant sprite.</p>

<p>Each sprite is stored as a sequence of pixel lines, with the lines in bottom
to top order as read in sequentially from the .dat file, similar to the lines
of each subtile within hblk01.dat. Each line within the sprite is comprised of
the smallest possible number of complete 8-pixel blocks that can represent the
entire width of the sprite; as such if the width of a sprite is not a multiple
of eight there will be extra "padding" pixels within the last pixel block of
every line that are not considered part of the sprite, and are usually
transparent.</p>

<hr />

<h2><a name="swars">Syndicate Wars File Formats</a></h2>

<h3><a name="swars_spr">Sprite Graphics</a></h3>

<h4>Occurrences</h4>
<p>TODO</p>

<h4>Format</h4>
<p>The sprite groups in Syndicate Wars are similar to those used in Syndicate,
with a .tab file indexing into a .dat file containing the actual pixel data.
The format of the .tab files is exactly the same as <a href="#synd_spr">those
used by Syndicate</a>, so I won't repeat myself by describing it here.</p>

<p>In the same way that pixel lines within Syndicate sprites are stored in
bottom to top order, so too are pixel lines within Syndicate Wars sprites. The
difference between the two formats, however, is the way in which the pixel data
is represented.</p>

<p>Pixel data within the sprite files is stored using a RLE scheme, however
only runs of transparent pixels are actually compressed; pixel data is
represented verbatim. The simplest way to describe this would probably be to
give the RLE algorithm in psuedo-code:</p>

<pre>Uint8 * dat_data, * pixel_data;
TabEntry sprite;

// Assume sprite has been populated by reading from a .tab file...

// Assume that pixel_data points to the beginning of the sprite graphic (of
//   dimensions sprite.width * sprite.height) that we're reading into, and that
//   dat_data points to the beginning of the .dat file data...

// Assume a color index of zero indicates transparency...

Uint8 * sprite_data = dat_data + tab_entry.offset;

for (int i = 0; i &lt; sprite.height; ++i) {
    Uint8 * current_pixel = pixel_data + (sprite.height - 1 - i) * sprite.width;

    Sint8 run_length = *sprite_data++;
    while (run_length != 0) {
        if (run_length > 0) {               // pixel run

            for (int j = 0; j &lt; run_length; ++j)
                *current_pixel++ = *sprite_data++;

        } else if (run_length &lt; 0) {     // transparent run

            run_length *= -1;
            memset(current_pixel, 0, run_length);
            current_pixel += run_length;

        } else if (run_length == 0) {       // end of the row
        }

        run_length = *sprite_data++;
    }
}
</pre>

<?php printFooter(); ?>
