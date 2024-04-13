#include "LevelB.h"
#include "Utility.h"

#define LEVELB_WIDTH 14
#define LEVELB_HEIGHT 8
#define ENEMYB_COUNT 2



unsigned int LEVELB_DATA[] =
{
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 3, 2, 0, 0, 0, 3, 2, 0, 0, 0, 0,
    2, 0, 3, 2, 0, 3, 1, 2, 0, 3, 2, 0, 0, 0,
    2, 0, 0, 3, 2, 0, 0, 0, 3, 2, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 3, 1, 2, 0, 0, 0, 0, 0, 0,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2
};

LevelB::~LevelB()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.weapon;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise()
{
    GLuint map_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/nuvem.png");
    m_state.map = new Map(LEVELB_WIDTH, LEVELB_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 3, 1);

    // Code from main.cpp's initialise()
    m_state.won = false;
    m_state.lost = false;
    m_state.next_scene_id = 3;
    
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
    m_state.player->set_lives(m_state.lives_left);
    m_state.player->set_invincible(false);
    
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
    
    // CHOCOLATE ENEMY //
    GLuint enemy_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/choco_bar.png");
    
    m_state.enemies = new Entity[ENEMYB_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(7.0f, -5.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(0.5f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(JUMPER);
    m_state.enemies[1].set_ai_state(IDLE);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    m_state.enemies[1].set_position(glm::vec3(6.0f, -2.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_speed(0.5f);
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.enemies[1].m_jumping_power = 4.0f;
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/Rainbows.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    
    m_state.jump_sfx = Mix_LoadWAV("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/poof.mp3");
}

void LevelB::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMYB_COUNT, m_state.map);
    if (m_state.summon_bone){
        m_state.weapon->update(delta_time, m_state.weapon, m_state.enemies, ENEMYB_COUNT, m_state.map);
    }
    for (int i = 0; i < ENEMYB_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.enemies, m_state.player, 1, m_state.map);
    }
    int total_gone = 0;
    for (int i = 0; i < ENEMYB_COUNT; i++){
        if (m_state.enemies[i].m_is_active == false){
            total_gone+=1;
        }
    }
    
    if (total_gone == ENEMYB_COUNT){
        m_state.won = true;
        m_state.lives_left = m_state.player->get_lives();
    }
    if (m_state.player->m_is_active == false){
        m_state.lost = true;
        //message_display_time += delta_time;
    }
    
}

//void LevelB::set_lives_left(float lives){
//    m_state.player->set_lives(lives);
//}

void LevelB::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    if (m_state.summon_bone){
        m_state.weapon->render(program);
    }
    for (int i = 0; i < ENEMYB_COUNT; i++)    m_state.enemies[i].render(program);
    GLuint font_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/font1.png");
    if (m_state.lost){
        Utility::draw_text(program, font_texture_id, "You Lose", 1.0f, 0.01f, glm::vec3( 1.5f, -3.0f, 0.0f));
    }
    if(m_state.player->get_lives() == 3){
        Utility::draw_text(program, font_texture_id, "Lives: 3", 0.5f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));
    }
    else if(m_state.player->get_lives() == 2){
        Utility::draw_text(program, font_texture_id, "Lives: 2", 0.5f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));
    }
    else if(m_state.player->get_lives() == 1){
        Utility::draw_text(program, font_texture_id, "Lives: 1", 0.5f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));
    }
    else {
        Utility::draw_text(program, font_texture_id, "Lives: 0", 0.5f, 0.01f, glm::vec3(1.0f, -1.0f, 0.0f));
    }
}
