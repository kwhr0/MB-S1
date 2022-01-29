/** @file ag_volume_dlg.h

	Skelton for retropc emulator
	SDL + Agar edition

	@author Sasaji
	@date   2012.4.5

	@brief [ ag_volume_dlg ]
*/

#ifndef AG_VOLUME_DLG_H
#define AG_VOLUME_DLG_H

#include "ag_dlg.h"

#if defined(_MBS1)
#define VOLUME_ITEMS 12
#else
#define VOLUME_ITEMS 7
#endif

namespace GUI_AGAR
{

/**
	@brief volume dialog
*/
class AG_VOLUME_DLG : public AG_DLG {
	friend class AG_GUI_BASE;

private:
	int volume[VOLUME_ITEMS];
	int mute[VOLUME_ITEMS];
	AG_Label *voltxt[VOLUME_ITEMS];

	int volmax;
	int volmin;

	void get_volume();
	void set_volume();

	// volume dialog event handler
	static void OnClose(AG_Event *);
	static void OnCancel(AG_Event *);
	static void OnChange(AG_Event *);

public:
	AG_VOLUME_DLG(EMU *, AG_GUI_BASE *);
	~AG_VOLUME_DLG();

	void Create();
	void Change();
	void Close(AG_Window *);
	void SetVolumeText(int idx);
};

}; /* namespace GUI_AGAR */

#endif /* AG_VOLUME_DLG_H */
