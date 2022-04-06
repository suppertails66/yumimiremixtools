
; enable for debug display
DEBUG_FEATURES_ON equ 0



.sh2

.loadtable "font/scene/table.tbl"

;===================================================================
; macros
;===================================================================

; macro to simplify loading of constants to registers
.macro movC,num,reg
;  .if substr(tostring(num), 0, 1) == "r"
;    .notice "here"
  .if (num >= -0x80) && (num <= 0x7F)
    ; this syntax is not standard --
    ; this form of mov.b is defined as an alias of the standard mov
    ; in code for the specific purpose of being used in this macro
    ; (since without it, we would get stuck recursively calling the macro
    ; when trying to handle the byte-sized case)
    mov.b num,reg
  .elseif (num >= -0x8000 && num <= 0x7FFF)
    mov.w $(#num,pc),reg
  .else
    mov.l $(#num,pc),reg
  .endif
.endmacro

.macro loadMem.b,addr,reg
  movC addr,reg
  mov.b $reg,reg
.endmacro

.macro loadMem.w,addr,reg
  movC addr,reg
  mov.w $reg,reg
.endmacro

.macro loadMem.l,addr,reg
  movC addr,reg
  mov.l $reg,reg
.endmacro

.macro saveMem.b,srcReg,addr,addrReg
  movC addr,addrReg
  mov.b srcReg,$addrReg
.endmacro

.macro saveMem.w,srcReg,addr,addrReg
  movC addr,addrReg
  mov.w srcReg,$addrReg
.endmacro

.macro saveMem.l,srcReg,addr,addrReg
  movC addr,addrReg
  mov.l srcReg,$addrReg
.endmacro

.macro saveMemR0.b,srcReg,addr
  saveMem.b srcReg,addr,r0
.endmacro

.macro saveMemR0.w,srcReg,addr
  saveMem.w srcReg,addr,r0
.endmacro

.macro saveMemR0.l,srcReg,addr
  saveMem.l srcReg,addr,r0
.endmacro



.macro ble,reg1,reg2,addr
  cmp/ge reg1,reg2
  bt addr
.endmacro

.macro blt,reg1,reg2,addr
  cmp/gt reg1,reg2
  bt addr
.endmacro

.macro bge,reg1,reg2,addr
  cmp/ge reg2,reg1
  bt addr
.endmacro

.macro bgt,reg1,reg2,addr
  cmp/gt reg2,reg1
  bt addr
.endmacro

.macro bleSlot,reg1,reg2,addr
  cmp/ge reg1,reg2
  bt/s addr
.endmacro

.macro bltSlot,reg1,reg2,addr
  cmp/gt reg1,reg2
  bt/s addr
.endmacro

.macro bgeSlot,reg1,reg2,addr
  cmp/ge reg2,reg1
  bt/s addr
.endmacro

.macro bgtSlot,reg1,reg2,addr
  cmp/gt reg2,reg1
  bt/s addr
.endmacro

; branch if reg zero
.macro bze,reg,label
  tst reg,reg
  bt label
.endmacro

.macro bnz,reg,label
  tst reg,reg
  bf label
.endmacro

.macro beq,reg1,reg2,label
  cmp/eq reg1,reg2
  bt label
.endmacro

.macro bne,reg1,reg2,label
  cmp/eq reg1,reg2
  bf label
.endmacro

.macro beqR0,reg,value,label
  movC value,r0
  cmp/eq r0,reg
  bt label
.endmacro

.macro bneR0,reg,value,label
  movC value,r0
  cmp/eq r0,reg
  bf label
.endmacro

.macro beqSlotR0,reg,value,label
  movC value,r0
  cmp/eq r0,reg
  bt/s label
.endmacro

.macro bneSlotR0,reg,value,label
  movC value,r0
  cmp/eq r0,reg
  bf/s label
.endmacro

.macro jsrReg,target,reg
  movC target,reg
  jsr $reg
.endmacro

.macro jsrR0,target
  jsrReg target,r0
.endmacro



.macro addTo,leftReg,rightReg,dstReg
  mov leftReg,dstReg
  add rightReg,dstReg
.endmacro

.macro subTo,leftReg,rightReg,dstReg
  mov leftReg,dstReg
  sub rightReg,dstReg
.endmacro


; write a byte to an offset from a base address
; in a register, using r0
.macro writeOffsetR0.b,valueReg,baseReg,offset
  movC offset,r0
  mov.b valueReg,$(r0,baseReg)
.endmacro

.macro writeOffsetR0.w,valueReg,baseReg,offset
  movC offset,r0
  mov.w valueReg,$(r0,baseReg)
.endmacro

.macro writeOffsetR0.l,valueReg,baseReg,offset
  movC offset,r0
  mov.l valueReg,$(r0,baseReg)
.endmacro

; write a byte literal to an offset from a base address
; in a register, using r0 and r1
.macro writeOffsetR0R1.b,value,baseReg,offset
  movC value,r1
  writeOffsetR0.b r1,baseReg,offset
.endmacro

.macro writeOffsetR0R1.w,value,baseReg,offset
  movC value,r1
  writeOffsetR0.w r1,baseReg,offset
.endmacro

.macro writeOffsetR0R1.l,value,baseReg,offset
  movC value,r1
  writeOffsetR0.l r1,baseReg,offset
.endmacro

; read a byte from an offset of a base address
; in a register, using r0 and r1
.macro readOffsetR0.b,baseReg,offset,dstReg
  movC offset,r0
  mov.b $(r0,baseReg),dstReg
.endmacro

.macro readOffsetR0.w,baseReg,offset,dstReg
  movC offset,r0
  mov.w $(r0,baseReg),dstReg
.endmacro

.macro readOffsetR0.l,baseReg,offset,dstReg
  movC offset,r0
  mov.l $(r0,baseReg),dstReg
.endmacro

;===================================================================
; existing ram
;===================================================================
  
  buttonsPressed equ 0x0601F648
  currentFileTime equ 0x06030B78
  sceneSkipActive equ 0x06030B92
  gameMode equ 0x6030B9E
    gameMode_regular  equ 0
    gameMode_continue equ 1
    gameMode_replay   equ 2
    gameMode_puzzle   equ 3
  pendingDisplayListSize equ 0x06033028
  pendingDisplayList equ 0x06033848
  currentSpritesDisplayedCount equ 0x0603B848
  
  

;===================================================================
; existing functions
;===================================================================
  
  loadSceneFile equ 0x06012544
  prepSceneGraphics equ 0x06012A4C
  playSubscene equ 0x06013490
  advanceSceneNFrames equ 0x06014390
  ; FIXME: not sure exactly what kind of division this implements
  ; (signed, unsigned, size of input regs), and there are several
  ; other division routines as well.
  ; but this one divides r1 by r0 in some capacity that's hopefully
  ; what we need.
  divideStd equ 0x0601E6E4
  memcpy equ 0x0601EA74

;===================================================================
; constants
;===================================================================
  
  cpuNoCache equ 0x20000000
  
  
  vdp1VramBase equ 0x05C00000
  vdp1Edsr equ 0x05D00010
  vdp2VramBase equ 0x05E00000
  vdp2CramBase equ 0x05F00000
  
  
  bytesPerDispSlot equ 0x20
  

  fontCharRawW equ 16
;  fontCharH equ 12
  fontCharH equ 14
  fontLineSeparation equ 16
;  fontBpp equ 4
  fontCharByteSize equ (fontCharRawW*fontCharH)/2
  fontDigitBaseIndex equ 1
  font_numChars equ 0xA0
  
  screenW equ 320
  screenH equ 224
  ; center subtitles vertically about the horizontal line at this position
  subtitlesUpperBaseY equ 24
  subtitlesLowerBaseY equ screenH-subtitlesUpperBaseY
 
 ; this is the width used for the automatic word wrap
  maxSubWidth equ screenW-(16*2)
  
  
;  numOverlayDispSlots equ 0x180
  numOverlayDispSlots equ 0xC0
  overlayDispSlotMemSize equ numOverlayDispSlots*bytesPerDispSlot
  
  
  
;  overlayDispSlotVramOffset equ 0x6000
;  overlayDispSlotVramOffset equ 0x16800
;  overlayDispSlotVramOffset equ 0x18000
  overlayDispSlotVramOffset equ 0x17000
    overlayDispSlotBufAVramOffset equ overlayDispSlotVramOffset
    overlayDispSlotBufBVramOffset equ (overlayDispSlotVramOffset+overlayDispSlotMemSize)
    
    overlayDispSlotBufAVramCopyTarget equ (cpuNoCache+(vdp1VramBase+overlayDispSlotBufAVramOffset))
    overlayDispSlotBufBVramCopyTarget equ (cpuNoCache+(vdp1VramBase+overlayDispSlotBufBVramOffset))
    
    overlayDispSlotJumpCmdVramOffset equ (overlayDispSlotVramOffset-bytesPerDispSlot)
    overlayDispSlotJumpCmdVramCopyTarget equ (cpuNoCache+(vdp1VramBase+overlayDispSlotJumpCmdVramOffset))
;  fontVramBaseOffset equ 0x13000
;  fontVramBaseOffset equ 0x1B800
  fontVramBaseOffset equ 0x1A800
  
  ; armips appears to have a bug in its handling of OR operations;
  ; doing one to this value causes the assembler to somehow think
  ; it's a very small number (zero?), despite reporting it as having
  ; the correct value when it is checked externally by most means.
  ; this then causes the MOV macro to fail because it tries to generate
  ; a byte operation for a very large value.
  ; here, we can get the same result without the issues by doing addition instead,
  ; so it's fine, but why...?
;  fontVramCopyTarget equ cpuNoCache|(vdp1VramBase+fontVramBaseOffset)
  fontVramCopyTarget equ (cpuNoCache+(vdp1VramBase+fontVramBaseOffset))
  
  
  
  fontPalettesBaseOffset equ 0x600
  fontPalettesVramCopyTarget equ (cpuNoCache+(vdp2CramBase+fontPalettesBaseOffset))
  fontPalBaseIndex equ fontPalettesBaseOffset/32
  fontPalIndex_std equ fontPalBaseIndex+0
  ; FIXME?
  fontPalIndex_fg equ fontPalBaseIndex+1
  fontPalIndex_bg equ fontPalBaseIndex+2
  fontPalIndex_gradientBase equ fontPalBaseIndex+3
  pauseMenuPalIndex_std equ fontPalBaseIndex+7
  pauseMenuPalIndex_selected equ fontPalBaseIndex+8
  pauseMenuPalIndex_off equ fontPalBaseIndex+9
;  fontPalIndex_dropshad equ fontPalBaseIndex+7
  
  font_exclamationMarkIndex equ 0x3F
  font_questionMarkIndex equ 0x40
  font_periodIndex equ 0x48
  font_hyphenIndex equ 0x4D
  font_dashIndex equ 0x4E
  font_spaceIndex equ 0x4F
  font_ellipsisIndex equ 0x50
  font_enDashIndex equ 0x9C
  
  font_controlCharsBase equ 0xE0
  font_newlineIndex equ font_controlCharsBase+0x00
  font_leftAngleBracketIndex equ font_controlCharsBase+0x01
  font_rightAngleBracketIndex equ font_controlCharsBase+0x02
  font_plusIndex equ font_controlCharsBase+0x03
  
  colorGradientPixelW equ 4
  
  
  ; maximum length in bytes of a subtitle string
;  subString_maxSize equ 0x180
  subString_maxSize equ 0xC0
  
  
  
  
  sceneFileLoadPos equ 0x00200000
  sceneFileLoadPosNoCache equ cpuNoCache+sceneFileLoadPos
  
  
  
  ; "SUBS"
  subBlockMagicSig equ 0x53554253
  subBlockDataMaxSize equ 0x2000
  
  
  
  subAlignMode_center equ 0
  subAlignMode_left   equ 1

;===================================================================
; structs
;===================================================================
  
  ;==================================
  ; sub render state structs
  ;==================================
  
  ;======
  ; subTargetPoint
  ;======
  
  subTargetPoint_size       equ 12
    ; 4
    subTargetPoint_time             equ 0
    ; 4
    subTargetPoint_pixelX           equ 4
    ; 4
    subTargetPoint_bytePos          equ 8
  
  ;======
  ; subRenderBuf
  ;======
  
  maxSubTargetPoints equ 16
  subRenderBuf_pointDataSize equ (maxSubTargetPoints * subTargetPoint_size)
  
  maxSubtitleLines equ 4
  
  subRenderBuf_size          equ 28+(maxSubtitleLines*4)+subRenderBuf_pointDataSize
    ; 4
    subRenderBuf_srcStrPtr          equ 0
    ; 4
;    subRenderBuf_srcStrW            equ 4
    ; 4
    subRenderBuf_basePal            equ 4
    ; 4
    subRenderBuf_numPoints          equ 8
    ; 4
    subRenderBuf_currentPointIndex  equ 12
    ; 4
    subRenderBuf_numLines           equ 16
    ; 4
    subRenderBuf_timer              equ 20
    ; 4
    subRenderBuf_alignMode          equ 24
    ; maxSubtitleLines*4
    subRenderBuf_lineWArray         equ 28
    ; (maxSubTargetPoints * subTargetPoint_size)
    subRenderBuf_pointData          equ 28+(maxSubtitleLines*4)

  .macro makeSubRenderBuf,label
    .align 4
    label:
      ; srcStrPtr
/*      .dw 0
      ; srcStrW
      .dw 0
      ; baseClut
      .dh 0
      ; numPoints
      .dh 0
      ; currentPointIndex
      .db 0
      ; pointData
      .fill subRenderBuf_pointDataSize,0*/
      .fill subRenderBuf_size,0
    .align 4
  .endmacro
  
  ;======
  ; subStream
  ;======
  
  subStream_size          equ 20
    ; 4
    subStream_srcPtr                equ 0
    ; 4
    subStream_timer                 equ 4
    ; 4
    subStream_targetSlot            equ 8
    ; 4
    subStream_basePal               equ 12
    ; 4
    subStream_alignMode             equ 16

  .macro makeSubStream,label
    .align 4
    label:
      .fill subStream_size,0
    .align 4
  .endmacro

;===================================================================
; code
;===================================================================

.open "out/asm/SATANIME.BIN",0x06012000

  ;===================================================================
  ;===================================================================
  ; HACKS
  ;===================================================================
  ;===================================================================
  
  

  ;=============================
  ; load font whenever scene changes
  ; TODO: put this somewhere during startup, we don't really need
  ; to recopy it every scene change (though it probably doesn't matter)
  ;=============================
  
  .org 0x06019C64
    movC doSceneStartFontLoad,r4
    jmp $r4
    nop
    .pool

  ;=============================
  ; load font palettes during some init function
  ;=============================
  
  .org 0x06019142
    movC doExtraFontPaletteLoad,r4
    jmp $r4
    nop
    .pool

  ;=============================
  ; modify advanceSceneNFrames to refresh the display list
  ; every frame rather than only sending it once.
  ; this is necessary so the subtitles can be updated
  ; at any time.
  ;=============================
  
  .org 0x06014390
    movC doSceneAdvanceFrameSplit,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; TEST
  ;=============================
  
/*  .org 0x0601A248
    movC doTest,r0
    jmp $r0
    nop
    .pool*/

  ;=============================
  ; extra vsync handler logic
  ;=============================
  
  .org 0x0601A55C
    movC doExtraVsyncUpdate,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; force overlay e.g. after making a dialogue choice
  ; (needed for "ghosts" multiple choice scene)
  ;=============================
  
  .org 0x0601779C
    ; change from word write to long
;    mov.w r3,@r4
    mov.l r3,$r4
  .org 0x06017824
    ; replace terminator with jump targeting overlay
;    .dw 0x00008000
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)

  ;=============================
  ; force special zoom scenes to link to
  ; the overlay area (they normally send
  ; their commands to VRAM manually,
  ; bypassing the advance() logic that
  ; our code uses to inject the overlay)
  ;=============================
  
  ; we replace the terminator with a jump to the overlay,
  ; like with the standard logic
  
  .org 0x0601864A
    ; change from word write to long
;    mov.w r3,@r5
    mov.l r3,$r5
  .org 0x06018670
    ; replace terminator with jump targeting overlay
;    .dw 0x00008000
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)
  
/*  .org 0x06016302
    mov.l r2,$r14
  .org 0x06016398
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)*/

  ;=============================
  ; force some specially handled
  ; sprite thing to show the overlay.
  ; used e.g. in C711.
  ; NOTE: in case it's ever relevant,
  ; something also happens at the very
  ; end of that scene that makes the
  ; overlay stop getting rendered briefly there too.
  ; i haven't bothered fixing it because
  ; the subtitles aren't needed at that point anyway.
  ;=============================
  
  .org 0x06018352
    ; change from word write to long
;    mov.w r3,@r4
    mov.l r3,$r4
  .org 0x060183B0
    ; replace terminator with jump targeting overlay
;    .dw 0x00008000
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)

  ;=============================
  ; force some other specially handled sprite thing
  ; to draw the overlay (e.g. C915A)
  ;=============================
  
  ; shared constant from pool, cannot use normal method
  
  .org 0x06014188
    movC doExtraSceneSomethingTerminatorThing,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; force overlay when clearing vdp1 display list
  ;=============================
  
;   .org 0x060129EA
;     ; change from word write to long
; ;    mov.w r3,@r2
;     mov.l r3,$r2
;   .org 0x06012A34
;     ; replace terminator with jump targeting overlay
; ;    .dw 0x00008000
;     .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)

  ;=============================
  ; display overlay during pause
  ;=============================
  
  .org 0x06013D12
    ; change from word write to long
;    mov.w r3,@r10
    mov.l r3,$r10
  .org 0x06013D68
    ; replace terminator with jump targeting overlay
;    .dw 0x00008000
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)

  ;=============================
  ; display overlay during stuff at least including pause
  ;=============================
  
  .org 0x06017F64
    ; change from word write to long
;    mov.w r2,@r7
    mov.l r2,$r7
  .org 0x06018024
    ; replace terminator with jump targeting overlay
;    .dw 0x00008000
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)

  ;=============================
  ; correctly restore jump target when flashing
  ; "pause" graphic
  ;=============================
  
  .org 0x06013BE6
    movC doExtraPauseStartSetup,r0
    jmp $r0
    nop
    .pool
  
  .org 0x06013C84
    movC doExtraPauseSoundStoppedSetup,r0
    jmp $r0
    nop
    .pool
    
  .org 0x06013D86
    movC doExtraPauseSoundRestartingSetup,r0
    jmp $r0
    nop
    .pool
    
  ; save jump target during setup so we can restore it later
  .org 0x06013CD6
    movC doExtraPauseGrpSetup,r0
    jmp $r0
    nop
    .pool
  
  ; not enough space at loop end to do this cleanly,
  ; moving function end out to free up pool space
  .org 0x06013DB8
    movC makeUpStdIdleEnd,r1
    jmp $r1
    nop
    .pool
  
  .org 0x06013DC4
    doExtraPauseGrpRestorePoolCmd:
      .dw doExtraPauseGrpRestore
  
  .org 0x06013D6C
    mov.l $(doExtraPauseGrpRestorePoolCmd,pc),r0
    jmp $r0
    nop

  ;=============================
  ; 
  ;=============================
  
/*  .org 0x06016302
    mov.l r2,$r14
  .org 0x06016398
    .dw 0x50000000+(overlayDispSlotJumpCmdVramOffset/8)*/

  ;=============================
  ; check for and handle loading of subtitle block
  ; from scene files
  ;=============================
  
  .org 0x0601255A
    movC doSubtitleBlockLoadCheck,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; do extra setup when starting scenes
  ;=============================
  
  .org 0x06012C7E
    movC doExtraSceneStartSetup1,r0
    jmp $r0
    nop
    .pool
  
  .org 0x06012E5C
    movC doExtraSceneStartSetup2,r0
    jmp $r0
    nop
    .pool
  
  .org 0x06012EE2
    movC doExtraSceneStartSetup3,r0
    jmp $r0
    nop
    .pool
  
  .org 0x0601306E
    movC doExtraSceneStartSetup4,r0
    jmp $r0
    nop
    .pool
  
  .org 0x060131B0
    movC doExtraSceneStartSetup5,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; do extra setup when starting sounds
  ;=============================
  
  .org 0x0601ADC6
    movC doExtraSoundStartSetup,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; replace original interface graphics with new
  ;=============================
  
  .org 0x0601F8FA
  .area 0x1180
    .incbin "out/rsrc/interface_block.bin"
  .endarea

  ;=============================
  ; extra init when starting a subscene
  ;=============================
  
;  .org 0x060134A0
;    movC doExtraSubsceneStartSetup,r0
;    jmp $r0
;    nop
;    .pool

  ;=============================
  ; tick sync counter on cd audio track start
  ;=============================

  .org 0x0601392C
    movC doExtraCdAudioStartSetup,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; tick sync counter on cd audio track wait completion
  ;=============================
  
  .org 0x06013998
    movC doExtraCdAudioSyncWaitSetup,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; someone decided to be clever and optimize
  ; the interface bubbles by using a random
  ; blank tile in the "save" graphic area
  ; as a generic blank tile for all generic windows.
  ; we need to use that tile, so now we have to
  ; go through and change all occurences of it
  ; in existing tilemaps.
  ;=============================
 
  oldInterfaceBlankTile equ 0x1C
  newInterfaceBlankTile equ 0x20
  
  .org 0x601F6D8
    .fill 2,newInterfaceBlankTile
  ; this is our newly non-blank tile,
  ; which we want to keep the same
;  .org 0x601F6DE
;    .fill 1,newInterfaceBlankTile
  .org 0x601F6E4
    .fill 2,newInterfaceBlankTile
  .org 0x601F6F0
    .fill 2,newInterfaceBlankTile
  .org 0x601F6FC
    .fill 2,newInterfaceBlankTile
  ; used to blank out the "cartridge" option if no RAM cart
  .org 0x601F708
    .fill 14,newInterfaceBlankTile

  ;=============================
  ; x/y position of internal/cartridge RAM
  ; labels on save format needed message
  ;=============================
  
;  saveFormatNeededTypeLabelY equ 0x50
;  saveFormatNeededTypeLabelX equ 0x90
  ; see rsrc/backup_format.png for the location being targeted
  saveFormatNeededTypeLabelY equ 102
  saveFormatNeededTypeLabelX equ 95
  
  ; force internal ram format error on startup
;  .org 0x0601563A
;    cmp/eq 0,r0
  ; force cartridge ram format error on startup
;  .org 0x06015676
;    cmp/eq 0,r0
  
  ; force backup ram insufficient error on startup
  ; (not the only check, one is probably for internal
  ; and the other for cartridge)
;  .org 0x0601578C
;    nop
  
  .org 0x060158AC
    mov saveFormatNeededTypeLabelY,r7
  
  .org 0x060158B4
    ; just to be irritating, the compiler optimized this load
    ; by turning the x into (y + 0x40), so we have to account for that
    add (saveFormatNeededTypeLabelX-saveFormatNeededTypeLabelY),r6

  ;=============================
  ; DEBUG: always allow scene skipping,
  ; regardless of whether noskip flag has
  ; been set by scene
  ;=============================
  
  .org 0x0601A998
    .if DEBUG_FEATURES_ON
      nop
    .endif

  ;=============================
  ; DEBUG: skip startup splash screen
  ;=============================
  
  .org 0x0601210E
    .if DEBUG_FEATURES_ON
      nop
    .endif

  ;=============================
  ; do not set scene noskip flag on scene start
  ;=============================
  
;  .org 0x0601350C
;    nop
;    nop
  
  .org 0x060134D0
    nop
    nop

  ;=============================
  ; do modified scene skipping logic
  ;=============================
  
  .org 0x0601A99A
    movC doExtraSceneSkipLogic,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; do modified scene skipping logic 2
  ;=============================
  
  .org 0x060135A0
    movC doExtraSceneSkipLogic2,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; stop subtitles when a dialogue choice occurs
  ; (they won't be shown anyway because the overlay
  ; isn't injected when the choice balloon is drawn,
  ; but it's better not to have them running in the background
  ; just in case there are any surprises)
  ;=============================
  
  .org 0x06015488
    movC doExtraChoiceStartLogic,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; extra logic for in-game soft reset
  ; (the main menus and some other areas
  ; have a separate check, but it just
  ; resets to the BIOS, so there's no
  ; real need to change it)
  ;=============================
  
  .org 0x0601A982
    movC doExtraIngameSoftResetCheckLogic,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; hack-loading of new tilemaps for yumimi puzzle
  ;=============================
  
  .org 0x0601E5D4
    movC doExtraYmmPuzTilemapLoadLogic,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; hack-loading of new tiles for yumimi puzzle
  ;=============================
  
  .org 0x0601E388
    movC doExtraYmmPuzTileLoadLogic,r0
    jmp $r0
    nop
    .pool

  ;=============================
  ; yumimi puzzle text fixes
  ;=============================
  
  .org 0x0601F578
    ; original: "BACK GROUND"
    .ascii "BACKGROUND "
  
  
  
  
  
  

  ;===================================================================
  ;===================================================================
  ; FREE SPACE
  ;===================================================================
  ;===================================================================
  
  

  .org 0x06058800
  .area 0x0606C000-.

    ;=============================
    ; font loading
    ;=============================
    
    doSceneStartFontLoad:
      ; make up work
      mov.l $r15+,r10
      mov.l $r15+,r11
      mov.l $r15+,r12
      mov.l $r15+,r13
      ; load font
      bra loadFont
      mov.l $r15+,r14
    
    loadFont:
      mov.l r4,$-r15
      sts.l pr,$-r15
      
        movC memcpy,r0
        ; dst
        movC fontVramCopyTarget,r4
        ; src
        movC fontBitmap,r5
        ; size
        movC fontBitmapEnd-fontBitmap,r6
        jsr $r0
        nop
        
        ; TEMP??
        movC loadFontPalettes,r0
        jsr $r0
        nop
      
      lds.l $r15+,pr
      rts
      mov.l $r15+,r4
    
    .pool

    ;=============================
    ; font data
    ;=============================
    
    fontBitmap:
      .incbin "out/font/font_scene_bitmap.bin"
      
      fontBitmap_pauseMenu_honorifics:
        .incbin "out/rsrc/pause_menu_honorifics.bin"
      fontBitmap_pauseMenu_subtitles:
        .incbin "out/rsrc/pause_menu_subtitles.bin"
      fontBitmap_pauseMenu_signs:
        .incbin "out/rsrc/pause_menu_signs.bin"
      fontBitmap_pauseMenu_sceneskip:
        .incbin "out/rsrc/pause_menu_sceneskip.bin"
      fontBitmap_pauseMenu_sceneskipdesc:
        .incbin "out/rsrc/pause_menu_sceneskipdesc.bin"
      fontBitmap_pauseMenu_on:
        .incbin "out/rsrc/pause_menu_on.bin"
      fontBitmap_pauseMenu_off:
        .incbin "out/rsrc/pause_menu_off.bin"
      fontBitmap_pauseMenu_cursor:
        .incbin "out/rsrc/pause_menu_cursor.bin"
    fontBitmapEnd:

    ;=============================
    ; color loading
    ;=============================
    
    doExtraFontPaletteLoad:
      ; load font palettes
      bsr loadFontPalettes
      nop
      
      ; TEMP
;      movC overlayDispSlotVramCopyTarget,r0
;      movC 0x8000,r1
;      mov.w r1,$r0
      
      ; make up work
      movC 0x0604B9D8,r3
      jsr $r3
      movC 0,r4
      
      movC 0x0604DA34,r3
      jsr $r3
      movC 1,r4
      
      movC 0x0601914E,r0
      jmp $r0
      nop
    
    loadFontPalettes:
;      sts.l pr,$-r15
      
;        movC memcpy,r0
        ; dst
;        movC fontPalettesVramCopyTarget,r4
        ; src
;        movC fontPalettes,r5
;        jsr $r0
        ; size
;        movC fontPalettesEnd-fontPalettes,r6
        
        ; r4 = dst
        movC fontPalettesVramCopyTarget,r4
        ; r5 = src
        movC fontPalettes,r5
        ; r6 = remaining
        movC fontPalettesEnd-fontPalettes,r6
        ; r1 = check value
        movC 0,r1
        
        @@loop:
          ; FIXME: is long copy better/possible?
          mov.w $r5+,r0
          mov.w r0,$r4
          add 2,r4
          add -2,r6
          ; loop while nonzero
          cmp/eq r1,r6
          bf @@loop
      
;      lds.l $r15+,pr
      rts
      nop
    
    .pool

    ;=============================
    ; font color data
    ;=============================
    
    fontPalettes:
      .incbin "out/font/font_scene_pal_std.pal"
      .incbin "out/font/font_scene_pal_karaokeback.pal"
      .incbin "out/font/font_scene_pal_karaokefront.pal"
      .incbin "out/font/font_scene_pal_karaokefade4.pal"
      .incbin "out/font/font_scene_pal_karaokefade3.pal"
      .incbin "out/font/font_scene_pal_karaokefade2.pal"
      .incbin "out/font/font_scene_pal_karaokefade1.pal"
      .incbin "out/rsrc/pause_menu_std.pal"
      .incbin "out/rsrc/pause_menu_selected.pal"
      .incbin "out/rsrc/pause_menu_off.pal"
    fontPalettesEnd:
      
    ;=============================
    ; 
    ;=============================
    
    makeUpStdIdleEnd:
      ; clear overlay pause flag
;      movC 0,r1
;      saveMemR0.w r1,overlayPauseFlag
      
      lds.l $r15+,pr
      mov.l $r15+,r8
      mov.l $r15+,r9
      mov.l $r15+,r10
      mov.l $r15+,r11
      mov.l $r15+,r12
      mov.l $r15+,r13
      rts
      mov.l $r15+,r14
    .pool
      
    ;=============================
    ; 
    ;=============================
    
    doExtraSceneSomethingTerminatorThing:
      ; this instead of terminator
      movC 0x50000000+(overlayDispSlotJumpCmdVramOffset/8),r2
      extu.w r3,r3
      add -1,r3
      shll2 r3
      shll2 r3
      shll r3
      add r4,r3
      ; long instead of word
      mov.l r2,$r3
      
      movC 0x06014198,r1
      jmp $r1
      nop
    .pool
      
    ;=============================
    ; 
    ;=============================
      
    doExtraPauseStartSetup:
      ; set overlay pause flag
;      movC 1,r1
;      saveMemR0.w r1,overlayPauseFlag
      
      ; make up work
      movC 0x06012460,r3
      jsr $r3
      nop
      movC 0x06032FF6,r3
      extu.w r12,r0
      mov 0x10,r6
      
      movC 0x06013BF2,r1
      jmp $r1
      nop
    .pool
      
    ;=============================
    ; 
    ;=============================
      
    doExtraPauseSoundStoppedSetup:
      ; set overlay pause flag
      movC 1,r1
      saveMemR0.w r1,overlayPauseFlag
      ; reset last buttons pressed value
;      movC 0,r1
      movC 0,r1
      saveMemR0.w r1,pauseMenu_lastButtonsPressed
      
      ; make up work
      mov.w $r11,r3
      movC 0x06033028,r2
      extu.w r3,r3
      add 1,r3
      mov.w r3,$r2
      jsrR0 0x06014142
      nop
      
      movC 0x06013C92,r1
      jmp $r1
      nop
    .pool
      
    ;=============================
    ; 
    ;=============================
      
    doExtraPauseSoundRestartingSetup:
      
      
      ; make up work
      jsr $r3
      movC 1,r4
      movC 1,r2
      movC 0x06020A7C,r3
      mov.w r2,$r3
      extu.w r12,r8
      
      ; FIXME: timing??
      
      ; do first call of loop to wait for vsync
      ; (and hopefully for sound to resume)
      jsr $r13
      nop
;      add 1,r8
      ; ???
;      jsr $r13
;      nop
;      jsr $r13
;      nop
;      jsr $r13
;      nop
      movC 1,r8
      
      ; clear overlay pause flag
      movC 0,r1
      saveMemR0.w r1,overlayPauseFlag
      ; refresh needed to get rid of menu
      movC 1,r1
      saveMemR0.w r1,unpausedLastFrame
      
;      movC 0x06013D92,r1
      movC 0x06013D98,r1
      jmp $r1
      nop
    .pool
    
    unpausedLastFrame:
      .dh 0
    
    ;=============================
    ; 
    ;=============================
      
    doExtraPauseGrpSetup:
      ; make up work
      ; compute target of jump command for pause graphic
      ; display commands
      mov.w $r11,r3
      extu.w r3,r3
      shll2 r3
      extu.w r3,r0
      mov.w r0,$(2,r10)
      
      ; save local copy
      movC lastPauseGrpJumpDst,r1
      mov.l r0,$r1
      
      ; make up work
      movC 0x06017E78,r3
    
      movC 0x06013CE2,r0
      jmp $r0
      nop
    .pool
    
    .align 4
    lastPauseGrpJumpDst:
      .dw 0
      
    ;=============================
    ; 
    ;=============================
      
    doExtraPauseGrpRestore:
      ; set command to jump
      movC 0x5000,r2
      mov.w r2,$r10
      ; set target
      loadMem.l lastPauseGrpJumpDst,r0
;      mov.w $r11,r3
;      extu.w r3,r3
;      shll2 r3
;      extu.w r3,r0
      mov.w r0,$(2,r10)
      
      ; make up work
      jsr $r9
      mov.w $r11,r4
      
      movC 0x06013D74,r0
      jmp $r0
      nop
    .pool

    ;=============================
    ; 
    ;=============================
    
    doSubtitleBlockLoadCheck:
      ; make up work
      movC 0x00200004,r11
      movC 0x06030B9C,r2
      mov.w r8,$r2
      ; do CD load
      movC 0x06019DC0,r2
      jsr $r2
      mov 0,r10
        
        ; stop all streams; obviously we don't want them
        ; to try to keep running as we paste their source scripts
        ; over with new data.
        ; render buffers continue to display their own buffered contents, though.
        ; FIXME: this means that a subtitle could potentially "stick"
        ; into the next scene instead of getting cut off as intended.
        ; but since that's sometimes what we want to happen, i don't want to
        ; force a cutoff here.
;          movC subStreamA,r4
;          writeOffsetR0R1.l 0,r4,subStream_srcPtr
;          movC subStreamB,r4
;          writeOffsetR0R1.l 0,r4,subStream_srcPtr
        ; ugh, whatever
        jsrR0 clearAndResetOverlay
        nop
      
        ; r12 = subtitle block size (default = 0)
        movC 0,r12
      
        ; check first word for subtitle block signature
        loadMem.l sceneFileLoadPosNoCache,r0
        movC subBlockMagicSig,r1
        cmp/eq r0,r1
        bf @@noSubs
          ; r12 = r6 = block size
          loadMem.l sceneFileLoadPosNoCache+4,r6
          mov r6,r12
          ; r5 = src
          movC sceneFileLoadPosNoCache,r5
          ; r4 = dst
          movC subtitleBlockData,r4
          movC memcpy,r3
          jsr $r3
          nop
        
        @@noSubs:
        
        ; write size of loaded subtitle block as subs-exist flag
        saveMemR0.w r12,subtitleBlockExists
          
        ; skip past subtitle block.
        ; this is achieved by self-modifying pool constants within existing code
        ; that correspond to base addresses for the scene data (which previously
        ; was a fixed location, but now depends on the size of the subtitle block).
        ; register r11 is also updated, as it's being used as a "shortcut" register
        ; for a frequently-accessed related constant.
        ; TODO: do we need/want to use nocache here?
        ; i still don't understand how it works...
        
;        movC sceneFileLoadPos,r0
;        saveMem.l r0,0+0x060125D0,r1
;        movC sceneFileLoadPosNoCache,r0
        mov r12,r0
        movC sceneFileLoadPosNoCache,r1
        add r1,r0
        ; 0x20240000
        saveMem.l r0,0+0x060125EC,r1
        saveMem.l r0,0+0x060126FC,r1
;        movC sceneFileLoadPosNoCache+2,r0

        ; 0x20240002
        add 2,r0
        saveMem.l r0,0+0x060125F0,r1
        
        ; 0x20240004
        add 2,r0
;        movC sceneFileLoadPosNoCache+4,r0
        saveMem.l r0,0+0x06012608,r1
        ; code for initializing subscenes other than first in file
        saveMem.l r0,0+0x06012800,r1
        
        ; 0x00240004
        ; remove cache bits from pointer
        movC 0x07FFFFFF,r1
        and r1,r0
        saveMem.l r0,0+0x060125D0,r1
        ; code for dynamically loaded sounds
        ; (when there is too much sound data to all fit in memory at once,
        ; the game loads each file one at a time it's needed)
        ; e.g. the joke-choice scene with the ghosts
        saveMem.l r0,0+0x0601AE24,r1
        
        ; update r11
        mov r0,r11
      
      @@done:
      ; make up work
      movC 0x060469B8,r12
      movC 0x0603B8A4,r13
      movC 0x06030BD0,r14
      ; FIXME:
      ; resume normal logic
      movC 0x0601256C,r0
      jmp $r0
      nop
      
    .pool

    ;=============================
    ; 
    ;=============================
    
    subBlock_soundCountOffset equ 8
    subBlock_indexStart equ 12
    
    .macro makeDoExtraSceneStartSetup,label,retAddr
      label:
        bsr doExtraSceneStartSetup
        nop
        movC retAddr,r0
        jmp $r0
        nop
      .pool
    .endmacro
    
    makeDoExtraSceneStartSetup doExtraSceneStartSetup1,0x06012C8A
    makeDoExtraSceneStartSetup doExtraSceneStartSetup2,0x06012E68
    makeDoExtraSceneStartSetup doExtraSceneStartSetup3,0x06012EEE
    makeDoExtraSceneStartSetup doExtraSceneStartSetup4,0x0601307A
    makeDoExtraSceneStartSetup doExtraSceneStartSetup5,0x060131BC
    
    doExtraSceneStartSetup:
      sts.l pr,$-r15
      ; make up work
      jsrR0 loadSceneFile
      nop
      jsrR0 prepSceneGraphics
      nop
        
        ; reset state vars (sync counters, etc.)
        jsrR0 resetOverlaySceneStateVars
        nop
        
        ; check if subtitle block exists
        loadMem.w subtitleBlockExists,r4
        ; do nothing if not
        bze r4,@@done
        
        ; check if scene-start subtitle stream exists
        loadMem.l subtitleBlockData+subBlock_indexStart,r4
        ; do nothing if index offset zero (= doesn't exist)
        bze r4,@@done
        
        ; start subtitle stream in stream slot 1
        ; raw index offset += base pointer
        movC subtitleBlockData+subBlock_indexStart,r1
        add r1,r4
        jsrR0 startSubStream
        ; r5 = target slot
        movC 1,r5
      
      @@done:
      
      .if DEBUG_FEATURES_ON
        ; DEBUG: reset sceneTimer
        movC 0,r1
        saveMemR0.l r1,sceneTimer
      .endif
      
      ; make up work
      jsrR0 playSubscene
      nop
      
      lds.l $r15+,pr
      rts
      nop
    
    .pool

    ;=============================
    ; automatically start sound streams
    ;=============================
    
    ; r14 = index of sound that has just been queued
    doExtraSoundStartSetup:
      ; increment sound playback counter
      movC syncCounter_soundCount,r4
      mov.w $r4,r1
      extu.w r1,r1
      add 1,r1
      mov.w r1,$r4
      
      ; check if subtitle block exists
      loadMem.w subtitleBlockExists,r4
      ; do nothing if not
      bze r4,@@done
      
      ; check if this sound is valid at all
      ; (at least one scene, C334B, tries to play a sound that
      ; doesn't exist)
      loadMem.l subtitleBlockData+subBlock_soundCountOffset,r4
      bge r14,r4,@@done
      
      ; check if a script exists for this sound
      ; r0 = offset into index: (soundNum + 1) * 4
      mov r14,r0
      add 1,r0
      shll2 r0
      movC subtitleBlockData+subBlock_indexStart,r4
      mov.l $(r0,r4),r1
      ; do nothing if index offset zero (= doesn't exist)
      bze r1,@@done
      
      ; start subtitle stream in stream slot 0
      ; raw index offset += base pointer
      add r1,r4
      jsrR0 startSubStream
      ; r5 = target slot
      movC 0,r5
    
      @@done:
      ; make up work
      lds.l $r15+,pr
      mov.l $r15+,r8
      mov.l $r15+,r9
      mov.l $r15+,r10
      mov.l $r15+,r11
      mov.l $r15+,r12
      mov.l $r15+,r13
      rts
      mov.l $r15+,r14
    
    .pool

    ;=============================
    ; automatically reset stuff on new subscene
    ;=============================
    
/*    doExtraSubsceneStartSetup:
      jsrR0 resetOverlaySubsceneState
      nop
      
      ; make up work
      add -8,r15
      movC 0x0602E030,r8
      movC 1,r9
      movC 0x06032DB8,r10
      movC 0x0603B920,r11
      movC 0,r12
      
      movC 0x060134AC,r0
      jmp $r0
      nop
      
    .pool*/

    ;=============================
    ; 
    ;=============================
    
    doExtraCdAudioStartSetup:
      ; make up work
      movC 0x0603FB40,r2
      mov.w $r2,r2
      extu.w r2,r2
      tst r2,r2
      bt @@not
        movC 0x0601A0D2,r3
        jsr $r3
        nop
      @@not:
      jsrR0 0x06013F20
      nop
      
      ; tick counter
      movC syncCounter_cddaPlay,r3
      mov.w $r3,r2
      extu.w r2,r2
      add 1,r2
      mov.w r2,$r3
      
      ; make up work
      movC 0x06013B14,r2
      jmp $r2
      nop
     
    .pool

    ;=============================
    ; 
    ;=============================
    
    doExtraCdAudioSyncWaitSetup:
      ; tick counter
      movC syncCounter_cddaWait,r3
      mov.w $r3,r2
      extu.w r2,r2
      add 1,r2
      mov.w r2,$r3
      
      ; make up work
      mov.w $r13,r2
      add -1,r2
      mov.w r2,$r13
      extu.w r9,r3
      movC 0x0603B8A0,r2
      mov.w r3,$r2
      movC 0x06013B14,r2
      jmp $r2
      nop
     
    .pool

    ;=============================
    ; 
    ;=============================
    
    doExtraSceneSkipLogic:
    
      ; at this point, we know the scene noskip flag
      ; is zero, so the scene itself has not disabled
      ; skipping.
      ; so, we need to decide whether our own configuration
      ; allows the skip to happen.
      
      ; if game mode == replay, skipping is always allowed
      ; (assuming scene has not disabled it)
      loadMem.w gameMode,r1
      beqR0 r1,gameMode_replay,@@doSkipCheck
      
      ; otherwise, skipping is allowed only if
      ; our own enable flag is set
      loadMem.b option_allowSceneSkip,r1
      bze r1,@@done
      
      @@doSkipCheck:
      
      ; do not allow skip flag to be set if paused
      loadMem.w overlayPauseFlag,r0
      bnz r0,@@done
      
      ; make up work: check if skip button is pressed
      
      movC buttonsPressed,r3
      mov.w $r3,r2
      extu.w r2,r2
      
      movC 0x0400,r1
      beq r2,r1,@@doSkip
      movC 0x0200,r1
      bne r2,r1,@@done
      @@doSkip:
        ; set scene skip request flag
        movC 1,r3
        saveMemR0.w r3,sceneSkipActive
        
        ; shut subtitles off
        ; (this wouldn't matter except that due to input
        ; handling shenanigans, if you hold the skip button down,
        ; the game stops the sound but does not actually skip the
        ; scene until it's released. so, we have to shut the
        ; subtitles off right away or they'll keep running
        ; until the button is no longer held.)
        jsrR0 clearAndResetOverlay
        nop
        
      @@done:
      movC 0x0601A9B8,r2
      jmp $r2
      nop
    .pool

    ;=============================
    ; 
    ;=============================
    
    doExtraSceneSkipLogic2:
      ; shut off subtitles
      jsrR0 clearAndResetOverlay
      nop
    
      ; make up work
      movC 0x0603FB40,r2
      mov.w $r2,r2
      extu.w r2,r2
      tst r2,r2
      bt @@not
        movC 0x0601A0D2,r3
        jsr $r3
        nop
      @@not:
      
      movC 0x060135B0,r2
      jmp $r2
      nop
    .pool

    ;=============================
    ; 
    ;=============================
    
    doExtraChoiceStartLogic:
      ; make up work
      mov.l r14,$-r15
      mov.l r13,$-r15
      mov.l r12,$-r15
      mov.l r11,$-r15
      mov.l r10,$-r15
      mov.l r9,$-r15
      mov.l r8,$-r15
      sts.l pr,$-r15
      
        ; shut off subtitles
        jsrR0 clearAndResetOverlay
        nop
      
      ; make up work
      movC 0,r8
      movC 0x0601549A,r2
      jmp $r2
      nop
    
    .pool

    ;=============================
    ; 
    ;=============================
    
    doExtraIngameSoftResetCheckLogic:
      movC buttonsPressed,r3
      movC 0x0F00,r2
      mov.w $r3,r3
      extu.w r3,r3
      bne r3,r2,@@noSoftReset
        ; set flag
        movC 1,r2
        mov.w r2,$r4
        
        ; shut off subtitles
        jsrR0 clearAndResetOverlay
        nop
      @@noSoftReset:
      
      ; make up work
      movC 0x0601A992,r2
      jmp $r2
      nop
    .pool

    ;=============================
    ; 
    ;=============================
    
    ymmPuzOldTitleTilemapPtr equ 0x002138B4
    ymmPuzOldTitleTilesPtr equ 0x0021393E
    
    ymmPuzOldKabe0TilemapPtr equ 0x00200244
    ymmPuzOldKabe0TilesPtr equ 0x002003A5
    
    ymmPuzOldOsimaiTilemapPtr equ 0x002204D8
    ymmPuzOldOsimaiTilesPtr equ 0x00220530
    
    ymmPuzOldTeaseTilemapPtr equ 0x002204FC
    ymmPuzOldTeaseTilesPtr equ 0x00220558
    
    ; r0 = size (must be nonzero)
    ; r4 = dst + size
    ; r5 = src
    .macro macroRevCopyLoop
      @@loop:
        add -1,r0
        mov.b $(r0,r5),r1
        mov.b r1,$-r4
        ; loop while counter nonzero
        tst r0,r0
        bf @@loop
    .endmacro
    
    ; r0 = size (must be nonzero)
    ; r4 = dst
    ; r5 = src
    .macro macroCopyLoop
      add r0,r4
      macroRevCopyLoop
    .endmacro
    
    ; *** NONSTANDARD ***
    ; r0 = len
    ; r1 = src1
    ; r2 = src2
    ; returns r0 zero if same
    .macro macro_memcmp
      @@loop:
        bze r0,@@done
        add -1,r0
        mov.b $(r0,r1),r3
        mov.b $(r0,r2),r6
        beq r3,r6,@@loop
      @@done:
    .endmacro
    
    doExtraYmmPuzTilemapLoadLogic:
      ; if the src address (r5) is something we're changing,
      ; copy from new src and return
      
      bneR0 r5,ymmPuzOldTitleTilemapPtr,@@notTitle
        movC (ymmpuz_titleTilemap_end-ymmpuz_titleTilemap),r0
        movC ymmpuz_titleTilemap,r5
        macroCopyLoop
        
        ; pretend we decompressed from the normal location
        movC 0x21393C,r0
        rts
        nop
      @@notTitle:
      
      bneR0 r5,ymmPuzOldKabe0TilemapPtr,@@notKabe0
        movC (ymmpuz_kabe0Tilemap_end-ymmpuz_kabe0Tilemap),r0
        movC ymmpuz_kabe0Tilemap,r5
        macroCopyLoop
        
        ; pretend we decompressed from the normal location
        movC 0x2003A3,r0
        rts
        nop
      @@notKabe0:
      
      bneR0 r5,ymmPuzOldOsimaiTilemapPtr,@@notOsimai
        movC (osimaiTilemapVerifyStrEnd-osimaiTilemapVerifyStr),r0
        movC osimaiTilemapVerifyStr,r1
        mov r5,r2
        ; !!!
;        add 0x30,r2
        macro_memcmp
        bnz r0,@@notOsimai
        
        movC (ymmpuz_osimaiTilemap_end-ymmpuz_osimaiTilemap),r0
        movC ymmpuz_osimaiTilemap,r5
        macroCopyLoop
        
        ; pretend we decompressed from the normal location
        movC 0x22052E,r0
        rts
        nop
      @@notOsimai:
      
      bneR0 r5,ymmPuzOldTeaseTilemapPtr,@@notTease
        movC (teaseTilemapVerifyStrEnd-teaseTilemapVerifyStr),r0
        movC teaseTilemapVerifyStr,r1
        mov r5,r2
        ; !!!
        add 0x30,r2
        macro_memcmp
        bnz r0,@@notTease
        
        movC (ymmpuz_teaseTilemap_end-ymmpuz_teaseTilemap),r0
        movC ymmpuz_teaseTilemap,r5
        macroCopyLoop
        
        ; pretend we decompressed from the normal location
        movC 0x220556,r0
        rts
        nop
      @@notTease:
      
      ; make up work
      mov.l r14,$-r15
      mov.l r13,$-r15
      mov.l r12,$-r15
      mov.l r10,$-r15
      mov.l r9,$-r15
      mov.l r8,$-r15
      movC 0x0601E5E0,r2
      jmp $r2
      nop
    .pool

    ;=============================
    ; 
    ;=============================
    
    doExtraYmmPuzTileLoadLogic:
      ; if the src address (r5) is something we're changing,
      ; copy from new src and return
      
      bneR0 r5,ymmPuzOldTitleTilesPtr,@@notTitle
        movC (ymmpuz_titleTiles_end-ymmpuz_titleTiles),r0
        movC ymmpuz_titleTiles,r5
        macroCopyLoop
        
        ; pretend we decompressed normally
        movC (ymmpuz_titleTiles_end-ymmpuz_titleTiles),r0
        add r4,r0
        rts
        nop
      @@notTitle:
      
      bneR0 r5,ymmPuzOldKabe0TilesPtr,@@notKabe0
        movC (ymmpuz_kabe0Tiles_end-ymmpuz_kabe0Tiles),r0
        movC ymmpuz_kabe0Tiles,r5
        macroCopyLoop
        
        ; pretend we decompressed to the normal location
        movC (ymmpuz_kabe0Tiles_end-ymmpuz_kabe0Tiles),r0
        add r4,r0
        rts
        nop
      @@notKabe0:
      
      bneR0 r5,ymmPuzOldOsimaiTilesPtr,@@notOsimai
        ; additional verification, because these image files
        ; are placed in a shared buffer before being loaded
;        readOffsetR0.b r5,12,r2
;        movC 0x4F471201,r1
        movC (osimaiTilesVerifyStrEnd-osimaiTilesVerifyStr),r0
        movC osimaiTilesVerifyStr,r1
        mov r5,r2
        macro_memcmp
        bnz r0,@@notOsimai
        
        movC (ymmpuz_osimaiTiles_end-ymmpuz_osimaiTiles),r0
        movC ymmpuz_osimaiTiles,r5
        macroCopyLoop
        
        ; pretend we decompressed normally
        movC (ymmpuz_osimaiTiles_end-ymmpuz_osimaiTiles),r0
        add r4,r0
        rts
        nop
      @@notOsimai:
      
      bneR0 r5,ymmPuzOldTeaseTilesPtr,@@notTease
        movC (teaseTilesVerifyStrEnd-teaseTilesVerifyStr),r0
        movC teaseTilesVerifyStr,r1
        mov r5,r2
        macro_memcmp
        bnz r0,@@notTease
        
        movC (ymmpuz_teaseTiles_end-ymmpuz_teaseTiles),r0
        movC ymmpuz_teaseTiles,r5
        macroCopyLoop
        
        ; pretend we decompressed normally
        movC (ymmpuz_teaseTiles_end-ymmpuz_teaseTiles),r0
        add r4,r0
        rts
        nop
      @@notTease:
      
      ; make up work
      mov.l r14,$-r15
      mov.l r13,$-r15
      mov.l r12,$-r15
      mov.l r11,$-r15
      mov.l r10,$-r15
      mov.l r9,$-r15
      mov.l r8,$-r15
      movC 0x0601E396,r2
      jmp $r2
      nop
    .pool
    
    osimaiTilemapVerifyStr:
      .db 0x4F,0x4F,0x4F,0x4F,0x4F,0x47,0x12,0x01
    osimaiTilemapVerifyStrEnd:
    
    osimaiTilesVerifyStr:
      .db 0x0F,0x6D,0xC6,0x03,0xCE,0xCF,0x18,0x03
    osimaiTilesVerifyStrEnd:
    
    
    
    ; src = +0x30
    teaseTilemapVerifyStr:
      .db 0x47,0x00,0x02,0x49,0x1A,0x03,0x40,0x2F
    teaseTilemapVerifyStrEnd:
    
    teaseTilesVerifyStr:
      .db 0x0F,0x76,0x01,0x02,0x17,0x3D,0xC7,0x02
    teaseTilesVerifyStrEnd:

    ;=============================
    ; subtitle block data
    ;=============================
    
    subtitleBlockExists:
      .dh 0
    
    .align 4
    subtitleBlockData:
      .fill subBlockDataMaxSize,0
    .align 2

    ;=============================
    ; force overlay to be shown in most normal scenarios
    ;=============================
    
    doSceneAdvanceFrameSplit:
      sts.l pr,$-r15
        ; r4 = number of frames to advance
        ; save
;        movC @@framesLeft,r0
;        mov.w r4,$r0

;        saveMem.w r4,@@framesLeft,r0

        ; if pending display list has a standard terminator at its end,
        ; convert it to a jump to the overlay list area instead so the
        ; subtitles can appear
        loadMem.w pendingDisplayListSize,r0
        extu.w r0,r0
;        bnz r0,@@doListUpdateCheck
        bze r0,@@noPendingList
        
        ; if current list size zero, check last nonzero list size
        ; and use it instead
        ; (basically, we force a refresh using the most recently available
        ; size. this is needed for occasional scenes where audio is played
        ; without sprite refresh occurring, such as the big multiple choice
        ; scene with the "ghosts" towards the start of the game.)
;        loadMem.w @@lastNonzeroPendingDisplayListSize,r0
;        bze r0,@@noPendingList
        
        @@doListUpdateCheck:
          ; save nonzero size
;          movC @@lastNonzeroPendingDisplayListSize,r1
;          mov.w r0,$r1
          ; overwrite pending list size in case we changed it
;          movC pendingDisplayListSize,r1
;          mov.w r0,$r1
          
          ; target last entry in pending list
          add -1,r0
          shll2 r0
          shll2 r0
          shll r0
          movC pendingDisplayList,r1
          add r1,r0
          
          ; r1 = command word
          mov.w $r0,r1
          ; ignore if not terminator command
          movC 0x8000,r2
          and r1,r2
          tst r2,r2
          bt @@noPendingList
          
            ; replace with jump to overlay area
            movC 0x50000000+(overlayDispSlotJumpCmdVramOffset/8),r2
            mov.l r2,$r0
        @@noPendingList:
        
        ; FIXME: can probably remove this and just do the normal logic now
        
        ; if frame count is 0 (why??), set tick amount to 0;
        ; otherwise, it's 1
/*        mov r4,r0
        cmp/eq 0,r0
        bt @@frameCountZero
          movC 1,r0
        @@frameCountZero:
        saveMem.w r0,@@numFramesToTickPerLoop,r1*/
        
        ; loop one frame at a time until done
        
;        @@loop:
          ; make up work
          mov.l r14,$-r15
          mov.l r13,$-r15
          mov.l r12,$-r15
          mov.l r11,$-r15
          mov.l r10,$-r15
          mov.l r9,$-r15
;          loadMem.w @@numFramesToTickPerLoop,r4
          ; call advanceSceneNFrames with frame count of 1
          movC 0x0601439C,r0
          jsr $r0
          nop
          
          ; tick counter
/*;          mov.w @@framesLeft,r0
;          loadMem.w @@framesLeft,r0
          movC @@framesLeft,r1
          movC 0,r2
          mov.w $r1,r0
          extu.w r0,r0
          add -1,r0
;          cmp/eq 0,r0
;          bt @@done
          ; if counter <= 0, done
          cmp/ge r0,r2
          bt @@done
          
          ; save updated counter
          mov.w r0,$r1
          
          ; reset display list size so it will be refreshed on next advance() call
;          movC currentSpritesDisplayedCount,r0
;          mov.w $r0,r0
          loadMem.w currentSpritesDisplayedCount,r0
          ; this count doesn't include the terminator,
          ; but pendingDisplayListSize does, so add 1
          add 1,r0
          
          ; set pendingDisplayListSize to currentSpritesDisplayedCount+1
;          movC pendingDisplayListSize,r1
;          mov.w r0,$r1
          saveMem.w r0,pendingDisplayListSize,r1
          
          bra @@loop
          nop*/
      
      @@done:
      lds.l $r15+,pr
      rts
      nop
      
      .pool
      
      
    @@lastNonzeroPendingDisplayListSize:
      .dh 0
;      @@numFramesToTickPerLoop:
;      .dh 0
;      @@framesLeft:
;      .dh 0

    ;=============================
    ; TEST
    ;=============================
    
/*    doTest:
      ; make up work
      mov.l r14,$-r15
      mov.l r13,$-r15
      mov.l r12,$-r15
      mov.l r11,$-r15
      mov.l r10,$-r15
      movC 1,r11
      mov.l r9,$-r15
      
      movC 0x25D00010,r0
      mov.w $r0,r0
      
      movC 0x0601A256,r0
      jmp $r0
      nop
      
    .pool */

    ;=============================
    ; 
    ;=============================
    
    doExtraVsyncUpdate:
      ; make up work
      mov.l $r15+,r11
      mov.l $r15+,r12
      mov.l $r15+,r13
      mov.l $r15+,r14
      sts.l pr,$-r15
        movC 0x0604DB64,r3
        jsr $r3
        nop
      lds.l $r15+,pr
      
      ; all normal vsync logic is now done;
      ; follow up with our additions
      
      mov.l r4,$-r15
      sts.l pr,$-r15
      
        ; if not initialized, set up
        loadMem.w overlayInitialized,r0
        bnz r0,@@initialized
          ; write placeholder terminator to overlay jump slot
          movC overlayDispSlotJumpCmdVramCopyTarget,r0
          movC 0x8000,r1
          mov.w r1,$r0
          
          ; mark as initialized
          movC 1,r1
          saveMemR0.w r1,overlayInitialized
        @@initialized:
      
;        movC 0x25D00010,r0
;        mov.w $r0,r0
        
;        movC fontPalettesVramCopyTarget+14,r0
;        mov.w $r0,r0
        
        ;=====
        ; send generated content from previous frame to vram
        ; (this goes first so it's as close to the start of vblank
        ; as possible -- we want it to happen before the game
        ; starts drawing the next frame whenever possible)
        ;=====
        
        ; skip vram send if refresh flag not set,
        ; unless debug mode is on, because then
        ; we need to refresh the extra HUD every frame
;        .if DEBUG_FEATURES_ON == 0
          loadMem.w overlayDisplayRefreshNeeded,r0
          bze r0,@@noOverlaySend
;        .endif
          movC sendOverlayDispSlotsToVram,r0
          jsr $r0
          nop
        @@noOverlaySend:
        
        ;=====
        ; prepare next frame
        ;=====
        
        ; reset count of overlay display slots in preparation
        ; for generating new ones
        movC 0,r0
        saveMem.l r0,currentOverlayDispSlotsUsed,r1
;        saveMem.l r0,currentOverlayShadowSlotsUsed,r1
        
        ; TEST
/*        movC overlayDispSlotMem,r2
        ; command = 0 (textured draw)
        movC 0x0000,r0
        mov.w r0,$r2
        ; link = doesn't matter
        movC 0x0000,r0
        mov.w r0,$(0x02,r2)
        ; draw mode = no end code
        movC 0x0080,r0
        mov.w r0,$(0x04,r2)
        ; color mode (priority = 1)
        movC 0x1000+((fontPalettesBaseOffset/32)<<4),r0
        mov.w r0,$(0x06,r2)
        ; CMDSRCA
        movC (fontVramBaseOffset+(fontCharByteSize*1))/8,r0
        mov.w r0,$(0x08,r2)
        ; CMDSIZE
        movC ((fontCharRawW/8)<<8)+fontCharH,r0
        mov.w r0,$(0x0A,r2)
        ; CMDXA
        movC 64,r0
        mov.w r0,$(0x0C,r2)
        ; CMDYA
        movC 64,r0
        mov.w r0,$(0x0E,r2)
        ; terminator
        add 32,r2
        movC 0x8000,r0
        mov.w r0,$r2
        
        ; TEST
        movC currentOverlayDispSlotsUsed,r1
        mov.l $r1,r0
        add 2,r0
        mov.l r0,$r1*/
        
        ;=====
        ; TEST
        ;=====
        
/*        movC addCharToOverlayDisp,r0
        ; char index
        movC 1,r4
        ; target palette
        movC fontPalettesBaseOffset/32,r5
        ; x
        movC 64,r6
        ; y
        movC 64,r7
        jsr $r0
        nop*/
        
        ;=====
        ; update subtitle streams
        ;=====
        
        ; skip if paused
        loadMem.w overlayPauseFlag,r0
        bnz r0,@@noSubStreamUpdate
          movC subStreamA,r4
          jsrR0 runSubtitleInterpreter
          nop
          
          movC subStreamB,r4
          jsrR0 runSubtitleInterpreter
          nop
          
          .if DEBUG_FEATURES_ON

            ; increment scene timer
            movC sceneTimer,r0
            mov.l $r0,r1
            add 1,r1
            mov.l r1,$r0
            
          .endif
        @@noSubStreamUpdate:
        
        ; TODO:
        ; if conditions required for a subtitle refresh
        ; are not met, skip rendering logic?
        
        ;=====
        ; show debug stuff if on
        ;=====
        
        .if DEBUG_FEATURES_ON
        
          ;=====
          ; DEBUG: display scene timer
          ;=====
          
          movC printOverlayNum,r0
          ; value
  ;        movC 943,r4
          loadMem.l sceneTimer,r4
          ; digit count
          movC 5,r5
          ; x
          movC 8,r6
          ; y
          movC 0x08,r7
          jsr $r0
          nop
        
          ;=====
          ; DEBUG: display current timer values for substreams
          ;=====
          
          ; value
          movC subStreamA,r4
          readOffsetR0.l r4,subStream_timer,r4
          ; digit count
          movC 5,r5
          ; x
          movC 8,r6
          ; y
          movC 0x18,r7
          jsrR0 printOverlayNum
          nop
          
          ; value
          movC subStreamB,r4
          readOffsetR0.l r4,subStream_timer,r4
          ; digit count
          movC 5,r5
          ; x
          movC 8,r6
          ; y
          movC 0x28,r7
          jsrR0 printOverlayNum
          nop
        
          ;=====
          ; DEBUG: display sync counters
          ;=====
          
          ; value
          loadMem.w syncCounter_soundCount,r4
          ; digit count
          movC 2,r5
          ; x
          movC 0x8,r6
          ; y
          movC 0x48,r7
          jsrR0 printOverlayNum
          nop
          
          ; value
          loadMem.w syncCounter_cddaPlay,r4
          ; digit count
          movC 2,r5
          ; x
          movC 0x20,r6
          ; y
          movC 0x48,r7
          jsrR0 printOverlayNum
          nop
          
          ; value
          loadMem.w syncCounter_cddaWait,r4
          ; digit count
          movC 2,r5
          ; x
          movC 0x38,r6
          ; y
          movC 0x48,r7
          jsrR0 printOverlayNum
          nop
          
        .endif
        
        ;=====
        ; render subtitles
        ;=====
        
        movC renderSubtitles,r0
        jsr $r0
        nop
        
        ;=====
        ; render pause menu if on
        ;=====
        
        loadMem.w overlayPauseFlag,r0
        bze r0,@@noPauseMenuDraw
          jsrR0 renderPauseMenu
          nop
          
          ; always refresh during pause
          movC 1,r1
          saveMemR0.w r1,overlayDisplayRefreshNeeded
          
          bra @@pauseMenuDrawDone
          nop
        @@noPauseMenuDraw:
          ; if we unpaused the last frame, refresh
          loadMem.w unpausedLastFrame,r0
          bze r0,@@pauseMenuDrawDone
            movC 1,r1
            saveMemR0.w r1,overlayDisplayRefreshNeeded
            movC 0,r1
            saveMemR0.w r1,unpausedLastFrame
        @@pauseMenuDrawDone:
        
        ;=====
        ; DEBUG: display final count of overlay slots used
        ;=====
        
        .if DEBUG_FEATURES_ON
          
          ; value
          loadMem.l currentOverlayDispSlotsUsed,r4
          ; digit count
          movC 3,r5
          ; add digit count to value (since we'll be using
          ; that many additional slots to show this number)
          add r5,r4
          ; x
          movC 8,r6
          ; y
          movC 0x38,r7
          jsrR0 printOverlayNum
          nop
          
        .endif
        
        ;=====
        ; add display list terminator
        ; (main buffer)
        ;=====
        
        ; note that the shadow buffer doesn't get a terminator --
        ; it's always placed before the main buffer and thus
        ; gets its terminator
      
        ; r2 = pointer to current putpos
        movC currentOverlayDispSlotsUsed,r3
        mov.l $r3,r2
        
        ; increment slot count
        mov r2,r0
        add 1,r0
        mov.l r0,$r3
        
        ; *32
        shll2 r2
        shll2 r2
        shll r2
        movC overlayDispSlotMem,r0
        add r0,r2
        
        movC 0x8000,r0
        mov.w r0,$r2
        
        ;=====
        ; update render buffer timers
        ;=====
        
        ; skip if paused
        loadMem.w overlayPauseFlag,r0
        bnz r0,@@noRenderBufTimerUpdate
          movC subRenderBuf_timer,r0
          
          movC subRenderBufA,r4
          mov.l $(r0,r4),r1
          add 1,r1
          mov.l r1,$(r0,r4)
          ; if active and points exist, refresh always needed
          movC subRenderBuf_srcStrPtr,r0
          mov.l $(r0,r4),r1
          bze r1,@@bufANoPoints
          movC subRenderBuf_numPoints,r0
          mov.l $(r0,r4),r1
          bze r1,@@bufANoPoints
            movC 1,r1
            saveMemR0.w r1,overlayDisplayRefreshNeeded
          @@bufANoPoints:
          
          movC subRenderBuf_timer,r0
          movC subRenderBufB,r4
          mov.l $(r0,r4),r1
          add 1,r1
          mov.l r1,$(r0,r4)
          ; if active and points exist, refresh always needed
          movC subRenderBuf_srcStrPtr,r0
          mov.l $(r0,r4),r1
          bze r1,@@bufBNoPoints
          movC subRenderBuf_numPoints,r0
          mov.l $(r0,r4),r1
          bze r1,@@bufBNoPoints
            movC 1,r1
            saveMemR0.w r1,overlayDisplayRefreshNeeded
          @@bufBNoPoints:
          
        @@noRenderBufTimerUpdate:
      
      lds.l $r15+,pr
      rts
      mov.l $r15+,r4
      
    .pool
    
    .if DEBUG_FEATURES_ON
      .align 4
      sceneTimer:
        .dw 0
    .endif
    
    overlayPauseFlag:
      .dh 0
    
    overlayDisplayRefreshNeeded:
      .dh 0
    
    overlayInitialized:
      .dh 0

    ;=============================
    ; draw the pause menu
    ;=============================
    
    pauseMenu_baseX equ 59
    pauseMenu_baseY equ 124
    pauseMenu_lineSpacing equ 12
    pauseMenu_onBaseX equ 114
    pauseMenu_offBaseX equ 147
    pauseMenu_cursorLeftOffset equ 11
    
    .macro pauseMenu_setUpSrcStruct,rawAddr,rawW,h
      movC (rawAddr/8),r1
      saveMemR0.w r1,renderPauseMenu_srcStruct_srcWord
      movC (rawW/8),r1
      saveMemR0.w r1,renderPauseMenu_srcStruct_w
      movC h,r1
      saveMemR0.w r1,renderPauseMenu_srcStruct_h
    .endmacro
    
    .macro pauseMenu_prepPalForOptionAtIndex,index
      ; default = standard palette
      movC pauseMenuPalIndex_std,r5
      
      loadMem.w pauseMenu_selectedIndex,r1
      bneR0 r1,index,@@notSelected
        movC pauseMenuPalIndex_selected,r5
      @@notSelected:
    .endmacro
    
    .macro pauseMenu_drawSettingsForIndex,index
      ;=====
      ; "on"
      ;=====
    
      ; src data
      pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_on-fontBitmap)),16,10
      ; palette
      ; standard if on, gray if off
      movC pauseMenuPalIndex_std,r5
      loadMem.b (optionsArray+index),r1
      bnz r1,@@drawOn_notOff
        movC pauseMenuPalIndex_off,r5
      @@drawOn_notOff:
      ; x
      movC pauseMenu_baseX+pauseMenu_onBaseX,r6
      ; y
      movC pauseMenu_baseY+(index*pauseMenu_lineSpacing),r7
      movC renderPauseMenu_srcStruct,r4
      jsrR0 addGrpToOverlayDisp
      nop
      
      ;=====
      ; "off"
      ;=====
    
      ; src data
      pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_off-fontBitmap)),24,10
      ; palette
      ; standard if off, gray if on
      movC pauseMenuPalIndex_off,r5
      loadMem.b (optionsArray+index),r1
      bnz r1,@@drawOff_notOff
        movC pauseMenuPalIndex_std,r5
      @@drawOff_notOff:
      ; x
      movC pauseMenu_baseX+pauseMenu_offBaseX,r6
      ; y
      movC pauseMenu_baseY+(index*pauseMenu_lineSpacing),r7
      movC renderPauseMenu_srcStruct,r4
      jsrR0 addGrpToOverlayDisp
      nop
      
      ;=====
      ; if selected, draw cursor next to current option
      ;=====
      
      loadMem.w pauseMenu_selectedIndex,r1
      bneR0 r1,index,@@done
        ; src data
        pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_cursor-fontBitmap)),16,16
        ; palette
        ; standard if off, gray if on
        movC pauseMenuPalIndex_selected,r5
        ; x
        movC (pauseMenu_baseX+pauseMenu_onBaseX-pauseMenu_cursorLeftOffset),r6
        ; if off, use alternate x
        loadMem.b (optionsArray+index),r1
        bnz r1,@@drawCursor_notOff
          movC (pauseMenu_baseX+pauseMenu_offBaseX-pauseMenu_cursorLeftOffset),r6
        @@drawCursor_notOff:
        ; y
        movC (pauseMenu_baseY+(index*pauseMenu_lineSpacing)-2),r7
        movC renderPauseMenu_srcStruct,r4
        jsrR0 addGrpToOverlayDisp
        nop
      @@done:
    .endmacro
    
    buttonCode_up equ 0x1000
    buttonCode_down equ 0x2000
    buttonCode_left equ 0x4000
    buttonCode_right equ 0x8000
    
    pauseMenu_numOptions equ 4
    
    pauseMenu_lastButtonsPressed:
      .dw 0
    
    renderPauseMenu:
      mov.l r8,$-r15
      mov.l r9,$-r15
      sts.l pr,$-r15
      
        ;=====
        ; do input logic
        ; (TODO: move out of rendering routine?)
        ;=====
        
        ; r4 = buttons triggered this frame
        loadMem.w buttonsPressed,r5
        loadMem.w pauseMenu_lastButtonsPressed,r4
        xor r5,r4
        and r5,r4
        ; save last buttons pressed for use next frame
        saveMemR0.w r5,pauseMenu_lastButtonsPressed
        
        ; up pressed?
        movC buttonCode_up,r6
        and r4,r6
        bze r6,@@upNotPressed
          loadMem.w pauseMenu_selectedIndex,r1
          movC 0,r2
          bgt r1,r2,@@noUpWrap
            movC (pauseMenu_numOptions),r1
          @@noUpWrap:
          add -1,r1
          saveMemR0.w r1,pauseMenu_selectedIndex
          
          bra @@buttonCheckDone
          nop
        @@upNotPressed:
        
        ; down pressed?
        movC buttonCode_down,r6
        and r4,r6
        bze r6,@@downNotPressed
          loadMem.w pauseMenu_selectedIndex,r1
          movC (pauseMenu_numOptions-1),r2
          blt r1,r2,@@noDownWrap
            movC -1,r1
          @@noDownWrap:
          add 1,r1
          saveMemR0.w r1,pauseMenu_selectedIndex
          
          bra @@buttonCheckDone
          nop
        @@downNotPressed:
        
        ; left pressed?
        movC buttonCode_left,r6
        and r4,r6
        bze r6,@@leftNotPressed
          ; toggle selected option
/*          loadMem.w pauseMenu_selectedIndex,r1
          movC optionsArray,r0
          mov.b $(r0,r1),r0
          xor 1,r0
          mov r0,r2
          movC optionsArray,r0
          mov.b r2,$(r0,r1)*/
          bra @@rightPressed
          nop
        @@leftNotPressed:
        
        ; right pressed?
        movC buttonCode_right,r6
        and r4,r6
        bze r6,@@rightNotPressed
        @@rightPressed:
          ; toggle selected option
          loadMem.w pauseMenu_selectedIndex,r1
          movC optionsArray,r0
          mov.b $(r0,r1),r0
          xor 1,r0
          mov r0,r2
          movC optionsArray,r0
          mov.b r2,$(r0,r1)
          
          bra @@buttonCheckDone
          nop
        @@rightNotPressed:
        
        @@buttonCheckDone:
      
        ;=====
        ; draw static elements
        ;=====
        
        ; "honorifics"
        pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_honorifics-fontBitmap)),64,10
        pauseMenu_prepPalForOptionAtIndex 0
        movC pauseMenu_baseX+34,r6
        movC pauseMenu_baseY+(pauseMenu_lineSpacing*0),r7
        movC renderPauseMenu_srcStruct,r4
        jsrR0 addGrpToOverlayDisp
        nop
        
        ; "subtitles"
        pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_subtitles-fontBitmap)),56,10
        pauseMenu_prepPalForOptionAtIndex 1
        movC pauseMenu_baseX+40,r6
        movC pauseMenu_baseY+(pauseMenu_lineSpacing*1),r7
        movC renderPauseMenu_srcStruct,r4
        jsrR0 addGrpToOverlayDisp
        nop
        
        ; "signs"
        pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_signs-fontBitmap)),80,10
        pauseMenu_prepPalForOptionAtIndex 2
        movC pauseMenu_baseX+16,r6
        movC pauseMenu_baseY+(pauseMenu_lineSpacing*2),r7
        movC renderPauseMenu_srcStruct,r4
        jsrR0 addGrpToOverlayDisp
        nop
        
        ; "allow skip"
        pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_sceneskip-fontBitmap)),96,10
        pauseMenu_prepPalForOptionAtIndex 3
        movC pauseMenu_baseX+0,r6
        movC pauseMenu_baseY+(pauseMenu_lineSpacing*3),r7
        movC renderPauseMenu_srcStruct,r4
        jsrR0 addGrpToOverlayDisp
        nop
        
        ; skip instructions
        pauseMenu_setUpSrcStruct (fontVramBaseOffset+(fontBitmap_pauseMenu_sceneskipdesc-fontBitmap)),152,10
        movC pauseMenuPalIndex_std,r5
        movC pauseMenu_baseX+0,r6
        movC pauseMenu_baseY+(pauseMenu_lineSpacing*4),r7
        movC renderPauseMenu_srcStruct,r4
        jsrR0 addGrpToOverlayDisp
        nop
        
        bra @@afterPool1
        nop
        .pool
        @@afterPool1:
      
        ;=====
        ; draw on/off text + cursor
        ;=====
        
        pauseMenu_drawSettingsForIndex 0
        pauseMenu_drawSettingsForIndex 1
        
        bra @@afterPool2
        nop
        .pool
        @@afterPool2:
        
        pauseMenu_drawSettingsForIndex 2
        pauseMenu_drawSettingsForIndex 3
      
      lds.l $r15+,pr
      mov.l $r15+,r9
      rts
      mov.l $r15+,r8
      
    .pool
      
      renderPauseMenu_srcStruct:
        ; addr/8
        renderPauseMenu_srcStruct_srcWord:
        .dh 0
        renderPauseMenu_srcStruct_w:
        ; w/8
        .dh 0
        renderPauseMenu_srcStruct_h:
        ; h
        .dh 0
      
      pauseMenu_selectedIndex:
        .dh 0
      
      optionsArray:
        ; nonzero for honorifics on
        option_tagMode:
          .db 1
        option_lowerSubsOn:
          .db 1
        option_upperSubsOn:
          .db 1
        option_allowSceneSkip:
          ; if debug on, scene skip defaults to on;
          ; otherwise, off
          .if DEBUG_FEATURES_ON
            .db 1
          .else
            .db 0
          .endif
      .align 2

    ;=============================
    ; stop streams, stop render bufs,
    ; reset any related state variables
    ;=============================
    
    clearAndResetOverlay:
      sts.l pr,$-r15
      
        ; clear substream srcptrs
        movC subStreamA,r4
        writeOffsetR0R1.l 0,r4,subStream_srcPtr
        movC subStreamB,r4
        writeOffsetR0R1.l 0,r4,subStream_srcPtr
      
        ; clear renderbuf src strings
        movC subRenderBufA,r4
        writeOffsetR0R1.l 0,r4,subRenderBuf_srcStrPtr
        movC subRenderBufB,r4
        writeOffsetR0R1.l 0,r4,subRenderBuf_srcStrPtr
        
        jsrR0 resetOverlaySceneStateVars
        nop
        
        ; refresh (next frame, after render-off takes effect)
        movC 1,r1
        saveMemR0.w r1,unpausedLastFrame
        
      lds.l $r15+,pr
      rts
      nop
      
    .pool

    ;=============================
    ; reset anything that's refreshed at scene start
    ; (sync counters, etc.)
    ;=============================
    
    resetOverlaySceneStateVars:
      sts.l pr,$-r15
      
        ; clear counter memory
        movC syncCounterArray,r0
        movC 0,r4
        movC (syncCounterArrayEnd-syncCounterArray),r5
        @@loop:
          bze r5,@@loopDone
          add -2,r5
          bra @@loop
          mov.w r4,$(r0,r5)
        @@loopDone:
        
        ; reset renderbufs to default palette
        movC fontPalIndex_std,r4
;        movC subRenderBufA,r5
;        writeOffsetR0 r4,r5,subRenderBuf_basePal
;        movC subRenderBufB,r5
;        writeOffsetR0 r4,r5,subRenderBuf_basePal
        saveMemR0.l r4,subRenderBufA+subRenderBuf_basePal
        saveMemR0.l r4,subRenderBufB+subRenderBuf_basePal
        
        ; ensure overlay pause flag is not set
        ; (it should not be if this is happening,
        ; but i'm not 100% sure of how it interacts
        ; with soft resetting, so this is just a precaution)
        movC 0,r4
        saveMemR0.w r4,overlayPauseFlag
        
      @@done:
      lds.l $r15+,pr
      rts
      nop
      
    .pool
    
    syncCounterArray:
      syncCounter_soundCount:
        .dh 0
      syncCounter_cddaPlay:
        .dh 0
      syncCounter_cddaWait:
        .dh 0
      .align 2
    syncCounterArrayEnd:

    ;=============================
    ; add a character to the pending overlay display list
    ; r4 = index
    ; r5 = target palette index (byte address / 32)
    ; r6 = x
    ; r7 = y
    ; returns pointer to the newly added command
    ;=============================
    
    addCharToOverlayDisp:
      sts.l macl,$-r15
      sts.l pr,$-r15
      
        ; r2 = pointer to current putpos
        movC currentOverlayDispSlotsUsed,r3
        mov.l $r3,r2
        
        ; if current slot count == max, do nothing
        ; (-1 to allow for terminator)
        movC numOverlayDispSlots-1,r1
        ; return null pointer
        movC 0,r0
        beq r2,r1,@@done
        
        ; increment slot count
        mov r2,r0
        add 1,r0
        mov.l r0,$r3
        
        ; *32
        shll2 r2
        shll2 r2
        shll r2
        movC overlayDispSlotMem,r0
        add r0,r2
        
        ; command = 0 (textured draw)
        movC 0x0000,r0
        mov.w r0,$r2
        ; link = doesn't matter
;        movC 0x0000,r0
;        mov.w r0,$(0x02,r2)
        ; draw mode = no end code
        movC 0x0080,r0
        mov.w r0,$(0x04,r2)
        ; color mode
        shll2 r5
        shll2 r5
        ; priority = 1
        movC 0x1000,r0
        or r5,r0
        mov.w r0,$(0x06,r2)
        ; CMDSRCA
;        movC fontCharByteSize,r1
;        mulu.w r4,r1
;        sts macl,r0
;        movC fontVramBaseOffset,r1
;        add r1,r0
;        ; /8
;        shlr2 r0
;        shlr r0
        movC fontCharByteSize/8,r1
        mulu.w r4,r1
        sts macl,r0
        movC fontVramBaseOffset/8,r1
        add r1,r0
        mov.w r0,$(0x08,r2)
        ; CMDSIZE
        movC ((fontCharRawW/8)<<8)+fontCharH,r0
        mov.w r0,$(0x0A,r2)
        ; CMDXA
;        movC 64,r0
;        mov.w r0,$(0x0C,r2)
        mov r6,r0
        mov.w r0,$(0x0C,r2)
        ; CMDYA
;        movC 64,r0
;        mov.w r0,$(0x0E,r2)
        mov r7,r0
        mov.w r0,$(0x0E,r2)
      
      ; return pointer to command
      mov r2,r0
      
      @@done:
      lds.l $r15+,pr
      rts
      lds.l $r15+,macl
    .pool

    ;=============================
    ; add a character to the pending overlay shadow list
    ; r4 = index
    ; r5 = target palette index (byte address / 32)
    ; r6 = x
    ; r7 = y
    ; returns pointer to the newly added command.
    ; the specified x/y positions are for the "actual" character
    ; that the shadow applies to;
    ; they are offset up and to the left, and expanded down and to the right,
    ; according to the shadow radius
    ;=============================
    
/*    fontShadowRadius equ 1
    
    addCharToOverlayShadow:
      sts.l macl,$-r15
      sts.l pr,$-r15
      
        ; r2 = pointer to current putpos
        movC currentOverlayShadowSlotsUsed,r3
        mov.l $r3,r2
        
        ; increment slot count
        mov r2,r0
        add 1,r0
        mov.l r0,$r3
        
        ; *32
        shll2 r2
        shll2 r2
        shll r2
        movC overlayShadowSlotMem,r0
        add r0,r2
        
        ; command = 0 (textured draw)
        movC 0x0000,r0
        mov.w r0,$r2
        ; link = doesn't matter
;        movC 0x0000,r0
;        mov.w r0,$(0x02,r2)
        ; draw mode = no end code
        movC 0x0080,r0
        mov.w r0,$(0x04,r2)
        ; color mode
        shll2 r5
        shll2 r5
        ; priority = 1
        movC 0x1000,r0
        or r5,r0
        mov.w r0,$(0x06,r2)
        ; CMDSRCA
        movC fontCharByteSize/8,r1
        mulu.w r4,r1
        sts macl,r0
        movC fontVramBaseOffset/8,r1
        add r1,r0
        mov.w r0,$(0x08,r2)
        ; CMDSIZE
        movC ((fontCharRawW/8)<<8)+fontCharH,r0
        mov.w r0,$(0x0A,r2)
        ; CMDXA
        mov r6,r0
        add 1,r0
        mov.w r0,$(0x0C,r2)
        ; CMDYA
        mov r7,r0
        add 1,r0
        mov.w r0,$(0x0E,r2)
      
      ; return pointer to command
      mov r2,r0
      
      lds.l $r15+,pr
      rts
      lds.l $r15+,macl
    .pool*/

    ;=============================
    ; add a user clipping window set command
    ; to the pending overlay display list
    ; r4 = x
    ; r5 = y
    ; r6 = w
    ; r7 = h
    ; returns pointer to newly added command
    ; 
    ; note that the coordinates are inclusive on the
    ; top/left and exclusive on the bottom/right,
    ; unlike the actual input to the raw command,
    ; which is inclusive on all sides
    ;=============================
    
    addUserClipToOverlayDisp:
      sts.l pr,$-r15
      
        ; r2 = pointer to current putpos
        movC currentOverlayDispSlotsUsed,r3
        mov.l $r3,r2
        
        ; if current slot count == max, do nothing
        ; (-1 to allow for terminator)
        movC numOverlayDispSlots-1,r1
        ; return null pointer
        movC 0,r0
        beq r2,r1,@@done
        
        ; increment slot count
        mov r2,r0
        add 1,r0
        mov.l r0,$r3
        
        ; *32
        shll2 r2
        shll2 r2
        shll r2
        movC overlayDispSlotMem,r0
        add r0,r2
        
        ; command = 8 (user clipping coordinate set)
        movC 0x0008,r0
        mov.w r0,$r2
        ; link = doesn't matter
;        movC 0x0000,r0
;        mov.w r0,$(0x02,r2)
        ; UL coord x
        mov r4,r0
        mov.w r0,$(0x0C,r2)
        ; UL coord y
        mov r5,r0
        mov.w r0,$(0x0E,r2)
        
        ; add w/h to coords
        add r6,r4
        add r7,r5
        ; subtract 1 to account for command taking
        ; inclusive coordinates on right/bottom
        add -1,r4
        add -1,r5
        
        ; LR coord x
        mov r4,r0
        mov.w r0,$(0x14,r2)
        ; LR coord y
        mov r5,r0
        mov.w r0,$(0x16,r2)
      
      ; return pointer to command
      mov r2,r0
      
      @@done:
      lds.l $r15+,pr
      rts
      nop
    .pool

    ;=============================
    ; add a standard graphic to the pending overlay display list
    ; r4 = struct pointer: srcinfo
    ;      - 2b base vram position / 8
    ;      - 2b w / 8
    ;      - 2b h
    ; r5 = palette index
    ; r6 = dstX
    ; r7 = dstY
    ; returns pointer to the newly added command
    ;=============================
    
    addGrpToOverlayDisp:
      sts.l macl,$-r15
      sts.l pr,$-r15
      
        ; r2 = pointer to current putpos
        movC currentOverlayDispSlotsUsed,r3
        mov.l $r3,r2
        
        ; if current slot count == max, do nothing
        ; (-1 to allow for terminator)
        movC numOverlayDispSlots-1,r1
        ; return null pointer
        movC 0,r0
        beq r2,r1,@@done
        
        ; increment slot count
        mov r2,r0
        add 1,r0
        mov.l r0,$r3
        
        ; *32
        shll2 r2
        shll2 r2
        shll r2
        movC overlayDispSlotMem,r0
        add r0,r2
        
        ; command = 0 (textured draw)
        movC 0x0000,r0
        mov.w r0,$r2
        ; link = doesn't matter
;        movC 0x0000,r0
;        mov.w r0,$(0x02,r2)
        ; draw mode = no end code
        movC 0x0080,r0
        mov.w r0,$(0x04,r2)
        ; color mode
        shll2 r5
        shll2 r5
        ; priority = 1
        movC 0x1000,r0
        or r5,r0
        mov.w r0,$(0x06,r2)
        ; CMDSRCA
        readOffsetR0.w r4,0,r0
        mov.w r0,$(0x08,r2)
        ; CMDSIZE
        ; w
        readOffsetR0.w r4,2,r1
        extu.w r1,r1
        shll8 r1
        ; h
        readOffsetR0.w r4,4,r0
        extu.w r0,r0
        add r1,r0
        mov.w r0,$(0x0A,r2)
        ; CMDXA
        mov r6,r0
        mov.w r0,$(0x0C,r2)
        ; CMDYA
        mov r7,r0
        mov.w r0,$(0x0E,r2)
      
      ; return pointer to command
      mov r2,r0
      
      @@done:
      lds.l $r15+,pr
      rts
      lds.l $r15+,macl
    .pool

    ;=============================
    ; print a number to the overlay
    ; r4 = value
    ; r5 = number of digits to display
    ; r6 = x
    ; r7 = y
    ;=============================
    
    printOverlayNum:
      sts.l macl,$-r15
      mov.l r11,$-r15
      mov.l r10,$-r15
      mov.l r9,$-r15
      mov.l r8,$-r15
      sts.l pr,$-r15
      
        mov r4,r8
        mov r5,r9
        mov r6,r10
        mov r7,r11
        
        ; x += digitCount * 8
        shll2 r5
        shll r5
        add r5,r10
        
        @@loop:
          ; x -= 8
          add -8,r10
          
          ; TEMP
          ; returns r0 = num/10
          movC 10,r0
          mov r8,r1
          movC divideStd,r4
          jsr $r4
          nop
          
          ; dividend - divisor * quotient = remainder
          movC 10,r2
          mulu.w r0,r2
          sts macl,r1
          mov r8,r2
          sub r1,r2
          
          ; now:
          ; r0 = quotient
          ; r2 = remainder
        
          ; update running value to quotient
          mov r0,r8
          
          ; print remainder as digit
          movC addCharToOverlayDisp,r3
          ; character index
          movC fontDigitBaseIndex,r4
          add r2,r4
          ; palette index
          movC fontPalIndex_std,r5
          ; x
          mov r10,r6
          ; y
          jsr $r3
          mov r11,r7
          
          ; decrement digit counter
          add -1,r9
          ; loop while nonzero
          tst r9,r9
          bf @@loop
      
      lds.l $r15+,pr
      mov.l $r15+,r8
      mov.l $r15+,r9
      mov.l $r15+,r10
      mov.l $r15+,r11
      rts
      lds.l $r15+,macl
      
    .pool

    ;=============================
    ; send all active overlay display slots to vram
    ;=============================
    
    sendOverlayDispSlotsToVram:
      sts.l pr,$-r15
      
        ; check CEF bit of EDSR, which is set if drawing is
        ; currently occuring.
        ; if so, skip updating the list in vram.
        ; this will by no means catch all errors (drawing
        ; may begin anyway as we are writing the list),
        ; but it will help.
        ; maybe.
        movC 0x02,r1
        movC cpuNoCache+vdp1Edsr,r2
        mov.l $r2,r2
        and r2,r1
        bnz r1,@@done
        
        ;=====
        ; regular buffer
        ;=====
      
        ; size (if not zero)
        loadMem.l currentOverlayDispSlotsUsed,r0
        cmp/eq 0,r0
        bt @@done
        
        ; dst
        ; offset by count of entries in shadow buffer
;        loadMem.l currentOverlayShadowSlotsUsed,r1
;        add r1,r0
        shll2 r0
        shll2 r0
        shll r0
        movC overlayDispSlotBufAVramCopyTarget,r4
        ; apply buffer parity
        loadMem.l currentOverlayDispWriteParity,r3
        bze r3,@@defaultParity
          movC overlayDispSlotBufBVramCopyTarget,r4
        @@defaultParity:
        ;overlayDispSlotJumpCmdVramCopyTarget
        ; before copying, set first entry in VRAM to terminator.
        ; this makes the subtitles "flicker" rather than "glitch"
        ; if the game does end up redrawing while we're in the middle
        ; of the copy loop below.
;        movC 0x8000,r1
;        mov.w r1,$r4
        ; add size to dst to get end position
        add r0,r4
        
        ; size
        loadMem.l currentOverlayDispSlotsUsed,r0
        shll2 r0
        shll2 r0
        shll r0
        
        ; src
        movC overlayDispSlotMem,r5
        
        jsrReg INLINE_copyToVram,r1
        nop
        
        ;=====
        ; shadow buffer
        ;=====
        
/*        loadMem.l currentOverlayShadowSlotsUsed,r0
        cmp/eq 0,r0
        bt @@done
        
        shll2 r0
        shll2 r0
        shll r0
        
        ; dst
        movC overlayDispSlotVramCopyTarget,r4
        ; add size to dst to get end position
        add r0,r4
        ; src
        movC overlayShadowSlotMem,r5
        ; copy
        jsrReg INLINE_copyToVram,r1
        nop*/
        
        ;=====
        ; write jump command for current parity
        ;=====
        
        movC overlayDispSlotBufAVramOffset/8,r4
        loadMem.l currentOverlayDispWriteParity,r3
        bze r3,@@defaultJumpParity
          movC overlayDispSlotBufBVramOffset/8,r4
        @@defaultJumpParity:
        
        movC overlayDispSlotJumpCmdVramCopyTarget,r5
        ; destination
        mov r4,r0
        mov.w r0,$(2,r5)
        ; jump command
        movC 0x5000,r0
        mov.w r0,$(0,r5)
        
        ; update parity
        movC 1,r0
        xor r0,r3
        saveMemR0.l r3,currentOverlayDispWriteParity
          
        ; clear refresh flag
        movC 0,r1
        saveMemR0.w r1,overlayDisplayRefreshNeeded
          
      @@done:
      lds.l $r15+,pr
      rts
      nop
      
    .pool
    
    ; *** NONSTANDARD ***
    ; r0 = size (must be nonzero)
    ; r4 = dst + size
    ; r5 = src
    INLINE_copyToVram:
      
      @@loop:
        add -1,r0
        mov.b $(r0,r5),r1
        mov.b r1,$-r4
        ; loop while counter nonzero
        tst r0,r0
        bf @@loop
          
      @@done:
      rts
      nop
    
    .align 4
    overlayDispSlotMem:
      .fill overlayDispSlotMemSize,0
;    overlayShadowSlotMem:
;      .fill overlayDispSlotMemSize,0
    
    .align 4
    currentOverlayDispSlotsUsed:
      .dw 0
    currentOverlayDispWriteParity:
      .dw 0
;    currentOverlayShadowSlotsUsed:
;      .dw 0

    ;============================================================================
    ; font
    ;============================================================================
      
      ; difference in codepoints between italic/standard charsets
;      font_charSetOffset equ 0x50
      
      fontWidthTable:
        .incbin "out/font/font_scene_width.bin"
      
      fontKerningMatrix:
        .incbin "out/font/font_scene_kern.bin"

      ;==================================
      ; r4 = current char
      ; r5 = previous char
      ; 
      ; returns kerning offset to correctly
      ; position current char relative to
      ; previous
      ;==================================
      
      getCharKerning:
        ; r5 = last char id * chars per matrix row (= number of chars in font)
;        shll8 r5
;        shlr r5
        movC font_numChars,r0
        mulu.w r5,r0
        sts macl,r5
        ; add current char id
        add r4,r5
        ; r0 = pointer to kerning matrix entry
        movC fontKerningMatrix,r0
;        add r5,r0
        
        ; get kerning amount
;        mov.b $r0,r0
        mov.b $(r0,r5),r0
        
        rts
        nop
              
      .pool
      
    ;============================================================================
    ; subtitle interpreter
    ;============================================================================

      ;==================================
      ; opcodes
      ;==================================
      
      subOpsBase equ 0xF0
      
      subOp_terminator  equ 0x00
      subOp_waitUntil   equ subOpsBase+0x00
      subOp_subOff      equ subOpsBase+0x01
      subOp_setSlot     equ subOpsBase+0x02
      subOp_setPal      equ subOpsBase+0x03
      subOp_addPoints   equ subOpsBase+0x04
      subOp_alignMode   equ subOpsBase+0x05
      subOp_syncTo      equ subOpsBase+0x06
      subOp_startStream equ subOpsBase+0x07

      ;==================================
      ; streams
      ;==================================
      
      .align 4
      makeSubStream subStreamA
      makeSubStream subStreamB
      
      .align 4
      subStreamSlotTable:
        .dw subStreamA
        .dw subStreamB
      
      .align 4
;      renderBufferDefaultPaletteTable:
;        .dw fontPalIndex_std
;        .dw fontPalIndex_fg

      ;==================================
      ; starts a subtitle stream in the
      ; specified stream slot
      ; r4 = initial stream pointer
      ; r5 = target stream slot index
      ;==================================
      
      startSubStream:
        ; r6 = target substream pointer
        mov r5,r6
        shll2 r6
        movC subStreamSlotTable,r0
        mov.l $(r0,r6),r6
        
        ; write srcptr
        writeOffsetR0.l r4,r6,subStream_srcPtr
        ; reset timer
        movC 0,r1
        writeOffsetR0.l r1,r6,subStream_timer
        ; default display slot is same as stream slot
        ; (i.e. stream 0 defaults to bottom of screen,
        ; stream 1 defaults to top)
        writeOffsetR0.l r5,r6,subStream_targetSlot
        ; reset to default palette
        movC fontPalIndex_std,r1
        writeOffsetR0.l r1,r6,subStream_basePal
        ; reset to default alignment
        movC subAlignMode_center,r1
        writeOffsetR0.l r1,r6,subStream_alignMode
        
        rts
        nop
      
      .pool

      ;==================================
      ; runs subtitle interpreter on a
      ; substream until a breaking operation
      ; occurs
      ; r4 = target substream pointer
      ;==================================
      
      runSubtitleInterpreter:
        mov.l r8,$-r15
;        mov.l r9,$-r15
        sts.l pr,$-r15
        
          ; r8 = target substream pointer
          mov r4,r8
          
          @@interpreterLoop:
            ; r4 = srcptr
            readOffsetR0.l r8,subStream_srcPtr,r4
            ; do nothing (and don't update timer) if src null
            bze r4,@@done
            
            ; r1 = next opcode
            mov.b $r4,r1
            extu.b r1,r1
            
            ; check if terminator
            bnz r1,@@notTerminator
              ; clear substream's srcptr
              movC 0,r1
              writeOffsetR0.l r1,r8,subStream_srcPtr
              
              bra @@done
              nop
            @@notTerminator:
            
            ; check if opcode
            movC subOpsBase,r2
            blt r1,r2,@@notOpcode
              ; subtract base code and use to call handler
              sub r2,r1
              shll2 r1
              movC subOp_handlerTable,r0
              mov.l $(r0,r1),r1
              
              ; r4 = substream obj pointer
              mov r8,r4
              jsr $r1
              nop
              
              ; handler returns zero if interpreter should break
              bnz r0,@@interpreterLoopBroken
              
              bra @@interpreterLoop
              nop
            @@notOpcode:
            
            ; anything else is assumed to be the start of a string
            jsrR0 startSubString
            mov r8,r4
            ; refresh required due to new string
            movC 1,r1
            saveMemR0.w r1,overlayDisplayRefreshNeeded
            
            bra @@interpreterLoop
            nop
          @@interpreterLoopBroken:
          
          ; increment stream timer
          ; (this happens after running the interpreter
          ; so that frame 0 has timer == 0, etc.)
          movC subStream_timer,r0
          mov.l $(r0,r8),r1
          add 1,r1
          mov.l r1,$(r0,r8)
        
        @@done:
        lds.l $r15+,pr
;        mov.l $r15+,r9
        rts
        mov.l $r15+,r8
      
      .pool
      
      .align 4
      subOp_handlerTable:
        ; 00: subOp_waitUntil
        .dw subOp_waitUntil_handler
        ; 01: subOp_startSub
        .dw subOp_subOff_handler
        ; 02: subOp_endSub
        .dw subOp_setSlot_handler
        ; 03: subOp_setPal
        .dw subOp_setPal_handler
        ; 04: subOp_addPoints
        ; (special-cased in string read code,
        ; dummy value here)
        .dw 0
        ; 05: subOp_alignMode
        .dw subOp_alignMode_handler
        ; 06: subOp_syncTo
        .dw subOp_syncTo_handler
        ; 07: subOp_startStream
        .dw subOp_startStream_handler

      ;==================================
      ; op handlers
      ;
      ; r4 = pointer to substream object
      ; 
      ; handlers must:
      ; - update substream srcptr
      ;   if needed
      ; - return 0 if the interpreter
      ;   should continue running or
      ;   nonzero if not
      ;==================================
      
      ;=====
      ; idle until this stream's timer counter
      ; reaches a given value
      ; args: 2b target time
      ;=====
      
      subOp_waitUntil_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
          
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; advance past opcode
          add 1,r4
          ; r5 = timer
          readOffsetR0.l r8,subStream_timer,r5
          ; r0 = target value
          bsr INLINE_fetchSubOpWord
          nop
          extu.w r0,r0
          ; if timer < target, do nothing and return 1 to break
          bltSlot r5,r0,@@done
          movC 1,r0
          
          ; target time reached: update srcptr
          writeOffsetR0.l r4,r8,subStream_srcPtr
          ; return 0 = continue
          movC 0,r0
        
        @@done:
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ;=====
      ; turn off subtitles in current slot
      ;=====
      
      subOp_subOff_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
          
          ; r4 = target slot num
          readOffsetR0.l r8,subStream_targetSlot,r4
          ; r4 = target renderbuf
          shll2 r4
          movC subRenderBufTable,r0
          mov.l $(r0,r4),r4
          ; clear renderbuf's srcptr
          movC 0,r1
          writeOffsetR0.l r1,r4,subRenderBuf_srcStrPtr
          
          ; advance srcptr
          ; r4 = srcptr
          movC subStream_srcPtr,r0
          mov.l $(r0,r8),r4
          ; advance past opcode
          add 1,r4
          mov.l r4,$(r0,r8)
          
          ; refresh required
          movC 1,r1
          saveMemR0.w r1,overlayDisplayRefreshNeeded
          
          ; return 0 = continue
          movC 0,r0
        
        @@done:
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ;=====
      ; change target render buffer display slot
      ; args: 1b index
      ; 0 = bottom, 1 = top
      ;=====
      
      subOp_setSlot_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
        
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; advance past opcode
          add 1,r4
          
          ; r0 = param byte
          bsr INLINE_fetchSubOpByte
          nop
          ; save to obj
          mov r0,r1
          writeOffsetR0.l r1,r8,subStream_targetSlot
          
          ; update srcptr
          writeOffsetR0.l r4,r8,subStream_srcPtr
          
          ; return 0 = continue
          movC 0,r0
        
        @@done:
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ;=====
      ; set base palette for subtitles to a specified index
      ; args: 1b index
      ; 0 = standard text, 1 = karaoke base color
      ; (note that the actual karaoke color transition effect
      ; uses hardcoded colors)
      ;=====
      
      subOp_setPal_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
        
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; advance past opcode
          add 1,r4
          
          ; r0 = param byte
          bsr INLINE_fetchSubOpByte
          nop
          ; add base palette
          add fontPalBaseIndex,r0
          ; save to obj
          mov r0,r1
;          writeOffsetR0.l r1,r8,subStream_basePal
          ; never mind, this is stupid and results in
          ; having to reset the text color every time
          ; we switch slots during karaoke sequences.
          ; instead, write the target value directly to the
          ; current slot.
          readOffsetR0.l r8,subStream_targetSlot,r2
          shll2 r2
          movC subRenderBufTable,r3
          add r3,r2
          mov.l $r2,r3
          writeOffsetR0.l r1,r3,subRenderBuf_basePal
          
          ; update srcptr
          writeOffsetR0.l r4,r8,subStream_srcPtr
          
          ; return 0 = continue
          movC 0,r0
        
        @@done:
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ;=====
      ; set alignment mode for subtitles to a specified index
      ; args: 1b index
      ; 0 = center, 1 = left (centered to width of longest line)
      ;=====
      
      subOp_alignMode_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
        
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; advance past opcode
          add 1,r4
          
          ; r0 = param byte
          bsr INLINE_fetchSubOpByte
          nop
          ; save to obj
          mov r0,r1
          writeOffsetR0.l r1,r8,subStream_alignMode
          
          ; update srcptr
          writeOffsetR0.l r4,r8,subStream_srcPtr
          
          ; return 0 = continue
          movC 0,r0
        
        @@done:
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ;=====
      ; idle stream until a specific sync counter reaches
      ; a target value, then when it occurs, set the stream
      ; timer to a new value
      ; 
      ; args:
      ; - 1b sync var id:
      ;      0 = count of sounds played this scene
      ;      1 = count of cd audio tracks started this scene
      ;      2 = count of cd audio wait events fully executed this scene
      ; - 2b minimum target counter value before stream continues
      ; - 2b new stream timer value after sync condition reached
      ;=====
      
      subOp_syncTo_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
          
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; advance past opcode
          add 1,r4
          
          ; r0 = param byte
          bsr INLINE_fetchSubOpByte
          nop
          ; r5 = target sync var
          shll r0
          movC syncCounterArray,r1
          mov.w $(r0,r1),r5
          extu.w r5,r5
          
          ; r0 = target value
          bsr INLINE_fetchSubOpWord
          nop
          extu.w r0,r0
          
          ; check if target value reached or exceeded
          ; NOTE: results in returning nonzero = stop interpreter
          blt r5,r0,@@done
          
            ; target reached
          
            ; r1 = new timer
            bsr INLINE_fetchSubOpWord
            nop
            extu.w r0,r1
            ; save to object
            writeOffsetR0.l r1,r8,subStream_timer
            
            ; update srcptr
            writeOffsetR0.l r4,r8,subStream_srcPtr
            
            ; return 0 = continue
            movC 0,r0
        
        @@done:
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ;=====
      ; start a stream in a specified slot
      ; 
      ; args:
      ; - 1b stream number (0 or 1)
      ; - 1b string index in current subtitle block
      ;=====
      
      subOp_startStream_handler:
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = substream obj
          mov r4,r8
          
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; advance past opcode
          add 1,r4
          
          ; r0 = param byte
          bsr INLINE_fetchSubOpByte
          nop
          ; r5 = target slot
          mov r0,r5
          
          ; r0 = param byte
          bsr INLINE_fetchSubOpByte
          nop
          ; r6 = target script index
          mov r0,r6
          
          ; save updated srcptr
          writeOffsetR0.l r4,r8,subStream_srcPtr
      
          ; check if subtitle block exists
          loadMem.w subtitleBlockExists,r4
          ; do nothing if not
          bze r4,@@done
          
          ; look up target script's offset
          mov r6,r0
          ; add 1 to skip scene-start script
          add 1,r0
          ; skip sound block by adding sound count to index
          loadMem.l subtitleBlockData+subBlock_soundCountOffset,r1
          add r1,r0
          ; look up in index
          shll2 r0
          movC subtitleBlockData+subBlock_indexStart,r4
          mov.l $(r0,r4),r1
          ; do nothing if index offset zero (= doesn't exist)
          bze r1,@@done
          
          ; start subtitle stream in stream slot 0
          ; raw index offset += base pointer
          jsrR0 startSubStream
          add r1,r4
        
        @@done:
        ; return 0 = continue
        movC 0,r0
        
        lds.l $r15+,pr
        rts
        mov.l $r15+,r8
      
      .pool
      
      ; *** NONSTANDARD ***
      ; r4 = src
      ; advances src and returns r0 = next byte (unsigned)
      INLINE_fetchSubOpByte:
        mov.b $r4+,r0
;        add 1,r4
        rts
        extu.b r0,r0
      
      ; *** NONSTANDARD ***
      ; r4 = src
      ; advances src and returns r0 = next word
      INLINE_fetchSubOpWord:
        mov.b $r4+,r0
;        add 1,r4
        extu.b r0,r0
        shll8 r0
        
        mov.b $r4+,r1
;        add 1,r4
        extu.b r1,r1
        rts
        or r1,r0

      ;==================================
      ; starts a string for a substream object
      ; r4 = pointer to substream object
      ;==================================
      
      startSubString:
        mov.l r8,$-r15
        mov.l r9,$-r15
        sts.l pr,$-r15
        
          ; r8 = obj
          mov r4,r8
          ; r9 = target slot num
          readOffsetR0.l r8,subStream_targetSlot,r9
          
          ; r4 = srcptr
          readOffsetR0.l r8,subStream_srcPtr,r4
          ; r5 = target slot num
          mov r9,r5
          jsrR0 assignRenderBufString
          nop
          ; r6 = obj
;          mov r4,r6
          
          ; update srcptr
          ; returned pointer = one past end of string
          mov r0,r4
          writeOffsetR0.l r4,r8,subStream_srcPtr
          
          ;=====
          ; update target slot with timer info, etc.
          ;=====
          
          ; r4 = target slot num
          mov r9,r4
          shll2 r4
          movC subRenderBufTable,r0
          ; r4 = target buffer pointer
          mov.l $(r0,r4),r4
          
          ; slot's timer starts at stream's current timer value
          readOffsetR0.l r8,subStream_timer,r1
          writeOffsetR0.l r1,r4,subRenderBuf_timer
          ; copy palette
          ; (removed, this was a dumb idea)
;          readOffsetR0.l r8,subStream_basePal,r1
;          writeOffsetR0.l r1,r4,subRenderBuf_basePal
          ; copy alignment mode
          readOffsetR0.l r8,subStream_alignMode,r1
          writeOffsetR0.l r1,r4,subRenderBuf_alignMode
        
        @@done:
        lds.l $r15+,pr
        mov.l $r15+,r9
        rts
        mov.l $r15+,r8
      
      .pool

      ;==================================
      ; stream string memory
      ;==================================
      
      ; temporary storage for resolving tagged sections
      subStringPrepBuf:
        .fill subString_maxSize,0
      
      ; final storage for word-wrapped output
      subStringBufA:
        .fill subString_maxSize,0
      subStringBufB:
        .fill subString_maxSize,0
      
      .align 4
      subStringBufTable:
        .dw subStringBufA
        .dw subStringBufB

      ;==================================
      ; assigns string to a render buf object
      ; r4 = src string pointer
      ; r5 = buffer slot num
      ; returns one past end of src string
      ;==================================
      
      assignRenderBufString:
        mov.l r8,$-r15
        mov.l r9,$-r15
        mov.l r10,$-r15
        sts.l pr,$-r15
        
          ; r8 = string ptr
          ; (needed later)
;          mov r4,r8
          ; r9 = buffer num
          mov r5,r9
          ; r10 = buffer ptr
          shll2 r5
          movC subRenderBufTable,r0
          mov.l $(r0,r5),r10

          ; r6 = buffer ptr
;          mov r5,r6
;          shll2 r6
;          movC subRenderBufTable,r0
;          mov.l $(r0,r6),r6
          
          ;=====
          ; filter tagged content
          ;=====
          
          ; r5 = dst
          movC subStringPrepBuf,r5
          ; r6 = tag flag (1 = take first section)
          loadMem.b option_tagMode,r6
          ; r7 = tag status (0 = not in tagged section)
;          movC 0,r7
          @@tagFilterLoop:
            ; check for terminator
            mov.b $r4,r1
            add 1,r4
            bze r1,@@tagFilterDone
            extu.b r1,r1
            
            ; tag marker?
            bneR0 r1,font_plusIndex,@@notTagMarker
;              bnz r6,@@takeSecondSection
              bze r6,@@takeSecondSection
              
              @@takeFirstSection:
                bsr INLINE_copyToTagChar
                nop
                bsr INLINE_advanceToTagChar
                nop
                bra @@tagFilterLoop
                nop
              @@takeSecondSection:
                bsr INLINE_advanceToTagChar
                nop
                bsr INLINE_copyToTagChar
                nop
                bra @@tagFilterLoop
                nop
            @@notTagMarker:
            
            ; copy to dst
            mov.b r1,$r5
            bra @@tagFilterLoop
            add 1,r5
          
          .pool
          @@tagFilterDone:
          
          ; r8 = return value
          mov r4,r8
          
          ; add terminator
          movC 0,r1
          mov.b r1,$r5
          
          ;=====
          ; now apply word wrap
          ;=====
          
          ; r9 = final dst stringbuf pointer
          shll2 r9
          movC subStringBufTable,r0
          mov.l $(r0,r9),r9
          
          ; r4 = src
          movC subStringPrepBuf,r4
          ; r5 = dst
          mov r9,r5
          ; r6 = wrap width
          movC maxSubWidth,r6
          jsrR0 wordWrapString
          nop
          
          ;=====
          ; check original string for point data
          ;=====
          
          ; r5 = one past end of dst, which is
          ; where readSubData expects to find the point data
          mov r0,r5
          
          ; check if string is immediately followed by a set-point "op"
          ; (which is not actually handled as a real op)
/*          mov.b $r8,r4
          extu.b r4,r4
          bneR0 r4,subOp_addPoints,@@noPointData
            ; skip opcode
            add 1,r8
            ; r4 = point count
            mov.b $r8+,r4
            ; done if zero
            bze r4,@@noPointData
            
            ; copy all points
            extu.b r4,r4
            @@pointCopyLoop:
              ; read point
              
              
              add -1,r4
              bnz r4,@@pointCopyLoop
            
            
          @@noPointData:*/
          
          ; check if string is immediately followed by any set-point "ops"
          ; (which are handled here, not as real ops)
          @@pointCopyLoop:
            mov.b $r8,r4
            extu.b r4,r4
            bneR0 r4,subOp_addPoints,@@pointCopyDone
              ; skip opcode
              add 1,r8
              
              ; copy 16-bit point value A to dst
              mov.b $r8+,r4
              mov.b r4,$r5
              add 1,r5
              mov.b $r8+,r4
              mov.b r4,$r5
              add 1,r5
              
              ; copy 16-bit point value B to dst
              mov.b $r8+,r4
              mov.b r4,$r5
              add 1,r5
              mov.b $r8+,r4
              mov.b r4,$r5
              add 1,r5
              
              bra @@pointCopyLoop
              nop
          @@pointCopyDone:
          
          
          ;=====
          ; write finalized values to target
          ;=====
          
          ; string pointer
;          writeOffsetR0.l r9,r10,subRenderBuf_srcStrPtr
          
          ; r4 = string pointer
          mov r9,r4
          ; r5 = target buffer pointer
          mov r10,r5
          jsrR0 readSubData
          nop
        
        ; return one past end of src string
        mov r8,r0
        
        @@done:
        lds.l $r15+,pr
        mov.l $r15+,r10
        mov.l $r15+,r9
        rts
        mov.l $r15+,r8
      
      .pool
      
      ; *** NONSTANDARD ***
      ; r4 = src
      ; - r4 = one past next tag marker
      INLINE_advanceToTagChar:
        @@loop:
          mov.b $r4+,r2
          ; shouldn't happen
          bze r2,@@done
          
          extu.b r2,r2
          beqR0 r2,font_plusIndex,@@done
          
          bra @@loop
          nop
        @@done:
        rts
        nop
      
      .pool
      
      ; *** NONSTANDARD ***
      ; r4 = src
      ; r5 = dst
      ; - r4 = one past next tag marker
      ; - r5 = next copy dst
      INLINE_copyToTagChar:
        @@loop:
          mov.b $r4+,r2
          ; shouldn't happen
          bze r2,@@done
          
          extu.b r2,r2
          beqR0 r2,font_plusIndex,@@done
          
          mov.b r2,$r5
          bra @@loop
          add 1,r5
        @@done:
        rts
        nop
      
      .pool

      ;==================================
      ; applies word wrapping to a string
      ; r4 = src string pointer
      ; r5 = dst
      ; r6 = max width
      ; returns one past end of dst
      ;==================================
      
      wordWrapString:
        mov.l r8,$-r15
        mov.l r9,$-r15
        mov.l r10,$-r15
        mov.l r11,$-r15
        sts.l pr,$-r15
        
          ; r8 = src
          mov r4,r8
          ; r9 = dst
          mov r5,r9
          ; r10 = max width
          mov r6,r10
          ; r11 = current line width
          movC 0,r11
          
          @@loop:
            ; get next word width
            bsr getNextWordWidth
            mov r8,r4
            
            ; add to current width
            add r0,r11
            ; if > max width, add newline and skip spaces
            ble r11,r10,@@noBreak
              ; add newline to output
              movC font_newlineIndex,r1
              mov.b r1,$r9
              add 1,r9
              
              ; reset width
              movC 0,r11
              
              ; skip spaces in input
              mov r8,r4
              bsr skipSpaces
              nop
              
              ; redo with spaces now omitted
              bra @@loop
              ; r8 = updated position
              mov r0,r8
            @@noBreak:
            
            ; copy to end of next word
            loadMem.l nextWordWidth_lastEndPos,r2
            @@copyLoop:
              beq r8,r2,@@copyLoopDone
              mov.b $r8+,r3
              mov.b r3,$r9
              bra @@copyLoop
              add 1,r9
            @@copyLoopDone:
            
            ; check for terminator
            mov.b $r8,r3
            extu.b r3,r3
            bze r3,@@done
            
            ; check for manual newline in input
            bneR0 r3,font_newlineIndex,@@notNewline
              ; copy newline to output
              mov.b r3,$r9
              add 1,r9
              add 1,r8
              ; reset width
              movC 0,r11
            @@notNewline:
            
            bra @@loop
            nop
        @@done:
        ; add terminator
        movC 0,r1
        mov.b r1,$r9
        ; return one past end of dst
        add 1,r9
        mov r9,r0
        
        lds.l $r15+,pr
        mov.l $r15+,r11
        mov.l $r15+,r10
        mov.l $r15+,r9
        rts
        mov.l $r15+,r8
        
      .pool
        
      
      ; *** NONSTANDARD ***
      ; r4 = src
      ; r5 = dst
/*      INLINE_copyNextStringWord
          
          ; r2 = non-space seen flag
          movC 0,r2
          @@checkLoop:
            ; r1 = fetch next char
            mov.b $r4,r1
            extu.b r1,r1
            add 1,r1
            
            ; if newline or terminator, done
            bze r1,@@done
            beqR0 r1,font_newlineIndex,@@done
            
            ; copy other control chars as-is
            movC font_controlCharsBase,r3
            bge r1,r3,@@skip
            
            ; if a space and we've seen a non-space character, done
            bneR0 r1,font_spaceIndex,@@notSpace
              bze r2,@@spaceChecksDone
              @@isSpaceAndSpaceFound:
                ; done
                bra @@done
                nop
            @@notSpace:
              ; set non-space found flag
              movC 1,r2
            @@spaceChecksDone:
            
            ; copy to dst
            
            
            bra @@checkLoop
            nop
        
        rts
        nop*/
      
      ; r4 = src
      ; returns r0 = past spaces
      skipSpaces:
        @@skipLoop:
          mov.b $r4,r1
          extu.b r1,r1
          bneR0 r1,font_spaceIndex,@@done
          add 1,r4
          bra @@skipLoop
          nop
        @@done:
        rts
        mov r4,r0
        
      .pool
      
      ; r4 = src
      ; returns r0 = width of next word
      ; (including preceding spaces)
      getNextWordWidth:
        mov.l r8,$-r15
        mov.l r9,$-r15
        mov.l r10,$-r15
        mov.l r11,$-r15
        sts.l pr,$-r15
          
          ; r8 = src
          mov r4,r8
          ; r9 = current width
          movC 0,r9
          ; r10 = previous character (for kerning)
          movC font_spaceIndex,r10
          ; r11 = flag set once non-space character reached
          movC 0,r11
          
          @@checkLoop:
            ; r4 = fetch next char
            mov.b $r8,r4
            extu.b r4,r4
            add 1,r8
            
            ; if newline or terminator, done
            bze r4,@@done
            beqR0 r4,font_newlineIndex,@@done
            
            ; ignore other control chars
            movC font_controlCharsBase,r1
            bge r4,r1,@@checkLoop
            
            ; if a space and we've seen a non-space character, done
            bneR0 r4,font_spaceIndex,@@notSpace
              bze r11,@@spaceChecksDone
              @@isSpaceAndSpaceFound:
                ; done
                bra @@done
                nop
            @@notSpace:
              ; set non-space found flag
              movC 1,r11
            @@spaceChecksDone:
            
            ; add character's width to total
            
            ; r5 = char width
            movC fontWidthTable,r0
            mov.b $(r0,r4),r5
            ; add width to X
            add r5,r9
            
            ; look up kerning if not a space
            ; r5 = previous char id
            mov r10,r5
            movC font_spaceIndex,r0
            cmp/eq r0,r5
            bt/s @@skipKerning
            ; r10 = new char id
            mov r4,r10
              jsrR0 getCharKerning
              nop
              
              ; add kerning offset to X
              add r0,r9
            @@skipKerning:
            
            ; if a hyphen or dash, break, and don't decrement the end position
            ; so that that character will be included in the copied output
/*            beqR0 r4,font_hyphenIndex,@@isBreakingInclusiveChar
            bneR0 r4,font_dashIndex,@@isNotBreakingInclusiveChar
            @@isBreakingInclusiveChar:
              ; ++src
              bra @@doneNoEndInc
              nop
            @@isNotBreakingInclusiveChar:*/
            beqR0 r4,font_hyphenIndex,@@doneNoEndInc
            beqR0 r4,font_enDashIndex,@@doneNoEndInc
            ; dash and ellipsis both get this handling
            beqR0 r4,font_dashIndex,@@isEllipsis
            bneR0 r4,font_ellipsisIndex,@@notEllipsis
            @@isEllipsis:
              ; peek at next character
              mov.b $r8,r1
              extu.b r1,r1
              
              ; if a question mark or exclamation mark, no break
              ; (we do not want to insert a linebreak between
              ; an ellipsis and punctuation)
              beqR0 r1,font_exclamationMarkIndex,@@notEllipsis
              beqR0 r1,font_questionMarkIndex,@@notEllipsis
              
              ; otherwise, break
              bra @@doneNoEndInc
              nop
            @@notEllipsis:
            
            bra @@checkLoop
            nop
        @@done:
        
        ; save end position
        add -1,r8
        
        @@doneNoEndInc:
        saveMemR0.l r8,nextWordWidth_lastEndPos
        
        ; return width
        mov r9,r0
        lds.l $r15+,pr
        mov.l $r15+,r11
        mov.l $r15+,r10
        mov.l $r15+,r9
        rts
        mov.l $r15+,r8
        
      .pool
      
      nextWordWidth_lastEndPos:
        .dw 0

    ;===================================================================
    ; subtitle rendering
    ;===================================================================

      ;==================================
      ; subtitle render buffers
      ;==================================
      
      makeSubRenderBuf subRenderBufA
      makeSubRenderBuf subRenderBufB
      
      .align 4
      subRenderBufTable:
        .dw subRenderBufA
        .dw subRenderBufB
        
      ;==================================
      ; main rendering routine
      ;==================================
      
      ; TEST
/*      testString:
        .string "Te<st> 1\n<line 2 line 2 line 2 line 2 line 2 line 2 line 2>"
        .dh 0x0010,0x0038
        .dh 0x0056,0x012A
      .align 2
      testString:
        .string "This string is long enough that it will trigger an automatic line break to ensure that it fits on the screen"
      .align 2*/
      
      renderSubtitles:
        sts.l pr,$-r15
;        mov.l r8,$-r15
          
          ; TEST
/*          movC testString,r4
          movC subRenderBufA,r5
          jsrReg readSubData,r0
          nop
          
          ; TEST
          movC subRenderBufA,r4
          readOffsetR0.l r4,subRenderBuf_timer,r1
          add 1,r1
          writeOffsetR0.l r1,r4,subRenderBuf_timer*/
          
          ; TEST
/*          movC testString,r4
          movC 0,r5
          jsrR0 assignRenderBufString
          nop
          ; correct palette
          movC subRenderBufA,r4
          movC fontPalIndex_std,r5
          writeOffsetR0.w r5,r4,subRenderBuf_basePal*/
          
          ; bottom
          
          loadMem.b option_lowerSubsOn,r0
          bze r0,@@noLowerSubDraw
            ; target buffer
            movC subRenderBufA,r4
            ; palette index
  ;          movC fontPalIndex_std,r5
            readOffsetR0.l r4,subRenderBuf_basePal,r5
            ; base Y
            movC subtitlesLowerBaseY,r6
            movC renderSubtitleBuffer,r0
            jsr $r0
            nop
          @@noLowerSubDraw:
          
          ; top
          
          loadMem.b option_upperSubsOn,r0
          bze r0,@@noUpperSubDraw
            ; target buffer
            movC subRenderBufB,r4
            ; palette index
  ;          movC fontPalIndex_fg,r5
            readOffsetR0.l r4,subRenderBuf_basePal,r5
            ; base Y
            movC subtitlesUpperBaseY,r6
            movC renderSubtitleBuffer,r0
            jsr $r0
            nop
          @@noUpperSubDraw:
        
        lds.l $r15+,pr
        rts
        nop
;        mov.l $r15+,r8
      
      .pool

      ;==================================
      ; render buffer operations
      ;==================================

      ;==================================
      ; read subtitle data to a render buffer
      ;
      ; r4 = srcptr
      ; r5 = dst subRenderBuf ptr
      ; returns pointer to end of src data
      ;==================================
      
      readSubData_stackSize equ 28
      
      readSubData:
        sts.l macl,$-r15
        mov.l r14,$-r15
        mov.l r13,$-r15
        mov.l r12,$-r15
        mov.l r11,$-r15
        mov.l r10,$-r15
        mov.l r9,$-r15
        mov.l r8,$-r15
        sts.l pr,$-r15
        
          ; r8 = srcptr
          mov r4,r8
          ; r9 = dstptr
          mov r5,r9
          
          ;======
          ; read src string
          ;======
          
          ; r10 = "real" x-pos (within current line)
          movC 0,r10
          ; r11 = point counter
          movC 0,r11
          ; r12 = previous char id
          movC font_spaceIndex,r12
          ; r13 = string pointer start
          mov r8,r13
          ; r14 = "virtual" x-pos (not reset by linebreak)
          movC 0,r14
          
          ; reset buffer line counter
;          movC subRenderBuf_numLines,r0
;          movC 1,r1
;          mov.l r1,$(r0,r9)
          writeOffsetR0R1.l 1,r9,subRenderBuf_numLines
          
          ; set string pointer
;          movC subRenderBuf_srcStrPtr,r0
;          mov.w r8,$(r0,r9)
          writeOffsetR0.l r8,r9,subRenderBuf_srcStrPtr
          
          @@stringReadLoop:
            ; r4 = fetch next char
            mov.b $r8,r4
            extu.b r4,r4
            ; increment src
            add 1,r8
            
            ; done if next char zero == terminator
            tst r4,r4
            bt @@stringReadLoopDone
            
            ; check if this character is '<' or '>',
            ; indicating start/end of a point sequence
            ; (both characters are handled identically;
            ; this is purely for notational convenience)
            movC font_leftAngleBracketIndex,r0
            cmp/eq r0,r4
            bt/s @@isPoint
            movC font_rightAngleBracketIndex,r0
            
            cmp/eq r0,r4
            bf @@notPoint
            
            @@isPoint:
              ; r5 = offset to current position in point data
              movC subTargetPoint_size,r0
              mulu.w r0,r11
              sts macl,r5
              ; r4 = pointer to target position
              movC subRenderBuf_pointData,r4
              add r5,r4
              add r9,r4
              
              ; save point x-pos (virtual)
;              movC subTargetPoint_pixelX,r0
;              mov.w r14,$(r0,r4)
              mov r14,r3
              ; HACK: do some hacks to try to make the timing look right
              mov r11,r0
              and 1,r0
              bze r0,@@notEndpoint
                ; if this is an endpoint (odd-numbered point),
                ; add size of gradient to virtual X.
                ; this means that (more of) the gradient will be
                ; completely gone at the time the endpoint is reached.
                add (colorGradientPixelW/2),r3
                bra @@pointParityDone
                nop
              @@notEndpoint:
                ; ...okay yeah whatever.
                ; this results in a slight "pop-in"
                ; at the start of the new segment,
                ; but it's not very noticeable and arguably
                ; actually looks better...
                add (colorGradientPixelW/2),r3
              @@pointParityDone:
;              writeOffsetR0.l r14,r4,subTargetPoint_pixelX
              writeOffsetR0.l r3,r4,subTargetPoint_pixelX
              
              ; r1 = point's byte position within string
              mov r8,r1
              sub r13,r1
              ; save
;              movC subTargetPoint_bytePos,r0
;              mov.w r1,$(r0,r4)
              writeOffsetR0.l r1,r4,subTargetPoint_bytePos
              
              bra @@stringReadLoop
              ; increment point counter
              add 1,r11
              
            @@notPoint:
            
            ; check if a newline
            movC font_newlineIndex,r0
            cmp/eq r0,r4
            bf @@notNewline
              ; increment line count
              movC subRenderBuf_numLines,r0
              mov.l $(r0,r9),r4
              mov r4,r5
              add 1,r5
              mov.l r5,$(r0,r9)
              
              ; write line's final realX to width array
              ; subtract 1 from line count to get array index
              mov r4,r5
              add -1,r5
              shll2 r5
              add r9,r5
              writeOffsetR0.l r10,r5,subRenderBuf_lineWArray
              
              ; reset last char to space to prevent kerning
              movC font_spaceIndex,r12
              
              bra @@stringReadLoop
              ; reset realX
              movC 0,r10
              
            .pool
            @@notNewline:
            
            ; check if a space
            bneR0 r4,font_spaceIndex,@@notSpace
              ; read width
              readOffsetR0.b r4,fontWidthTable,r1
              ; add to x
              add r1,r10
              ; do NOT add to virtual x.
              ; this results in spaces being skipped in karaoke coloration.
;              add r1,r14
              
              ; set last-char value
              bra @@stringReadLoop
              mov r4,r12
            @@notSpace:
            
            ; r6 = char width
;            movC fontWidthTable,r5
;            add r5,r4
;            mov.b $r5,r6
            movC fontWidthTable,r0
            mov.b $(r0,r4),r6
            ; add width to X
            add r6,r10
            ; add width to virtual X
            add r6,r14
            
            ; look up kerning if not a space
            ; r5 = previous char id
            mov r12,r5
            movC font_spaceIndex,r0
            cmp/eq r0,r5
            bt/s @@skipKerning
            ; r12 = new char id
            mov r4,r12
              jsrReg getCharKerning,r0
              nop
              
              ; add kerning offset to X
              add r0,r10
              ; virtual X
              add r0,r14
            @@skipKerning:
            
            bra @@stringReadLoop
            nop
          @@stringReadLoopDone:
          
          ;======
          ; save results
          ;======
          
          ; width of final line
          readOffsetR0.l r9,subRenderBuf_numLines,r4
          ; subtract 1 from line count to get array index
          add -1,r4
          shll2 r4
          add r9,r4
          writeOffsetR0.l r10,r4,subRenderBuf_lineWArray
          
          ; point count
          writeOffsetR0.l r11,r9,subRenderBuf_numPoints
          ; placeholder point index
          writeOffsetR0R1.l 0,r9,subRenderBuf_currentPointIndex
          ; placeholder timer?
;          writeOffsetR0R1.l 0,r9,subRenderBuf_timer
          
          ;======
          ; read point timing data
          ;======
          
          ; done if no points
          tst r11,r11
          bt @@pointTimeReadLoopDone
            ; r4 = dst for point data
            mov r9,r4
            movC subRenderBuf_pointData,r0
            add r0,r4
            
            @@pointTimeReadLoop:
              ; r1 = fetch word = target time for point
              mov.b $r8+,r1
              extu.b r1,r1
              shll8 r1
;              add 1,r8
              
              mov.b $r8+,r2
              extu.b r2,r2
              or r2,r1
;              add 1,r8
              
              ; copy to dst
              writeOffsetR0.l r1,r4,subTargetPoint_time
              
              ; decrement point counter
              add -1,r11
              tst r11,r11
              bf/s @@pointTimeReadLoop
              ; advance dstptr
              add subTargetPoint_size,r4
          @@pointTimeReadLoopDone:
        
        ;======
        ; return updated srcptr
        ;======
        
        mov r8,r0
        
        lds.l $r15+,pr
        mov.l $r15+,r8
        mov.l $r15+,r9
        mov.l $r15+,r10
        mov.l $r15+,r11
        mov.l $r15+,r12
        mov.l $r15+,r13
        mov.l $r15+,r14
        rts
        lds.l $r15+,macl
      
      .pool
      
      .align 4

      ;==================================
      ; r4 = subRenderBuffer pointer
      ; r5 = target point index
      ;==================================
      
      getSubRenderBufferPointPtr:
;        sts.l macl,$-r15
        
          ; convert index to offset
          movC subTargetPoint_size,r6
          mulu.w r6,r5
          sts macl,r6
          ; add point data base offset
          movC subRenderBuf_pointData,r0
          add r6,r0
          
          ; add buffer pointer
          add r4,r0
        
        rts
;        lds.l $r15+,macl
        nop
      
      .pool
      
      ;==================================
      ; r4 = subRenderBuffer pointer
      ; r5 = base palette index
      ; r6 = base y
      ;==================================
      renderSubtitleBuffer:
        sts.l macl,$-r15
        mov.l r14,$-r15
        mov.l r13,$-r15
        mov.l r12,$-r15
        mov.l r11,$-r15
        mov.l r10,$-r15
        mov.l r9,$-r15
        mov.l r8,$-r15
        sts.l pr,$-r15
          
          ;=====
          ; set up initial parameters
          ;=====
        
          ; r9 = render string ptr
          ; ensure buffer has content
          readOffsetR0.l r4,subRenderBuf_srcStrPtr,r9
          bnz r9,@@bufferHasContent
            bra @@done
            nop
          @@bufferHasContent:
          
          ; r8 = render buffer ptr
          mov r4,r8
          
          ; r13 = y-base
          mov r6,r13
          ; apply y-offset according to number of lines in buffer:
          ; we want content to be centered about the base position,
          ; so we should position the first line at
          ; baseY - ((lineCount * fontLineSeparation) / 2)
          readOffsetR0.l r8,subRenderBuf_numLines,r1
          movC fontLineSeparation,r2
          mulu.w r1,r2
          sts macl,r1
          shlr r1
          sub r1,r13
          
          ; save palette index
          saveMemR0.l r5,renderPalIndex
          
          ; r14 = virtual x
;          mov r5,r14
          movC 0,r14
          
          ; set current line num to 0
          movC 0,r1
          saveMemR0.l r1,currentBufferLineNum
;          saveMemR0.l r1,currentVirtualX
          
          ; r10 = initial x-centering offset: (fullW - actualW) / 2
          movC screenW,r10
          ; r1 = pixel width of first line of buffer
;          readOffsetR0 r4,subRenderBuf_srcStrW,r1
          readOffsetR0.l r8,subRenderBuf_lineWArray,r1
          ; if in left-align mode, use longest line in buffer
          ; instead of first
          readOffsetR0.l r8,subRenderBuf_alignMode,r2
          bneR0 r2,subAlignMode_left,@@notLeftAlign_init
            ; start at second entry in width array
            movC subRenderBuf_lineWArray+4,r2
            add r8,r2
            ; loop counter
            readOffsetR0.l r8,subRenderBuf_numLines,r3
            add -1,r3
            bze r3,@@leftAlignSearchDone
              @@leftAlignSearchLoop:
                mov.l $r2,r4
                ble r4,r1,@@notLonger
                  mov r4,r1
                @@notLonger:
                
                ; move to next array position
                add 4,r2
                ; decrement counter
                add -1,r3
                bnz r3,@@leftAlignSearchLoop
            @@leftAlignSearchDone:
            ; save longest line for future use
            saveMemR0.w r1,currentLongestLine
          @@notLeftAlign_init:
          sub r1,r10
          shlr r10
          
          ;=====
          ; if more point pairs remain in buffer,
          ; check if (current time >= next start point's time).
          ; if so, advance current point pointer.
          ;=====
          
          readOffsetR0.l r8,subRenderBuf_currentPointIndex,r5
          readOffsetR0.l r8,subRenderBuf_numPoints,r1
          ; r5 = next start point's index
          add 2,r5
          ; if on final point pair, do nothing:
          ; final span lasts "infinitely" until interrupted
          bge r5,r1,@@noPointChange
            ; look up next start point's buffer
            jsrReg getSubRenderBufferPointPtr,r0
            mov r8,r4
            
            ; r3 = result buffer pointer
            mov r0,r3
            
            ; r4 = current time
            readOffsetR0.l r8,subRenderBuf_timer,r4
            ; r5 = next point's start time
            readOffsetR0.l r3,subTargetPoint_time,r5
            
            ; no change if current time less than next start point's time
            blt r4,r5,@@noPointChange
              movC subRenderBuf_currentPointIndex,r0
              mov.l $(r0,r8),r1
              ; move to next point
              add 2,r1
              mov.l r1,$(r0,r8)
          @@noPointChange:
          
          ;=====
          ; compute completion rate of current point span
          ;=====
          
          movC 0,r1
          saveMemR0.l r1,currentSpanStartX
          saveMemR0.l r1,currentSpanEndX
          
          ; fill percentage is zero if no points
          readOffsetR0.l r8,subRenderBuf_numPoints,r2
;          movC currentSpanGradEndX,r4
;          mov.l r1,$r4
          saveMemR0.l r1,currentSpanGradEndX
          bze r2,@@noFillPercentCheck
            
            ;=====
            ; compute completion percentage of current span:
            ; fillPercent = (currentTime - spanStartTime) / (endTime - startTime)
            ;=====
            
            ; r11 = start point's buffer
            readOffsetR0.l r8,subRenderBuf_currentPointIndex,r5
            jsrReg getSubRenderBufferPointPtr,r0
            mov r8,r4
            mov r0,r11
            
            ; r12 = end point's buffer
            readOffsetR0.l r8,subRenderBuf_currentPointIndex,r5
            mov r8,r4
            jsrReg getSubRenderBufferPointPtr,r0
            add 1,r5
            mov r0,r12
            
            ; fill percentage is zero if start time not yet reached
            ; (should only happen on first point)
            readOffsetR0.l r8,subRenderBuf_timer,r4
            readOffsetR0.l r11,subTargetPoint_time,r5
            movC currentSpanGradEndX,r6
            movC 0,r0
            mov.l r0,$r6
            blt r4,r5,@@noFillPercentCheck
            
            ; r4 = (currentTime - spanStartTime)
            ; this should technically be capped at 0xFFFF,
            ; but since it would take 18 minutes to reach that high,
            ; it's really not a problem in practice
            sub r5,r4
            
            ; r7 = spanTime = (spanEndTime - spanStartTime)
            readOffsetR0.l r12,subTargetPoint_time,r7
            sub r5,r7
            
            ; r4 = (currentTime - spanStartTime) / (endTime - startTime),
            ; as fixed-point with 16 bits of precision
            ; we're assuming endTime - startTime is nonzero
            ; (there is no reason it should ever not be)
            mov r4,r1
            shll8 r1
            shll8 r1
            mov r7,r0
            jsrReg divideStd,r3
            nop
            mov r0,r4
            
            ;=====
            ; compute how many pixels into the span the gradation extends:
            ; gradePixelEndX = (spanEndX - spanStartX) * fillPercent
            ; gradePixelStartX = gradePixelEndX - gradientWidth
            ; everything left of the startX is pure shade;
            ; everything right of the endX is pure base;
            ; everything between uses an intermediate shade
            ;=====
            
            ; r2 = spanWidth = (spanEndX - spanStartX)
            readOffsetR0.l r12,subTargetPoint_pixelX,r2
            readOffsetR0.l r11,subTargetPoint_pixelX,r3
            saveMemR0.l r2,currentSpanEndX
            saveMemR0.l r3,currentSpanStartX
            sub r3,r2
            
            ; multiply by percentage
;            mulu.w r2,r4
            mul.l r2,r4
            ; take high word to get pixel result
            sts macl,r2
            shlr8 r2
            shlr8 r2
            ; save to currentSpanGradEndX
            mov.l r2,$r6
            
            bra @@noFillPercentCheck
            nop
            
          .pool
          @@noFillPercentCheck:
          
          ; compute gradient start pos
          
          loadMem.l currentSpanGradEndX,r2
          add -colorGradientPixelW,r2
          saveMemR0.l r2,currentSpanGradStartX
          
          ;=====
          ; render characters
          ;=====
          
          ; r11 = previous character id (dummy for now)
          movC font_spaceIndex,r11
          
          @@charRenderLoop:
            ; r12 = fetch from src
            mov.b $r9,r12
            extu.b r12,r12
            add 1,r9
            
            ; done if terminator
            bze r12,@@charRenderLoopDone
            
            ; ignore if a target point marker
            beqR0 r12,font_leftAngleBracketIndex,@@charRenderLoop
            beqR0 r12,font_rightAngleBracketIndex,@@charRenderLoop
            
            ; check if a newline
            bneR0 r12,font_newlineIndex,@@notNewline
              ; update line num
              movC currentBufferLineNum,r3
              mov.l $r3,r2
              add 1,r2
              mov.l r2,$r3
              
              ; if in left-align mode, use previously computed longest line length
              ; to determine next line's starting x-position
              readOffsetR0.l r8,subRenderBuf_alignMode,r3
              bneR0 r3,subAlignMode_left,@@notLeftAlign_reload
                loadMem.w currentLongestLine,r1
                bra @@lineWidthFound_reload
                nop
              @@notLeftAlign_reload:
                ; r10 = new line x-centering offset: (fullW - actualW) / 2
                shll2 r2
                add r8,r2
                readOffsetR0.l r2,subRenderBuf_lineWArray,r1
              @@lineWidthFound_reload:
              
              movC screenW,r10
              sub r1,r10
              shlr r10
              
              ; reset last-char value to prevent kerning
              movC font_spaceIndex,r11
              
              bra @@charRenderLoop
              ; add line height to y-pos
              add fontLineSeparation,r13
            @@notNewline:
            
            ; check if a space and skip rendering if so
            bneR0 r12,font_spaceIndex,@@notSpace
              ; read width
              readOffsetR0.b r12,fontWidthTable,r1
              ; add to x
              add r1,r10
              ; do NOT add to virtual x.
              ; this results in spaces being skipped in karaoke coloration.
;              add r1,r14
              
              ; set last-char value
              bra @@charRenderLoop
              mov r12,r11
            @@notSpace:
            
            ;======
            ; apply kerning
            ;======
            
            ; apply kerning if prevous char not a space
            beqSlotR0 r11,font_spaceIndex,@@skipKerning
            ; r5 = previous char id
            mov r11,r5
              jsrR0 getCharKerning
              mov r12,r4
              
              ; add kerning offset to X
              add r0,r10
              ; virtual x
              add r0,r14
            @@skipKerning:
            ; r11 = new char id
            mov r12,r11
            
            ;======
            ; render new char
            ;======
            
            ; r4 = render buffer pointer
            ; r5 = codepoint
            mov r8,r4
            mov r12,r5
            ; r6 = param struct
            ;      - 2b dstX
            ;      - 2b dstY
            ;      - 2b base palette index
            ;      - 2b bytepos of this char in src string
            ;      - 2b virtual x
            ;      - 2b charW
            movC subtitleCharParam,r6
            ; X
            writeOffsetR0.w r10,r6,0
            ; Y
            writeOffsetR0.w r13,r6,2
            ; base palette index
            loadMem.l renderPalIndex,r1
            writeOffsetR0.w r1,r6,4
            ; bytepos
            readOffsetR0.l r8,subRenderBuf_srcStrPtr,r1
            mov r9,r2
            sub r1,r2
            ; we incremented our src earlier, so account for that
            add -1,r2
            writeOffsetR0.w r2,r6,6
            ; virtual x
            writeOffsetR0.w r14,r6,8
            
            ;======
            ; update width
            ;======
            
            ; read width
            readOffsetR0.b r12,fontWidthTable,r1
            ; save as param
            writeOffsetR0.w r1,r6,10
            ; add to x
            add r1,r10
            ; virtual x
            add r1,r14
            
            ;======
            ; render char
            ;======
            
            jsrR0 renderSubtitleChar
            nop
            
            bra @@charRenderLoop
            nop
          @@charRenderLoopDone:
          
        @@done:
        lds.l $r15+,pr
        mov.l $r15+,r8
        mov.l $r15+,r9
        mov.l $r15+,r10
        mov.l $r15+,r11
        mov.l $r15+,r12
        mov.l $r15+,r13
        mov.l $r15+,r14
        rts
        lds.l $r15+,macl
      
      .pool
      
      subtitleCharParam:
        .dh 0 ; 0  = dstX
        .dh 0 ; 2  = dstY
        .dh 0 ; 4  = base palette index
        .dh 0 ; 6  = bytepos of char in src string
        .dh 0 ; 8  = virtual x-pos of char in src string
        .dh 0 ; 10 = charW
      .align 4
      
      currentBufferLineNum:
        .dw 0
      currentSpanStartX:
        .dw 0
      currentSpanEndX:
        .dw 0
      currentSpanGradStartX:
        .dw 0
      currentSpanGradEndX:
        .dw 0
      renderPalIndex:
        .dw 0
      currentLongestLine:
        .dw 0

      ;==================================
      ; r4 = render buffer pointer
      ; r5 = codepoint
      ; r6 = pointer to struct:
      ;      - 2b dstX
      ;      - 2b dstY
      ;      - 2b base clut
      ;      - 2b bytepos of this char in src string
      ;      - 2b virtual x-pos (raw x within string,
      ;           ignoring on-screen positioning and linebreaks)
      ;      - 2b charW
      ;=================================
      
      renderSubtitleChar:
        mov.l r14,$-r15
        mov.l r13,$-r15
        mov.l r12,$-r15
        mov.l r11,$-r15
        mov.l r10,$-r15
        mov.l r9,$-r15
        mov.l r8,$-r15
        sts.l pr,$-r15
          
          mov r4,r8
          mov r5,r9
          mov r6,r10
          
          ;=====
          ; draw shadow
          ; (doesn't have to be first because of the internal
          ; layering that's being done, but why not)
          ;=====
          
          ; r4 = codepoint
/*          mov r9,r4
          ; r5 = target palette index
;          readOffsetR0.w r10,4,r5
          movC fontPalIndex_dropshad,r5
          ; r6 = x
          readOffsetR0.w r10,0,r6
          ; r7 = y
          readOffsetR0.w r10,2,r7
          jsrR0 addCharToOverlayShadow
          nop*/
          
          ;=====
          ; check active points in buffer.
          ; if none, everything is base color.
          ; (no need to check, this will be caught below since
          ; currentSpanGradEndX is zeroed in this case)
          ;=====
          
          ;=====
          ; if char bytepos is < start point,
          ; use all shaded color.
          ; do NOT do this if (current time > active span end time),
          ; because we need to allow the gradient to "flow out".
          ;=====
          
          ;=====
          ; if char bytepos is >= end point,
          ; or (current time < active span start time), <-- already checked
          ; use all base color
          ;=====
          
          ;=====
          ; otherwise, we're in the active zone.
          ; compute our pixel offset within the current span:
          ; ourSpanStartX = (thisCharStringX - spanStartX)
          ; ourSpanEndX = ourSpanStartX + charW
          ;=====
          
          ; r4 = virtual x (thisCharStringX)
          readOffsetR0.w r10,8,r4
          ; subtract currentSpanStartX to get ourSpanStartX
          loadMem.l currentSpanStartX,r5
          sub r5,r4
          
          ; r11 = charW
          readOffsetR0.w r10,10,r11
          
          ; r6 = currentSpanGradStartX
          loadMem.l currentSpanGradStartX,r6
          ; r7 = currentSpanGradEndX
          loadMem.l currentSpanGradEndX,r7
          
          ;=====
          ; compute three line segments relative to our position:
          ; - pure shade
          ;   - left point: zero
          ;   - right point: currentSpanGradStartX - ourSpanStartX
          ;     - clamp(0, charW)
          ; - pure base
          ;   - left point: currentSpanGradEndX - ourSpanStartX
          ;     - clamp(0, charW)
          ;   - right point: charW
          ; - gradient area
          ;   - left point: currentSpanGradStartX - ourSpanStartX
          ;     - cap at charX, but allow to be negative
          ;   - right point: currentSpanGradEndX - ourSpanStartX
          ;     - clamp(0, charW)
          ;=====
          
          ; r12 = left point of gradient = currentSpanGradStartX - ourSpanStartX
          subTo r6,r4,r12
          ; r13 = right point of gradient = currentSpanGradEndX - ourSpanStartX
          subTo r7,r4,r13
          
          ; check if we fall outside the byte boundaries of the active span
          readOffsetR0.l r8,subRenderBuf_numPoints,r1
          bze r1,@@noPoints
            ; r0 = start point's buffer
            readOffsetR0.l r8,subRenderBuf_currentPointIndex,r5
            jsrR0 getSubRenderBufferPointPtr
            mov r8,r4
            
            ; r1 = span's byte start
            mov r0,r1
            readOffsetR0.l r1,subTargetPoint_bytePos,r1
            ; r4 = this char's byte position
            readOffsetR0.w r10,6,r4
            bge r4,r1,@@notToLeft
            ; we are to the left of the span
              ; force shade coloration
              ; pretend the gradient is just to the right of us
;              movC colorGradientPixelW,r13
              movC colorGradientPixelW,r13
              add r11,r13
              bra @@noPoints
              mov r11,r12
            @@notToLeft:
            
            ; r0 = end point's buffer
            readOffsetR0.l r8,subRenderBuf_currentPointIndex,r5
            mov r8,r4
            jsrR0 getSubRenderBufferPointPtr
            add 1,r5
            
            ; r1 = span's byte end
            mov r0,r1
            readOffsetR0.l r1,subTargetPoint_bytePos,r1
            ; r4 = this char's byte position
            readOffsetR0.w r10,6,r4
            blt r4,r1,@@notToRight
              ; force base coloration
              ; pretend the gradient is just to the left of us
              movC -colorGradientPixelW,r12
              bra @@noPoints
              movC 0,r13
            @@notToRight:
            
          @@noPoints:
          
          ;=====
          ; draw shade area (unless width is zero or less)
          ;=====
          
          movC 0,r0
          ble r12,r0,@@noShade
            ; set up call to renderSubtitleCharSegment
          
            ; ensure shade width <= total width
            mov r12,r7
            blt r12,r11,@@noShadeCap
            ; if right endpoint >= charW
              ; set target to charW
;              mov r11,r7
              ; set target to 0, signalling the full width of a raw character
              ; should be drawn
              movC 0,r7
            @@noShadeCap:
            
            ; set up param struct
            movC renderSubtitleCharSegment_param,r5
            ; dstX
            readOffsetR0.w r10,0,r2
            ; dstY
            readOffsetR0.w r10,2,r3
            writeOffsetR0.w r2,r5,0
            writeOffsetR0.w r3,r5,2
            ; palette index
            movC fontPalIndex_bg,r2
            writeOffsetR0.w r2,r5,4
            
            ; x-offset
            movC 0,r6
            jsrR0 renderSubtitleCharSegment
            ; codepoint
            mov r9,r4
            
          @@noShade:
          
          ;=====
          ; draw base area (unless width is zero or less)
          ;=====
          
          ; if base x >= charW, do nothing
          bge r13,r11,@@noBase
            ; ensure base x >= 0
            mov r13,r6
            movC 0,r0
            bgt r13,r0,@@noBaseCap
            ; if left endpoint <= zero
              ; set target to zero
              movC 0,r6
              bra @@baseWidthSet
              ; set width to zero to signal full width
              movC 0,r7
            @@noBaseCap:
            
              ; width
              ; charW - leftX
              subTo r11,r6,r7
            
            @@baseWidthSet:
            
            ; set up param struct
            movC renderSubtitleCharSegment_param,r5
            ; dstX
            readOffsetR0.w r10,0,r2
            ; dstY
            readOffsetR0.w r10,2,r3
            writeOffsetR0.w r2,r5,0
            writeOffsetR0.w r3,r5,2
            ; palette index
            readOffsetR0.w r10,4,r2
            writeOffsetR0.w r2,r5,4
            
            jsrR0 renderSubtitleCharSegment
            ; codepoint
            mov r9,r4
          @@noBase:
          
          ;=====
          ; draw gradient by iterating over each pixel column
          ; starting from its left boundary (which may be negative).
          ; if x relative to us is negative for that pixel column,
          ; or is >= charX,
          ; skip it; otherwise, draw it.
          ;=====
          
          ; skip drawing if no columns intersect target area
          mov r12,r1
          add colorGradientPixelW,r1
          mov 0,r0
          ble r1,r0,@@gradientDrawLoopDone
          
          ; r4 = pixel column counter
;          loadMem.l charRenderPixelColCounter,r4
          movC 0,r4
          movC charRenderPixelColCounter,r1
          mov.l r4,$r1
          @@gradientDrawLoop:
            ; r6 = target column x
            addTo r4,r12,r6
            ; skip if negative
            movC 0,r0
            blt r6,r0,@@gradientDrawLoopNext
            ; done if >= charW
            bge r6,r11,@@gradientDrawLoopDone
            
            ; set up param struct
            movC renderSubtitleCharSegment_param,r5
            ; dstX
            readOffsetR0.w r10,0,r2
            ; dstY
            readOffsetR0.w r10,2,r3
            writeOffsetR0.w r2,r5,0
            writeOffsetR0.w r3,r5,2
            ; palette index
            movC fontPalIndex_gradientBase,r1
            ; adjust gradient level according to column
            add r4,r1
            writeOffsetR0.w r1,r5,4
            
            ; width
            movC 1,r7
            jsrR0 renderSubtitleCharSegment
            ; codepoint
            mov r9,r4
            
            ; loop over all gradient columns
            @@gradientDrawLoopNext:
            ; increment pixel column counter
            movC charRenderPixelColCounter,r1
            mov.l $r1,r4
            add 1,r4
            mov.l r4,$r1
            bneR0 r4,colorGradientPixelW,@@gradientDrawLoop
          @@gradientDrawLoopDone:
          
        @@done:
        lds.l $r15+,pr
        mov.l $r15+,r8
        mov.l $r15+,r9
        mov.l $r15+,r10
        mov.l $r15+,r11
        mov.l $r15+,r12
        mov.l $r15+,r13
        rts
        mov.l $r15+,r14
      
      .pool
      
      renderSubtitleCharSegment_param:
        .dh 0 ; 0 = dstX
        .dh 0 ; 2 = dstY
        .dh 0 ; 4 = palette index
      .align 4
      
      .align 4
      charRenderPixelColCounter:
        .dw 0

      ;==================================
      ; draws a horizontal "slice" of a
      ; character
      ;
      ; r4 = char codepoint
      ; r5 = param struct pointer
      ;      - 2b dstX (of entire char)
      ;      - 2b dstY (of entire char)
      ;      - 2b clut
      ; r6 = x-offset of start of slice
      ;      (should not exceed 16)
      ; r7 = width of slice
      ;      (should not exceed 16)
      ; if both r6 and r7 are zero,
      ; the character is drawn "raw";
      ; otherwise, the user clipping window
      ; is set and reset before and after
      ; the character draw command so that
      ; it appears only in the specified area.
      ;=================================
      
      renderSubtitleCharSegment:
        mov.l r9,$-r15
        mov.l r8,$-r15
        sts.l pr,$-r15
          
          ; if slice x and width are both set to 0,
          ; draw the full character as-is
          mov r6,r1
          or r7,r1
          bnz r1,@@notRaw
            
            ; dstX
            readOffsetR0.w r5,0,r6
            ; dstY
            readOffsetR0.w r5,2,r7
            ; palette index
            readOffsetR0.w r5,4,r5
            jsrR0 addCharToOverlayDisp
            nop
            
            bra @@done
            nop
          @@notRaw:
          
          mov r4,r8
          mov r5,r9
;          mov r6,r10
;          mov r7,r11
          
          ; add user clipping coordinate command
          
          ; r4 = x (dstX + sliceStart)
          readOffsetR0.w r5,0,r4
          add r6,r4
          ; r5 = y
          readOffsetR0.w r5,2,r5
          ; r6 = w
          mov r7,r6
          ; r7 = h
          movC fontCharH,r7
          
          jsrR0 addUserClipToOverlayDisp
          nop
          
          ; add character draw command
          
          ; char ID
          mov r8,r4
          ; dstX
          readOffsetR0.w r9,0,r6
          ; dstY
          readOffsetR0.w r9,2,r7
          ; palette index
          readOffsetR0.w r9,4,r5
          jsrR0 addCharToOverlayDisp
          nop
          
          ; enable user clipping for the character
          
          ; do nothing if result null (adding character failed)
          bze r0,@@done
          
          ; r4 = pointer to returned draw command
          mov r0,r4
          
          ; set user clipping bit
          mov.w $(0x04,r4),r0
          movC 0x0400,r1
          or r1,r0
          mov.w r0,$(0x04,r4)
          
        @@done:
        lds.l $r15+,pr
        mov.l $r15+,r8
        rts
        mov.l $r15+,r9
      
      .pool
      
    
  .endarea
  
;  test:
;  mov 5,r0
;  
;  .pool
  
  
.close

;===================================================================
; extras
;===================================================================

.open "out/files/MINISND.ABK",0x002A0000
  
  ; add at end of file
  .orga 0x49AFC
  
  ymmpuz_titleTiles:
    .incbin "out/rsrc/ymmpuz_title_tiles.bin"
  ymmpuz_titleTiles_end:
  ymmpuz_titleTilemap:
    .incbin "out/rsrc/ymmpuz_title_tilemap.bin"
  ymmpuz_titleTilemap_end:
  
  ymmpuz_kabe0Tiles:
    .incbin "out/rsrc/ymmpuz_kabe0_tiles.bin"
  ymmpuz_kabe0Tiles_end:
  ymmpuz_kabe0Tilemap:
    .incbin "out/rsrc/ymmpuz_kabe0_tilemap.bin"
  ymmpuz_kabe0Tilemap_end:
  
  ymmpuz_osimaiTiles:
    .incbin "out/rsrc/ymmpuz_osimai_tiles.bin"
  ymmpuz_osimaiTiles_end:
  ymmpuz_osimaiTilemap:
    .incbin "out/rsrc/ymmpuz_osimai_tilemap.bin"
  ymmpuz_osimaiTilemap_end:
  
  ymmpuz_teaseTiles:
    .incbin "out/rsrc/ymmpuz_tease_tiles.bin"
  ymmpuz_teaseTiles_end:
  ymmpuz_teaseTilemap:
    .incbin "out/rsrc/ymmpuz_tease_tilemap.bin"
  ymmpuz_teaseTilemap_end:
  
  
  
.close
