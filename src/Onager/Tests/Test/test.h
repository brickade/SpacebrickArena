#pragma once

#include "Entity.h"
#include "Player.h"
#include "SDL.h"

using namespace ong;

class Test
{
public:
	Test();

	virtual void init();
	void run();

	virtual bool procEvent(SDL_Event event) { return false; };

	virtual void update(float dt) {};
	virtual void render() {};

protected:
	World* m_world;
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_toAdd;

	GLuint m_colorLocation;
	bool m_stepping = false;

	Transform m_eye;

	Entity* addBox(World* world, BodyDescription descr, Material* material);
	Entity* addSlope(World* world, Transform t, Material* material);
	Entity* addFloor(World* world, Material* material);
 	Player* Test::addPlayer(World* world, const Transform& transform);
	SDL_Window* m_window;

private:
	void initGL();
	void initSDL();

	int m_numSteps = 0;

	
	Player* m_player;
	bool m_playerSpec;


	SDL_GLContext m_context;

	GLuint m_programID;
};


