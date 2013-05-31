#include "HelloPolycodeApp.h"

HelloPolycodeApp::HelloPolycodeApp(PolycodeView *view) : EventHandler() {

	core = new SDLCore(view, 640,480,false,false,0,0,90);	  

	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	Screen *screen = new Screen();
	label = new ScreenLabel("Welcome to the Flow", 32);
	screen->addChild(label);
	
	cscene = new CollisionScene();
	
	ground = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 100,.1,10);
	ground->setPosition(0,-5,0);
	ground->setColor(0,0,.4,1);
	cscene->addCollisionChild(ground);
	
	ScenePrimitive *lwall = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 100,10,.1);
	lwall->setColor(0,0,.5,1);
	lwall->setPosition(0,0,5);
	cscene->addCollisionChild(lwall);
	
	ScenePrimitive *rwall = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 100,10,.1);
	rwall->setColor(0,0,.5,1);
	rwall->setPosition(0,0,-5);
	cscene->addCollisionChild(rwall);
	
	ScenePrimitive *top = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 100,.1,10);
	top->setColor(0,0,.8,1);
	top->setPosition(0,5.05,0);
	cscene->addCollisionChild(top);
	
	obj = new ScenePrimitive(ScenePrimitive::TYPE_BOX, 1,1,1);
	obj->setPosition(Vector3(10,-1,0));
	obj->setColor(.5,0,0,1);
	cscene->addCollisionChild(obj);
	
	yspeed=0;
	zspeed=0;
	
	cscene->getActiveCamera()->setPosition(0,0,0);
	cscene->getActiveCamera()->lookAt(Vector3(10,-1,0));
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);
	
}

void HelloPolycodeApp::handleEvent(Event *e) {
	if(e->getDispatcher() == core->getInput()) {
		InputEvent *inputEvent = (InputEvent*)e;
		
		switch(e->getEventCode()) {
			case InputEvent::EVENT_KEYDOWN:
				switch (inputEvent->keyCode()) {		
					case KEY_UP:
						yspeed = 1;
					break;
					case KEY_DOWN:
						yspeed = -1;
					break;
					case KEY_LEFT:
						zspeed = -1;
					break;
					case KEY_RIGHT:
						zspeed = 1;
					break;														
				}
			break;
			case InputEvent::EVENT_KEYUP:
				switch (inputEvent->key) {
					case KEY_UP:
					case KEY_DOWN:
						yspeed = 0;					
					break;					
					case KEY_LEFT:
					case KEY_RIGHT:
						zspeed = 0;					
					break;										
				}
			break;			
		}
	}
}

HelloPolycodeApp::~HelloPolycodeApp() {

}

bool HelloPolycodeApp::Update() {
	Number elapsed = core->getElapsed();
	Vector3 pos = obj->getPosition();
	pos.y += yspeed*elapsed;
	pos.z += zspeed*elapsed;
	obj->setPosition(pos);
	
	cscene->getActiveCamera()->lookAt(pos);
	
	//Test collision
	CollisionResult res = cscene->testCollision(obj, ground);
	if(res.collided) {
		obj->setColor(0,.5,0,1);
		pos += res.colNormal*res.colDist;
		obj->setPosition(pos);
	} else {
		obj->setColor(.5,0,0,1);
	}
	
    return core->updateAndRender();
}
