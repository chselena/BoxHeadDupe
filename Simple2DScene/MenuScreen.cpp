#include "MenuScreen.h"
#include "Utility.h"
#define MENU_WIDTH 14
#define MENU_HEIGHT 8

unsigned int MENU_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0,
    0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0,
    2, 3, 1, 1, 1, 2, 0, 0, 3, 2, 0, 3, 1, 2
};


MenuScreen::~MenuScreen()
{
    delete    m_state.player;
    delete    m_state.weapon;
}

// Initialize method
void MenuScreen::initialise() {
    
    GLuint map_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/nuvem.png");
    m_state.map = new Map(MENU_WIDTH, MENU_HEIGHT, MENU_DATA, map_texture_id, 1.0f, 3, 1);

    // Code from main.cpp's initialise()

    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/dog.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 5, 6, 7, 4 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 9, 10, 11, 8 };
    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 1, 2, 3, 0 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 4;
    m_state.player->m_animation_rows   = 4;
    m_state.player->set_height(0.9f);
    m_state.player->set_width(0.9f);
    m_state.player->set_lives(3.0f);
    m_state.player->set_invincible(true);
    
    // Jumping
    m_state.player->m_jumping_power = 5.0f;
    
    // BONE SET_UP //
    m_state.weapon = new Entity();
    m_state.weapon->set_entity_type(WEAPON);
    m_state.weapon->set_position(m_state.player->m_position);
    m_state.weapon->set_movement(glm::vec3(0.0f));
    m_state.weapon->set_speed(2.5f);
    m_state.weapon->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.weapon->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/bone.png");
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/Paradise_Found.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    
    m_state.jump_sfx = Mix_LoadWAV("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/poof.mp3");
    
}

// Update method
void MenuScreen::update(float delta_time) {
    // Update menu screen logic here if needed
    m_state.player->update(delta_time, m_state.player, NULL, 0, m_state.map);
    if (m_state.summon_bone){
        m_state.weapon->update(delta_time, m_state.weapon, NULL, 0, m_state.map);
    }
}

// Render method
void MenuScreen::render(ShaderProgram *program) {
    m_state.player->render(program);
    m_state.map->render(program);
    if (m_state.summon_bone){
        m_state.weapon->render(program);
    }
    GLuint font_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/font1.png");
    Utility::draw_text(program, font_texture_id, "Dogs Belong In Heaven", 0.5f, 0.01f, glm::vec3(2.0f, -2.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Press Enter to Start", 0.5f, 0.01f, glm::vec3(2.0f, -3.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Attack Keys: D, A", 0.5f, 0.01f, glm::vec3(2.0f, -4.0f, 0.0f));
}
