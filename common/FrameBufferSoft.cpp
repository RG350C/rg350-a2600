//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: FrameBufferSoft.cxx,v 1.52 2006/03/25 00:34:17 stephena Exp $
//============================================================================

#include <SDL/SDL.h>
#include <sstream>

#include "Atari.h"
#include "psp_sdl.h"

#include "Console.hxx"
#include "FrameBuffer.hxx"
#include "FrameBufferSoft.hxx"
#include "MediaSrc.hxx"
#include "Settings.hxx"
#include "OSystem.hxx"
# if 0 //LUDO:
#include "Font.hxx"
# endif
#include "GuiUtils.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FrameBufferSoft::FrameBufferSoft(OSystem* osystem)
  : FrameBuffer(osystem),
    myRectList(NULL),
    myOverlayRectList(NULL),
    myRenderType(kSoftZoom)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FrameBufferSoft::~FrameBufferSoft()
{
  delete myRectList;
  delete myOverlayRectList;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FrameBufferSoft::initSubsystem()
{
  // Set up the rectangle list to be used in the dirty update
  delete myRectList;
  myRectList = new RectList();
  delete myOverlayRectList;
  myOverlayRectList = new RectList();

  if(!myRectList || !myOverlayRectList)
  {
    cerr << "ERROR: Unable to get memory for SDL rects" << endl;
    return false;
  }

  // Create the screen
  if(!createScreen())
    return false;

  // Show some info
# if 0 //LUDO:
  if(myOSystem->settings().getBool("showinfo"))
    cout << "Video rendering: Software mode" << endl << endl;
# endif

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::setAspectRatio()
{
  // Aspect ratio correction not yet available in software mode
  theAspectRatio = 1.0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FrameBufferSoft::createScreen()
{
  myScreenDim.x = myScreenDim.y = 0;

  myScreenDim.w = myBaseDim.w * theZoomLevel;
  myScreenDim.h = myBaseDim.h * theZoomLevel;

  // In software mode, the image and screen dimensions are always the same
  myImageDim = myScreenDim;

# if 0 //LUDO:
  myScreen = SDL_SetVideoMode(myScreenDim.w, myScreenDim.h, 0, mySDLFlags);
  if(myScreen == NULL)
  {
    cerr << "ERROR: Unable to open SDL window: " << SDL_GetError() << endl;
    return false;
  }
# else
  extern SDL_Surface *back_surface;
  mySDLFlags = SDL_HWSURFACE;
  myScreenDim.w = myDesktopDim.w;
  myScreenDim.h = myDesktopDim.w;
  myScreen = back_surface;
  switch(myScreen->format->BitsPerPixel)
  {
    case 16:
      myPitch = myScreen->pitch/2;
      break;
    case 24:
      myPitch = myScreen->pitch;
      break;
    case 32:
      myPitch = myScreen->pitch/4;
      break;
  }
# endif

  myOSystem->eventHandler().refreshDisplay();

  return true;
}


//LUDO:
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

/* 

  LUDO: 16-bit HiColor (565 format) 
  see http://www.compuphase.com/graphic/scale3.htm

 */
static inline word loc_coloraverage(word a, word b)
{
  return (word)(((a ^ b) & 0xf7deU) >> 1) + (a & b);
}

static inline void 
render16bpp_X15_pixel(word *dist, const byte *b_src, Uint32* my_palette)
{
  word src[4];

  src[0] = my_palette[b_src[0]];
  src[1] = my_palette[b_src[1]];
  src[2] = my_palette[b_src[2]];
  src[3] = my_palette[b_src[3]];

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = loc_coloraverage(src[2], src[3]);
  dist[9] = dist[8];

  dist[10] = src[3];
  dist[11] = dist[10];
}

static inline void 
render16bpp_X15_pixelSimple(word *dist, const byte *b_src, const byte *b_prev, Uint32* my_palette)
{
  word src[4];

  src[0] = my_palette[b_src[0] ? b_src[0] : b_prev[0] ];
  src[1] = my_palette[b_src[1] ? b_src[1] : b_prev[1] ];
  src[2] = my_palette[b_src[2] ? b_src[2] : b_prev[2] ];
  src[3] = my_palette[b_src[3] ? b_src[3] : b_prev[3] ];

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = loc_coloraverage(src[2], src[3]);
  dist[9] = dist[8];

  dist[10] = src[3];
  dist[11] = dist[10];
}

static inline void 
render16bpp_X15_pixelPhos(word *dist, const byte *b_src, const byte *b_prev, Uint32* my_palette)
{
  word src[4];

  src[0] = my_palette[(b_src[0] << 8) + b_prev[0]];
  src[1] = my_palette[(b_src[1] << 8) + b_prev[1]];
  src[2] = my_palette[(b_src[2] << 8) + b_prev[2]];
  src[3] = my_palette[(b_src[3] << 8) + b_prev[3]];

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = loc_coloraverage(src[2], src[3]);
  dist[9] = dist[8];

  dist[10] = src[3];
  dist[11] = dist[10];
}

static inline void 
render16bpp_X15_pixelAvg(word *dist, const byte *b_src, const byte *b_prev, Uint32* my_palette)
{
  word src[4];

  src[0] = loc_coloraverage(my_palette[b_src[0]], my_palette[b_prev[0]]);
  src[1] = loc_coloraverage(my_palette[b_src[1]], my_palette[b_prev[1]]);
  src[2] = loc_coloraverage(my_palette[b_src[2]], my_palette[b_prev[2]]);
  src[3] = loc_coloraverage(my_palette[b_src[3]], my_palette[b_prev[3]]);

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = loc_coloraverage(src[2], src[3]);
  dist[9] = dist[8];

  dist[10] = src[3];
  dist[11] = dist[10];
}

static inline void 
render16bpp_X125_pixel(word *dist, const byte *b_src, Uint32* my_palette)
{
  word src[4];

  src[0] = my_palette[b_src[0]];
  src[1] = my_palette[b_src[1]];
  src[2] = my_palette[b_src[2]];
  src[3] = my_palette[b_src[3]];

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = src[3];
  dist[9] = dist[8];
}

static inline void 
render16bpp_X125_pixelSimple(word *dist, const byte *b_src, const byte *b_prev, Uint32* my_palette)
{
  word src[4];

  src[0] = my_palette[b_src[0] ? b_src[0] : b_prev[0] ];
  src[1] = my_palette[b_src[1] ? b_src[1] : b_prev[1] ];
  src[2] = my_palette[b_src[2] ? b_src[2] : b_prev[2] ];
  src[3] = my_palette[b_src[3] ? b_src[3] : b_prev[3] ];

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = src[3];
  dist[9] = dist[8];
}

static inline void 
render16bpp_X125_pixelPhos(word *dist, const byte *b_src, const byte *b_prev, Uint32* my_palette)
{
  word src[4];

  src[0] = my_palette[(b_src[0] << 8) + b_prev[0]];
  src[1] = my_palette[(b_src[1] << 8) + b_prev[1]];
  src[2] = my_palette[(b_src[2] << 8) + b_prev[2]];
  src[3] = my_palette[(b_src[3] << 8) + b_prev[3]];

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = src[3];
  dist[9] = dist[8];
}

static inline void 
render16bpp_X125_pixelAvg(word *dist, const byte *b_src, const byte *b_prev, Uint32* my_palette)
{
  word src[4];

  src[0] = loc_coloraverage(my_palette[b_src[0]], my_palette[b_prev[0]]);
  src[1] = loc_coloraverage(my_palette[b_src[1]], my_palette[b_prev[1]]);
  src[2] = loc_coloraverage(my_palette[b_src[2]], my_palette[b_prev[2]]);
  src[3] = loc_coloraverage(my_palette[b_src[3]], my_palette[b_prev[3]]);

  dist[0] = src[0];
  dist[1] = dist[0];

  dist[2] = loc_coloraverage(src[0], src[1]);
  dist[3] = dist[2];

  dist[4] = src[1];
  dist[5] = dist[4];

  dist[6] = src[2];
  dist[7] = dist[6];

  dist[8] = src[3];
  dist[9] = dist[8];
}



void
FrameBufferSoft::drawMediaSourceBlitSurface(uInt32 pitch, uInt16* buffer)
{
  MediaSource& mediasrc = myOSystem->console().mediaSource();
  uInt8* currentFrame   = mediasrc.currentFrameBuffer();
  uInt8* previousFrame  = mediasrc.previousFrameBuffer();

  /* w 160 x h 210 */
  uInt32 width  = mediasrc.width();
  uInt32 height = mediasrc.height();

  uInt32 y;

  if (ATARI.atari_flicker_mode && previousFrame) {

    for(y = 0; y < height; ++y )
    {
      const uInt32 bufofsY    = y * width;
      const uInt32 screenofsY = y * pitch;

      if (ATARI.atari_flicker_mode == ATARI_FLICKER_PHOSPHOR) {

        uInt32 x;
        for(x = 0; x < width; ++x )
        {
          const uInt32 off = screenofsY + (x << 1);
          byte v = currentFrame[bufofsY + x];
          byte w = previousFrame[bufofsY + x];
          buffer[off] = buffer[off+1] = myAvgPalette[v][w];
        }

      } else
      if (ATARI.atari_flicker_mode == ATARI_FLICKER_AVERAGE) {

        uInt32 x;
        for(x = 0; x < width; ++x )
        {
          const uInt32 off = screenofsY + (x << 1);
          byte v = currentFrame[bufofsY + x];
          byte w = previousFrame[bufofsY + x];
          buffer[off] = buffer[off+1] = loc_coloraverage(myDefPalette[v], myDefPalette[w]);
        }
      } else { /* Simple */

        uInt32 x;
        for(x = 0; x < width; ++x )
        {
          const uInt32 off = screenofsY + (x << 1);
          byte color = currentFrame[bufofsY + x];
          if (! color) color = previousFrame[bufofsY + x];
          buffer[off] = buffer[off+1] = myDefPalette[color];
        }
      }

    }

  } else {

    for(y = 0; y < height; ++y )
    {
      const uInt32 bufofsY    = y * width;
      const uInt32 screenofsY = y * pitch;

      uInt32 x;
      for(x = 0; x < width; ++x )
      {
        const uInt32 off = screenofsY + (x << 1);
        byte color = currentFrame[bufofsY + x];
        buffer[off] = buffer[off+1] = myDefPalette[color];
      }
    }
  }

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//LUDO:
void 
FrameBufferSoft::drawMediaSourceNormal()
{
  MediaSource& mediasrc = myOSystem->console().mediaSource();
  uInt8* currentFrame   = mediasrc.currentFrameBuffer();
  uInt8* previousFrame  = mediasrc.previousFrameBuffer();

  uInt32 pitch = 0;
  uInt16* buffer;

  uInt32 width  = mediasrc.width() * 2;
  uInt32 height = mediasrc.height();

  int delta_x = (PSP_SDL_SCREEN_WIDTH -  width) / 2;
  int delta_y = (PSP_SDL_SCREEN_HEIGHT - height) / 2;
  if (delta_x < 0) delta_x = 0;
  if (delta_y < 0) delta_y = 0;

  buffer = (uInt16*)psp_sdl_get_vram_addr(delta_x, delta_y);
  pitch  = PSP_LINE_SIZE;

  drawMediaSourceBlitSurface(pitch, buffer);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::preFrameUpdate()
{
# if 0 //LUDO:
  // Start a new rectlist on each display update
  myRectList->start();

  // Add all previous overlay rects, then erase
  SDL_Rect* dirtyOverlayRects = myOverlayRectList->rects();
  for(unsigned int i = 0; i < myOverlayRectList->numRects(); ++i)
    myRectList->add(&dirtyOverlayRects[i]);
  myOverlayRectList->start();
# endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::postFrameUpdate()
{
# if 0 //LUDO:
  // This is a performance hack until I have more time to work
  // on the Win32 code.  It seems that SDL_UpdateRects() is very
  // expensive in Windows, so we force a full screen update instead.
  if(myUseDirtyRects)
    SDL_UpdateRects(myScreen, myRectList->numRects(), myRectList->rects());
  else if(myRectList->numRects() > 0)
  {
    SDL_Flip(myScreen);
    myRectList->start();
  }
# endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void 
FrameBufferSoft::scanline(uInt32 row, uInt8* data)
{
  // Make sure no pixels are being modified
  SDL_LockSurface(myScreen);

  uInt32 bpp     = myScreen->format->BytesPerPixel;
  uInt8* start   = (uInt8*) myScreen->pixels;
  uInt32 yoffset = row * myScreen->pitch;
  uInt32 pixel = 0;
  uInt8 *p, r, g, b;

  for(Int32 x = 0; x < myScreen->w; x++)
  {
    p = (Uint8*) (start    +  // Start at top of RAM
                 (yoffset) +  // Go down 'row' lines
                 (x * bpp));  // Go in 'x' pixels

    switch(bpp)
    {
      case 1:
        pixel = *p;
        break;

      case 2:
        pixel = *(Uint16*) p;
        break;

      case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
          pixel = p[0] << 16 | p[1] << 8 | p[2];
        else
          pixel = p[0] | p[1] << 8 | p[2] << 16;
        break;

      case 4:
        pixel = *(Uint32*) p;
        break;
    }

    SDL_GetRGB(pixel, myScreen->format, &r, &g, &b);

    data[x * 3 + 0] = r;
    data[x * 3 + 1] = g;
    data[x * 3 + 2] = b;
  }

  SDL_UnlockSurface(myScreen);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::toggleFilter()
{
  // No filter added yet ...
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::hLine(uInt32 x, uInt32 y, uInt32 x2, int color)
{
  SDL_Rect tmp;

  // Horizontal line
  tmp.x = x * theZoomLevel;
  tmp.y = y * theZoomLevel;
  tmp.w = (x2 - x + 1) * theZoomLevel;
  tmp.h = theZoomLevel;
  SDL_FillRect(myScreen, &tmp, myDefPalette[color]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::vLine(uInt32 x, uInt32 y, uInt32 y2, int color)
{
  SDL_Rect tmp;

  // Vertical line
  tmp.x = x * theZoomLevel;
  tmp.y = y * theZoomLevel;
  tmp.w = theZoomLevel;
  tmp.h = (y2 - y + 1) * theZoomLevel;
  SDL_FillRect(myScreen, &tmp, myDefPalette[color]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::fillRect(uInt32 x, uInt32 y, uInt32 w, uInt32 h,
                               int color)
{
  SDL_Rect tmp;

  // Fill the rectangle
  tmp.x = x * theZoomLevel;
  tmp.y = y * theZoomLevel;
  tmp.w = w * theZoomLevel;
  tmp.h = h * theZoomLevel;
  SDL_FillRect(myScreen, &tmp, myDefPalette[color]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# if 0 //LUDO:
void FrameBufferSoft::drawChar(const GUI::Font* FONT, uInt8 chr,
                               uInt32 xorig, uInt32 yorig, int color)
{
  GUI::Font* font = (GUI::Font*)FONT;
  const FontDesc& desc = font->desc();

  // If this character is not included in the font, use the default char.
  if(chr < desc.firstchar || chr >= desc.firstchar + desc.size)
  {
    if (chr == ' ')
      return;
    chr = desc.defaultchar;
  }

  const Int32 w = font->getCharWidth(chr);
  const Int32 h = font->getFontHeight();
  chr -= desc.firstchar;
  const uInt16* tmp = desc.bits + (desc.offset ? desc.offset[chr] : (chr * h));

  SDL_Rect rect;
  for(int y = 0; y < h; y++)
  {
    const uInt16 buffer = *tmp++;
    uInt16 mask = 0x8000;

    for(int x = 0; x < w; x++, mask >>= 1)
    {
      if ((buffer & mask) != 0)
      {
        rect.x = (x + xorig) * theZoomLevel;
        rect.y = (y + yorig) * theZoomLevel;
        rect.w = rect.h = theZoomLevel;
        SDL_FillRect(myScreen, &rect, myDefPalette[color]);
      }
    }
  }
}
# endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::drawBitmap(uInt32* bitmap, Int32 xorig, Int32 yorig,
                                 int color, Int32 h)
{
  SDL_Rect rect;
  for(int y = 0; y < h; y++)
  {
    uInt32 mask = 0xF0000000;

    for(int x = 0; x < 8; x++, mask >>= 4)
    {
      if(bitmap[y] & mask)
      {
        rect.x = (x + xorig) * theZoomLevel;
        rect.y = (y + yorig) * theZoomLevel;
        rect.w = rect.h = theZoomLevel;
        SDL_FillRect(myScreen, &rect, myDefPalette[color]);
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::translateCoords(Int32* x, Int32* y)
{
  // We don't bother checking offsets or aspect ratios, since
  // they're not yet supported in software mode.
  *x /= theZoomLevel;
  *y /= theZoomLevel;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::addDirtyRect(uInt32 x, uInt32 y, uInt32 w, uInt32 h)
{
  x *= theZoomLevel;
  y *= theZoomLevel;
  w *= theZoomLevel;
  h *= theZoomLevel;

  // Check if rect is in screen area
  // This is probably a bug, since the GUI code shouldn't be setting
  // a dirty rect larger than the screen
  int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
  int sx1 = myScreenDim.x, sy1 = myScreenDim.y,
      sx2 = myScreenDim.x + myScreenDim.w, sy2 = myScreenDim.y + myScreenDim.h;
  if(x1 < sx1 || y1 < sy1 || x2 > sx2 || y2 > sy2)
    return;

  // Add a dirty rect to the overlay rectangle list
  // They will actually be added to the main rectlist in preFrameUpdate()
  // TODO - intelligent merging of rectangles, to avoid overlap
  SDL_Rect temp;
  temp.x = x;
  temp.y = y;
  temp.w = w;
  temp.h = h;

  myOverlayRectList->add(&temp);

//  cerr << "addDirtyRect():  "
//       << "x=" << temp.x << ", y=" << temp.y << ", w=" << temp.w << ", h=" << temp.h << endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::enablePhosphor(bool enable)
{
# if 0 //LUDO:
  myUsePhosphor   = enable;
  myPhosphorBlend = myOSystem->settings().getInt("ppblend");

  if(myUsePhosphor)
  {
    switch(myScreen->format->BitsPerPixel)
    {
      case 16:
        myPitch      = myScreen->pitch/2;
        myRenderType = kPhosphor_16;
        break;
      case 24:
        myPitch      = myScreen->pitch;
        myRenderType = kPhosphor_24;
        break;
      case 32:
        myPitch      = myScreen->pitch/4;
        myRenderType = kPhosphor_32;
        break;
      default:
        myRenderType = kSoftZoom; // What else should we do here?
        break;
    }
  }
  else
    myRenderType = kSoftZoom;
# endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FrameBufferSoft::cls()
{
  if(myScreen)
  {
    SDL_FillRect(myScreen, NULL, 0);
    SDL_UpdateRect(myScreen, 0, 0, 0, 0);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RectList::RectList(Uint32 size)
{
  currentSize = size;
  currentRect = 0;

  rectArray = new SDL_Rect[currentSize];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RectList::~RectList()
{
  delete[] rectArray;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RectList::add(SDL_Rect* newRect)
{
  if(currentRect >= currentSize)
  {
    currentSize = currentSize * 2;
    SDL_Rect *temp = new SDL_Rect[currentSize];

    for(Uint32 i = 0; i < currentRect; ++i)
      temp[i] = rectArray[i];

    delete[] rectArray;
    rectArray = temp;
  }

//cerr << "RectList::add():  "
//     << "x=" << newRect->x << ", y=" << newRect->y << ", w=" << newRect->w << ", h=" << newRect->h << endl;

  rectArray[currentRect].x = newRect->x;
  rectArray[currentRect].y = newRect->y;
  rectArray[currentRect].w = newRect->w;
  rectArray[currentRect].h = newRect->h;

  ++currentRect;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SDL_Rect* RectList::rects()
{
  return rectArray;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Uint32 RectList::numRects()
{
  return currentRect;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RectList::start()
{
  currentRect = 0;
}
