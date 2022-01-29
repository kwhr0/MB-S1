/** @file mb8866.h

	HITACHI BASIC MASTER LEVEL3 Mark5 / MB-S1 Emulator 'EmuB-6892/EmuB-S1'
	Skelton for retropc emulator

	@par Origin
	mb8877.cpp
	@author Sasaji
	@date   2012.03.24 -

	@brief [ MB8866 modoki (same as MB8877/MB8876) ]
*/

#ifndef MB8866_H
#define MB8866_H

#include "vm_defs.h"
#include "device.h"

#define SIG_MB8866_DRIVEREG	0
#define SIG_MB8866_SIDEREG	1
#define SIG_MB8866_MOTOR	2

#define SIG_MB8866_CLOCKNUM	16

#define MB8866_REGISTER_IDS	9

// #define _DEBUG_MB8866

class EMU;
//class FLOPPY;

/**
	@brief MB8866 modoki (same as MB8877/MB8876) - Floppy Disk Controller
*/
class MB8866 : public DEVICE
{
private:
	DEVICE *d_fdd;

	// config
	bool ignore_crc;

	// output signals
	outputs_t outputs_irq;
	outputs_t outputs_drq;
#ifdef _DEBUG_MB8866
	DEVICE *d_cpu;
#endif


	// registor
	uint8_t status;
	uint8_t cmdreg;
	uint8_t trkreg;
	uint8_t secreg;
	uint8_t datareg;
	uint8_t cmdtype;

	// event
	int register_id[MB8866_REGISTER_IDS];

	// status
	bool now_search;
	bool now_seek, after_seek;
	int seektrk;
	bool seekvct;
	bool now_irq;
	bool now_drq;
	bool irq_mask;
#ifdef _DEBUG_MB8866
	uint32_t val0_prev;
	bool drq_prev;
#endif

	int data_idx;

	int channel;

	// clock
	int clk_num;	// 0:1MHz 1:2MHz
	uint8_t density;	// 0:single density(FM) 1:double density(MFM)

	//for resume
#pragma pack(1)
	struct vm_state_st {
		int register_id[7];

		uint8_t status;
		uint8_t cmdreg;
		uint8_t trkreg;
		uint8_t secreg;

		uint8_t datareg;
		uint8_t cmdtype;
		int seektrk;
		int data_idx;
		uint8_t flags;

		// version 2 
		uint8_t flags2;
		int register_id2[1];

		// version 3 
		int register_id3[1];
		char dummy[12];
	};
#pragma pack()

	void cancel_my_event(int event);
	void cancel_my_events();
	void register_my_event(int event, double usec);
	void register_seek_event();
	void register_search_event(int wait);
	void register_drq_event(int bytes);
	void register_lost_event(int bytes);
	void register_restore_event();

	// image handler
	uint8_t verify_track();
	uint8_t search_sector(int side, bool compare);
	uint8_t search_addr();
	bool make_track();
	bool parse_track();

	// command
	void process_cmd();
	void cmd_restore();
	void cmd_seek();
	void cmd_step();
	void cmd_stepin();
	void cmd_stepout();
	void cmd_readdata();
	void cmd_writedata();
	void cmd_readaddr();
	void cmd_readtrack();
	void cmd_writetrack();
	void cmd_forceint();

	// irq/dma
	inline void set_irq(bool val);
	inline void set_drq(bool val);

public:
	MB8866(VM* parent_vm, EMU* parent_emu, const char* identifier) : DEVICE(parent_vm, parent_emu, identifier) {
		set_class_name("MB8866");
		init_output_signals(&outputs_irq);
		init_output_signals(&outputs_drq);
		d_fdd = NULL;
		clk_num = 0;
#ifdef _DEBUG_MB8866
		d_cpu = NULL;
#endif
		channel = 0;
	}
	~MB8866() {}

	// common functions
	void initialize();
	void release();
	void reset();
	void warm_reset();
	void write_io8(uint32_t addr, uint32_t data);
	uint32_t read_io8(uint32_t addr);
	void write_dma_io8(uint32_t addr, uint32_t data);
	uint32_t read_dma_io8(uint32_t addr);
	void write_signal(int id, uint32_t data, uint32_t mask);
	uint32_t read_signal(int ch);
	void event_callback(int event_id, int err);
	void update_config();

	// unique function
	void set_context_irq(DEVICE* device, int id, uint32_t mask) {
		register_output_signal(&outputs_irq, device, id, mask);
	}
	void set_context_drq(DEVICE* device, int id, uint32_t mask) {
		register_output_signal(&outputs_drq, device, id, mask);
	}
	void set_context_fdd(DEVICE* device) {
		d_fdd = device;
	}
	void set_context_clock_num(int clock_num) {
		clk_num = clock_num;
	}
#ifdef _DEBUG_MB8866
	void set_context_cpu(DEVICE* device) {
		d_cpu = device;
	}
#endif
	void set_channel(int ch) {
		channel = (uint32_t)(ch << 16);
	}

	void save_state(FILEIO *fp);
	void load_state(FILEIO *fp);

#ifdef USE_DEBUGGER
	uint32_t debug_read_io8(uint32_t addr);
	bool debug_write_reg(uint32_t reg_num, uint32_t data);
	void debug_regs_info(_TCHAR *buffer, size_t buffer_len);
#endif
};

#endif /* MB8866_H */
