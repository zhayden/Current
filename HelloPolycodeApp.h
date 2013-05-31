#include <Polycode.h>
#include "PolycodeView.h"
#include "Polycode3DPhysics.h"

using namespace Polycode;

class HelloPolycodeApp : public EventHandler {
public:
    HelloPolycodeApp(PolycodeView *view);
    ~HelloPolycodeApp();
    bool Update();
    void handleEvent(Event *e);
    
private:
	Core *core;
	ScreenLabel *label;
	CollisionScene *cscene;
	ScenePrimitive *obj;
	Number yspeed;
	Number zspeed;
	
	ScenePrimitive *ground;
};
