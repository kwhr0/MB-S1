/** @file cocoa_volumepanel.h

 HITACHI BASIC MASTER LEVEL3 Mark5 Emulator 'EmuB-6892'
 HITACHI MB-S1 Emulator 'EmuB-S1'
 Skelton for retropc emulator
 SDL edition + Cocoa GUI

 @author Sasaji
 @date   2015.04.30 -

 @brief [ volume panel ]
 */

#ifndef COCOA_VOLUMEPANEL_H
#define COCOA_VOLUMEPANEL_H

#import <Cocoa/Cocoa.h>
#import "cocoa_basepanel.h"

#if defined(_MBS1)
#define VOLUME_ITEMS 12
#else
#define VOLUME_ITEMS 7
#endif

@interface CocoaVolumePanel : CocoaBasePanel
{
	int  *p_volume[VOLUME_ITEMS];
	bool *p_mute[VOLUME_ITEMS];
	CocoaLabel *p_lbl[VOLUME_ITEMS];
}
- (id)init;
- (NSInteger)runModal;
- (void)close;
- (void)dialogClose:(id)sender;

- (void)setPtr;
- (void)setVolumeText:(int)idx;

- (bool)mute:(int)idx;
- (int)volume:(int)idx;

- (void)changeSlider:(CocoaSlider *)sender;
- (void)changeMute:(CocoaCheckBox *)sender;
@end

#endif /* COCOA_VOLUMEPANEL_H */