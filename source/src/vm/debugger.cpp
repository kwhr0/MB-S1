/** @file debugger.cpp

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2014.09.02-

	@brief [ debugger ]

	@note Modified by Sasaji at 2016.02.01 -
*/

#include "debugger.h"

#ifdef USE_DEBUGGER

#include "../emu.h"
#include "../config.h"
#include "../cmutex.h"
#include "../utility.h"
#include "../osd/debugger_console.h"


DEBUGGER::DEBUGGER(VM* parent_vm, EMU* parent_emu, const char *identifier)
	: DEVICE(parent_vm, parent_emu, identifier), DEBUGGER_BPOINTS(), DEBUGGER_SYMBOLS()
{
	dc = NULL;

	UTILITY::tcscpy(file_path, sizeof(file_path) / sizeof(file_path[0]), _T("debug.bin"));
	now_going = 0;
	now_debugging = now_suspended = false;
//	now_breakpoint = now_tracepoint = false;
//	now_basicreason = false;

#if 0
	server = false;
	client_ch = -1;
	server_ch = -1;
	connect = false;

	memset(send_buff, 0, sizeof(send_buff));
	send_buff_w_pos = 0;
	send_buff_r_pos = 0;
	memset(recv_buff, 0, sizeof(recv_buff));
	recv_buff_w_pos = 0;
	recv_buff_r_pos = 0;

//	memset(command_history, 0, sizeof(command_history));
//	last_history_ptr = 0;

	init_lock();
#endif
}

DEBUGGER::~DEBUGGER()
{
//	term_lock();
}

// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

bool DEBUGGER::reach_break_point_at(uint32_t addr)
{
	if (now_debugging) {
		check_break_points(addr);
		if(now_going > 0) {
			now_going--;
		}
		if(!now_going) {
			now_suspended = true;
		}
		return now_suspended;
	} else {
		return false;
	}
}

void DEBUGGER::check_break_points(uint32_t addr)
{
#ifdef _MBS1
	find_fetch_break_trace_points(BreakPoints::BP_FETCH_OP, addr, 1, now_suspended);
	uint32_t phy_addr = d_mem->debug_latch_address(addr);
	find_fetch_break_trace_points(BreakPoints::BP_FETCH_OP_PH, phy_addr, 1, now_suspended);
#else
	find_fetch_break_trace_points(BreakPoints::BP_FETCH_OP, addr, 1, now_suspended);
#endif
}

void DEBUGGER::check_intr_break_points(uint32_t addr, uint32_t mask)
{
	find_intr_break_trace_points(BreakPoints::BP_INTERRUPT, addr, mask, now_suspended);
}

// ----------------------------------------------------------------------------

void DEBUGGER::write_data8(uint32_t addr, uint32_t data)
{
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 1, data & 0xff, now_suspended);
	d_mem->write_data8(addr, data);
	find_basic_break_trace_points(BreakPoints::BP_BASIC_NUMBER, d_mem, addr, data & 0xff, now_suspended);
}
uint32_t DEBUGGER::read_data8(uint32_t addr)
{
	uint32_t data = d_mem->read_data8(addr);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 1, data & 0xff, now_suspended);
	return data;
}
void DEBUGGER::write_data16(uint32_t addr, uint32_t data)
{
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 2, data & 0xffff, now_suspended);
	d_mem->write_data16(addr, data);
}
uint32_t DEBUGGER::read_data16(uint32_t addr)
{
	uint32_t data = d_mem->read_data16(addr);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 2, data & 0xffff, now_suspended);
	return data;
}
void DEBUGGER::write_data32(uint32_t addr, uint32_t data)
{
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 4, data, now_suspended);
	d_mem->write_data32(addr, data);
}
uint32_t DEBUGGER::read_data32(uint32_t addr)
{
	uint32_t data = d_mem->read_data32(addr);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 4, data, now_suspended);
	return data;
}
void DEBUGGER::write_data8w(uint32_t addr, uint32_t data, int* wait)
{
#ifdef _MBS1
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 1, data & 0xff, now_suspended);
	uint32_t phy_addr = d_mem->debug_latch_address(addr);
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY_PH, phy_addr, 1, data & 0xff, now_suspended);
#else
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 1, data & 0xff, now_suspended);
#endif
	d_mem->write_data8w(addr, data, wait);
#ifdef _MBS1
	find_basic_break_trace_points(BreakPoints::BP_BASIC_NUMBER, d_mem, phy_addr, data & 0xff, now_suspended);
#else
	find_basic_break_trace_points(BreakPoints::BP_BASIC_NUMBER, d_mem, addr, data & 0xff, now_suspended);
#endif
}
uint32_t DEBUGGER::read_data8w(uint32_t addr, int* wait)
{
	uint32_t data = d_mem->read_data8w(addr, wait);
#ifdef _MBS1
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 1, data & 0xff, now_suspended);
	uint32_t phy_addr = d_mem->debug_latch_address(addr);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY_PH, phy_addr, 1, data & 0xff, now_suspended);
#else
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 1, data & 0xff, now_suspended);
#endif
	return data;
}
void DEBUGGER::write_data16w(uint32_t addr, uint32_t data, int* wait)
{
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 2, data & 0xffff, now_suspended);
	d_mem->write_data16w(addr, data, wait);
}
uint32_t DEBUGGER::read_data16w(uint32_t addr, int* wait)
{
	uint32_t data = d_mem->read_data16w(addr, wait);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 2, data & 0xffff, now_suspended);
	return data;
}
void DEBUGGER::write_data32w(uint32_t addr, uint32_t data, int* wait)
{
	find_mem_break_trace_points(BreakPoints::BP_WRITE_MEMORY, addr, 4, data, now_suspended);
	d_mem->write_data32w(addr, data, wait);
}
uint32_t DEBUGGER::read_data32w(uint32_t addr, int* wait)
{
	uint32_t data = d_mem->read_data32w(addr, wait);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 4, data, now_suspended);
	return data;
}
uint32_t DEBUGGER::fetch_op(uint32_t addr, int *wait)
{
	uint32_t data = d_mem->fetch_op(addr, wait);
	find_mem_break_trace_points(BreakPoints::BP_READ_MEMORY, addr, 1, data, now_suspended);
	return data;
}
void DEBUGGER::latch_address(uint32_t addr, int *wait)
{
	d_mem->latch_address(addr, wait);
}

void DEBUGGER::write_io8(uint32_t addr, uint32_t data)
{
	find_io_break_trace_points(BreakPoints::BP_OUTPUT_IO, addr, 1, data & 0xff, now_suspended);
	d_io->write_io8(addr, data);
}
uint32_t DEBUGGER::read_io8(uint32_t addr)
{
	uint32_t data = d_io->read_io8(addr);
	find_io_break_trace_points(BreakPoints::BP_INPUT_IO, addr, 1, data & 0xff, now_suspended);
	return data;
}
void DEBUGGER::write_io16(uint32_t addr, uint32_t data)
{
	find_io_break_trace_points(BreakPoints::BP_OUTPUT_IO, addr, 2, data & 0xffff, now_suspended);
	d_io->write_io16(addr, data);
}
uint32_t DEBUGGER::read_io16(uint32_t addr)
{
	uint32_t data = d_io->read_io16(addr);
	find_io_break_trace_points(BreakPoints::BP_INPUT_IO, addr, 2, data & 0xffff, now_suspended);
	return data;
}
void DEBUGGER::write_io32(uint32_t addr, uint32_t data)
{
	find_io_break_trace_points(BreakPoints::BP_OUTPUT_IO, addr, 4, data, now_suspended);
	d_io->write_io32(addr, data);
}
uint32_t DEBUGGER::read_io32(uint32_t addr)
{
	uint32_t data = d_io->read_io32(addr);
	find_io_break_trace_points(BreakPoints::BP_INPUT_IO, addr, 4, data, now_suspended);
	return data;
}
void DEBUGGER::write_io8w(uint32_t addr, uint32_t data, int* wait)
{
	find_io_break_trace_points(BreakPoints::BP_OUTPUT_IO, addr, 1, data & 0xff, now_suspended);
	d_io->write_io8w(addr, data, wait);
}
uint32_t DEBUGGER::read_io8w(uint32_t addr, int* wait)
{
	uint32_t data = d_io->read_io8w(addr, wait);
	find_io_break_trace_points(BreakPoints::BP_INPUT_IO, addr, 1, data & 0xff, now_suspended);
	return data;
}
void DEBUGGER::write_io16w(uint32_t addr, uint32_t data, int* wait)
{
	find_io_break_trace_points(BreakPoints::BP_OUTPUT_IO, addr, 2, data & 0xffff, now_suspended);
	d_io->write_io16w(addr, data, wait);
}
uint32_t DEBUGGER::read_io16w(uint32_t addr, int* wait)
{
	uint32_t data = d_io->read_io16w(addr, wait);
	find_io_break_trace_points(BreakPoints::BP_INPUT_IO, addr, 2, data & 0xffff, now_suspended);
	return data;
}
void DEBUGGER::write_io32w(uint32_t addr, uint32_t data, int* wait)
{
	find_io_break_trace_points(BreakPoints::BP_OUTPUT_IO, addr, 4, data, now_suspended);
	d_io->write_io32w(addr, data, wait);
}
uint32_t DEBUGGER::read_io32w(uint32_t addr, int* wait)
{
	uint32_t data = d_io->read_io32w(addr, wait);
	find_io_break_trace_points(BreakPoints::BP_INPUT_IO, addr, 4, data, now_suspended);
	return data;
}

void DEBUGGER::write_signal(int id, uint32_t data, uint32_t mask)
{
	d_mem->write_signal(id, data, mask);
}
uint32_t DEBUGGER::read_signal(int id)
{
	return d_mem->read_signal(id);
}

//void DEBUGGER::add_cpu_trace(uint32_t pc)
//{
//	if(prev_cpu_trace != pc) {
//		cpu_trace[cpu_trace_ptr++] = prev_cpu_trace = pc;
//		cpu_trace_ptr &= (MAX_CPU_TRACE - 1);
//	}
//}

void DEBUGGER::set_debugger_console(DebuggerConsole *dc_)
{
	dc = dc_;
}

// ----------------------------------------------------------------------------

void DEBUGGER::go_suspend()
{
	if (now_debugging) now_suspended = true;
}

bool DEBUGGER::now_suspend()
{
	return (now_debugging && now_suspended);
}

void DEBUGGER::clear_suspend()
{
	now_suspended = m_now_basicreason = false;
}

#endif
