/** @file ym2203.cpp

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2006.09.15-

	@note
	Modified by Sasaji at 2012.06.20
	@note
	AY-3-8910 functions has gone to AY38910 class

	@brief [ YM2203 / YM2608 ]
*/

#include "ym2203.h"
#include "../emu.h"
#include "../fileio.h"
#include "../utility.h"

#define EVENT_FM_TIMER	0

void YM2203::initialize()
{
#ifdef HAS_YM2608
	if(is_ym2608) {
		opna = new FM::OPNA;
	} else
#endif
	opn = new FM::OPN;
	register_vline_event(this);
	mute = false;
	clock_prev = clock_accum = clock_busy = 0;
}

void YM2203::release()
{
#ifdef HAS_YM2608
	if(is_ym2608) {
		delete opna;
	} else
#endif
	delete opn;
}

void YM2203::reset()
{
#ifdef HAS_YM2608
	if(is_ym2608) {
		opna->Reset();
	} else
#endif
	opn->Reset();
	fnum2 = 0;
#ifdef HAS_YM2608
	fnum21 = 0;
#endif
#ifdef USE_DEBUGGER
	memset(debug_regs, 0, sizeof(debug_regs));
#endif

	// stop timer
	timer_event_id = -1;
	this->set_reg(0x27, 0);

	port[0].first = port[1].first = true;
	port[0].wreg = port[1].wreg = 0;//0xff;
#ifdef YM2203_PORT_MODE
	mode = YM2203_PORT_MODE;
#else
	mode = 0;
#endif
	irq_prev = busy = false;
}

#ifdef HAS_YM2608
#define amask (is_ym2608 ? 3 : 1)
#else
#define amask 1
#endif

void YM2203::write_io8(uint32_t addr, uint32_t data)
{
	switch(addr & amask) {
	case 0:
		ch = data;
		// write dummy data for prescaler
		if(0x2d <= ch && ch <= 0x2f) {
			update_count();
			this->set_reg(ch, 0);
			update_interrupt();
			clock_busy = get_current_clock();
			busy = true;
		}
		break;
	case 1:
		if(ch == 7) {
#ifdef YM2203_PORT_MODE
			mode = (data & 0x3f) | YM2203_PORT_MODE;
#else
			mode = data;
#endif
		}
		if (ch == 14) {
			if(port[0].wreg != data || port[0].first) {
				write_signals(&port[0].outputs, data);
				port[0].wreg = data;
				port[0].first = false;
			}
		} else if(ch == 15) {
			if(port[1].wreg != data || port[1].first) {
				write_signals(&port[1].outputs, data);
				port[1].wreg = data;
				port[1].first = false;
			}
		} else if(0x2d <= ch && ch <= 0x2f) {
			// don't write again for prescaler
		} else if(0xa4 <= ch && ch <= 0xa6) {
			// XM8 version 1.20
			fnum2 = data;
		} else {
			update_count();
			// XM8 version 1.20
			if(0xa0 <= ch && ch <= 0xa2) {
				this->set_reg(ch + 4, fnum2);
			}
			this->set_reg(ch, data);
			if(ch == 0x27) {
				update_event();
			}
			update_interrupt();
			clock_busy = get_current_clock();
			busy = true;
		}
		break;
#ifdef HAS_YM2608
	case 2:
		ch1 = data1 = data;
		break;
	case 3:
		if(0xa4 <= ch1 && ch1 <= 0xa6) {
			// XM8 version 1.20
			fnum21 = data;
		} else {
			update_count();
			// XM8 version 1.20
			if(0xa0 <= ch1 && ch1 <= 0xa2) {
				this->set_reg(0x100 | (ch1 + 4), fnum21);
			}
			this->set_reg(0x100 | ch1, data);
			data1 = data;
			update_interrupt();
			clock_busy = get_current_clock();
			busy = true;
		}
		break;
#endif
	}
}

uint32_t YM2203::read_io8(uint32_t addr)
{
	switch(addr & amask) {
	case 0:
		{
			/* BUSY : x : x : x : x : x : FLAGB : FLAGA */
			update_count();
			update_interrupt();
			return read_status();
		}
	case 1:
		if(ch == 14) {
			return (mode & 0x40) ? port[0].wreg : port[0].rreg;
		} else if(ch == 15) {
			return (mode & 0x80) ? port[1].wreg : port[1].rreg;
		}
#ifdef HAS_YM2608
		if(is_ym2608) {
			return opna->GetReg(ch);
		} else
#endif
		return opn->GetReg(ch);
#ifdef HAS_YM2608
	case 2:
		{
			/* BUSY : x : PCMBUSY : ZERO : BRDY : EOS : FLAGB : FLAGA */
			update_count();
			update_interrupt();
			uint32_t status = opna->ReadStatusEx() & ~0x80;
			if(busy) {
				// FIXME: we need to investigate the correct busy period
				if(get_passed_usec(clock_busy) < 8) {
					status |= 0x80;
				} else {
					busy = false;
				}
			}
			return status;
		}
	case 3:
		if(ch1 == 8) {
			return opna->GetReg(0x100 | ch1);
//		} else if(ch1 == 0x0f) {
//			return 0x80; // from mame fm.c
		}
		return data1;
#endif
	}
	return 0xff;
}

void YM2203::write_signal(int id, uint32_t data, uint32_t mask)
{
	if(id == SIG_YM2203_MUTE) {
		mute = ((data & mask) != 0);
	} else if(id == SIG_YM2203_PORT_A) {
		port[0].rreg = (port[0].rreg & ~mask) | (data & mask);
	} else if(id == SIG_YM2203_PORT_B) {
		port[1].rreg = (port[1].rreg & ~mask) | (data & mask);
	} else if (id == SIG_CPU_RESET) {
		now_reset = (data & mask) ? true : false;
		reset();
	}
}

void YM2203::event_vline(int v, int clock)
{
	update_count();
	update_interrupt();
}

void YM2203::event_callback(int event_id, int error)
{
	update_count();
	update_interrupt();
	timer_event_id = -1;
	update_event();
}

void YM2203::update_count()
{
	clock_accum += clock_const * get_passed_clock(clock_prev);
	uint32_t count = (uint32_t)(clock_accum >> 20);
	if(count) {
#ifdef HAS_YM2608
		if(is_ym2608) {
			opna->Count(count);
		} else
#endif
		opn->Count(count);
		clock_accum -= count << 20;
	}
	clock_prev = get_current_clock();
}

void YM2203::update_event()
{
	if(timer_event_id != -1) {
		cancel_event(this, timer_event_id);
		timer_event_id = -1;
	}

	int count;
#ifdef HAS_YM2608
	if(is_ym2608) {
		count = opna->GetNextEvent();
	} else
#endif
	count = opn->GetNextEvent();

	if(count > 0) {
#ifdef HAS_YM2608
		if(is_ym2608) {
			register_event(this, EVENT_FM_TIMER, 1000000.0 / (double)chip_clock * (double)count * 2.0, false, &timer_event_id);
		} else
#endif
		register_event(this, EVENT_FM_TIMER, 1000000.0 / (double)chip_clock * (double)count, false, &timer_event_id);
	}
}

#ifdef HAS_YM_SERIES
void YM2203::update_interrupt()
{
	bool irq;
#ifdef HAS_YM2608
	if(is_ym2608) {
		irq = opna->ReadIRQ();
	} else
#endif
	irq = opn->ReadIRQ();
	if(!irq_prev && irq) {
		write_signals(&outputs_irq, 0xffffffff);
	} else if(irq_prev && !irq) {
		write_signals(&outputs_irq, 0);
	}
	irq_prev = irq;
}
#endif

void YM2203::mix(int32_t* buffer, int cnt)
{
#if 0 // def USE_EMU_INHERENT_SPEC
	memset(buffer_tmp, 0, sizeof(buffer_tmp));
	if (cnt > 20) cnt = 20;
	if(cnt > 0 && !mute) {
#ifdef HAS_YM2608
		if(is_ym2608) {
			opna->Mix(buffer_tmp, cnt);
		} else
#endif
		opn->Mix(buffer_tmp, cnt);
	}
	for(int i=0; i<(cnt * 2); i++) {
		*buffer++ += (buffer_tmp[i] * sound_volume / 100);
	}
#else
	if(cnt > 0 && !mute) {
#ifdef HAS_YM2608
		if(is_ym2608) {
			opna->Mix(buffer, cnt);
		} else
#endif
		opn->Mix(buffer, cnt);
	}
#endif
}

void YM2203::set_volume(int ch, int decibel_l, int decibel_r, bool vol_mute, int pattern)
{
	if (vol_mute) {
		decibel_l = -192;
		decibel_r = -192;
	}
	if(ch == 0) {
#ifdef HAS_YM2608
		if(is_ym2608) {
			opna->SetVolumeFM(base_decibel_fm + decibel_l, base_decibel_fm + decibel_r);
		} else
#endif
		opn->SetVolumeFM(base_decibel_fm + decibel_l, base_decibel_fm + decibel_r);
	} else if(ch == 1) {
#ifdef HAS_YM2608
		if(is_ym2608) {
			opna->SetVolumePSG(base_decibel_psg + decibel_l, base_decibel_psg + decibel_r);
		} else
#endif
		opn->SetVolumePSG(base_decibel_psg + decibel_l, base_decibel_psg + decibel_r, pattern);
#ifdef HAS_YM2608
	} else if(ch == 2) {
		if(is_ym2608) {
			opna->SetVolumeADPCM(decibel_l, decibel_r);
		}
	} else if(ch == 3) {
		if(is_ym2608) {
			opna->SetVolumeRhythmTotal(decibel_l, decibel_r);
		}
#endif
	}
}

void YM2203::initialize_sound(int rate, int clock, int samples, int decibel_fm, int decibel_psg, int ptn)
{
#ifdef HAS_YM2608
	if(is_ym2608) {
		opna->Init(clock, rate, false, get_application_path());
		opna->SetVolumeFM(decibel_fm, decibel_fm);
		opna->SetVolumePSG(decibel_psg, decibel_psg, ptn);
	} else {
#endif
		opn->Init(clock, rate, false, NULL);
		opn->SetVolumeFM(decibel_fm, decibel_fm);
		opn->SetVolumePSG(decibel_psg, decibel_psg, ptn);
#ifdef HAS_YM2608
	}
#endif
#ifdef USE_EMU_INHERENT_SPEC
//	sound_volume = 0;
#endif
	chip_clock = clock;
}

void YM2203::set_reg(uint32_t addr, uint32_t data)
{
#ifdef HAS_YM2608
	if(is_ym2608) {
		opna->SetReg(addr, data);
	} else
#endif
	opn->SetReg(addr, data);

#ifdef USE_DEBUGGER
	uint32_t d_addr = (addr & 0xff);
	if (d_addr >= 0x2d && d_addr <= 0x2e) {
		debug_regs[0xf] = 0;
		debug_regs[d_addr - 0x20] = 1;
	} else if (d_addr == 0x2f) {
		debug_regs[0xd] = debug_regs[0xe] = 0;
		debug_regs[d_addr - 0x20] = 1;
	} else if (d_addr >= 0x20) {
		debug_regs[d_addr - 0x20] = (data & 0xff);
	}
#endif
}

uint32_t YM2203::read_status()
{
	uint32_t status;

	/* BUSY : x : x : x : x : x : FLAGB : FLAGA */
#ifdef HAS_YM2608
	if(is_ym2608) {
		status = opna->ReadStatus() & ~0x80;
	} else
#endif
	status = opn->ReadStatus() & ~0x80;
	if(busy) {
		// from PC-88 machine language master bible (XM8 version 1.00)
#ifdef HAS_YM2608
		if (get_passed_usec(clock_busy) < (is_ym2608 ? 4.25 : 2.13)) {
#else
		if (get_passed_usec(clock_busy) < 2.13) {
#endif
			status |= 0x80;
		} else {
			busy = false;
		}
	}
	return status;
}

void YM2203::update_timing(int new_clocks, double new_frames_per_sec, int new_lines_per_frame)
{
#ifdef HAS_YM2608
	if(is_ym2608) {
		clock_const = (uint32_t)((double)chip_clock * 1024.0 * 1024.0 / (double)new_clocks / 2.0 + 0.5);
	} else
#endif
	clock_const = (uint32_t)((double)chip_clock * 1024.0 * 1024.0 / (double)new_clocks + 0.5);
}


// ----------------------------------------------------------------------------
#ifdef USE_EMU_INHERENT_SPEC
void YM2203::save_state(FILEIO *fio)
{
	struct vm_state_st vm_state;
	size_t state_size = 0;

	//
	vm_state_ident.version = Uint16_LE(3);

	// copy values
	memset(&vm_state, 0, sizeof(vm_state));

	// reserved header
	fio->Fwrite(&vm_state_ident, sizeof(vm_state_ident), 1);

	vm_state.v3.ch = ch;
	vm_state.v3.fnum2 = fnum2;
#ifdef HAS_YM2608
	vm_state.v3.ch1 = ch1;
	vm_state.v3.data1 = data1;
	vm_state.v3.fnum21 = fnum21;
#endif

	vm_state.v3.mode = mode;
	for(int i=0; i<2; i++) {
		vm_state.v3.port[i].wreg = port[i].wreg;
		vm_state.v3.port[i].rreg = port[i].rreg;
		vm_state.v3.port[i].first = port[i].first ? 1 : 0;
	}
	vm_state.v3.irq_prev = irq_prev ? 1 : 0;
	vm_state.v3.mute = mute ? 1 : 0;
	vm_state.v3.busy = busy ? 1 : 0;

	vm_state.v3.chip_clock = Int32_LE(chip_clock);
	vm_state.v3.clock_prev = Uint64_LE(clock_prev);
	vm_state.v3.clock_accum = Uint64_LE(clock_accum);
	vm_state.v3.clock_const = Uint64_LE(clock_const);
	vm_state.v3.clock_busy = Uint64_LE(clock_busy);
	vm_state.v3.timer_event_id = Int32_LE(timer_event_id);

	fio->Fwrite(&vm_state, sizeof(vm_state), 1);

#ifdef HAS_YM2608
	if(is_ym2608) {
		opna->SaveState((void *)fio, &state_size);
	} else
#endif
	opn->SaveState((void *)fio, &state_size);

	// set total size
	state_size += sizeof(vm_state);
	vm_state_ident.size = Uint32_LE((uint32_t)(sizeof(vm_state_ident) + state_size));

	// overwrite header
	fio->Fseek(-(long)(state_size + sizeof(vm_state_ident)), FILEIO_SEEK_CUR);
	fio->Fwrite(&vm_state_ident, sizeof(vm_state_ident), 1);
	fio->Fseek((long)state_size, FILEIO_SEEK_CUR);

}

void YM2203::load_state(FILEIO *fio)
{
	vm_state_ident_t vm_state_i;
	struct vm_state_st vm_state;

	int match = 0;
	if (find_state_chunk(fio, &vm_state_i) == true) {
		if (vm_state_i.version >= 3) {
			match = 1;
		} else {
			match = 2;
		}
	} else {
		// read AY38910 parameter
		if (find_state_chunk(fio, "AY38910", this_identifier, &vm_state_i)) {
			if (vm_state_i.version >= 3) {
				match = 2;
			}
		}
	}

	if (!match) {
		return;
	}

	uint32_t i_size = Uint32_LE(vm_state_i.size);
	memset(&vm_state, 0, sizeof(vm_state));
	if (i_size >= (sizeof(vm_state) + sizeof(vm_state_i))) {
		fio->Fread(&vm_state, sizeof(vm_state), 1);
	} else {
		fio->Fread(&vm_state, i_size - sizeof(vm_state_i), 1);
	}

	if (match == 2) {
		// read AY38910 parameter
		struct vm_state_psg_st *vm_state_psg = (struct vm_state_psg_st *)&vm_state;
		opn->SetReg(0x2d, 0);
		opn->SetReg(0x28, 0);
		opn->SetReg(0x28, 1);
		opn->SetReg(0x28, 2);
		ch = vm_state_psg->ch;
		if (Uint16_LE(vm_state_i.version) >= 2) {
			for(int i=0; i<14; i++) {
				 opn->SetReg(i, vm_state_psg->v2.reg[i]);
			}
#ifdef SUPPORT_YM2203_PORT
			mode = vm_state_psg->mode;
			for(int i=0; i<2; i++) {
				port[i].wreg = vm_state_psg->v2.port[i].wreg;
				port[i].rreg = vm_state_psg->v2.port[i].rreg;
				port[i].first = vm_state_psg->v2.port[i].first ? true : false;
			}
#endif
		} else {
			for(int i=0; i<13; i++) {
				 opn->SetReg(i, vm_state_psg->v1.reg[i]);
			}
#ifdef SUPPORT_YM2203_PORT
			mode = vm_state_psg->mode;
			for(int i=0; i<2; i++) {
				port[i].wreg = vm_state_psg->v1.port[i].wreg;
				port[i].rreg = vm_state_psg->v1.port[i].rreg;
				port[i].first = vm_state_psg->v1.port[i].first ? true : false;
			}
#endif
		}
	} else {
		ch = vm_state.v3.ch;
		fnum2 = vm_state.v3.fnum2;
#ifdef HAS_YM2608
		ch1 = vm_state.v3.ch1;
		data1 = vm_state.v3.data1;
		fnum21 = vm_state.v3.fnum21;
#endif

		mode = vm_state.v3.mode;
		for(int i=0; i<2; i++) {
			port[i].wreg = vm_state.v3.port[i].wreg;
			port[i].rreg = vm_state.v3.port[i].rreg;
			port[i].first = (vm_state.v3.port[i].first == 1);
		}
		irq_prev = (vm_state.v3.irq_prev == 1);
		mute = (vm_state.v3.mute == 1);
		busy = (vm_state.v3.busy == 1);

		chip_clock = Int32_LE(vm_state.v3.chip_clock);
		clock_prev = Uint64_LE(vm_state.v3.clock_prev);
		clock_accum = Uint64_LE(vm_state.v3.clock_accum);
		clock_const = Uint64_LE(vm_state.v3.clock_const);
		clock_busy = Uint64_LE(vm_state.v3.clock_busy);
		timer_event_id = Int32_LE(vm_state.v3.timer_event_id);

#ifdef HAS_YM2608
		if(is_ym2608) {
			opna->LoadState((void *)fio);
		} else
#endif
		opn->LoadState((void *)fio);
	}
}
#endif

// ----------------------------------------------------------------------------

#ifdef USE_DEBUGGER
uint32_t YM2203::debug_read_io8(uint32_t addr)
{
	return read_io8(addr);
}

bool YM2203::debug_write_reg(uint32_t reg_num, uint32_t data)
{
	 set_reg(reg_num, data);
	 return true;
}

void YM2203::debug_regs_info(_TCHAR *buffer, size_t buffer_len)
{
	int val = 0;

	buffer[0] = _T('\0');

	// status
	val = (int)read_status();
	UTILITY::sntprintf(buffer, buffer_len, _T(" STATUS:%02X\n"), val);

	// for opn register
	for(uint32_t i=0; i<256; i++) {
		bool valid = false;
		bool lf = false;

		if (i < 0x20) val = (int)opn->GetReg(i);
		else val = debug_regs[i-0x20];

		if (i < 0x10
		|| (i >= 0x24 && i <= 0x28)
		|| (i >= 0x2d && i <= 0x2f)
		|| (i >= 0x30 && i < 0xb3 && (i & 3) != 3)) {
			valid = true;
		}
		if (i == 7
		|| ((i & 0x0f) == 0x0f)
		|| (i >= 0x30 && ((i & 0x0f) == 0x0e))) {
			lf = true;
		}

		if (!valid) continue;

		UTILITY::sntprintf(buffer, buffer_len, _T(" %02X:%02X"), i, val);
		if (_tcslen(buffer) + 6 > buffer_len) break;
		if (lf) UTILITY::tcscat(buffer, buffer_len, _T("\n"));
	}
}
#endif
