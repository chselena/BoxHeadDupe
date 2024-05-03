#include "MenuScreen.h"
#include "Utility.h"
#define MENU_WIDTH 14
#define MENU_HEIGHT 8

unsigned int MENU_DATA[] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0
};


MenuScreen::~MenuScreen()
{
    delete    m_state.player;
    delete    m_state.weapon;
    delete    m_state.loot;
    delete    m_state.loot2;
    delete     m_state.bullet;
    //delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);

}

// Initialize method
void MenuScreen::initialise() {
    
    GLuint map_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/Brickwall4_Texture.png");
    m_state.map = new Map(MENU_WIDTH, MENU_HEIGHT, MENU_DATA, map_texture_id, 1.0f, 1, 1);

    // Code from main.cpp's initialise()

    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(5.0f, -5.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/cat.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 16,17,18,19 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] {16,17,18,19 };
    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 0,1,2,3 };
    m_state.player->m_walking[m_state.player->DOWN]    = new int[4] { 0,1,2,3 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 16;
    m_state.player->m_animation_rows   = 16;
    m_state.player->set_height(0.9f);
    m_state.player->set_width(0.9f);
    m_state.player->set_lives(3.0f);
    m_state.player->set_invincible(false);
    
    // Jumping
    m_state.player->m_jumping_power = 0.0f;
    
    // weapon SET_UP //
    m_state.weapon = new Entity();
    m_state.weapon->set_entity_type(WEAPON);
    m_state.weapon->set_position(m_state.player->m_position);
    m_state.weapon->set_movement(glm::vec3(0.0f));
    m_state.weapon->set_speed(2.5f);
    m_state.weapon->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.weapon->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/energy_effect_base.png");
    
    // animation set up for weapon
    m_state.weapon->m_walking[m_state.weapon->LEFT]  = new int[4] {2,3,4,5 };
    m_state.weapon->m_walking[m_state.weapon->RIGHT] = new int[4] {2,3,4,5 };
    m_state.weapon->m_walking[m_state.weapon->UP]    = new int[4] { 17,18,19,20};
    m_state.weapon->m_walking[m_state.weapon->DOWN]    = new int[4] { 29,30,31,32 };

    m_state.weapon->m_animation_indices = m_state.weapon->m_walking[m_state.weapon->RIGHT];  // start George looking left
    m_state.weapon->m_animation_frames = 4;
    m_state.weapon->m_animation_index  = 0;
    m_state.weapon->m_animation_time   = 0.0f;
    m_state.weapon->m_animation_cols   = 16;
    m_state.weapon->m_animation_rows   = 8;
    
    // LOOT SETUP //
    m_state.loot = new Entity();
    m_state.loot->set_entity_type(LOOT);
    m_state.loot->set_position(glm::vec3(3.0f, -6.0f, 0.0f));
    m_state.loot->set_movement(glm::vec3(0.0f));
    m_state.loot->set_speed(0.0f);
    m_state.loot->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.loot->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/box_open.png");
    
    //FIREBALL LOOT2
    m_state.loot2 = new Entity();
    m_state.loot2->set_entity_type(LOOT2);
    m_state.loot2->set_position(glm::vec3(9.0f, -5.0f, 0.0f));
    m_state.loot2->set_movement(glm::vec3(0.0f));
    m_state.loot2->set_speed(0.0f);
    m_state.loot2->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.loot2->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/box_closed.png");
    
    //BULLET setup
    m_state.bullet = new Entity();
    m_state.bullet->set_entity_type(BULLET);
    m_state.bullet->set_position(glm::vec3(m_state.player->get_position().x, m_state.player->get_position().y - 0.15f , 0.0f));
    m_state.bullet->set_movement(glm::vec3(0.0f));
    m_state.bullet->set_speed(5.0f);
    m_state.bullet->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.bullet->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/realbull.png");
    
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/apoc.mp3");
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
    if (m_state.summon_bullet && m_state.bullet->nullbullet == false){
        m_state.bullet->update(delta_time, m_state.bullet, NULL, 0, m_state.map);
    }
    m_state.loot->update(delta_time, m_state.loot, m_state.player, 1, m_state.map);
    m_state.loot2->update(delta_time, m_state.loot2, m_state.player, 1, m_state.map);
    if (m_state.player->gunner == true){
        m_state.player->shootbullet();
        m_state.player->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/catgun.png");
        
        // Animation
        m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 48,49,50,51 };
        m_state.player->m_walking[m_state.player->RIGHT] = new int[4] {48,49,50,51 };
        m_state.player->m_walking[m_state.player->UP]    = new int[4] { 129,130,129,130 };
        m_state.player->m_walking[m_state.player->DOWN]    = new int[4] { 48,49,50,51 };
        m_state.player->m_walking[m_state.player->SHOOT]    = new int[4] {33,33,33,33};

        //m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
        m_state.player->m_animation_frames = 4;
        m_state.player->m_animation_index  = 0;
        m_state.player->m_animation_time   = 0.0f;
        m_state.player->m_animation_cols   = 16;
        m_state.player->m_animation_rows   = 16;
        
        m_state.player->transform();
    }
    
}

// Render method
void MenuScreen::render(ShaderProgram *program) {
    
    m_state.map->render(program);
    if (m_state.summon_bone){
        m_state.weapon->render(program);
    }
    if (m_state.summon_bullet && m_state.bullet->nullbullet == false){
        m_state.bullet->render(program);
    }
    
    GLuint font_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/font1.png");
    Utility::draw_text(program, font_texture_id, "Apocalypse Shooter", 0.5f, 0.01f, glm::vec3(2.0f, -2.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Kill Zombies, Get Boxes", 0.5f, 0.01f, glm::vec3(1.0f, -3.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Fire:D,A Gun: Space", 0.5f, 0.01f, glm::vec3(1.0f, -4.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, "Enter to Start", 0.5f, 0.01f, glm::vec3(2.0f, -5.0f, 0.0f));
    m_state.player->render(program);
    m_state.loot->render(program);
    m_state.loot2->render(program);
}
