/** @file msgs.cpp

	Skelton for retropc emulator

	@author Sasaji
	@date   2019.03.01 -

	@brief [ message string ]
*/

#include "msgs.h"
#include "clocale.h"
#include "utility.h"
#if defined(USE_QT)
#include <QString>
#endif

CMsg gMessages;

CMsg::CMsg()
#if defined(USE_QT)
	: QObject()
#endif
{
	int i = 0;
	msgs[i++] = _T("");
	msgs[i++] = _T(": ");
	msgs[i++] = _TX("None");
	msgs[i++] = _TX("None\tAlt+U");
	msgs[i++] = _TX("Point");
	msgs[i++] = _TX("Point\tAlt+U");
	msgs[i++] = _TX("Memory Without Wait");
	msgs[i++] = _TX("Memory With Wait");
	msgs[i++] = _TX("Filter...");
	msgs[i++] = _TX("Filter Type");
	msgs[i++] = _TX("Direct3D Filter");
	msgs[i++] = _TX("OpenGL Filter");
	msgs[i++] = _TX("Direct3D OFF");
	msgs[i++] = _TX("Direct3D ON (Sync)");
	msgs[i++] = _TX("Direct3D ON (Async)");
	msgs[i++] = _TX("OpenGL");
	msgs[i++] = _TX("OpenGL OFF");
	msgs[i++] = _TX("OpenGL ON (Sync)");
	msgs[i++] = _TX("OpenGL ON (Async)");
	msgs[i++] = _TX(" (Need restart program)");
	msgs[i++] = _TX(" (Need PowerOn)");
	msgs[i++] = _TX("* Need restart program.");
	msgs[i++] = _TX("* Need restart program or \"PowerOn\".");
	msgs[i++] = _TX("CPU Speed");
	msgs[i++] = _TX("CPU x0.5");
	msgs[i++] = _TX("CPU x1");
	msgs[i++] = _TX("CPU x2");
	msgs[i++] = _TX("CPU x4");
	msgs[i++] = _TX("CPU x8");
	msgs[i++] = _TX("CPU x16");
	msgs[i++] = _TX("CPU x0.5\tAlt+9");
	msgs[i++] = _TX("CPU x1\tAlt+1");
	msgs[i++] = _TX("CPU x2\tAlt+2");
	msgs[i++] = _TX("CPU x4\tAlt+3");
	msgs[i++] = _TX("CPU x8\tAlt+4");
	msgs[i++] = _TX("CPU x16\tAlt+5");
	msgs[i++] = _TX("CPU x%d");
	msgs[i++] = _TX("Sync Machine Speed With CPU Speed");
	msgs[i++] = _TX("Async Machine Speed With CPU Speed");
	msgs[i++] = _TX("Checker Drawing");
	msgs[i++] = _TX("Stripe Drawing");
	msgs[i++] = _TX("Scanline Drawing");
	msgs[i++] = _TX("Full Drawing");
	msgs[i++] = _TX("Afterimage OFF");
	msgs[i++] = _TX("Afterimage%d ON");
	msgs[i++] = _TX("Keepimage OFF");
	msgs[i++] = _TX("Keepimage%d ON");
	msgs[i++] = _TX("Pause");
	msgs[i++] = _TX("LED");
	msgs[i++] = _TX("Show LED (Inside)");
	msgs[i++] = _TX("Show LED (Outside)");
	msgs[i++] = _TX("Show LED");
	msgs[i++] = _TX("Hide LED");
	msgs[i++] = _TX("LED is disable.");
	msgs[i++] = _TX("Inside LED");
	msgs[i++] = _TX("Outside LED");
	msgs[i++] = _TX("Hide");
	msgs[i++] = _TX("Show");
	msgs[i++] = _TX("Show Inside");
	msgs[i++] = _TX("Show Outside");
	msgs[i++] = _TX("Position");
	msgs[i++] = _TX("LeftTop");
	msgs[i++] = _TX("RightTop");
	msgs[i++] = _TX("LeftBottom");
	msgs[i++] = _TX("RightBottom");
	msgs[i++] = _TX("Capture Type");
	msgs[i++] = _TX("Snapshot Path");
	msgs[i++] = _TX("Font File (*)");
	msgs[i++] = _TX("Font Path");
	msgs[i++] = _TX("Folder...");
	msgs[i++] = _TX("Font...");
	msgs[i++] = _TX("Message Font");
	msgs[i++] = _TX("Info Font");
	msgs[i++] = _TX("Menu Font (*)");
	msgs[i++] = _TX(" Size");
	msgs[i++] = _TX("Load Wav File from Tape");
	msgs[i++] = _TX("Save Wav File to Tape");
	msgs[i++] = _TX("Reverse Wave");
	msgs[i++] = _TX("Half Wave");
	msgs[i++] = _TX("Correct");
	msgs[i++] = _TX("COS Wave");
	msgs[i++] = _TX("SIN Wave");
	msgs[i++] = _TX("Sample Rate");
	msgs[i++] = _TX("Sample Bits");
	msgs[i++] = _TX("Show Message");
	msgs[i++] = _TX("Hide Message");
	msgs[i++] = _TX("Message board is disable.");
	msgs[i++] = _TX("Enable Joypad (Key Assigned)");
	msgs[i++] = _TX("Enable Joypad (PIA Type)");
	msgs[i++] = _TX("Disable Joypad");
	msgs[i++] = _TX("Enable Lightpen");
	msgs[i++] = _TX("Disable Lightpen");
	msgs[i++] = _TX("Enable Mouse");
	msgs[i++] = _TX("Disable Mouse");
	msgs[i++] = _TX("Enable DirectInput");
	msgs[i++] = _TX("Disable DirectInput");
	msgs[i++] = _TX("(no label)");
	msgs[i++] = _TX("Change Side to A");
	msgs[i++] = _TX("Change Side to B");
	msgs[i++] = _TX("Floppy Disk Drive");
	msgs[i++] = _TX("When start up, mount disk at:");
	msgs[i++] = _TX("Ignore delays to find sector.");
	msgs[i++] = _TX("Ignore delays to seek track.");
	msgs[i++] = _TX("Suppress checking for density.");
	msgs[i++] = _TX("Suppress checking for media type.");
	msgs[i++] = _TX("Drive");
	msgs[i++] = _TX("Select Drive");
	msgs[i++] = _TX("FDD Type");
	msgs[i++] = _TX("FDD Type (*)");
	msgs[i++] = _TX("Non FDD");
	msgs[i++] = _TX("3inch compact FDD");
	msgs[i++] = _TX("3inch compact FDD (for L3)");
	msgs[i++] = _TX("5.25inch mini FDD");
	msgs[i++] = _TX("5.25inch mini FDD (2D Type)");
	msgs[i++] = _TX("5.25inch mini FDD (2HD Type)");
	msgs[i++] = _TX("8inch standard FDD");
	msgs[i++] = _TX("Unsupported FDD");
	msgs[i++] = _TX("Control");
	msgs[i++] = _TX("PowerOn\tAlt+F3");
	msgs[i++] = _TX("MODE Switch");
	msgs[i++] = _TX("MODE Switch ");
	msgs[i++] = _TX("MODE Switch\tAlt+M");
	msgs[i++] = _TX("Reset Switch");
	msgs[i++] = _TX("Reset Switch\tAlt+R");
	msgs[i++] = _TX("System Mode");
	msgs[i++] = _TX("System Mode (*)");
	msgs[i++] = _TX("B Mode (L3)");
	msgs[i++] = _TX("A Mode (S1)");
	msgs[i++] = _TX("A Mode (S1)\tAlt+M");
	msgs[i++] = _TX("B Mode (L3)\tAlt+M");
	msgs[i++] = _TX("NEWON7");
	msgs[i++] = _TX("No FDD\tAlt+F");
	msgs[i++] = _TX("3inch compact FDD\tAlt+F");
	msgs[i++] = _TX("5.25inch mini FDD (2D Type)\tAlt+F");
	msgs[i++] = _TX("5.25inch mini FDD (2HD Type)\tAlt+F");
	msgs[i++] = _TX("5.25inch mini FDD\tAlt+F");
	msgs[i++] = _TX("8inch standard FDD\tAlt+F");
	msgs[i++] = _TX("Pause\tAlt+Q");
	msgs[i++] = _TX("Sync With CPU Speed");
	msgs[i++] = _TX("Sync With CPU Speed\tAlt+0");
	msgs[i++] = _TX("Auto Key");
	msgs[i++] = _TX("Open");
	msgs[i++] = _TX("Open...");
	msgs[i++] = _TX("Paste");
	msgs[i++] = _TX("Start");
	msgs[i++] = _TX("Stop");
	msgs[i++] = _TX("Record Key");
	msgs[i++] = _TX("Play...\tAlt+E");
	msgs[i++] = _TX("Stop Playing");
	msgs[i++] = _TX("Record...");
	msgs[i++] = _TX("Stop Recording");
	msgs[i++] = _TX("Load State...");
	msgs[i++] = _TX("Load State...\tAlt+O");
	msgs[i++] = _TX("Save State...");
	msgs[i++] = _TX("Recent State Files");
	msgs[i++] = _TX("Exit");
	msgs[i++] = _TX("Exit\tAlt+F4");
	msgs[i++] = _TX("Tape");
	msgs[i++] = _TX("Play...\tAlt+F7");
	msgs[i++] = _TX("Rec...");
	msgs[i++] = _TX("Eject");
	msgs[i++] = _TX("Rewind");
	msgs[i++] = _TX("Rewind\tAlt+F5");
	msgs[i++] = _TX("F.F.");
	msgs[i++] = _TX("F.F.\tAlt+F8");
	msgs[i++] = _TX("Stop\tAlt+F6");
	msgs[i++] = _TX("Real Mode");
	msgs[i++] = _TX("Recent Files");
	msgs[i++] = _TX("Insert...\tAlt+F%d");
	msgs[i++] = _TX("New");
	msgs[i++] = _TX("Insert Blank 2D...");
	msgs[i++] = _TX("Insert Blank 2HD...");
	msgs[i++] = _TX("Write Protect");
	msgs[i++] = _TX("Multi Volume");
	msgs[i++] = _TX("FDD%d");
	msgs[i++] = _TX("Frame Rate");
	msgs[i++] = _TX("Auto");
	msgs[i++] = _TX("60fps");
	msgs[i++] = _TX("30fps");
	msgs[i++] = _TX("20fps");
	msgs[i++] = _TX("15fps");
	msgs[i++] = _TX("12fps");
	msgs[i++] = _TX("10fps");
	msgs[i++] = _TX("Record Screen");
	msgs[i++] = _TX("Rec 60fps");
	msgs[i++] = _TX("Rec 30fps");
	msgs[i++] = _TX("Rec 20fps");
	msgs[i++] = _TX("Rec 15fps");
	msgs[i++] = _TX("Rec 12fps");
	msgs[i++] = _TX("Rec 10fps");
	msgs[i++] = _TX("Capture");
	msgs[i++] = _TX("Window");
	msgs[i++] = _TX("Display");
	msgs[i++] = _TX("Fullscreen");
	msgs[i++] = _TX("Stretch Screen");
	msgs[i++] = _TX("Stretch Screen\tAlt+X");
	msgs[i++] = _TX("Cutout Screen");
	msgs[i++] = _TX("Cutout Screen\tAlt+X");
	msgs[i++] = _TX("Aspect Ratio");
	msgs[i++] = _TX("Drawing Mode");
	msgs[i++] = _TX("Full Draw");
	msgs[i++] = _TX("Full Draw\tAlt+S");
	msgs[i++] = _TX("Scanline");
	msgs[i++] = _TX("Scanline\tAlt+S");
	msgs[i++] = _TX("Stripe");
	msgs[i++] = _TX("Stripe\tAlt+S");
	msgs[i++] = _TX("Checker");
	msgs[i++] = _TX("Checker\tAlt+S");
	msgs[i++] = _TX("Afterimage1");
	msgs[i++] = _TX("Afterimage1\tAlt+T");
	msgs[i++] = _TX("Afterimage2");
	msgs[i++] = _TX("Afterimage2\tAlt+T");
	msgs[i++] = _TX("Keepimage1");
	msgs[i++] = _TX("Keepimage2");
	msgs[i++] = _TX("Digital RGB");
	msgs[i++] = _TX("Analog RGB");
	msgs[i++] = _TX("Use Direct3D(Sync)");
	msgs[i++] = _TX("Use Direct3D(Sync)\tAlt+Y");
	msgs[i++] = _TX("Use Direct3D(Async)");
	msgs[i++] = _TX("Use Direct3D(Async)\tAlt+Y");
	msgs[i++] = _TX("Use OpenGL(Sync)");
	msgs[i++] = _TX("Use OpenGL(Sync)\tAlt+Y");
	msgs[i++] = _TX("Use OpenGL(Async)");
	msgs[i++] = _TX("Use OpenGL(Async)\tAlt+Y");
	msgs[i++] = _TX("Nearest Neighbour");
	msgs[i++] = _TX("Nearest Neighbour\tAlt+U");
	msgs[i++] = _TX("Linear");
	msgs[i++] = _TX("Linear\tAlt+U");
	msgs[i++] = _TX("Sound");
	msgs[i++] = _TX("Volume...");
	msgs[i++] = _TX("Volume...\tAlt+V");
	msgs[i++] = _TX("Record Sound");
	msgs[i++] = _TX("Frequency");
	msgs[i++] = _TX("2000Hz");
	msgs[i++] = _TX("4000Hz");
	msgs[i++] = _TX("8000Hz");
	msgs[i++] = _TX("11025Hz");
	msgs[i++] = _TX("22050Hz");
	msgs[i++] = _TX("44100Hz");
	msgs[i++] = _TX("48000Hz");
	msgs[i++] = _TX("96000Hz");
	msgs[i++] = _TX("Latency");
	msgs[i++] = _TX("50msec.");
	msgs[i++] = _TX("75msec.");
	msgs[i++] = _TX("100msec.");
	msgs[i++] = _TX("200msec.");
	msgs[i++] = _TX("300msec.");
	msgs[i++] = _TX("400msec.");
	msgs[i++] = _TX("Start...");
	msgs[i++] = _TX("Devices");
	msgs[i++] = _TX("Save");
	msgs[i++] = _TX("Save...");
	msgs[i++] = _TX("Print to mpprinter");
	msgs[i++] = _TX("Clear");
	msgs[i++] = _TX("Direct Send to mpprinter");
	msgs[i++] = _TX("Send to mpprinter concurrently");
	msgs[i++] = _TX("Online");
	msgs[i++] = _TX("Enable Server");
	msgs[i++] = _TX("Connect");
	msgs[i++] = _TX("Ethernet");
	msgs[i++] = _TX("Comm With Byte Data");
	msgs[i++] = _TX("Options For Telnet");
	msgs[i++] = _TX("Binary Mode");
	msgs[i++] = _TX("Send WILL ECHO");
	msgs[i++] = _TX("Options");
	msgs[i++] = _TX("Show LED\tAlt+L");
	msgs[i++] = _TX("Inside LED\tAlt+L");
	msgs[i++] = _TX("Show Message\tAlt+Z");
	msgs[i++] = _TX("Show Performance Meter");
	msgs[i++] = _TX("Use DirectInput");
	msgs[i++] = _TX("Use Joypad (Key Assigned)");
	msgs[i++] = _TX("Use Joypad (Key Assigned)\tAlt+J");
	msgs[i++] = _TX("Use Joypad (PIA Type)");
	msgs[i++] = _TX("Use Joypad (PIA Type)\tAlt+J");
	msgs[i++] = _TX("Enable Lightpen\tAlt+Ctrl");
	msgs[i++] = _TX("Use Mouse\tAlt+Ctrl");
	msgs[i++] = _TX("Loosen Key Stroke (For Game)");
	msgs[i++] = _TX("Keybind");
	msgs[i++] = _TX("Keybind...");
	msgs[i++] = _TX("Keybind...\tAlt+K");
	msgs[i++] = _TX("Virtual Keyboard");
	msgs[i++] = _TX("Start Debugger");
	msgs[i++] = _TX("Start Debugger\tAlt+D");
	msgs[i++] = _TX("Stop Debugger");
	msgs[i++] = _TX("Configure");
	msgs[i++] = _TX("Configure...");
	msgs[i++] = _TX("Configure...\tAlt+C");
	msgs[i++] = _TX("64KB");
	msgs[i++] = _TX("128KB");
	msgs[i++] = _TX("256KB");
	msgs[i++] = _TX("512KB");
	msgs[i++] = _TX("NMI");
	msgs[i++] = _TX("IRQ");
	msgs[i++] = _TX("FIRQ");
	msgs[i++] = _TX(" (Now ");
	msgs[i++] = _TX("Help");
	msgs[i++] = _TX("About...");
	msgs[i++] = _TX("Mode");
	msgs[i++] = _TX("Screen");
	msgs[i++] = _TX("Tape, FDD");
	msgs[i++] = _TX("Network");
	msgs[i++] = _TX("CPU, Memory");
	msgs[i++] = _TX("PowerOff");
	msgs[i++] = _TX("PowerOn");
	msgs[i++] = _TX("Enable the state of power off");
	msgs[i++] = _TX("I/O Port Address (*)");
	msgs[i++] = _TX("5.25inch FDC  $FF00 - $FF04");
	msgs[i++] = _TX("3inch FDC  $FF18 - $FF20");
	msgs[i++] = _TX("3inch FDC (for L3)  $FF18 - $FF20");
	msgs[i++] = _TX("6voice PSG  $FF30 - $FF33");
	msgs[i++] = _TX("6voice PSG (for L3)  $FF30 - $FF33");
	msgs[i++] = _TX("Ex LPT Port  $FF3C - $FF3F");
	msgs[i++] = _TX("Ex COM Port  $FF40 - $FF41");
	msgs[i++] = _TX("9voice PSG  $FF70 - $FF7F");
	msgs[i++] = _TX("9voice PSG (for L3)  $FF70 - $FF7F");
	msgs[i++] = _TX("KANJI ROM  $FF75 - $FF76");
	msgs[i++] = _TX("Ex PSG (for S1)  $FFE6 - $FFE7");
	msgs[i++] = _TX("OS-9 Ex Card  $FE11");
	msgs[i++] = _TX("Communication Card  $FF77");
	msgs[i++] = _TX("Keyboard  $FFE0 - $FFE1");
	msgs[i++] = _TX("Mouse  $FFDC - $FFDF");
	msgs[i++] = _TX("FM Synthesis Card  $FF1E - $FF1F");
	msgs[i++] = _TX("Disable ROM BASIC (Limelight)  $FFCE");
	msgs[i++] = _TX("Disable IG  $FFE9");
	msgs[i++] = _TX("Disable IG (Limelight)  $FFE9");
	msgs[i++] = _TX("Real Time Clock  $FF38-$FF3A");
	msgs[i++] = _TX("Z80B Card  $FF7F");
	msgs[i++] = _TX("DIP Switch (*)");
	msgs[i++] = _TX("Drawing");
	msgs[i++] = _TX("Method");
	msgs[i++] = _TX("Method (*)");
	msgs[i++] = _TX("Use (*)");
	msgs[i++] = _TX("CRTC");
	msgs[i++] = _TX("Disptmg Skew");
	msgs[i++] = _TX("Curdisp Skew");
	msgs[i++] = _TX("Curdisp Skew (L3 Only)");
	msgs[i++] = _TX("BMP");
	msgs[i++] = _TX("PNG");
	msgs[i++] = _TX("OFF");
	msgs[i++] = _TX("ON");
	msgs[i++] = _TX("OK");
	msgs[i++] = _TX("Cancel");
	msgs[i++] = _TX("Close");
	msgs[i++] = _TX("No");
	msgs[i++] = _TX("Yes");
	msgs[i++] = _TX("Yes(sync)");
	msgs[i++] = _TX("Yes(async)");
	msgs[i++] = _TX("Hostname");
	msgs[i++] = _TX("LPT%d");
	msgs[i++] = _TX("COM%d");
	msgs[i++] = _TX("LPT%d Hostname");
	msgs[i++] = _TX("COM%d Hostname");
	msgs[i++] = _TX(" Port");
	msgs[i++] = _TX(" Print Delay"),
	msgs[i++] = _TX("msec."),
	msgs[i++] = _TX("S:300baud/F:1200baud");
	msgs[i++] = _TX("S:600baud/F:2400baud");
	msgs[i++] = _TX("S:1200baud/F:4800baud");
	msgs[i++] = _TX("S:2400baud/F:9600baud");
	msgs[i++] = _TX("Connectable host to Debugger");
	msgs[i++] = _TX("Settings of serial ports on host");
	msgs[i++] = _TX("Need re-connect to serial port when modified this.");
	msgs[i++] = _TX("Baud Rate");
	msgs[i++] = _TX("Data Bit");
	msgs[i++] = _TX("Parity");
	msgs[i++] = _TX("Odd");
	msgs[i++] = _TX("Even");
	msgs[i++] = _TX("Stop Bit");
	msgs[i++] = _TX("Flow Control");
	msgs[i++] = _TX("Xon/Xoff");
	msgs[i++] = _TX("Hardware");
	msgs[i++] = _TX("ROM Path");
	msgs[i++] = _TX("ROM Path (*)");
	msgs[i++] = _TX("Use Extended Memory (64KB)");
	msgs[i++] = _TX("Extended RAM (*)");
	msgs[i++] = _TX("No wait to access the main memory.");
	msgs[i++] = _TX("Show message when the CPU fetches undefined opcode.");
	msgs[i++] = _TX("Clock of FM Synthesis Card (*)");
	msgs[i++] = _TX("Connect interrupt signal of FM Synthesis to (*)");
	msgs[i++] = _TX("Use FM Synthesis on Extended PSG port.");
	msgs[i++] = _TX("Connect interrupt signal of Z80B Card to (*)");
	msgs[i++] = _TX("MsgBoard: OK");
	msgs[i++] = _TX("MsgBoard: Failed");
	msgs[i++] = _TX("MsgBoard: Use \"%s\" for %s.");
	msgs[i++] = _TX("MsgBoard: Couldn't load font %s.");
	msgs[i++] = _TX("MsgBoard: Couldn't find fonts for %s.");
	msgs[i++] = _TX("MsgBoard: Couldn't load font \"%s\" for message.");
	msgs[i++] = _TX("MsgBoard: Couldn't load font \"%s\" for info.");
	msgs[i++] = _TX("info");
	msgs[i++] = _TX("message");
	msgs[i++] = _TX("Select a folder to save snapshot images.");
	msgs[i++] = _TX("Select a font file for showing messages.");
	msgs[i++] = _TX("Select a font folder for showing messages.");
	msgs[i++] = _TX("Select a folder containing the rom images.");
	msgs[i++] = _TX("Select a font.");
	msgs[i++] = _TX("File...");
	msgs[i++] = _TX("File Type :");
	msgs[i++] = _TX("Play Data Recorder Tape");
	msgs[i++] = _TX("Record Data Recorder Tape");
	msgs[i++] = _TX("Play [");
	msgs[i++] = _TX("Play...");
	msgs[i++] = _TX("Rec [");
	msgs[i++] = _TX("Open Floppy Disk %d");
	msgs[i++] = _TX("FDD");
	msgs[i++] = _TX("New Floppy Disk %d");
	msgs[i++] = _TX("Insert [");
	msgs[i++] = _TX("Insert...");
	msgs[i++] = _TX("Open HuCARD");
	msgs[i++] = _TX("Open Cartridge");
	msgs[i++] = _TX("Open Quick Disk");
	msgs[i++] = _TX("Open Media");
	msgs[i++] = _TX("Load RAM Pack Cartridge");
	msgs[i++] = _TX("Load Memory Dump");
	msgs[i++] = _TX("Save RAM Pack Cartridge");
	msgs[i++] = _TX("Save Memory Dump");
	msgs[i++] = _TX("Open Text File");
	msgs[i++] = _TX("Save Status Data");
	msgs[i++] = _TX("Load Status Data");
	msgs[i++] = _TX("Play Recorded Keys");
	msgs[i++] = _TX("Record Input Keys");
	msgs[i++] = _TX("Save Printing Data");
	msgs[i++] = _TX("Supported Files (*.ttf;*.otf)");
	msgs[i++] = _TX("Supported Files (*.cas;*.cmt;*.t88)");
	msgs[i++] = _TX("Supported Files (*.cas;*.cmt)");
	msgs[i++] = _TX("Supported Files (*.wav;*.cas;*.tap)");
	msgs[i++] = _TX("Supported Files (*.wav;*.cas;*.mzt;*.m12)");
	msgs[i++] = _TX("Supported Files (*.l3;*.l3b;*.l3c;*.wav;*.t9x)");
	msgs[i++] = _TX("Supported Files (*.wav;*.cas)");
	msgs[i++] = _TX("L3 File (*.l3)");
	msgs[i++] = _TX("L3B File (*.l3b)");
	msgs[i++] = _TX("L3C File (*.l3c)");
	msgs[i++] = _TX("Wave File (*.wav)");
	msgs[i++] = _TX("T9X File (*.t9x)");
	msgs[i++] = _TX("Supported Files (*.d88;*.d77;*.td0;*.imd;*.dsk;*.fdi;*.hdm;*.tfd;*.xdf;*.2d;*.sf7)");
	msgs[i++] = _TX("Supported Files (*.d88;*.td0;*.imd;*.dsk;*.fdi;*.hdm;*.tfd;*.xdf;*.2d;*.sf7)");
	msgs[i++] = _TX("Supported Files (*.d88;*.d77)");
	msgs[i++] = _TX("Supported Files (*.d88)");
	msgs[i++] = _TX("Supported Files (*.txt;*.bas;*.lpt)");
	msgs[i++] = _TX("Supported Files (*.l3r)");
	msgs[i++] = _TX("Supported Files (*.l3k)");
	msgs[i++] = _TX("Supported Files (*.lpt)");
	msgs[i++] = _TX("Supported Files (*.rom;*.bin;*.hex;*.gg;*.col)");
	msgs[i++] = _TX("Supported Files (*.rom;*.bin;*.hex;*.sms)");
	msgs[i++] = _TX("Supported Files (*.rom;*.bin;*.hex;*.60)");
	msgs[i++] = _TX("Supported Files (*.rom;*.bin;*.hex;*.pce)");
	msgs[i++] = _TX("Supported Files (*.rom;*.bin;*.hex)");
	msgs[i++] = _TX("Supported Files (*.mzt;*.q20;*.qdf)");
	msgs[i++] = _TX("Supported Files (*.bin)");
	msgs[i++] = _TX("Supported Files (*.ram;*.bin;*.hex)");
	msgs[i++] = _TX("All Files (*.*)");
	msgs[i++] = _TX("(Pause)");
	msgs[i++] = _TX("num INS/DEL");
	msgs[i++] = _TX("KATA/HIRA");
	msgs[i++] = _TX("HENKAN");
	msgs[i++] = _TX("MUHENKAN");
	msgs[i++] = _TX("Allow RIGHT");
	msgs[i++] = _TX("Allow LEFT");
	msgs[i++] = _TX("Allow UP");
	msgs[i++] = _TX("Allow DOWN");
	msgs[i++] = _TX("SPACE");
	msgs[i++] = _TX("num ,");
	msgs[i++] = _TX("num *");
	msgs[i++] = _TX("num +");
	msgs[i++] = _TX("num ?");
	msgs[i++] = _TX("num -");
	msgs[i++] = _TX("num .");
	msgs[i++] = _TX("num /");
	msgs[i++] = _TX("num enter");
	msgs[i++] = _TX("num =");
	msgs[i++] = _TX("num 0");
	msgs[i++] = _TX("num %c");
	msgs[i++] = _TX("num %d");
	msgs[i++] = _TX("PF%d");
	msgs[i++] = _TX("space");
	msgs[i++] = _TX("up");
	msgs[i++] = _TX("up+right");
	msgs[i++] = _TX("right");
	msgs[i++] = _TX("down+right");
	msgs[i++] = _TX("down");
	msgs[i++] = _TX("down+left");
	msgs[i++] = _TX("left");
	msgs[i++] = _TX("up+left");
	msgs[i++] = _TX("button");
	msgs[i++] = _TX("button %c");
	msgs[i++] = _TX("bit %d");
	msgs[i++] = _TX("kanji");
	msgs[i++] = _TX("henkan");
	msgs[i++] = _TX("muhenkan");
	msgs[i++] = _TX("katakana");
	msgs[i++] = _TX("eisu");
	msgs[i++] = _TX("kana");
	msgs[i++] = _TX("right shift");
	msgs[i++] = _TX("left shift");
	msgs[i++] = _TX("right ctrl");
	msgs[i++] = _TX("left ctrl");
	msgs[i++] = _TX("right option");
	msgs[i++] = _TX("right alt");
	msgs[i++] = _TX("left option");
	msgs[i++] = _TX("left alt");
	msgs[i++] = _TX("right command");
	msgs[i++] = _TX("right meta");
	msgs[i++] = _TX("left command");
	msgs[i++] = _TX("left meta");
	msgs[i++] = _TX("right win");
	msgs[i++] = _TX("left win");
	msgs[i++] = _TX("Next");
	msgs[i++] = _TX("Prev");
	msgs[i++] = _TX("Bind");
	msgs[i++] = _TX("Bind%d");
	msgs[i++] = _TX("S1 Key");
	msgs[i++] = _TX("PIA on S1");
	msgs[i++] = _TX("Level3 Key");
	msgs[i++] = _TX("PIA on L3");
	msgs[i++] = _TX("Keyboard");
	msgs[i++] = _TX("Joypad");
	msgs[i++] = _TX("Joypad (Key Assigned)");
	msgs[i++] = _TX("Joypad (PIA Type)");
	msgs[i++] = _TX("Joypad%d");
	msgs[i++] = _TX("Signals are negative logic");
	msgs[i++] = _TX("Recognize as another key when pressed two buttons");
	msgs[i++] = _TX("Load Default");
	msgs[i++] = _TX("Load Preset 1");
	msgs[i++] = _TX("Load Preset 2");
	msgs[i++] = _TX("Load Preset 3");
	msgs[i++] = _TX("Load Preset 4");
	msgs[i++] = _TX("Load Preset %d");
	msgs[i++] = _TX("Save Preset 1");
	msgs[i++] = _TX("Save Preset 2");
	msgs[i++] = _TX("Save Preset 3");
	msgs[i++] = _TX("Save Preset 4");
	msgs[i++] = _TX("Save Preset %d");
	msgs[i++] = _TX("Volume");
	msgs[i++] = _TX("Master");
	msgs[i++] = _TX("Beep");
	msgs[i++] = _TX("PSG");
	msgs[i++] = _TX("6voice\nPSG");
	msgs[i++] = _TX("9voice\nPSG");
	msgs[i++] = _TX("Relay");
	msgs[i++] = _TX("CMT");
	msgs[i++] = _TX("ExPSG\nFM");
	msgs[i++] = _TX("ExPSG\nSSG");
	msgs[i++] = _TX("OPN\nFM");
	msgs[i++] = _TX("OPN\nSSG");
	msgs[i++] = _TX("Mute");
	msgs[i++] = _TX("%s was loaded.");
	msgs[i++] = _TX("%s couldn't be loaded.");
	msgs[i++] = _TX("%s is invalid file.");
	msgs[i++] = _TX("%s is invalid version.");
	msgs[i++] = _TX("%s is old version.");
	msgs[i++] = _TX("%s was saved.");
	msgs[i++] = _TX("%s couldn't be saved.");
	msgs[i++] = _TX("%s is not compatible. use default setting.");
	msgs[i++] = _TX("Floppy image couldn't be opened.");
	msgs[i++] = _TX("Floppy image on drive %d couldn't be opened.");
	msgs[i++] = _TX("Floppy image on drive %d couldn't be saved.");
	msgs[i++] = _TX("Floppy image on drive %d is saved as the new file: %s.");
	msgs[i++] = _TX("There is the same disk in drive %d and %d.");
	msgs[i++] = _TX("The density in track %d, side %d is different from specified one.");
	msgs[i++] = _TX("The media type in drive %d is different from specified one.");
	msgs[i++] = _TX("Tape image couldn't be opened.");
	msgs[i++] = _TX("Tape image couldn't be saved.");
	msgs[i++] = _TX("Print image couldn't be saved.");
	msgs[i++] = _TX("Status image couldn't be saved.");
	msgs[i++] = _TX("Status image couldn't be loaded.");
	msgs[i++] = _TX("Load State: Cannot open.");
	msgs[i++] = _TX("Load State: Unsupported file.");
	msgs[i++] = _TX("Load State: Invalid version.");
	msgs[i++] = _TX("Load State: No longer support a status file for %s.");
	msgs[i++] = _TX("Auto key file couldn't be opened.");
	msgs[i++] = _TX("Record key file couldn't be saved.");
	msgs[i++] = _TX("%s is %d bytes smaller than assumed one.");
	msgs[i++] = _TX("%s is different image from assumed one.");
	msgs[i++] = _TX("This is not record key file.");
	msgs[i++] = _TX("Record key file is invalid version.");
	msgs[i++] = _TX("Record key file has invalid parameter.");
	msgs[i++] = _TX("The record key file is not supported.");
	msgs[i++] = _TX("The record key file for %s is no longer supported.");
	msgs[i++] = _TX("The version of the emulator used for recording is %d.%d.%d.");
	msgs[i++] = _TX("Couldn't start recording audio.");
	msgs[i++] = _TX("Couldn't start recording video.");
	msgs[i++] = _TX("Now saving video file...");
	msgs[i++] = _TX("Video file was saved.");
	msgs[i++] = _TX("Screen was saved successfully.");
	msgs[i++] = _TX("Select a sample rate on sound menu in advance.");
	msgs[i++] = _TX("You can set properties after pressing start button.");
	msgs[i++] = _TX("! Need install library.");
	msgs[i++] = _TX("Codec Type");
	msgs[i++] = _TX("Quality");
	msgs[i++] = _TX("Max (368Kbps)");
	msgs[i++] = _TX("High (256Kbps)");
	msgs[i++] = _TX("Normal (128Kbps)");
	msgs[i++] = _TX("Low (96Kbps)");
	msgs[i++] = _TX("Min (64Kbps)");
	msgs[i++] = _TX("Max (50Mbps)");
	msgs[i++] = _TX("High (10Mbps)");
	msgs[i++] = _TX("Normal (1Mbps)");
	msgs[i++] = _TX("Low (500Kbps)");
	msgs[i++] = _TX("Min (100Kbps)");
	msgs[i++] = _TX("Max");
	msgs[i++] = _TX("High");
	msgs[i++] = _TX("Normal");
	msgs[i++] = _TX("Low");
	msgs[i++] = _TX("Min");
	msgs[i++] = _TX("Debugger was started.");
	msgs[i++] = _TX("Cannot start debugger.");
	msgs[i++] = _TX("Debugger was stopped.");
	msgs[i++] = _TX("Save to: %s");
	msgs[i++] = _TX("About bml3mk5");
	msgs[i++] = _TX("About mbs1");
	msgs[i++] = _TX("Hide bml3mk5");
	msgs[i++] = _TX("Hide mbs1");
	msgs[i++] = _TX("Quit bml3mk5");
	msgs[i++] = _TX("Quit mbs1");
	msgs[i++] = _TX("Hide Others");
	msgs[i++] = _TX("Show All");
	msgs[i++] = _TX("Services");
	msgs[i++] = _TX("Preferences...");
	msgs[i++] = _TX("Language (*)");
	msgs[i++] = _TX("Default");
}

CMsg::~CMsg()
{
}

const _TCHAR *CMsg::Get(Id id)
{
#if defined(USE_QT)
	const _TCHAR *pmsg = NULL;

	QString tmp(tr(msgs[id]));
	if (tmp.isEmpty()) {
		pmsg = msgs[id];
	} else {
		_tcsncpy(tmp_msg[tmp_idx], tmp.toUtf8().data(), 255);
		tmp_msg[tmp_idx][255] = 0;
		pmsg = tmp_msg[tmp_idx];
		tmp_idx = ((tmp_idx + 1) % 32);
	}
	return pmsg;
#else
	return _tgettext(msgs[id]);
#endif
}

const _TCHAR *CMsg::Get(Id id, bool translate)
{
	if (translate) {
		return Get(id);
	} else {
		return msgs[id];
	}
}

const _TCHAR *CMsg::GetN(Id id) const
{
	return msgs[id];
}

int CMsg::Sprintf(_TCHAR *str, size_t size, Id id, ...)
{
	va_list ap;
	va_start(ap, id);
	int len = Vsprintf(str, size, id, ap);
	va_end(ap);
	return len;
}

int CMsg::Vsprintf(_TCHAR *str, size_t size, Id id, va_list ap)
{
	const _TCHAR *format = Get(id);
	return UTILITY::vstprintf(str, size, format, ap);
}
