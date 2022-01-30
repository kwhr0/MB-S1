/** @file mbs1.cpp

	HITACHI MB-S1 Emulator 'EmuB-S1'
	Skelton for retropc emulator

	@author Sasaji
	@date   2011.06.08 -

	@brief [ virtual machine ]
*/

#include "mbs1.h"
#include "../../emu.h"
#include "../../config.h"
#include "../device.h"
#include "../event.h"

#include "../hd46505.h"
#include "../ay38910.h"
#include "../ym2203.h"
#ifdef NEWMPU
#include "HD6309.h"
#endif
#ifdef CURMPU
#include "../mc6809.h"
#endif

#include "board.h"
#include "../pia.h"
#include "../acia.h"
#include "display.h"
#include "keyboard.h"
#include "mouse.h"
#include "memory.h"
#include "sound.h"
#include "comm.h"
#include "cmt.h"
#include "printer.h"
#include "timer.h"
#include "kanji.h"
#include "psgc.h"
#include "../via.h"
#include "psg9c.h"
#ifdef USE_FD1
#include "../mc6843.h"
#include "../mb8866.h"
#include "floppy.h"
#endif
#ifdef USE_RTC
#include "rtc.h"
#include "../msm58321.h"
#endif
#include "keyrecord.h"
#ifdef USE_DEBUGGER
#include "../debugger.h"
#include "../mc6809dasm.h"
#endif
#if defined(USE_Z80B_CARD)
#include "z80b_card.h"
#include "../z80.h"
#endif
#include "../../utility.h"
#include "../../version.h"

// ----------------------------------------------------------------------------

const int fmopn_clocks[] = {
	3579545,			// 3.58MHz
	CLOCKS_1MHZ,		// 1MHz
	CLOCKS_1MHZ * 2,	// 2MHz
	CLOCKS_1MHZ * 4,	// 4MHz
	-1
};

static MEMORY *sMemory;
static int sICount;

// ----------------------------------------------------------------------------
// initialize
// ----------------------------------------------------------------------------

VM::VM(EMU* parent_emu) : emu(parent_emu)
{
	//
	emu->set_parami(ParamFddType, config.fdd_type);
	emu->set_parami(ParamIOPort, config.io_port);
	emu->set_parami(ParamSysMode, config.sys_mode);
	emu->set_parami(ParamExMemNum, config.exram_size_num);

	// create devices
	first_device = last_device = NULL;
	dummy = new DEVICE(this, emu, NULL);	// must be 1st device
	event = new EVENT(this, emu, NULL);	// must be 2nd device
	event->initialize();		// must be initialized first

	crtc = new HD46505(this, emu, NULL);
	psgst = new AY38910(this, emu, "P01");
//	psgex = new AY38910(this, emu, "P02");
	psgex = new YM2203(this, emu, "P02");
	psg3[0] = new AY38910(this, emu, "P31");
	psg3[1] = new AY38910(this, emu, "P32");
	psg9[0] = new AY38910(this, emu, "P91");
	psg9[1] = new AY38910(this, emu, "P92");
	psg9[2] = new AY38910(this, emu, "P93");
	fmopn = new YM2203(this, emu, "F01");

	board = new BOARD(this, emu, NULL);
	pia = new PIA(this, emu, "MA");
	acia = new ACIA(this, emu, "MA");
	display = new DISPLAY(this, emu, NULL);
	key = new KEYBOARD(this, emu, NULL);
	mouse = new MOUSE(this, emu, NULL);
	memory = new MEMORY(this, emu, NULL);
	sound = new SOUND(this, emu, NULL);
	comm[0] = new COMM(this, emu, NULL, 0);
	comm[1] = new COMM(this, emu, "EX1", 1);
	cmt = new CMT(this, emu, NULL);
	printer[0] = new PRINTER(this, emu, NULL, 0);
	printer[1] = new PRINTER(this, emu, "EX1", 1);
	printer[2] = new PRINTER(this, emu, "EX2", 2);
	timer = new TIMER(this, emu, NULL);
	kanji = new KANJI(this, emu, NULL);
	psg3_pia = new PIA(this, emu, "P31");
	psg3c = new PSGC(this, emu, "P31");
	psg9_via = new VIA(this, emu, "P91");
	psg9c = new PSG9C(this, emu, "P91");
#ifdef USE_FD1
	fdc3 = new MC6843(this, emu, NULL);
	fdc5 = new MB8866(this, emu, NULL);
	fdc5ex = new MB8866(this, emu, "EX1");
	fdd = new FLOPPY(this, emu, NULL);
#endif
	pia_ex = new PIA(this, emu, "EX1");
	acia_ex = new ACIA(this, emu, "EX1");
#ifdef USE_RTC
	rtc = new RTC(this, emu, NULL);
	msm58321 = new MSM58321(this, emu, NULL);
#endif
#if defined(USE_Z80B_CARD)
	z80b_card = new Z80B_CARD(this, emu, "B");
	z80 = new Z80(this, emu, "B");
#endif

	//
#ifdef NEWMPU
	cpu = newmpu = new MPUWrap(this, emu, NULL);
#endif
#ifdef CURMPU
	cpu = curmpu = new MC6809(this, emu, NULL);
#endif

#ifdef USE_KEY_RECORD
	reckey = new KEYRECORD(emu);
#endif

	// set contexts
//	event->set_context_cpu(cpu, CPU_CLOCKS);
#if defined(USE_Z80B_CARD)
	event->set_context_cpu(z80, Z80B_CLOCKS);
#endif
	event->set_context_sound(sound);
	event->set_context_sound(psgst);
	event->set_context_sound(psgex);
	event->set_context_sound(psg3[0]);
	event->set_context_sound(psg3[1]);
	event->set_context_sound(psg9[0]);
	event->set_context_sound(psg9[1]);
	event->set_context_sound(psg9[2]);
	event->set_context_sound(cmt);
	event->set_context_sound(fdd);
	event->set_context_sound(fmopn);
	event->set_context_display(display);

	// crtc
//	crtc->set_context_vsync(timer, SIG_TIMER_VSYNC, 1);
	crtc->set_context_vsync(display, SIG_DISPLAY_VSYNC, 1);
	crtc->set_context_hsync(display, SIG_DISPLAY_HSYNC, 1);
	crtc->set_context_disp(display, SIG_DISPLAY_DISPTMG, 1);
//	crtc->set_context_hsync(key, SIG_KEYBOARD_HSYNC, 1);
	crtc->set_context_write_regs(display, SIG_DISPLAY_WRITE_REGS, 0x0f);

	// display
	display->set_context_crtc(crtc);
	display->set_context_board(board);
	display->set_s1vtram_ptr(memory->get_s1vtram());
	display->set_s1vcram_ptr(memory->get_s1vcram());
	display->set_s1vgrram_ptr(memory->get_s1vgrram());
	display->set_s1vggram_ptr(memory->get_s1vggram());
	display->set_s1vgbram_ptr(memory->get_s1vgbram());
	display->set_l3vram_ptr(memory->get_l3vram());
	display->set_color_ram_ptr(memory->get_color_ram());
	display->set_ig_ram_ptr(memory->get_ig_ram());
	display->set_regs_ptr(crtc->get_regs());
	display->set_crtc_vt_ptr(crtc->get_vt_total_ptr(), crtc->get_vt_count_ptr(), crtc->get_vt_disp_ptr());
	display->set_crtc_ma_ra_ptr(crtc->get_ma_ptr(),crtc->get_ra_ptr());
	display->set_crtc_max_ra_ptr(crtc->get_max_ra_ptr());
	display->set_crtc_odd_line_ptr(crtc->get_video_odd_line_ptr());
	display->set_crtc_reg8_ptr(crtc->get_videomode_ptr(), crtc->get_disptmg_skew_ptr(), crtc->get_curdisp_skew_ptr());
	display->set_crtc_curdisp_ptr(crtc->get_curdisp_ptr());
	display->set_crtc_vsync_ptr(crtc->get_vs_start_ptr(), crtc->get_vs_end_ptr());

	// keyboard
//	key->set_context_cpu(cpu);
	key->set_context_disp(display);
	key->set_context_board(board);
#ifdef USE_KEY_RECORD
	key->set_keyrecord(reckey);
	reckey->set_context(key);
#endif
	key->set_context_pia(pia);

	// mouse
	mouse->set_context_board(board);
#ifdef USE_KEY_RECORD
	mouse->set_keyrecord(reckey);
#endif

	// memory
//	memory->set_context_cpu(cpu);
#if defined(USE_Z80B_CARD)
//	memory->set_context_cpu(z80);
#endif
	memory->set_context_pia(pia);
	memory->set_context_acia(acia);
	memory->set_context_crtc(crtc);
	memory->set_context_display(display);
	memory->set_context_key(key);
	memory->set_context_mouse(mouse);
	memory->set_context_sound(sound);
	memory->set_context_psg(psgst, psgex);
	memory->set_context_psg3(psg3_pia);
	memory->set_context_psg9(psg9_via);
	memory->set_context_fmopn(fmopn);
	memory->set_context_comm(comm[0]);
	memory->set_context_cmt(cmt);
	memory->set_context_timer(timer);
	memory->set_context_kanji(kanji);
#ifdef USE_FD1
	memory->set_context_fdc(fdc3, fdc5, fdc5ex);
	memory->set_context_fdd(fdd);
#endif
	memory->set_context_pia_ex(pia_ex);
	memory->set_context_acia_ex(acia_ex);
	memory->set_context_comm1(comm[1]);
#ifdef USE_RTC
	memory->set_context_rtc(rtc);
#endif
	memory->set_context_board(board);
#if defined(USE_Z80B_CARD)
	memory->set_context_z80bcard(z80b_card);
#endif
	memory->set_context_event(event);

	memory->set_font(display->get_font());

	// cpu bus
#ifdef NEWMPU
	sMemory = memory;
	memory->cp.setmpu(newmpu);
#endif
#ifdef CURMPU
	curmpu->set_context_mem(memory);
#endif
#if defined(USE_Z80B_CARD)
	z80->set_context_mem(z80b_card);
	z80->set_context_io(z80b_card);
	z80->set_context_busack(z80b_card, SIG_Z80BCARD_BUSACK, 1);
#endif
#ifdef USE_DEBUGGER
//	debugger = new DEBUGGER(this, emu, NULL);
	cpu->set_context_debugger(new DEBUGGER(this, emu, NULL));
#if defined(USE_Z80B_CARD)
	z80->set_context_debugger(new DEBUGGER(this, emu, "B"));
#endif
#endif

	// acia
	acia->set_context_txdata(comm[0], SIG_ACIA_TXDATA, 0xff);
	acia->set_context_rts(comm[0], SIG_ACIA_RTS, 1);
	acia->set_context_dtr(comm[0], SIG_ACIA_DTR, 1);
	acia->set_context_res(comm[0], SIG_ACIA_RESET, 1);
	acia->set_context_irq(board, SIG_CPU_IRQ, SIG_IRQ_ACIA_MASK);

	// acia_ex
	acia_ex->set_context_txdata(comm[1], SIG_ACIA_TXDATA, 0xff);
	acia_ex->set_context_rts(comm[1], SIG_ACIA_RTS, 1);
	acia_ex->set_context_dtr(comm[1], SIG_ACIA_DTR, 1);
	acia_ex->set_context_res(comm[1], SIG_ACIA_RESET, 1);
	acia_ex->set_context_irq(board, SIG_CPU_IRQ, SIG_IRQ_EXACIA_MASK);

	// comm
	comm[0]->set_context_ctrl(acia);
	comm[0]->set_context_cmt(cmt);
	comm[1]->set_context_ctrl(acia_ex);

	// data recorder
	cmt->set_context_ctrl(comm[0]);

	// pia
	pia->set_context_pa(key, SIG_KEYBOARD_PIA_PA, 0xff);
	pia->set_context_irqa(board, SIG_CPU_IRQ, SIG_IRQ_PIAA_MASK);
	pia->set_context_pb(printer[0], SIG_PRINTER_PIA_PB, 0xff);
	pia->set_context_cb2(printer[0], SIG_PRINTER_PIA_CB2, 1);
	pia->set_context_irqb(board, SIG_CPU_IRQ, SIG_IRQ_PIAB_MASK);

	// pia ex
	pia_ex->set_context_pa(printer[1], SIG_PRINTER_PIA_PB, 0xff);
	pia_ex->set_context_ca2(printer[1], SIG_PRINTER_PIA_CB2, 1);
	pia_ex->set_context_irqa(board, SIG_CPU_IRQ, SIG_IRQ_EXPIAA_MASK);
	pia_ex->set_context_pb(printer[2], SIG_PRINTER_PIA_PB, 0xff);
	pia_ex->set_context_cb2(printer[2], SIG_PRINTER_PIA_CB2, 1);
	pia_ex->set_context_irqb(board, SIG_CPU_IRQ, SIG_IRQ_EXPIAB_MASK);

	// printer
	printer[0]->set_context_ctrl(pia);
	printer[0]->set_context_cb1(SIG_PIA_CB1);
	printer[1]->set_context_ctrl(pia_ex);
	printer[1]->set_context_cb1(SIG_PIA_CA1);
	printer[2]->set_context_ctrl(pia_ex);
	printer[2]->set_context_cb1(SIG_PIA_CB1);

	// timer
	timer->set_context_board(board);

	// psg ex
	psg3_pia->set_context_pa(psg3c, SIG_PSG_PIA_PA, 0xff);
	psg3_pia->set_context_pb(psg3c, SIG_PSG_PIA_PB, 0xff);
	psg3c->set_context_psg(psg3[0], psg3[1]);
	psg3c->set_context_pia(psg3_pia);

	// 9voice psg
	psg9_via->set_context_pa(psg9c, SIG_PSG9_VIA_PA, 0xff);
	psg9_via->set_context_pb(psg9c, SIG_PSG9_VIA_PB, 0xff);
	psg9_via->set_context_irq(board, SIG_CPU_IRQ, SIG_IRQ_9PSG_MASK);
	psg9_via->write_signal(SIG_VIA_CLOCK_UNIT, CPU_CLOCKS / CLOCKS_1MHZ, 0xff);
	psg9c->set_context_psg(psg9[0], psg9[1], psg9[2]);
	psg9c->set_context_via(psg9_via);

	// fm opn
	if (config.opn_irq > 0) {
		int id = (config.opn_irq == 2 ? SIG_CPU_FIRQ : SIG_CPU_IRQ);
		fmopn->set_context_irq(board, id, SIG_IRQ_FMOPN_MASK);
		psgex->set_context_irq(board, id, SIG_IRQ_FMOPNEX_MASK);
	}

#ifdef USE_FD1
	// fdc for 3inch compact floppy
	fdc3->set_context_irq(board, SIG_CPU_NMI, SIG_NMI_FD_MASK);
	fdc3->set_context_fdd(fdd);

	// fdc for 5inch mini floppy
	fdc5->set_context_irq(fdd, SIG_FLOPPY_IRQ, 1);
	fdc5->set_context_drq(fdd, SIG_FLOPPY_DRQ, 1);
	fdc5->set_context_fdd(fdd);
	fdc5->set_channel(0);

	// fdc for 5inch mini floppy (ex)
	fdc5ex->set_context_irq(fdd, SIG_FLOPPY_IRQ, 0x10001);
	fdc5ex->set_context_drq(fdd, SIG_FLOPPY_DRQ, 0x10001);
	fdc5ex->set_context_fdd(fdd);
	fdc5ex->set_channel(1);

	// fdd
	fdd->set_context_irq(board, SIG_CPU_NMI, SIG_NMI_FD_MASK);
	fdd->set_context_drq(board, SIG_CPU_HALT, SIG_HALT_FD_MASK);
	fdd->set_context_fdc(fdc3, fdc5, fdc5ex);
	fdd->set_context_board(board);
#endif

#ifdef USE_RTC
	rtc->set_context_rtc(msm58321);
#endif

#if defined(USE_Z80B_CARD)
	z80b_card->set_context_cpu(z80);
	z80b_card->set_context_board(board);
#endif

	// main board
	// reset signal
	// send reset to memory at first
	board->set_context_reset(memory, SIG_CPU_RESET, 1);
	board->set_context_reset(display, SIG_CPU_RESET, 1);
	board->set_context_reset(key, SIG_CPU_RESET, 1);
	board->set_context_reset(mouse, SIG_CPU_RESET, 1);
	board->set_context_reset(cmt, SIG_CPU_RESET, 1);
	board->set_context_reset(pia, SIG_CPU_RESET, 1);
	board->set_context_reset(timer, SIG_CPU_RESET, 1);
	board->set_context_reset(psgst, SIG_CPU_RESET, 1);
	board->set_context_reset(psgex, SIG_CPU_RESET, 1);
	board->set_context_reset(psg3[0], SIG_CPU_RESET, 1);
	board->set_context_reset(psg3[1], SIG_CPU_RESET, 1);
	board->set_context_reset(psg3_pia, SIG_CPU_RESET, 1);
	board->set_context_reset(psg9[0], SIG_CPU_RESET, 1);
	board->set_context_reset(psg9[1], SIG_CPU_RESET, 1);
	board->set_context_reset(psg9[2], SIG_CPU_RESET, 1);
	board->set_context_reset(psg9_via, SIG_CPU_RESET, 1);
	board->set_context_reset(fmopn, SIG_CPU_RESET, 1);
#ifdef USE_FD1
	board->set_context_reset(fdd, SIG_CPU_RESET, 1);
#endif
	board->set_context_reset(pia_ex, SIG_CPU_RESET, 1);
#ifdef USE_RTC
	board->set_context_reset(rtc, SIG_CPU_RESET, 1);
#endif
#if defined(USE_Z80B_CARD)
	board->set_context_reset(z80b_card, SIG_CPU_RESET, 1);
//	board->set_context_reset(z80, SIG_CPU_RESET, 1);
#endif
	// send reset to cpu at last
#ifdef NEWMPU
	board->set_context_reset(newmpu, SIG_CPU_RESET, 1);
	// nmi signal
	board->set_context_nmi(newmpu, SIG_CPU_NMI, 0xffffffff);
	// irq signal
	board->set_context_irq(newmpu, SIG_CPU_IRQ, 0xffffffff);
	// firq signal
	board->set_context_firq(newmpu, SIG_CPU_FIRQ, 0xffffffff);
	// halt signal
	board->set_context_halt(newmpu, SIG_CPU_HALT, 0xffffffff);
#endif
#ifdef CURMPU
	board->set_context_reset(curmpu, SIG_CPU_RESET, 1);
	// nmi signal
	board->set_context_nmi(curmpu, SIG_CPU_NMI, 0xffffffff);
	// irq signal
	board->set_context_irq(curmpu, SIG_CPU_IRQ, 0xffffffff);
	// firq signal
	board->set_context_firq(curmpu, SIG_CPU_FIRQ, 0xffffffff);
	// halt signal
	board->set_context_halt(curmpu, SIG_CPU_HALT, 0xffffffff);
#endif
#if defined(CURMPU) && !defined(NEWMPU)
	board->set_context_cpu(curmpu, nullptr);
	Switch(false);
#elif !defined(CURMPU) && defined(NEWMPU)
	board->set_context_cpu(nullptr, newmpu);
	Switch(true);
#else
	board->set_context_cpu(curmpu, newmpu);
	Switch(false);
#endif
	
	// initialize all devices
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
		if(device->get_id() != event->get_id()) {
			device->initialize();
		}
	}
}

VM::~VM()
{
	// delete all devices
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
		device->release();
	}
	for(DEVICE* device = first_device; device;) {
		DEVICE *next_device = device->get_next_device();
		delete device;
		device = next_device;
	}
#ifdef USE_KEY_RECORD
	delete reckey;
#endif
}

DEVICE* VM::get_device(int id)
{
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
		if(device->get_id() == id) {
			return device;
		}
	}
	return NULL;
}

DEVICE* VM::get_device(char *name, char *identifier)
{
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
		if(strncmp(name, device->get_class_name(), 12) == 0
		&& strncmp(identifier, device->get_identifier(), 4) == 0) {
			return device;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// drive virtual machine
// ----------------------------------------------------------------------------

void VM::reset()
{
	// power on / off
	emu->out_info_x(config.now_power_off ? CMsg::PowerOff : CMsg::PowerOn);

	// reset all devices
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
#ifdef CURMPU
		if (typeid(*device) == typeid(MC6809)) Switch(false);
		else
#endif
#ifdef NEWMPU
		if (typeid(*device) == typeid(HD6309)) Switch(true);
		else
#endif
		device->reset();
	}

	// disable unused devices
	msm58321->enable(IOPORT_USE_RTC != 0);

	// set initial port status
	sound->write_signal(SIG_SOUND_ON, 0, 1);
}

void VM::special_reset()
{
}

void VM::warm_reset(int onoff)
{
	// send reset signal
	if (onoff < 0) {
		board->write_signal(SIG_CPU_RESET, 1, 1);
		board->write_signal(SIG_CPU_RESET, 0, 1);
	} else {
		board->write_signal(SIG_CPU_RESET, onoff, 1);
	}
}

void VM::run(int split_num)
{
//	dasm.set_time(timeGetTime());
	event->drive(split_num);

}

double VM::frame_rate()
{
	return event->frame_rate();
}

void VM::change_dipswitch(int num)
{
	emu->out_infoc_x(CMsg::MODE_Switch_, (config.dipswitch & 4) ? CMsg::ON : CMsg::OFF, NULL);
}

bool VM::now_skip()
{
	return false;
}

void VM::update_params()
{
	change_fdd_type(emu->get_parami(ParamFddType), true);
	change_sys_mode(emu->get_parami(ParamSysMode) & SYS_MODE_S1L3, true);
	config.exram_size_num = emu->get_parami(ParamExMemNum);

	set_volume();
}

void VM::pause(int value)
{
	msm58321->pause(value);
}

// ----------------------------------------------------------------------------
// debugger
// ----------------------------------------------------------------------------

#ifdef USE_DEBUGGER
int VM::get_cpus() const
{
	int nums = 1;
#if defined(USE_Z80B_CARD)
	nums++;
#endif
	return nums;
}

DEVICE *VM::get_cpu(int index)
{
	if(index == 0) {
		return cpu;
	}
#if defined(USE_Z80B_CARD)
	else if (index == 1) {
		return z80;
	}
#endif
	return NULL;
}

DEVICE *VM::get_memory(int index)
{
	if(index == 0) {
		return memory;
	}
#if defined(USE_Z80B_CARD)
	else if (index == 1) {
		return z80b_card;
	}
#endif
	return NULL;
}

/// device names
static struct {
	const _TCHAR *name;
	uint32_t        num;
} device_names_map[] = {
	{ _T("PIA"), 2 },
	{ _T("ACIA"), 3 },
	{ _T("CRTC"), 1 },
	{ _T("KB"), 0 },
	{ _T("CMT"), 7 },
	{ _T("TIMER"), 8 },
	{ _T("FDC3"), 4 },
	{ _T("FDC5"), 5 },
	{ _T("FDC5EX"), 6 },
	{ _T("FDDUNIT"), 9 },
	{ _T("BOARD"), 10 },
	{ _T("PSG0"), 11 },
	{ _T("PSG1"), 12 },
	{ _T("PSG60"), 13 },
	{ _T("PSG61"), 14 },
	{ _T("PSG90"), 15 },
	{ _T("PSG91"), 16 },
	{ _T("PSG92"), 17 },
	{ _T("FMOPN"), 24 },
	{ _T("PIAEX"), 21 },
	{ _T("ACIAEX"), 22 },
	{ _T("VIA"), 23 },
	{ _T("RTC"), 27 },
	{ _T("MSM58321"), 28 },
	{ NULL, 0 }
};

bool VM::get_debug_device_name(const _TCHAR *param, uint32_t *num, int *idx, const _TCHAR **name)
{
	int i = 0; 
	for(; device_names_map[i].name != NULL; i++) {
		if (_tcsicmp(param, device_names_map[i].name) == 0) {
			if (num) *num = device_names_map[i].num;
			if (idx) *idx = i;
			if (name) *name = device_names_map[i].name;
			return true;
		}
	}
	return false;
}

void VM::get_debug_device_names_str(_TCHAR *buffer, size_t buffer_len)
{
	int i = 0;
	int len = 2;
	UTILITY::tcscpy(buffer, buffer_len, _T("  "));
	for(; device_names_map[i].name != NULL; i++) {
		if (i > 0) {
			UTILITY::tcscat(buffer, buffer_len, _T(","));
			len++;
		}
		int siz = (int)_tcslen(device_names_map[i].name);
		if (len + siz >= 80) {
			UTILITY::tcscat(buffer, buffer_len, _T("\n  "));
			len = 2;
		}
		UTILITY::tcscat(buffer, buffer_len, device_names_map[i].name);
		len += siz;
	}
}

bool VM::debug_write_reg(uint32_t num, uint32_t reg_num, uint32_t data)
{
	bool valid = false;
	switch(num) {
	case 0:
		// keyboard
		if (key) valid = key->debug_write_reg(reg_num, data);
		break;
	case 1:
		// crtc
		if (crtc) valid = crtc->debug_write_reg(reg_num, data);
		break;
	case 2:
		// pia
		if (pia) valid = pia->debug_write_reg(reg_num, data);
		break;
	case 3:
		// acia
		if (acia) valid = acia->debug_write_reg(reg_num, data);
		break;
	case 4:
		// fdc3
		if (fdc3) valid = fdc3->debug_write_reg(reg_num, data);
		break;
	case 5:
		// fdc5
		if (fdc5) valid = fdc5->debug_write_reg(reg_num, data);
		break;
	case 6:
		// fdc5ex
		if (fdc5ex) valid = fdc5ex->debug_write_reg(reg_num, data);
		break;
	case 7:
		// cmt
		if (cmt) valid = cmt->debug_write_reg(reg_num, data);
		break;
	case 8:
		// timer
		if (timer) valid = timer->debug_write_reg(reg_num, data);
		break;
	case 9:
		// fddunit
		if (fdd) valid = fdd->debug_write_reg(reg_num, data);
		break;
	case 10:
		// board
		if (board) valid = board->debug_write_reg(reg_num, data);
		break;
	case 11:
		// psg
		if (psgst) valid = psgst->debug_write_reg(reg_num, data);
		break;
	case 12:
		// psgex
		if (psgex) valid = psgex->debug_write_reg(reg_num, data);
		break;
	case 13:
	case 14:
		// psg6x
		if (psg3[num-13]) valid = psg3[num-13]->debug_write_reg(reg_num, data);
		break;
	case 15:
	case 16:
	case 17:
		// psg9x
		if (psg9[num-15]) valid = psg9[num-15]->debug_write_reg(reg_num, data);
		break;
	case 21:
		// pia ex
		if (pia_ex) valid = pia_ex->debug_write_reg(reg_num, data);
		break;
	case 22:
		// acia ex
		if (acia_ex) valid = acia_ex->debug_write_reg(reg_num, data);
		break;
	case 23:
		// via
		if (psg9_via) valid = psg9_via->debug_write_reg(reg_num, data);
		break;
	case 24:
		// fm opn
		if (fmopn) valid = fmopn->debug_write_reg(reg_num, data);
		break;
	case 27:
		// rtc
		if (rtc) valid = rtc->debug_write_reg(reg_num, data);
		break;
	case 28:
		// msm58321
		if (msm58321) valid = msm58321->debug_write_reg(reg_num, data);
		break;
	}
	return valid;
}

void VM::debug_regs_info(uint32_t num, _TCHAR *buffer, size_t buffer_len)
{
	switch(num) {
	case 0:
		// keyboard
		if (key) key->debug_regs_info(buffer, buffer_len);
		break;
	case 1:
		// crtc
		if (crtc) crtc->debug_regs_info(buffer, buffer_len);
		break;
	case 2:
		// pia
		if (pia) pia->debug_regs_info(buffer, buffer_len);
		break;
	case 3:
		// acia
		if (acia) acia->debug_regs_info(buffer, buffer_len);
		break;
	case 4:
		// fdc3
		if (fdc3) fdc3->debug_regs_info(buffer, buffer_len);
		break;
	case 5:
		// fdc5
		if (fdc5) fdc5->debug_regs_info(buffer, buffer_len);
		break;
	case 6:
		// fdc5ex
		if (fdc5ex) fdc5ex->debug_regs_info(buffer, buffer_len);
		break;
	case 7:
		// cmt
		if (cmt) cmt->debug_regs_info(buffer, buffer_len);
		break;
	case 8:
		// timer
		if (timer) timer->debug_regs_info(buffer, buffer_len);
		break;
	case 9:
		// fddunit
		if (fdd) fdd->debug_regs_info(buffer, buffer_len);
		break;
	case 10:
		// board
		if (board) board->debug_regs_info(buffer, buffer_len);
		break;
	case 11:
		// psg
		if (psgst) psgst->debug_regs_info(buffer, buffer_len);
		break;
	case 12:
		// psgex
		if (psgex) psgex->debug_regs_info(buffer, buffer_len);
		break;
	case 13:
	case 14:
		// psg6x
		if (psg3[num-13]) psg3[num-13]->debug_regs_info(buffer, buffer_len);
		break;
	case 15:
	case 16:
	case 17:
		// psg9x
		if (psg9[num-15]) psg9[num-15]->debug_regs_info(buffer, buffer_len);
		break;
	case 21:
		// pia_ex
		if (pia_ex) pia_ex->debug_regs_info(buffer, buffer_len);
		break;
	case 22:
		// acia_ex
		if (acia_ex) acia_ex->debug_regs_info(buffer, buffer_len);
		break;
	case 23:
		// via
		if (psg9_via) psg9_via->debug_regs_info(buffer, buffer_len);
		break;
	case 24:
		// fm opn
		if (fmopn) fmopn->debug_regs_info(buffer, buffer_len);
		break;
	case 27:
		// rtc
		if (rtc) rtc->debug_regs_info(buffer, buffer_len);
		break;
	case 28:
		// msm58321
		if (msm58321) msm58321->debug_regs_info(buffer, buffer_len);
		break;
	}
}

#endif

// ----------------------------------------------------------------------------
// event manager
// ----------------------------------------------------------------------------

void VM::register_event(DEVICE* dev, int event_id, int usec, bool loop, int* register_id)
{
	event->register_event(dev, event_id, usec, loop, register_id);
}

void VM::register_event_by_clock(DEVICE* dev, int event_id, int clock, bool loop, int* register_id)
{
	event->register_event_by_clock(dev, event_id, clock, loop, register_id);
}

void VM::cancel_event(DEVICE *dev, int register_id)
{
	event->cancel_event(dev, register_id);
}

void VM::register_frame_event(DEVICE* dev)
{
	event->register_frame_event(dev);
}

void VM::register_vline_event(DEVICE* dev)
{
	event->register_vline_event(dev);
}

uint64_t VM::get_current_clock()
{
	return event->get_current_clock();
}

uint64_t VM::get_passed_clock(uint64_t prev)
{
	return event->get_passed_clock(prev);
}

//uint32_t VM::get_pc()
//{
//	return cpu->get_pc();
//}

void VM::set_lines_per_frame(int lines) {
	event->set_lines_per_frame(lines);
}

// ----------------------------------------------------------------------------
// draw screen
// ----------------------------------------------------------------------------

void VM::set_display_size(int left, int top, int right, int bottom)
{
	display->set_display_size(left, top, right, bottom);
}

void VM::draw_screen()
{
//	display->set_vram_ptr(memory->get_vram());
	display->draw_screen();
}

// ----------------------------------------------------------------------------
uint64_t VM::update_led()
{
	uint64_t status = 0;

	// b0: power on/off
	status |= (config.now_power_off ? 0 : 1);
	// b1: mode
	status |= ((1 - (emu->get_parami(ParamSysMode) & SYS_MODE_S1L3)) << 1);
#ifdef USE_FD1
	// b2-b3: fdd type
	status |= ((config.fdd_type & MSK_FDD_TYPE) << 2);
#endif
	// b4-b6: kbd led
	status |= ((key->get_kb_mode()) << 4);
	// b8-b11: cmt led  b12-b27:cmt cnt
	status |= (cmt->get_cmt_mode() << 8);
#ifdef USE_FD1
	// b28-b31: drive select, b32-b35: 0:green led, 1:red led b36-39:inserted?
	status |= ((uint64_t)fdd->get_drive_select() << 28);
#endif
	// b40-b43 system mode
	status |= ((uint64_t)REG_SYS_MODE << 40);
	return status;
}

// ----------------------------------------------------------------------------
// sound manager
// ----------------------------------------------------------------------------

/// Initialize sound device at starting application
/// @param [in] rate : sampling rate
/// @param [in] samples : sample number per second
void VM::initialize_sound(int rate, int samples)
{
	// init sound manager
	event->initialize_sound(rate, samples);

	// init sound gen
	sound->initialize_sound(rate, 0);
	psgst->initialize_sound(rate, CLOCKS_1MHZ, samples, 0, 3);
//	psgex->initialize_sound(rate, CLOCKS_1MHZ, samples, 0, 3);
	psgex->initialize_sound(rate, CLOCKS_1MHZ * 2, samples, 0, 0, config.use_opn_expsg ? 0 : 3);
	psg3[0]->initialize_sound(rate, CLOCKS_1MHZ, samples, 0, 0);
	psg3[1]->initialize_sound(rate, CLOCKS_1MHZ, samples, 0, 0);
	psg9[0]->initialize_sound(rate, CLOCKS_1MHZ * 2, samples, 0, 0);
	psg9[1]->initialize_sound(rate, CLOCKS_1MHZ * 2, samples, 0, 0);
	psg9[2]->initialize_sound(rate, CLOCKS_1MHZ * 2, samples, 0, 0);
	cmt->initialize_sound(rate, 0);
#ifdef USE_FD1
	fdd->initialize_sound(rate, 0);
#endif
	fmopn->initialize_sound(rate, fmopn_clocks[config.opn_clock], samples, 0, 0, 0);

	//
	set_volume();
}

/// Re-initialize sound device under power-on operation
/// @param [in] rate : sampling rate
/// @param [in] samples : sample number per second
void VM::reset_sound(int rate, int samples)
{
	psgex->init_context_irq();
	fmopn->init_context_irq();
	psgex->initialize_sound(rate, CLOCKS_1MHZ * 2, samples, 0, 0, config.use_opn_expsg ? 0 : 3);
	fmopn->initialize_sound(rate, fmopn_clocks[config.opn_clock], samples, 0, 0, 0);
	if (config.opn_irq > 0) {
		int id = (config.opn_irq == 2 ? SIG_CPU_FIRQ : SIG_CPU_IRQ);
		fmopn->set_context_irq(board, id, SIG_IRQ_FMOPN_MASK);
		psgex->set_context_irq(board, id, SIG_IRQ_FMOPNEX_MASK);
	}

	set_volume();
}

/// @attention called by another thread
audio_sample_t* VM::create_sound(int* extra_frames, int samples)
{
	return event->create_sound(extra_frames, samples);
}

void VM::set_volume()
{
	int vol = 0;
	event->set_volume(config.volume - 81, config.mute);
	sound->set_volume(config.beep_volume - 81, config.beep_mute);
	vol = config.psg_volume - 81;
	psgst->set_volume(vol, vol, config.psg_mute, 3);
//	psgex->set_volume(vol, vol, config.psg_mute || (IOPORT_USE_EXPSG == 0), 3);
	vol = config.psgexfm_volume - 81;
	psgex->set_volume(0, vol, vol, config.psgexfm_mute || (IOPORT_USE_EXPSG == 0) || !config.use_opn_expsg, 0);
	vol = config.psgexssg_volume - 81;
	psgex->set_volume(1, vol, vol, config.psgexssg_mute || (IOPORT_USE_EXPSG == 0), config.use_opn_expsg ? 0 : 3);
	vol = config.psg6_volume - 81;
	psg3[0]->set_volume(vol, vol, config.psg6_mute || (IOPORT_USE_PSG6 == 0), 0);
	psg3[1]->set_volume(vol, vol, config.psg6_mute || (IOPORT_USE_PSG6 == 0), 0);
	vol = config.psg9_volume - 81;
	psg9[0]->set_volume(vol, vol, config.psg9_mute || (IOPORT_USE_PSG9 == 0), 0);
	psg9[1]->set_volume(vol, vol, config.psg9_mute || (IOPORT_USE_PSG9 == 0), 0);
	psg9[2]->set_volume(vol, vol, config.psg9_mute || (IOPORT_USE_PSG9 == 0), 0);
	cmt->set_volume(config.relay_volume - 81, config.cmt_volume - 81, config.relay_mute, config.cmt_mute);
#ifdef USE_FD1
	fdd->set_volume(config.fdd_volume - 81, config.fdd_mute);
#endif
	vol = config.opnfm_volume - 81;
	fmopn->set_volume(0, vol, vol, config.opnfm_mute || (IOPORT_USE_FMOPN == 0), 0);
	vol = config.opnssg_volume - 81;
	fmopn->set_volume(1, vol, vol, config.opnssg_mute || (IOPORT_USE_FMOPN == 0), 0);
}

// ----------------------------------------------------------------------------
// notify key
// ----------------------------------------------------------------------------

void VM::key_down(int code)
{
}

void VM::key_up(int code)
{
}

void VM::system_key_down(int code)
{
	key->system_key_down(code);
}

void VM::system_key_up(int code)
{
	key->system_key_up(code);
}

void VM::set_mouse_position(int px, int py)
{
	mouse->set_mouse_position(px, py);
}

// ----------------------------------------------------------------------------
// user interface
// ----------------------------------------------------------------------------

bool VM::play_datarec(const _TCHAR* file_path)
{
	return cmt->play_datarec(file_path);
}

bool VM::rec_datarec(const _TCHAR* file_path)
{
	return cmt->rec_datarec(file_path);
}

void VM::close_datarec()
{
	cmt->close_datarec();
}

void VM::rewind_datarec()
{
	cmt->rewind_datarec();
}

void VM::fast_forward_datarec()
{
	cmt->fast_forward_datarec();
}

void VM::stop_datarec()
{
	cmt->stop_datarec();
}

void VM::realmode_datarec()
{
	cmt->realmode_datarec();
}

bool VM::datarec_opened(bool play_mode)
{
	return cmt->datarec_opened(play_mode);
}

// ----------------------------------------------------------------------------

#ifdef USE_FD1
bool VM::open_disk(int drv, const _TCHAR* file_path, int offset, uint32_t flags)
{
	return fdd->open_disk(drv, file_path, offset, flags);
}

void VM::close_disk(int drv)
{
	fdd->close_disk(drv);
}

int VM::change_disk(int drv)
{
	switch(config.fdd_type) {
		case FDD_TYPE_3FDD:
			return fdd->change_disk(drv);
			break;
	}
	return 0;
}

bool VM::disk_inserted(int drv)
{
	return fdd->disk_inserted(drv);
}

int VM::get_disk_side(int drv)
{
	return fdd->get_disk_side(drv);
}

void VM::toggle_disk_write_protect(int drv)
{
	fdd->toggle_disk_write_protect(drv);
}

bool VM::disk_write_protected(int drv)
{
	return fdd->disk_write_protected(drv);
}
#endif
// ----------------------------------------------------------------------------
void VM::update_config()
{
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
		device->update_config();
	}
}

// ----------------------------------------------------------------------------
bool VM::save_printer(int dev, const _TCHAR* filename)
{
	return printer[dev]->save_printer(filename);
}

void VM::clear_printer(int dev)
{
	printer[dev]->reset();
}

int VM::get_printer_buffer_size(int dev)
{
	return printer[dev]->get_buffer_size();
}

uint8_t* VM::get_printer_buffer(int dev)
{
	return printer[dev]->get_buffer();
}

void VM::enable_printer_direct(int dev)
{
	printer[dev]->set_direct_mode();
}

bool VM::print_printer(int dev)
{
	return printer[dev]->print_printer();
}

void VM::toggle_printer_online(int dev)
{
	return printer[dev]->toggle_printer_online();
}

// ----------------------------------------------------------------------------
/// @note called by main thread
void VM::enable_comm_server(int dev)
{
	comm[dev]->enable_server();
}

/// @note called by main thread
void VM::enable_comm_connect(int dev, int num)
{
	comm[dev]->enable_connect(num);
}

/// @note called by main thread
bool VM::now_comm_connecting(int dev, int num)
{
	return comm[dev]->now_connecting(num);
}

/// @note called by main thread
void VM::send_comm_telnet_command(int dev, int num)
{
	comm[dev]->send_telnet_command(num);
}

// ----------------------------------------------------------------------------
void VM::save_keybind()
{
	key->save_keybind();
}

// ----------------------------------------------------------------------------
void VM::change_archtecture(int id, int num, bool reset)
{
	switch(id) {
	case ArchFddType:
		change_fdd_type(num, reset);
		break;
	case ArchSysMode:
		change_sys_mode(num, reset);
		break;
	default:
		break;
	}
}

void VM::change_fdd_type(int num, bool reset)
{
	const CMsg::Id list[] = {
		CMsg::Non_FDD,
		CMsg::FD3inch_compact_FDD,
		CMsg::FD5inch_mini_FDD_2D_Type,
		CMsg::FD5inch_mini_FDD_2HD_Type,
		CMsg::Unsupported_FDD,
		CMsg::End
	};

	if (num == -1) {
		num = emu->get_parami(ParamFddType);
		num = (num + 1) % 4;
	} else {
		num = num % 4;
	}
	emu->set_parami(ParamFddType, num);
	int io_port = emu->get_parami(ParamIOPort);
	switch(num) {
	case FDD_TYPE_NOFDD:
		io_port &= ~IOPORT_MSK_FDDALL;
		break;
	case FDD_TYPE_3FDD:
		io_port &= ~IOPORT_MSK_FDDALL;
		io_port |= IOPORT_MSK_3FDD;
		break;
	case FDD_TYPE_5FDD:
		io_port &= ~IOPORT_MSK_FDDALL;
		io_port |= IOPORT_MSK_5FDD;
		break;
	case FDD_TYPE_58FDD:
		io_port &= ~IOPORT_MSK_FDDALL;
		io_port |= IOPORT_MSK_5FDD;
		break;
	default:
		break;
	}
	CMsg::Id need = CMsg::Null;
	if (config.fdd_type != num && !reset) {
		if(!config.now_power_off) need = CMsg::LB_Need_PowerOn_RB;
	}
	emu->set_parami(ParamIOPort, io_port);
	if (reset) {
		config.fdd_type = num;
		config.io_port = io_port;
		if(!config.now_power_off) logging->out_log_x(LOG_INFO, list[num]);
	} else {
		emu->out_infoc_x(list[num], need, 0);
	}
}

void VM::change_sys_mode(int num, bool reset)
{
	const CMsg::Id list[] = {
		CMsg::B_Mode_L3,
		CMsg::A_Mode_S1,
		CMsg::End
	};

	int sys_mode = emu->get_parami(ParamSysMode);

	if (num == -1) {
		num = ((sys_mode & SYS_MODE_S1L3) + 1) % 2;
	}
	sys_mode = (num != 0 ? (sys_mode | SYS_MODE_S1L3) : (sys_mode & ~SYS_MODE_S1L3));
	emu->set_parami(ParamSysMode, sys_mode);
	CMsg::Id need = CMsg::Null;
	if ((config.sys_mode & SYS_MODE_S1L3) != (sys_mode & SYS_MODE_S1L3) && !reset) {
		if(!config.now_power_off) need = CMsg::LB_Need_PowerOn_RB;
	}
	if (reset) {
		config.sys_mode = sys_mode;
		if(!config.now_power_off) logging->out_log_x(LOG_INFO, list[num]);
	} else {
		emu->out_infoc_x(list[num], need, 0);
	}
}

// ----------------------------------------------------------------------------

/// @brief load a data from file
///
/// @param[in]  file_path : directory
/// @param[in]  file_name : data file
/// @param[out] data      : loaded data
/// @param[in]  size      : buffer size of data
/// @param[in]  first_data      : (nullable) first pattern to compare to loaded data
/// @param[in]  first_data_size :
/// @param[in]  last_data       : (nullable) last pattern to compare to loaded data
/// @param[in]  last_data_size  :
/// @return successfully loaded
bool VM::load_data_from_file(const _TCHAR *file_path, const _TCHAR *file_name
	, uint8_t *data, size_t size
	, const uint8_t *first_data, size_t first_data_size
	, const uint8_t *last_data,  size_t last_data_size)
{
	return EMU::load_data_from_file(file_path, file_name, data, size
		, first_data, first_data_size, last_data, last_data_size);
}

/// @brief get VM specific parameter
///
/// @param[in] id
/// @return parameter
int VM::get_parami(enumParamiId id) const
{
	return emu->get_parami(id);
}

/// @brief set VM specific parameter
///
/// @param[in] id
/// @param[in] val : parameter
void VM::set_parami(enumParamiId id, int val)
{
	emu->set_parami(id, val);
}

/// @brief get VM specific object
///
/// @param[in] id
/// @return object
void *VM::get_paramv(enumParamvId id) const
{
	return emu->get_paramv(id);
}

/// @brief set VM specific object
///
/// @param[in] id
/// @param[in] val : object
void VM::set_paramv(enumParamvId id, void *val)
{
	emu->set_paramv(id, val);
}

const _TCHAR *VM::application_path() const
{
	return emu->application_path();
}

const _TCHAR *VM::initialize_path() const
{
	return emu->initialize_path();
}

bool VM::get_pause(int idx) const
{
	return emu->get_pause(idx);
}

void VM::set_pause(int idx, bool val)
{
	emu->set_pause(idx, val);
}

//
void VM::get_edition_string(char *buffer, size_t buffer_len) const
{
	*buffer = '\0';
#if defined(USE_Z80B_CARD)
	UTILITY::strcat(buffer, buffer_len, *buffer == '\0' ? " with " : ", ");
	UTILITY::strcat(buffer, buffer_len, "Z80B Card");
#endif
#ifdef USE_DEBUGGER
	UTILITY::strcat(buffer, buffer_len, *buffer == '\0' ? " with " : ", ");
	UTILITY::strcat(buffer, buffer_len, "Debugger");
#endif
}

// ----------------------------------------------------------------------------
bool VM::save_state(const _TCHAR* filename)
{
	FILEIO *fio = new FILEIO();
	vm_state_header_t vm_state_h;
	bool rc = false;

	// header
	memset(&vm_state_h, 0, sizeof(vm_state_h));
	strncpy(vm_state_h.header, RESUME_FILE_HEADER, 16);
	vm_state_h.version = Uint16_LE(RESUME_FILE_VERSION);
	vm_state_h.revision = Uint16_LE(RESUME_FILE_REVISION);
	vm_state_h.param = Uint32_LE(0);
	vm_state_h.emu_major = Uint16_LE(APP_VER_MAJOR);
	vm_state_h.emu_minor = Uint16_LE(APP_VER_MINOR);
	vm_state_h.emu_rev   = Uint16_LE(APP_VER_REV);

	if(fio->Fopen(filename, FILEIO_WRITE_BINARY)) {
		// write header
		fio->Fwrite(&vm_state_h, sizeof(vm_state_h), 1);
		// write data
		for(DEVICE* device = first_device; device; device = device->get_next_device()) {
			device->save_state(fio);
		}
		fio->Fclose();
		rc = true;
	}
	delete fio;
	return rc;
}

bool VM::load_state(const _TCHAR* filename)
{
	FILEIO *fio = new FILEIO();
	vm_state_header_t vm_state_h;
	bool rc = false;

	if(!fio->Fopen(filename, FILEIO_READ_BINARY)) {
		logging->out_log_x(LOG_ERROR, CMsg::Load_State_Cannot_open);
		goto FIN;
	}
	// read header
	fio->Fread(&vm_state_h, sizeof(vm_state_h), 1);
	// check header
	if (strncmp(vm_state_h.header, RESUME_FILE_HEADER, 16) != 0) {
		if (strncmp(vm_state_h.header, RESUME_FILE_HEADER_L3, 16) != 0) {
			logging->out_log_x(LOG_ERROR, CMsg::Load_State_Unsupported_file);
			fio->Fclose();
			goto FIN;
		} else {
			logging->out_logf_x(LOG_WARN, CMsg::Load_State_No_longer_support_a_status_file_for_VSTR, _T("BML3MK5"));
		}
	}
	if (Uint16_LE(vm_state_h.version) != RESUME_FILE_VERSION) {
		logging->out_log_x(LOG_ERROR, CMsg::Load_State_Invalid_version);
		fio->Fclose();
		goto FIN;
	}
	// read data
	for(DEVICE* device = first_device; device; device = device->get_next_device()) {
		device->load_state(fio);
	}
	fio->Fclose();
	rc = true;
	//
	set_volume();
FIN:
	delete fio;
	return rc;
}

// ----------------------------------------------------------------------------

#ifdef USE_KEY_RECORD
bool VM::play_reckey(const _TCHAR* filename)
{
	return key->play_reckey(filename);
}

bool VM::record_reckey(const _TCHAR* filename)
{
	return key->record_reckey(filename);
}

void VM::stop_reckey(bool stop_play, bool stop_record)
{
	key->stop_reckey(stop_play, stop_record);
}
#endif



void VM::Switch(bool f) {
	cpu = f ? (DEVICE *)newmpu : (DEVICE *)curmpu;
	event->set_context_cpu(cpu, CPU_CLOCKS);
	key->set_context_cpu(cpu);
	memory->cp.setCompare(f);
}

#ifdef NEWMPU
MPUWrap::MPUWrap(VM *vm, EMU *emu, const char *s) : DEVICE(vm, emu, s), reset(false), halt(false), halt1(false) {
	hd6309 = new HD6309;
}
MPUWrap::~MPUWrap() {
	delete hd6309;
}
int MPUWrap::run(int clock, int accum, int cycle) {
	int r = reset || halt ? 1 : hd6309->Execute(0);
	halt = halt1;
	return r;
}
void MPUWrap::write_signal(int id, uint32_t data, uint32_t mask) {
	if (data & mask)
		switch (id) {
			case SIG_CPU_RESET:
				reset = true;
				break;
			case SIG_CPU_HALT:
				halt1 = true;
				break;
			case SIG_CPU_NMI:
				hd6309->NMI();
				break;
			case SIG_CPU_FIRQ:
				hd6309->FIRQ();
				break;
			case SIG_CPU_IRQ:
				hd6309->IRQ();
				break;
		}
	else if (reset && id == SIG_CPU_RESET) {
		hd6309->Reset();
		reset = false;
	}
	else if (halt && id == SIG_CPU_HALT)
		halt = halt1 = false;
}
void MPUWrap::StopTrace() {
#if HD6309_TRACE
	hd6309->StopTrace();
#endif
}
#endif

uint32_t memfetch(uint32_t addr) {
	return sMemory->read_data8w_fetch(addr, &sICount);
}
uint32_t memread(uint32_t addr) {
	return sMemory->read_data8w(addr, &sICount);
}
void memwrite(uint32_t addr, uint32_t data) {
	sMemory->write_data8w(addr, data, &sICount);
}
