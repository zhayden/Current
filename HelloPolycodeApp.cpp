#include "HelloPolycodeApp.h"

HelloPolycodeApp::HelloPolycodeApp(PolycodeView *view) : EventHandler() {

	core = new SDLCore(view, 640,480,false,false,0,0,90);	  

	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	Screen *screen = new Screen();
	label = new ScreenLabel("Welcome to the Flow", 32);
	screen->addChild(label);
	
	cscene = new CollisionScene();
	
	sections.push_back(section(10, 10, 20, Vector3(19,0,0)));
	sections[0].walls[0]->setColor(0,0,.4,1);
	sections[0].walls[1]->setColor(0,0,.8,1);
	sections[0].walls[2]->setColor(0,0,.5,1);
	sections[0].walls[3]->setColor(0,0,.5,1);
	cscene->addCollisionChild(sections[0].walls[0]);
	cscene->addCollisionChild(sections[0].walls[1]);
	cscene->addCollisionChild(sections[0].walls[2]);
	cscene->addCollisionChild(sections[0].walls[3]);
	
	sections.push_back(section(6, 8, 20, Vector3(39,0,0)));
	sections[1].walls[0]->setColor(0,0,.4,1);
	sections[1].walls[1]->setColor(0,0,.8,1);
	sections[1].walls[2]->setColor(0,0,.5,1);
	sections[1].walls[3]->setColor(0,0,.5,1);
	cscene->addCollisionChild(sections[1].walls[0]);
	cscene->addCollisionChild(sections[1].walls[1]);
	cscene->addCollisionChild(sections[1].walls[2]);
	cscene->addCollisionChild(sections[1].walls[3]);

	
	
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
						yspeed = MOVE_SPEED;
					break;
					case KEY_DOWN:
						yspeed = -MOVE_SPEED;
					break;
					case KEY_LEFT:
						zspeed = -MOVE_SPEED;
					break;
					case KEY_RIGHT:
						zspeed = MOVE_SPEED;
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
	//find section obj is in
	int sec;
	for(sec=0; sec<sections.size(); ++sec){	//Loop through sections
		if(sections[sec].inSection(obj->getPosition())){	//If in section
			break;
		}
	}
	if(sec >= sections.size()){
		sec = 0;
	}
	Number elapsed = core->getElapsed();
	Number speed = 100/sections[sec].getArea()*elapsed*MOVE_SPEED;
	
	Vector3 pos = obj->getPosition();
	pos.y += yspeed*elapsed;
	pos.z += zspeed*elapsed;
	pos.x += speed;
	obj->setPosition(pos);
	
	cscene->getActiveCamera()->lookAt(pos);
	Vector3 direction = obj->getPosition() - cscene->getActiveCamera()->getPosition();
	//direction.Normalize();
	direction = direction * (MOVE_SPEED/2*elapsed);
	direction.x = speed;
	
	cscene->getActiveCamera()->setPosition(cscene->getActiveCamera()->getPosition() + direction);
	
	
	//Test collision
	for(int j=0; j<sections[sec].walls.size(); ++j){	//Loop through walls in section
		CollisionResult res = cscene->testCollision(obj, sections[sec].walls[j]);
		if(res.collided) {
			//obj->setColor(0,.5,0,1);
			pos += res.colNormal*res.colDist;
			obj->setPosition(pos);
		} else {
			//obj->setColor(.5,0,0,1);
		}
	}
	
    return core->updateAndRender();
}

section::section(Number height, Number width, Number depth, Vector3 pos)
	:height(height), width(width), depth(depth), walls(), position(pos){
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth,.1, width));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth,.1, width));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth, height, .1));
	walls.push_back(new ScenePrimitive(ScenePrimitive::TYPE_BOX, depth, height, .1));
	setPosition(pos);
}

void section::setPosition(Vector3 pos){
	position = pos;
	walls[0]->setPosition(position+Vector3(0,-height/2,0));
	walls[1]->setPosition(position+Vector3(0,height/2,0));
	walls[2]->setPosition(position+Vector3(0,0,-width/2));
	walls[3]->setPosition(position+Vector3(0,0,width/2));
}

void section::setPosition(Number x, Number y, Number z){
	setPosition(Vector3(x,y,z));
}

bool section::inSection(Vector3 loc){
	return loc.x >= position.x-depth/2 && loc.x <= position.x+depth/2;
}

Number section::getArea(){
	return width*height;
}
