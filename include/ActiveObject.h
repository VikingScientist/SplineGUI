
#ifndef _ACTIVE_OBJECT_H
#define _ACTIVE_OBJECT_H

#include <stdlib.h>
#include <vector>

static const int GEOMETRY_MOVE_STOPPED       = 200;
static const int REQUEST_REPAINT             = 201;
static const int ACTION_REQUEST_REPAINT      =  1;
static const int ACTION_REQUEST_REMASK       =  2;
static const int ACTION_REQUEST_RETESSELATE  =  4;
static const int ACTION_BUTTON_PRESSED       =  8;
static const int ACTION_TEXTFIELD_ENTERED    = 16;


class ActiveObject {
	private:
		typedef void (*actionListener_actionPerformed)(ActiveObject *caller, int meta);
	protected:
		std::vector<actionListener_actionPerformed> listeners;

		virtual void fireActionEvent(int meta) {
			for(size_t i=0; i<listeners.size(); i++) {
				listeners[i](this, meta);
			}
		}
	public:
		ActiveObject() {
		}
		virtual void addActionListener(void (*actionPerformed)(ActiveObject*, int )) {
			listeners.push_back(actionPerformed);
		}
};

#endif
