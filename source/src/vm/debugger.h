/** @file debugger.h

	Skelton for retropc emulator

	@author Takeda.Toshiya
	@date   2014.09.02-

	@brief [ debugger ]

	@note Modified by Sasaji at 2016.02.01 -
*/

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "vm_defs.h"

#ifdef USE_DEBUGGER

#include "device.h"
#include "../debugger_defs.h"
#include "../debugger_bpoint.h"
#include "../debugger_symbol.h"

#define DEBUGGER_COMMAND_LEN	128
#define DEBUGGER_COMMAND_HISTORY	32

//#define DEBUGGER_MAX_BUFF 1024

class EMU;
class DebuggerConsole;
class CMutex;

class DEBUGGER : public DEVICE, public DEBUGGER_BPOINTS, public DEBUGGER_SYMBOLS
{
private:
	DEVICE *d_mem, *d_io;
	DebuggerConsole *dc;

// ----------------------------------------------------------------------------

public:
	DEBUGGER(VM* parent_vm, EMU* parent_emu, const char *identifier);
	~DEBUGGER();

	// common functions
	void write_data8(uint32_t addr, uint32_t data);
	uint32_t read_data8(uint32_t addr);
	void write_data16(uint32_t addr, uint32_t data);
	uint32_t read_data16(uint32_t addr);
	void write_data32(uint32_t addr, uint32_t data);
	uint32_t read_data32(uint32_t addr);
	void write_data8w(uint32_t addr, uint32_t data, int* wait);
	uint32_t read_data8w(uint32_t addr, int* wait);
	void write_data16w(uint32_t addr, uint32_t data, int* wait);
	uint32_t read_data16w(uint32_t addr, int* wait);
	void write_data32w(uint32_t addr, uint32_t data, int* wait);
	uint32_t read_data32w(uint32_t addr, int* wait);
	uint32_t fetch_op(uint32_t addr, int *wait);
	void latch_address(uint32_t addr, int *wait);

	void write_io8(uint32_t addr, uint32_t data);
	uint32_t read_io8(uint32_t addr);
	void write_io16(uint32_t addr, uint32_t data);
	uint32_t read_io16(uint32_t addr);
	void write_io32(uint32_t addr, uint32_t data);
	uint32_t read_io32(uint32_t addr);
	void write_io8w(uint32_t addr, uint32_t data, int* wait);
	uint32_t read_io8w(uint32_t addr, int* wait);
	void write_io16w(uint32_t addr, uint32_t data, int* wait);
	uint32_t read_io16w(uint32_t addr, int* wait);
	void write_io32w(uint32_t addr, uint32_t data, int* wait);
	uint32_t read_io32w(uint32_t addr, int* wait);

	void write_signal(int id, uint32_t data, uint32_t mask);
	uint32_t read_signal(int id);

//	void add_cpu_trace(uint32_t pc);

	void set_debugger_console(DebuggerConsole *dc);

	// unique functions
	void set_context_mem(DEVICE* device)
	{
		d_mem = device;
	}
	void set_context_io(DEVICE* device)
	{
		d_io = device;
	}

	bool reach_break_point_at(uint32_t addr);

	void check_break_points(uint32_t addr);
	void check_intr_break_points(uint32_t addr, uint32_t mask);

//	void store_break_points();
//	void restore_break_points();

//	int hit_break_point() const;
//	int hit_trace_point() const;
//	void set_break_point(bool val);
//	void set_trace_point(bool val);

//	symbol_t *first_symbol, *last_symbol;
	_TCHAR file_path[_MAX_PATH];
	int now_going;
	bool now_debugging, now_suspended;
//	bool now_breakpoint, now_tracepoint;
//	bool now_basicreason;

	void go_suspend();
	bool now_suspend();
	void clear_suspend();
};

#endif /* USE_DEBUGGER */
#endif /* DEBUGGER_H */

