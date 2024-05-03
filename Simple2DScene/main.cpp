/**
* Author: Selena Cheung
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL2_Mixer/SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "MenuScreen.h"

// ————— CONSTANTS ————— //
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;



const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "/Users/selenacheung/Desktop/game-prog/Rise_OfAI/Simple2DScene/shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "/Users/selenacheung/Desktop/game-prog/Rise_OfAI/Simple2DScene/shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
MenuScreen *g_menu_screen;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
const float BG_RED     = 0.2f,
           BG_BLUE    = 0.2f,
           BG_GREEN   = 0.2f,
           BG_OPACITY = 1.0f;
SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Box Head Dupe",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    //menu screen setup //
    g_menu_screen = new MenuScreen();
    switch_to_scene(g_menu_screen);
    // ————— LEVEL SETUP ————— //
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running  = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_game_is_running  = false;
                        break;
                    case SDLK_RETURN:
                        switch_to_scene(g_level_a);
                        break;
                    case SDLK_SPACE:
                        if(g_current_scene->m_state.player->bullet){
                            g_current_scene->m_state.summon_bullet = true;
                            g_current_scene->m_state.bullet->renderable = true;
                            g_current_scene->m_state.bullet->nullbullet = false;
                            g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->SHOOT];
                            
                            
                            g_current_scene->m_state.bullet->set_position(glm::vec3(g_current_scene->m_state.player->get_position().x, g_current_scene->m_state.player->get_position().y -0.15f , 0.0f));
                            g_current_scene->m_state.bullet->move_right();
                            //g_current_scene->m_state.player->transform();
                        }
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    
    if (key_state[SDL_SCANCODE_LEFT])
    {
        g_current_scene->m_state.player->move_left();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_current_scene->m_state.player->move_right();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
    }
    else if (key_state[SDL_SCANCODE_UP])
    {
        g_current_scene->m_state.player->move_up();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->UP];
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        g_current_scene->m_state.player->move_down();
        g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->DOWN];
    }
    else if (key_state[SDL_SCANCODE_D] && g_current_scene->m_state.player->fireball){
        g_current_scene->m_state.summon_bone = true;
        g_current_scene->m_state.weapon->m_position = glm::vec3(g_current_scene->m_state.player->get_position().x, g_current_scene->m_state.player->get_position().y, 0.0f);
        g_current_scene->m_state.weapon->move_right();
        Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
        g_current_scene->m_state.player->blastfire(); //no more after one shot
        
    }
    else if (key_state[SDL_SCANCODE_A] && g_current_scene->m_state.player->fireball){
        g_current_scene->m_state.summon_bone = true;
        g_current_scene->m_state.weapon->m_position = glm::vec3(g_current_scene->m_state.player->get_position().x, g_current_scene->m_state.player->get_position().y, 0.0f);
        g_current_scene->m_state.weapon->move_left();
        Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
        g_current_scene->m_state.player->blastfire(); //no more after one shot
    }

    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
    {
        g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
    }
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
    } else {
        g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
    }
    
    if (g_current_scene->m_state.won == true){
        float lives_prev = g_current_scene->m_state.player->get_lives();
        std::cout << g_current_scene->m_state.next_scene_id << std::endl;
        if (g_current_scene->m_state.next_scene_id == 2){
            switch_to_scene(g_level_b);
            g_current_scene->m_state.player->set_lives(lives_prev);
           // g_current_scene->m_state.lives_left = lives_prev;
        }
        else if (g_current_scene->m_state.next_scene_id == 3){
            switch_to_scene(g_level_c);
            g_current_scene->m_state.player->set_lives(lives_prev);
        }
    }
//    
//    std::cout << g_current_scene->m_state.player->m_position.x << std::endl;
//    std::cout << g_current_scene->m_state.player->m_position.y << std::endl;
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_menu_screen;
    delete g_level_a;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
