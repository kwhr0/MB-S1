/** @file cocoa_volumepanel.mm

 HITACHI BASIC MASTER LEVEL3 Mark5 Emulator 'EmuB-6892'
 HITACHI MB-S1 Emulator 'EmuB-S1'
 Skelton for retropc emulator
 SDL edition + Cocoa GUI

 @author Sasaji
 @date   2015.04.30 -

 @brief [ volume panel ]
 */

#import "cocoa_gui.h"
#import "cocoa_volumepanel.h"
#import "../../config.h"
#import "../../emu.h"
#import "../../clocale.h"

extern EMU *emu;

@implementation CocoaVolumePanel
- (id)init
{
	const CMsg::Id t[VOLUME_ITEMS] = {
		CMsg::Master,
		CMsg::Beep,
#if defined(_MBS1)
		CMsg::PSG,
		CMsg::ExPSG_CR_FM,
		CMsg::ExPSG_CR_SSG,
		CMsg::OPN_CR_FM,
		CMsg::OPN_CR_SSG,
#endif
		CMsg::PSG6_CR,
		CMsg::PSG9_CR,
		CMsg::Relay,
		CMsg::CMT,
		CMsg::FDD
	};

	[super init];

	[self setPtr];

	[self setTitleById:CMsg::Volume];
	[self setShowsResizeIndicator:NO];

	CocoaView *view = [self contentView];

	CocoaLayout *box_all = [CocoaLayout create:VerticalBox :0 :COCOA_DEFAULT_MARGIN :_T("box_all")];
	CocoaLayout *hbox;
	CocoaLayout *vbox;

	hbox = [box_all addBox:HorizontalBox];

	for(int i=0; i < VOLUME_ITEMS; i++) {
		if (i == 7) {
			NSBox *sep = [[NSBox alloc] init];
			[box_all addControl:sep :560 :2];
			[view addSubview:sep];

			hbox = [box_all addBox:HorizontalBox];
		}

		vbox = [hbox addBox:VerticalBox];
		CocoaLabel *label = [CocoaLabel createI:t[i] align:NSCenterTextAlignment];
		[vbox addControl:label :80 :32];
		[view addSubview:label];

		CocoaSlider *slider = [CocoaSlider createN:i action:@selector(changeSlider:) value:[self volume:i]];
		if (NSAppKitVersionNumber > 1349.0 /* NSAppKitVersionNumber10_10_Max */) {
			[slider.cell setVertical:YES];
		}
		[vbox addControl:slider :80 :120];
		[view addSubview:slider];

		p_lbl[i] = [CocoaLabel createT:"00" align:NSCenterTextAlignment];
//		[p_lbl[i] setEditable:TRUE];
		[vbox addControl:p_lbl[i] :80 :16];
		[view addSubview:p_lbl[i]];
		[self setVolumeText:i];

		CocoaCheckBox *check = [CocoaCheckBox createI:CMsg::Mute index:i action:@selector(changeMute:) value:[self mute:i]];
		[vbox addControl:check :80 :32];
		[view addSubview:check];

		if (i == 0) {
			// separator
			NSBox *sep = [[NSBox alloc] init];
			[hbox addControl:sep :2 :200];
			[view addSubview:sep];
		}
	}

	hbox = [box_all addBox:HorizontalBox :RightPos | TopPos :0 :_T("BTN")];
	CocoaButton *btnClose = [CocoaButton createI:CMsg::Close action:@selector(dialogClose:)];
	[hbox addControl:btnClose width:120];
	[view addSubview:btnClose];

	[box_all realize:self];

	return self;
}

- (NSInteger)runModal
{
	return [NSApp runModalForWindow:self];
}

- (void)close
{
	[NSApp stopModalWithCode:NSOKButton];
	[super close];
}

- (void)dialogClose:(id)sender
{
	[self close];
}

- (void)setPtr
{
	int i = 0;
	p_volume[i++] = &config.volume;
	p_volume[i++] = &config.beep_volume;
#if defined(_MBS1)
	p_volume[i++] = &config.psg_volume;
	p_volume[i++] = &config.psgexfm_volume;
	p_volume[i++] = &config.psgexssg_volume;
	p_volume[i++] = &config.opnfm_volume;
	p_volume[i++] = &config.opnssg_volume;
#endif
	p_volume[i++] = &config.psg6_volume;
	p_volume[i++] = &config.psg9_volume;
	p_volume[i++] = &config.relay_volume;
	p_volume[i++] = &config.cmt_volume;
	p_volume[i++] = &config.fdd_volume;

	i = 0;
	p_mute[i++] = &config.mute;
	p_mute[i++] = &config.beep_mute;
#if defined(_MBS1)
	p_mute[i++] = &config.psg_mute;
	p_mute[i++] = &config.psgexfm_mute;
	p_mute[i++] = &config.psgexssg_mute;
	p_mute[i++] = &config.opnfm_mute;
	p_mute[i++] = &config.opnssg_mute;
#endif
	p_mute[i++] = &config.psg6_mute;
	p_mute[i++] = &config.psg9_mute;
	p_mute[i++] = &config.relay_mute;
	p_mute[i++] = &config.cmt_mute;
	p_mute[i++] = &config.fdd_mute;
}

- (void)setVolumeText:(int)idx
{
	char str[8];
	sprintf(str, "%02d", [self volume:idx]);
	[p_lbl[idx] setStringValue:[NSString stringWithUTF8String:str]];
}

- (bool)mute:(int)idx
{
	return *p_mute[idx];
}

- (int)volume:(int)idx
{
	return *p_volume[idx];
}

- (void)changeSlider:(CocoaSlider *)sender
{
	*p_volume[sender.index] = [sender intValue];
	[self setVolumeText:sender.index];
	emu->set_volume(0);
}

- (void)changeMute:(CocoaCheckBox *)sender
{
	*p_mute[sender.index] = ([sender state] == NSOnState);
	emu->set_volume(0);
}

@end
