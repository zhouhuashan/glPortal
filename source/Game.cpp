#include "Game.hpp"
#include "Window.hpp"
#include <iostream>
#include <stdexcept>

namespace glPortal {
Game::Game(): closed(0) {
  config = Environment::getConfigPointer();
  int height, width;
  std::string fullscreen;
  try{
    height  = config->getIntByKey("height");
    width   = config->getIntByKey("width");
  } catch (const std::invalid_argument& e){
    height  = 800;
    width   = 800;
  }
  window.create("GlPortal", width, height);
  try{
    fullscreen  = config->getStringByKey("fullscreen");
  } catch (const std::invalid_argument& e){
  }

  if(fullscreen == "yes"){
    window.setFullscreen();
  }
  
  world.create();
  update();
}

void Game::update() {
  SDL_Event event;
  //Time the next update should be run to conform to UPDATE_RATE
  unsigned int nextUpdate = SDL_GetTicks();
  //Time of last FPS count
  unsigned int lastFpsTime = SDL_GetTicks();
  //Amount of times rendering has been skipped in favour of updating
  int skipped;
  //Amount of times rendered so far, gets reset each second
  int rendered = 0;
  while (!closed) {
    skipped = 0;
    //Count the FPS
    if(SDL_GetTicks() - lastFpsTime > 1000) {
      lastFpsTime = SDL_GetTicks();
      int fps = rendered;
      std::cout << fps << std::endl;
      rendered = 0;
    }

    //Update the game if it is time
    while(SDL_GetTicks() > nextUpdate && skipped < MAX_SKIP) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          closed = 1;
        }
        if (event.type == SDL_KEYDOWN) {
          int key = event.key.keysym.sym;
          int mod = event.key.keysym.mod;

          Input::keyPressed(key, mod);

          if(key == 'q') {
            close();
          }
        }
        if (event.type == SDL_KEYUP) {
          int key = event.key.keysym.sym;
          int mod = event.key.keysym.mod;

          Input::keyReleased(key, mod);
        }
      }
      world.update();
      nextUpdate += SKIP_TIME;
      skipped++;
    }
    world.render();
    rendered++;
    window.swapBuffers();
  }
  window.close();
}

void Game::close() {
  closed = true;
}

} /* namespace glPortal */

using namespace glPortal;

Window window;

int main(int argc, char *argv[]) {
  Environment::init(argc, argv);
  Game game;

  return 0;
}
