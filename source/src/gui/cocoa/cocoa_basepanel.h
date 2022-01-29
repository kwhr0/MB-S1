/** @file cocoa_basepanel.h

 Skelton for retropc emulator
 SDL edition + Cocoa GUI

 @author Sasaji
 @date   2015.04.30 -

 @brief [ panel ]
 */

#ifndef COCOA_BASEPANEL_H
#define COCOA_BASEPANEL_H

#import <Cocoa/Cocoa.h>
#import "../../cchar.h"
#import "../../cptrlist.h"
#import "../../msgs.h"

#define COCOA_DEFAULT_MARGIN 5

@class CocoaLayout;

@interface CocoaTextSize : NSObject
+ (NSSize)size:(NSControl *)ctrl constTo:(NSSize)constraint;
@end

@interface CocoaLabel : NSTextField
+ (CocoaLabel *)create:(NSRect)re title:(const char *)title;
+ (CocoaLabel *)create:(NSRect)re titleid:(CMsg::Id)titleid;
+ (CocoaLabel *)create:(NSRect)re title:(const char *)title align:(NSTextAlignment)align;
+ (CocoaLabel *)create:(NSRect)re titleid:(CMsg::Id)titleid align:(NSTextAlignment)align;
+ (CocoaLabel *)createT:(const char *)title;
+ (CocoaLabel *)createI:(CMsg::Id)titleid;
+ (CocoaLabel *)createT:(const char *)title align:(NSTextAlignment)align;
+ (CocoaLabel *)createI:(CMsg::Id)titleid align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithFitW:(NSRect *)re title:(const char *)title;
+ (CocoaLabel *)createWithFitW:(NSRect *)re titleid:(CMsg::Id)titleid;
+ (CocoaLabel *)createWithFitW:(NSRect *)re title:(const char *)title align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithFitW:(NSRect *)re titleid:(CMsg::Id)titleid align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithFitH:(NSRect *)re title:(const char *)title;
+ (CocoaLabel *)createWithFitH:(NSRect *)re title:(const char *)title align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithFit:(NSRect *)re title:(const char *)title;
+ (CocoaLabel *)createWithFit:(NSRect *)re titleid:(CMsg::Id)titleid;
+ (CocoaLabel *)createWithFit:(NSRect *)re title:(const char *)title align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithFit:(NSRect *)re titleid:(CMsg::Id)titleid align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithoutFit:(NSRect)re title:(const char *)title align:(NSTextAlignment)align;
+ (CocoaLabel *)createWithoutFit:(NSRect)re titleid:(CMsg::Id)titleid align:(NSTextAlignment)align;
@end

@interface CocoaTextField : NSTextField
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
+ (CocoaTextField *)create:(NSRect)re num:(int)num action:(SEL)action;
+ (CocoaTextField *)create:(NSRect)re num:(int)num action:(SEL)action align:(NSTextAlignment)align;
+ (CocoaTextField *)create:(NSRect)re text:(const char *)text action:(SEL)action;
+ (CocoaTextField *)create:(NSRect)re text:(const char *)text action:(SEL)action align:(NSTextAlignment)align;
+ (CocoaTextField *)create:(NSRect)re textid:(CMsg::Id)textid action:(SEL)action;
+ (CocoaTextField *)create:(NSRect)re textid:(CMsg::Id)textid action:(SEL)action align:(NSTextAlignment)align;
#endif
+ (CocoaTextField *)createN:(int)num action:(SEL)action;
+ (CocoaTextField *)createN:(int)num action:(SEL)action align:(NSTextAlignment)align;
+ (CocoaTextField *)createT:(const char *)text action:(SEL)action;
+ (CocoaTextField *)createT:(const char *)text action:(SEL)action align:(NSTextAlignment)align;
+ (CocoaTextField *)createI:(CMsg::Id)textid action:(SEL)action;
+ (CocoaTextField *)createI:(CMsg::Id)textid action:(SEL)action align:(NSTextAlignment)align;
@end

@interface CocoaObjectStructure : NSObject
{
	id obj1;
	id obj2;
}
@property (assign) id obj1;
@property (assign) id obj2;
+ (CocoaObjectStructure *)create:(id)newobj1 :(id)newobj2;
@end

@interface CocoaButton : NSButton
{
	id relatedObject;
}
@property (assign) id relatedObject;
+ (CocoaButton *)create:(NSRect)re title:(const char *)title action:(SEL)action;
+ (CocoaButton *)create:(NSRect)re titleid:(CMsg::Id)titleid action:(SEL)action;
+ (CocoaButton *)createT:(const char *)title action:(SEL)action;
+ (CocoaButton *)createI:(CMsg::Id)titleid action:(SEL)action;
@end

@interface CocoaPopUpButton : NSPopUpButton
+ (CocoaPopUpButton *)create:(NSRect)re items:(const char **)items action:(SEL)action selidx:(int)selidx;
+ (CocoaPopUpButton *)create:(NSRect)re itemids:(const CMsg::Id *)itemids action:(SEL)action selidx:(int)selidx;
+ (CocoaPopUpButton *)createT:(const char **)items action:(SEL)action selidx:(int)selidx;
+ (CocoaPopUpButton *)createI:(const CMsg::Id *)itemids action:(SEL)action selidx:(int)selidx;
+ (CocoaPopUpButton *)createL:(const CPtrList<CTchar> *)items action:(SEL)action selidx:(int)selidx;
- (void)addItemsT:(const char **)items selidx:(int)selidx;
- (void)addItemsI:(const CMsg::Id *)itemids selidx:(int)selidx;
- (void)addItemsL:(const CPtrList<CTchar> *)items selidx:(int)selidx;
@end

@interface CocoaCheckBox : NSButton
{
	int index;
}
@property (nonatomic) int index;
+ (CocoaCheckBox *)create:(NSRect)re title:(const char *)title action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)create:(NSRect)re titleid:(CMsg::Id)titleid action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)create:(NSRect)re title:(const char *)title index:(int)index action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)create:(NSRect)re titleid:(CMsg::Id)titleid index:(int)index action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)createT:(const char *)title action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)createI:(CMsg::Id)titleid action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)createT:(const char *)title index:(int)index action:(SEL)action value:(bool)value;
+ (CocoaCheckBox *)createI:(CMsg::Id)titleid index:(int)index action:(SEL)action value:(bool)value;
@end

@interface CocoaRadioButton : NSButton
{
	int index;
}
@property (nonatomic) int index;
+ (CocoaRadioButton *)createT:(const char *)title action:(SEL)action value:(bool)value;
+ (CocoaRadioButton *)createI:(CMsg::Id)titleid action:(SEL)action value:(bool)value;
+ (CocoaRadioButton *)createT:(const char *)title index:(int)index action:(SEL)action value:(bool)value;
+ (CocoaRadioButton *)createI:(CMsg::Id)titleid index:(int)index action:(SEL)action value:(bool)value;
@end

@interface CocoaRadioGroup : NSMatrix
+ (CocoaRadioGroup *)create:(NSRect)re rows:(int)rows cols:(int)cols titles:(const char **)titles action:(SEL)action selidx:(int)selidx;
+ (CocoaRadioGroup *)create:(NSRect)re rows:(int)rows cols:(int)cols titleids:(const CMsg::Id *)titleids action:(SEL)action selidx:(int)selidx;
+ (CocoaRadioGroup *)create:(int)height rows:(int)rows titles:(const char **)titles action:(SEL)action selidx:(int)selidx;
+ (CocoaRadioGroup *)create:(int)height rows:(int)rows titleids:(const CMsg::Id *)titleids action:(SEL)action selidx:(int)selidx;
+ (CocoaRadioGroup *)create:(int)width cols:(int)cols titles:(const char **)titles action:(SEL)action selidx:(int)selidx;
+ (CocoaRadioGroup *)create:(int)width cols:(int)cols titleids:(const CMsg::Id *)titleids action:(SEL)action selidx:(int)selidx;
- (void)addItemsT:(const char **)titles rows:(int)rows cols:(int)cols selidx:(int)selidx width:(int)width height:(int)height;
- (void)addItemsI:(const CMsg::Id *)titleids rows:(int)rows cols:(int)cols selidx:(int)selidx width:(int)width height:(int)height;
@end

@interface CocoaSlider : NSSlider
{
	int index;
}
@property (nonatomic) int index;
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
+ (CocoaSlider *)create:(NSRect)re action:(SEL)action value:(int)value;
+ (CocoaSlider *)create:(NSRect)re index:(int)index action:(SEL)action value:(int)value;
#endif
+ (CocoaSlider *)createN:(SEL)action value:(int)value;
+ (CocoaSlider *)createN:(int)index action:(SEL)action value:(int)value;
@end

@interface CocoaTabView : NSTabView
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
+ (CocoaTabView *)create:(NSRect)re;
+ (CocoaTabView *)create:(NSRect)re tabs:(const char **)tabs;
+ (CocoaTabView *)create:(NSRect)re tabids:(const CMsg::Id *)tabids;
#endif
+ (CocoaTabView *)create;
+ (CocoaTabView *)createT:(const char **)tabs;
+ (CocoaTabView *)createI:(const CMsg::Id *)tabids;
- (void)addTabItemsT:(const char **)tabs;
- (void)addTabItemsI:(const CMsg::Id *)tabids;
- (NSTabViewItem *)addTabItem:(const char *)label;
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
- (NSRect)adjustFrameSize:(NSRect)re;
#endif
@end

@interface CocoaBox : NSBox
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
+ (CocoaBox *)create:(NSRect)re title:(const char *)title;
+ (CocoaBox *)create:(NSRect)re titleid:(CMsg::Id)titleid;
#endif
+ (CocoaBox *)createT:(const char *)title;
+ (CocoaBox *)createI:(CMsg::Id)titleid;
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
- (NSRect)adjustFrameSize:(NSRect)re;
#endif
@end

@interface CocoaView : NSView
{
	int max_right;
	int max_bottom;

	int margin;
}
@property int max_right;
@property int max_bottom;
@property int margin;
- (id)init;
- (BOOL)isFlipped;
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
- (NSRect)makeRect:(int)left :(int)top :(int)w :(int)h;
- (NSRect)remakeSize:(NSRect)re :(int)w :(int)h;
- (NSRect)remakePosition:(NSRect)re :(int)left :(int)top :(int)new_w :(int)new_h;
- (NSRect)remakeLeft:(NSRect)re :(int)left :(int)new_w :(int)new_h;
- (NSRect)remakeLeft:(NSRect)re :(int)left;
- (NSRect)remakeTop:(NSRect)re :(int)top :(int)new_w :(int)new_h;
- (NSRect)remakeTop:(NSRect)re :(int)top;
- (NSRect)addPosition:(NSRect)re :(int)offsetx :(int)offsety :(int)new_w :(int)new_h;
- (NSRect)addWidth:(NSRect)re  :(int)new_w :(int)new_h;
- (NSRect)addHeight:(NSRect)re :(int)new_w :(int)new_h;
+ (int)topPos:(NSRect)re;
+ (int)bottomPos:(NSRect)re;
- (void)adjustFrameSize;
//- (NSRect)adjustFrameSize:(NSRect)re;
#endif
@end

@interface CocoaBasePanel : NSPanel
- (id)init;
- (int)margin;
#ifdef COCOA_USE_OLDSTYLE_LAYOUT
- (void)adjustFrame;
#endif
- (void)setTitleById:(CMsg::Id)titleid;
@end

//
// Layout for widget controls
//
enum enOrient {
	VerticalBox = 0,
	HorizontalBox,
	TabViewBox,
	BoxViewBox,
};
enum enAlign {
	LeftPos   = 0x00,
	CenterPos = 0x01,
	RightPos  = 0x02,
	TopPos    = 0x00,
	MiddlePos = 0x10,
	BottomPos = 0x20
};

@interface CocoaLayoutControls : NSObject
{
	CocoaLayout *box;
	NSView *ctrl;
	int x;	// relative position x from re.x
	int y;	// relative position y from re.y
	int px;	// parts position x from x
	int py;	// parts position y from y
	int w;
	int h;
	CocoaLayoutControls *next;
}
@property (retain) CocoaLayout *box;
@property (retain) NSView *ctrl;
@property int x;
@property int y;
@property int px;
@property int py;
@property int w;
@property int h;
@property (retain) CocoaLayoutControls *next;
+ (CocoaLayoutControls *)create:(CocoaLayout *)box_ :(NSView *)ctrl_ :(int)x_ :(int)y_ :(int)px_ :(int)py_ :(int)w_ :(int)h_;
@end

@interface CocoaLayout : NSObject
{
	enOrient orient;
	int      align;
	VmRectWH re;	// absolute position
	bool realized;

	int padding;
	VmRect margin;

	_TCHAR name[10];

	CocoaLayoutControls *controls;
	int control_nums;
}
//@property (retain) CocoaView *view;
@property enOrient orient;
@property int align;
@property VmRectWH re;
@property bool realized;
@property int padding;
@property VmRect margin;
@property (retain) CocoaLayoutControls *controls;
@property int control_nums;
+ (CocoaLayout *)create:(int)orient_;
+ (CocoaLayout *)create:(int)orient_ :(int)align_;
+ (CocoaLayout *)create:(int)orient_ :(int)align_ :(int)margin_;
+ (CocoaLayout *)create:(int)orient_ :(int)align_ :(int)margin_ :(const _TCHAR *)name_;
- (id)initWith:(int)orient_ :(int)align_ :(int)margin_ :(const _TCHAR *)name_;
- (void)addItem:(CocoaLayout *)box_;
- (void)addItem:(CocoaLayout *)box_ :(NSView *)ctrl_ :(int)width_ :(int)height_ :(int)px_ :(int)py_;
- (CocoaLayout *)addBox:(int)orient_;
- (CocoaLayout *)addBox:(int)orient_ :(int)align_;
- (CocoaLayout *)addBox:(int)orient_ :(int)align_ :(int)margin_;
- (CocoaLayout *)addBox:(int)orient_ :(int)align_ :(int)margin_ :(const _TCHAR *)name_;
- (void)addControl:(NSControl *)ctrl_;
- (void)addControl:(NSControl *)ctrl_ width:(int)width_;
- (void)addControl:(NSControl *)ctrl_ height:(int)height_;
- (void)addControl:(NSView *)ctrl_ :(int)width_ :(int)height_;
- (void)addControl:(NSView *)ctrl_ :(int)width_ :(int)height_ :(int)px_ :(int)py_;
- (void)addControlWithBox:(CocoaLayout *)box_ :(NSView *)ctrl_;
- (void)addSpace:(int)width_ :(int)height_;
- (void)addSpace:(int)width_ :(int)height_ :(int)px_ :(int)py_;
- (void)realize:(NSPanel *)dlg_;
- (void)adjustPosition:(int)x_ :(int)y_;

- (int)getWidthWithMargin;
- (int)getHeightWithMargin;
- (int)getWidthWithLeftMargin;
- (int)getHeightWithTopMargin;
- (int)getWidth;
- (int)getHeight;

- (void)getPositionByItem:(int)num_ :(int *)x_ :(int *)y_;

- (void)setLeftMargin:(int)val;
- (void)setTopMargin:(int)val;
@end

#endif /* COCOA_BASEPANEL_H */
