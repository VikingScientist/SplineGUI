
#ifndef _ACTIVE_OBJECT_H
#define _ACTIVE_OBJECT_H

#include <stdlib.h>

static const int GEOMETRY_MOVE_STOPPED       = 200;
static const int REQUEST_REPAINT             = 201;
static const int ACTION_REQUEST_REPAINT      = 1;
static const int ACTION_REQUEST_REMASK       = 2;
static const int ACTION_REQUEST_RETESSELATE  = 4;
static const int ACTION_BUTTON_PRESSED       = 8;

class ActiveObject {
	protected:
		void (*actionPerformed)(ActiveObject *caller, int meta);
		virtual void fireActionEvent(int meta) {
			if(actionPerformed)
				actionPerformed(this, meta);
		}
	public:
		ActiveObject() { actionPerformed = NULL; }
		void setActionListener(void (*actionPerformed)(ActiveObject*, int )) {
			this->actionPerformed = actionPerformed;
		}
};

#endif
