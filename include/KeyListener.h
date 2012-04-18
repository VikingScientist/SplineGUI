
#ifndef _KEY_LISTENER_H
#define _KEY_LISTENER_H

class KeyListener {
	private:

	public:
		KeyListener() { };
		virtual void handleKeypress(unsigned char key, int x, int y) { };
};

#endif
