// $Id: test_compression.cpp,v 1.1.2.1 2001-09-30 08:18:15 dmazzoni Exp $

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream.h>
#include <id3/tag.h>

int main( int argc, char *argv[])
{
  ID3D_INIT_DOUT();
  ID3D_INIT_WARNING();
  ID3D_INIT_NOTICE();

  ID3_Tag tag;
  ID3_Frame frame;
  
  tag.Link("test-230-compressed.tag");
  tag.Strip(ID3TT_ALL);
  tag.Clear();
  
  frame.SetID(ID3FID_USERTEXT);
  frame.GetField(ID3FN_DESCRIPTION)->Set("compression example");
  frame.GetField(ID3FN_TEXT)->Set("This sample user text frame came from an ID3v2-3.0 tag.  The frame has the 'compression' bit set in it's frame header.  This is the new method for compressing frames, which supercedes the 2.01 Compressed Data Metaframe.");
  frame.SetCompression(true);
  tag.AddFrame(frame);
  
  tag.SetPadding(false);
  tag.SetUnsync(false);
  tag.Update(ID3TT_ID3V2);

  return 0;
}
