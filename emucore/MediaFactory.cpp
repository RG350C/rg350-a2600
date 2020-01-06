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
// $Id: MediaFactory.cxx,v 1.4 2006/02/05 19:59:22 stephena Exp $
//============================================================================

////////////////////////////////////////////////////////////////////
// I think you can see why this mess was put into a factory class :)
////////////////////////////////////////////////////////////////////

#include "MediaFactory.hxx"

#include "OSystem.hxx"

#include "FrameBuffer.hxx"
#include "FrameBufferSoft.hxx"
#ifdef DISPLAY_OPENGL
  #include "FrameBufferGL.hxx"
#endif

#if defined(PSP)
  #include "FrameBufferPSP.hxx"
#elif defined (_WIN32_WCE)
  #include "FrameBufferWinCE.hxx"
#endif

#include "Sound.hxx"
#include "SoundNull.hxx"
#ifdef SOUND_SUPPORT
  #ifndef _WIN32_WCE
    #include "SoundSDL.hxx"
  #else
    #include "SoundWinCE.hxx"
  #endif
#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FrameBuffer* MediaFactory::createVideo(OSystem* osystem)
{
  FrameBuffer* fb = (FrameBuffer*) NULL;

  // OpenGL mode *may* fail, so we check for it first
#ifdef DISPLAY_OPENGL
  if(osystem->settings().getString("video") == "gl")
  {
    const string& gl_lib = osystem->settings().getString("gl_lib");
    if(FrameBufferGL::loadFuncs(gl_lib))
      fb = new FrameBufferGL(osystem);
  }
#endif

  // If OpenGL failed, or if it wasn't requested, create the appropriate
  // software framebuffer
  if(!fb)
  {
# if 0 //LUDO:
   #if defined (PSP)
    fb = new FrameBufferPSP(osystem);
   #elif defined (_WIN32_WCE)
    fb = new FrameBufferWinCE(osystem);
   #else
    fb = new FrameBufferSoft(osystem);
   #endif
# endif
    fb = new FrameBufferSoft(osystem);
  }

# if 0 //LUDO:
  // This should never happen
  assert(fb != NULL);
  switch(fb->type())
  {
    case kSoftBuffer:
      osystem->settings().setString("video", "soft");
      break;

    case kGLBuffer:
      osystem->settings().setString("video", "gl");
      break;
  }
# else
  osystem->settings().setString("video", "soft");
# endif

  return fb;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Sound* MediaFactory::createAudio(OSystem* osystem)
{
  Sound* sound = (Sound*) NULL;

#ifdef SOUND_SUPPORT
  #if defined (_WIN32_WCE)
    sound = new SoundWinCE(osystem);
  #else
    sound = new SoundSDL(osystem);
  #endif
#else
  sound = new SoundNull(osystem);
#endif

  return sound;
}
