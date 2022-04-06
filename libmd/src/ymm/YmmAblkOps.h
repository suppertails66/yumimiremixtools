#ifndef YMM_ABLK_OPS_H
#define YMM_ABLK_OPS_H


#include "ymm/YmmAblkOp.h"

namespace Md {


/*

  Op parameter strings work as follows:
    
    b     A byte (8 bits).
    w     A word (16 bits).
    l     A long (32 bits).
    *     A PARAMWORD (16 bits). If top bit is not set, this is a literal;
          otherwise, the low byte is an index into a runtime word array.
    ^     A 0x80 byte should exist at this position.
    []    Enclosed sequence is optional. Whether the sequence exists or not,
          the byte 0x80 must immediately follow it.
    <>    Enclosed sequence is optional, and repeats until the byte 0x80 is
          reached.
    !     Must be specially handled by user.
    // NEW FOR TRANSLATION
    &     A 16-bit signed relative offset.
          Destination is relative to the offset's address in the data.
    
*/

const YmmAblkOp ymmAblkOps[] = {
  YmmAblkOp(0x00, "END",
            ""),
  YmmAblkOp(0x01, "op01",
            "b"),
  YmmAblkOp(0x02, "op02",
            "w!"),
  YmmAblkOp(0x03, "draw",
            "b"),
  YmmAblkOp(0x04, "setPaletteColors",
            "b<w>"),
  YmmAblkOp(0x05, "op05",
            "bb<bb>"),
  YmmAblkOp(0x06, "renderObjsLow?",
            "<b>"),
  YmmAblkOp(0x07, "loadFgQuadrant",
            "bb"),
  YmmAblkOp(0x08, "changeFgTilemapOrNop",
            "[b]"),
  YmmAblkOp(0x09, "changeFgTilemap",
            "[b]"),
  YmmAblkOp(0x0A, "renderObjsHigh?",
            "<b>"),
  YmmAblkOp(0x0B, "op0B",
            "wbb<b><<w>>"),
  YmmAblkOp(0x0C, "setScrollX?",
            "**[wwww]"),
  YmmAblkOp(0x0D, "setScrollY?",
            "**[wwww]"),
  YmmAblkOp(0x0E, "setScroll",
            "**[wwww]"),
  YmmAblkOp(0x0F, "op0F",
            "b"),
  YmmAblkOp(0x10, "transferPatternData",
            ""),
  YmmAblkOp(0x11, "scrollFg",
            "**[ww]"),
  YmmAblkOp(0x12, "scrollBg",
            "**[ww]"),
  YmmAblkOp(0x13, "scrollFgBg",
            "**[ww]"),
  YmmAblkOp(0x14, "setLoopStart",
            "b"),
  YmmAblkOp(0x15, "triggerLoop",
            ""),
  YmmAblkOp(0x16, "op16_mcpu34",
            "**[ww]"),
  // note: does nothing, but is used in some scenes
  YmmAblkOp(0x17, "nop_17",
            ""),
  YmmAblkOp(0x18, "renderSpriteOverride?",
            "bb**"),
  YmmAblkOp(0x19, "setPaletteColors_19",
            "b<w>"),
  YmmAblkOp(0x1A, "playCdAudio?",
            "b"),
  YmmAblkOp(0x1B, "waitForCdd",
            "www"),
  YmmAblkOp(0x1C, "playPcm?",
            "b"),
  YmmAblkOp(0x1D, "waitForPcm?",
            ""),
  YmmAblkOp(0x1E, "writeWord",
            "b*"),
  YmmAblkOp(0x1F, "addWord",
            "b*"),
  YmmAblkOp(0x20, "advanceSeq",
            "bw[w]"),
  YmmAblkOp(0x21, "createSeq",
            "<l>^"),
  YmmAblkOp(0x22, "playSe??",
            "b^"),
  YmmAblkOp(0x23, "playFm??",
            "b"),
  // note: does nothing and is never used
  YmmAblkOp(0x24, "nop_24",
            ""),
  YmmAblkOp(0x25, "op25",
            "www"),
  YmmAblkOp(0x26, "op26",
            "ww"),
  // NEW FOR TRANSLATION
  YmmAblkOp(0x40, "branch",
            // param 1 = jump offset
            "&"),
  YmmAblkOp(0x41, "branchIfNotEq",
            // param 1 = value to check
            // param 2 = value to check param1 against
            // param 3 = jump offset if not equal
            "**&"),
};

const YmmAblkOp ymmAblkOp02Subops[] = {
  YmmAblkOp(0x00, "op02_00",
            ""),
  YmmAblkOp(0x01, "op02_01",
            "wwwww"),
  
  // leaving this commented out because, while implemented, it doesn't seem
  // to be used anywhere in the game.
  // when called, it uses the parameter as an index into a hardcoded
  // string table and triggers streaming playback of one of several special
  // audio files on the disc (P***.MKP).
  // these contain higher-quality versions of the character intro scenes,
  // with baked-in background music.
  // only two of the four files in the table actually exist on the disc,
  // which points to this being an abandoned feature.
  // maybe it was used in one of the other ports?
//  YmmAblkOp(0x03, "op02_03",
//            "w"),
  
  YmmAblkOp(0x04, "op02_04",
            "w*w"),
  // FIXME: i'm not sure about this one.
  // it's used to trigger hardware-based zoom effects,
  // but in the original game, i believe it's used only in a single scene
  // in the intro (as a space-saving measure); in other situations where
  // the screen appears to "zoom", it's actually faked using premade graphics.
  // i'm guessing the mcd's graphics coprocessor isn't fast enough to do a
  // full-screen zoom effect for the 320px resolution -- sonic cd had to
  // drop to low-res mode for the special stages, after all.
  // on the other hand, the saturn version uses it for all(?) such effects,
  // e.g. in C149.DAT, which originally had "fake" zoom, since obviously
  // the hardware limitations are no longer an issue.
  // in any case, the new calls to this in the saturn version seem to
  // indicate the parameter is optional, but i haven't checked closely.
  // hopefully the interface is the same between versions.
  YmmAblkOp(0x05, "op02_05",
            "[w]"),
  // FIXME: see above
  YmmAblkOp(0x06, "op02_06",
//            "ww"),
            "<w>"),
  YmmAblkOp(0x07, "op02_07",
            "ww"),
  YmmAblkOp(0x09, "op02_09",
            "w"),
  YmmAblkOp(0x0A, "op02_0A",
            "ww"),
  YmmAblkOp(0x0B, "op02_0B",
            "ww"),
  YmmAblkOp(0x0E, "op02_0E",
            ""),
  
  // FIXME: these are new for saturn, handler code not checked
  // CUT000A.CUT
  YmmAblkOp(0x10, "op02_10",
            ""),
  // CUT000B.CUT
  YmmAblkOp(0x11, "op02_11",
            ""),
  // CUT000C.CUT
  YmmAblkOp(0x12, "op02_12",
            ""),
  // C961A.DAT
  YmmAblkOp(0x13, "op02_13",
            "w"),
  // C513A.DAT
  YmmAblkOp(0x14, "op02_14",
            ""),
};

const int numYmmAblkOps = sizeof(ymmAblkOps) / sizeof(YmmAblkOp);
const int numYmmAblkOp02Subops = sizeof(ymmAblkOp02Subops) / sizeof(YmmAblkOp);

YmmAblkOp makeYmmAblkOp(int id);
YmmAblkOp makeYmmAblkOp02Subop(int id);


}


#endif
