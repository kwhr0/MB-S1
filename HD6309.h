// HD6309
// Copyright 2022 © Yasuo Kuwahara
// MIT License

#include "mbs1.h"
#include <stdint.h>

#define HD6309_TRACE	0

#if HD6309_TRACE
#define HD6309_TRACE_LOG(adr, data, type) \
	if (tracep->index < ACSMAX) tracep->acs[tracep->index++] = { adr, (uint16_t)data, type }
#else
#define HD6309_TRACE_LOG(adr, data, type)
#endif

class HD6309 {
	enum { M_SYNC = 1, M_IRQ = 2, M_FIRQ = 4, M_NMI = 8 };
	static constexpr int FBUFMAX = 128;
public:
	HD6309();
	void Reset();
	int Execute(int n);
	uint16_t GetPC() const { return (uint16_t &)r[10]; }
	void SYNC() { irq |= M_SYNC; }
	void IRQ() { irq |= M_IRQ; }
	void FIRQ() { irq |= M_FIRQ; }
	void NMI() { irq |= M_NMI; }
private:
	// customized access: MB-S1
	int32_t imm8() {
		uint8_t data = memfetch(((uint16_t &)r[10])++);
#if HD6309_TRACE
		if (tracep->opn < OPMAX) tracep->op[tracep->opn++] = data;
#endif
		return data;
	}
	int32_t imm16() {
		uint32_t adr = (uint16_t &)r[10];
		((uint16_t &)r[10]) += 2;
		uint8_t data0 = memfetch(adr), data1 = memfetch(adr + 1);
#if HD6309_TRACE
		if (tracep->opn < OPMAX) tracep->op[tracep->opn++] = data0;
		if (tracep->opn < OPMAX) tracep->op[tracep->opn++] = data1;
#endif
		return data0 << 8 | data1;
	}
	int32_t ld8(uint16_t adr) {
		int32_t data = memread(adr);
		HD6309_TRACE_LOG(adr, data, acsLoad8);
		return data;
	}
	int32_t ld16(uint16_t adr) {
		int32_t data = memread(adr) << 8;
		data |= memread(adr + 1);
		HD6309_TRACE_LOG(adr, data, acsLoad16);
		return data;
	}
	void st8(uint16_t adr, uint8_t data) {
		memwrite(adr, data);
		HD6309_TRACE_LOG(adr, data, acsStore8);
	}
	void st16(uint16_t adr, int16_t data) {
		memwrite(adr, data >> 8);
		memwrite(adr + 1, data);
		HD6309_TRACE_LOG(adr, data, acsStore16);
	}
	void st16r(uint16_t adr, int16_t data) {
		memwrite(adr + 1, data);
		memwrite(adr, data >> 8);
		HD6309_TRACE_LOG(adr, data, acsStore16);
	}
	// customized access -- end
	uint16_t ea();
	template<typename T> void bcc(T cond) {
		int8_t t = imm8();
		if (cond()) (uint16_t &)r[10] += t;
	}
	template<typename T> void lbcc(T cond) {
		int16_t t = imm16();
		if (cond()) (uint16_t &)r[10] += t;
		else if (!(md & 1)) clock--;
	}
	template<typename T> void bitTransfer(T op);
	template<typename T1, typename T2> void interRegister(T1 f8, T2 f16);
	void transfer(uint8_t op, uint8_t t8);
	int ResolvC();
	int ResolvV();
	int ResolvZ();
	int ResolvN();
	int ResolvH();
	int ResolvFlags();
	void SetupFlags(int x);
	struct FlagDecision {
		uint32_t dm;
		uint16_t s, b, a;
	};
	FlagDecision fbuf[FBUFMAX];
	FlagDecision *fp;
	const uint8_t *clktbl;
	uint8_t r[16];
	uint8_t dp, md, irq, waitflags, intflags;
	uint32_t clock;
#if HD6309_TRACE
	static constexpr int TRACEMAX = 10000;
	static constexpr int ACSMAX = 19; // max case interrupt follows pshs/puls
	static constexpr int OPMAX = 4;
	enum {
		acsStore8 = 4, acsStore16, acsLoad8, acsLoad16
	};
	struct Acs {
		uint16_t adr, data;
		uint8_t type;
	};
	struct TraceBuffer {
		uint16_t r[8];
		Acs acs[ACSMAX];
		uint8_t op[OPMAX];
		uint8_t index, cc, opn;
	};
	TraceBuffer tracebuf[TRACEMAX];
	TraceBuffer *tracep;
public:
	void StopTrace();
#endif
};
