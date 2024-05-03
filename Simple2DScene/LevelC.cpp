#include "LevelC.h"
#include "Utility.h"

#define LEVELC_WIDTH 30
#define LEVELC_HEIGHT 8
#define ENEMYC_COUNT 20

unsigned int LEVELC_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
    0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
};

LevelC::~LevelC()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.weapon;
    delete    m_state.loot;
    delete    m_state.map;
    delete    m_state.loot2;
    delete    m_state.health;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/trash.png");
    m_state.map = new Map(LEVELC_WIDTH, LEVELC_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 1, 1);
    
    float BG_REDB   = 0.2f,
          BG_BLUEB  = 0.0f,
          BG_GREENB = 0.3f, 
    BG_OPACITY = 1.0f;
    glClearColor(BG_REDB, BG_BLUEB, BG_GREENB, BG_OPACITY);
    // Code from main.cpp's initialise()
    m_state.won = false;
    m_state.lost = false;
    m_state.next_scene_id = 4;
    
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(3.0f, -2.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/cat.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 16,17,18,19 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] {16,17,18,19 };
    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 0,1,2,3 };
    m_state.player->m_walking[m_state.player->DOWN]  = new int[4] { 0,1,2,3 };

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
    m_state.loot->set_position(glm::vec3(6.0f, -2.0f, 0.0f));
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
    
    //Health pack setup
    m_state.health = new Entity();
    m_state.health->set_entity_type(HEALTH);
    m_state.health->set_position(glm::vec3(11.0f, -6.0f, 0.0f));
    m_state.health->set_movement(glm::vec3(0.0f));
    m_state.health->set_speed(0.0f);
    m_state.health->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.health->m_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/healthpack.png");
    
    // ENEMY //
    GLuint enemy_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/BoxHeadDupe/Simple2DScene/assets/monster.png");
    m_state.enemies = new Entity[ENEMYC_COUNT];
    for (int i = 0; i < ENEMYC_COUNT; i++){
        m_state.enemies[i].set_entity_type(ENEMY);
        m_state.enemies[i].set_ai_type(GUARD);
        m_state.enemies[i].set_ai_state(IDLE);
        m_state.enemies[i].m_texture_id = enemy_texture_id;
        
        //random locations
        float randX = 5.0f + (rand() % 2001) / 100;
        float randY = rand() % 9 - 7;
        //random speed
        float randSpeed = 0.2f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 0.8f);
        std::cout << randSpeed << std::endl;
        //std::cout << randY << std::endl;
        if (10 > i > 5){
            randX += 5.0f;
        } else if ( 15 > i > 10){
            randX += 8.0f;
        } else if ( i > 15){
            randX += 11.0f;
        }
        m_state.enemies[i].set_position(glm::vec3(randX, randY, 0.0f));
        m_state.enemies[i].set_movement(glm::vec3(0.0f));
        m_state.enemies[i].set_speed(randSpeed);
        m_state.enemies[i].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
        
        float randLives = rand() % 3 + 1;
        m_state.enemies[i].set_lives(randLives);
        
        if (randSpeed > 0.8){
            m_state.enemies[i].m_walking[m_state.enemies[0].LEFT]  = new int[4] { 40,41,40,41 };
            m_state.enemies[i].m_walking[m_state.enemies[0].RIGHT] = new int[4] {40,41,40,41};
            m_state.enemies[i].m_walking[m_state.enemies[0].UP]    = new int[4] { 40,41,40,41 };
            m_state.enemies[i].m_walking[m_state.enemies[0].DOWN]  = new int[4] { 40,41,40,41 };
            m_state.enemies[i].set_lives(1);
        }
        else if (randLives > 2 && randSpeed < 0.8){
            m_state.enemies[i].set_lives(3);
            m_state.enemies[i].m_walking[m_state.enemies[0].LEFT]  = new int[4] { 42,43,44,42 };
            m_state.enemies[i].m_walking[m_state.enemies[0].RIGHT] = new int[4] {42,43,44,42 };
            m_state.enemies[i].m_walking[m_state.enemies[0].UP]    = new int[4] { 42,43,44,42  };
            m_state.enemies[i].m_walking[m_state.enemies[0].DOWN]  = new int[4] { 42,43,44,42  };
        }
        else {
            m_state.enemies[i].m_walking[m_state.enemies[0].LEFT]  = new int[4] { 8,9,10,11 };
            m_state.enemies[i].m_walking[m_state.enemies[0].RIGHT] = new int[4] {8,9,10,11};
            m_state.enemies[i].m_walking[m_state.enemies[0].UP]    = new int[4] { 16,17,18,19 };
            m_state.enemies[i].m_walking[m_state.enemies[0].DOWN]  = new int[4] { 0,1,2,3 };
            m_state.enemies[i].set_lives(1);
        }
        
        m_state.enemies[i].m_animation_indices = m_state.enemies[0].m_walking[m_state.enemies[0].LEFT];
        m_state.enemies[i].m_animation_frames = 4;
        m_state.enemies[i].m_animation_index  = 0;
        m_state.enemies[i].m_animation_time   = 0.0f;
        m_state.enemies[i].m_animation_cols   = 8;
        m_state.enemies[i].m_animation_rows   = 8;
    }
    
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_state.bgm = Mix_LoadMUS("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/burn.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);
    
    m_state.jump_sfx = Mix_LoadWAV("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/poof.mp3");
}

void LevelC::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMYC_COUNT, m_state.map);
    if (m_state.summon_bone){
        m_state.weapon->update(delta_time, m_state.weapon, m_state.enemies, ENEMYC_COUNT, m_state.map);
    }
    if (m_state.summon_bullet){
        m_state.bullet->update(delta_time, m_state.bullet, m_state.enemies, ENEMYC_COUNT, m_state.map);
    }
    m_state.loot->update(delta_time, m_state.loot, m_state.player, 1, m_state.map);
    m_state.loot2->update(delta_time, m_state.loot2, m_state.player, 1, m_state.map);
    m_state.health->update(delta_time, m_state.health, m_state.player, 1, m_state.map);
    for (int i = 0; i < ENEMYC_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.enemies, m_state.player, 1, m_state.map);
    }
    int total_gone = 0;
    for (int i = 0; i < ENEMYC_COUNT; i++){
        if (m_state.enemies[i].m_is_active == false){
            total_gone+=1;
        }
    }
    //std::cout << total_gone << std::endl;
    if (total_gone == ENEMYC_COUNT){
        m_state.won = true;
    }
    if (m_state.player->m_is_active == false){
        m_state.lost = true;
    }
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

void LevelC::render(ShaderProgram *program)
{
    m_state.map->render(program);
    m_state.player->render(program);
    if (m_state.summon_bone){
        m_state.weapon->render(program);
    }
    if (m_state.summon_bullet && m_state.bullet->renderable){
        m_state.bullet->render(program);
    }
    m_state.loot->render(program);
    m_state.loot2->render(program);
    m_state.health->render(program);
    
    for (int i = 0; i < ENEMYC_COUNT; i++)    m_state.enemies[i].render(program);
    GLuint font_texture_id = Utility::load_texture("/Users/selenacheung/Desktop/Platformer/Simple2DScene/assets/font1.png");
    if(m_state.player->get_lives() == 6){
        Utility::draw_text(program, font_texture_id, "HP:6", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x - 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    else if(m_state.player->get_lives() == 5){
        Utility::draw_text(program, font_texture_id, "HP:5", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    else if(m_state.player->get_lives() == 4){
        Utility::draw_text(program, font_texture_id, "HP:4", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    else if(m_state.player->get_lives() == 3){
        Utility::draw_text(program, font_texture_id, "HP:3", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    else if(m_state.player->get_lives() == 2){
        Utility::draw_text(program, font_texture_id, "HP:2", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    else if(m_state.player->get_lives() == 1){
        Utility::draw_text(program, font_texture_id, "HP:1", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    else {
        Utility::draw_text(program, font_texture_id, "HP:0", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 0.5, 0.0f));
    }
    
    if (m_state.lost){
        Utility::draw_text(program, font_texture_id, "You Lose", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 1, 0.0f));
    }
    if(m_state.won) {
        Utility::draw_text(program, font_texture_id, "You Win", 0.5f, 0.01f, glm::vec3(m_state.player->m_position.x- 0.5, m_state.player->m_position.y + 1, 0.0f));
    }
}
