#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

Entity::Entity()
{
    // ––––– PHYSICS ––––– //
    m_position = glm::vec3(0.0f);
    m_velocity = glm::vec3(0.0f);
    m_acceleration = glm::vec3(0.0f);

    // ––––– TRANSLATION ––––– //
    m_movement = glm::vec3(0.0f);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
}

Entity::~Entity()
{
    delete[] m_animation_up;
    delete[] m_animation_down;
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Entity::ai_activate(Entity* player)
{
    switch (m_ai_type)
    {

    case GUARD:
        ai_guard(player);
        break;
    case TANK:
        ai_morehealth();
        break;
            
    default:
        break;
    }
}


void Entity::ai_morehealth()
{
    set_lives(2);
}

void Entity::ai_guard(Entity* player)
{
    switch (m_ai_state) {
        case IDLE:
            if (glm::distance(m_position, player->get_position()) < 8.0f) m_ai_state = WALKING;
            break;
            
        case WALKING:
        {
            glm::vec3 playerPos = player->get_position();
            if (glm::distance(m_position, playerPos) < 5.0f) {
                if (abs(m_position.x - playerPos.x) > abs(m_position.y - playerPos.y)) {
                    if (m_position.x > playerPos.x) {
                        m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
                        m_animation_indices = m_walking[LEFT];
                    } else {
                        m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
                        m_animation_indices = m_walking[RIGHT];
                    }
                } else {
                    if (m_position.y > playerPos.y) {
                        m_movement = glm::vec3(0.0f, -1.0f, 0.0f);
                        m_animation_indices = m_walking[DOWN];
                    } else {
                        m_movement = glm::vec3(0.0f, 1.0f, 0.0f);
                        m_animation_indices = m_walking[UP];
                    }
                }
            }
        }
            break;
    }

}


void Entity::update(float delta_time, Entity* player, Entity* objects, int object_count, Map* map)
{
    if (!m_is_active) return;
    if (m_entity_type == PLAYER || m_entity_type == ENEMY) {
        if (m_lives == 0){
            deactivate();
        }
        if (m_position.y < -9){
            m_lives -= 1;
            set_position(glm::vec3(3.0,0.0,0.0));
        }
    }
    if (m_entity_type != WEAPON && m_position.y < -9){
        deactivate();
    }
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) ai_activate(objects);

    if (m_animation_indices != NULL)
    {
        if (glm::length(m_movement) != 0)
        {
            m_animation_time += delta_time;
            float frames_per_second = (float)1 / SECONDS_PER_FRAME;

            if (m_animation_time >= frames_per_second)
            {
                m_animation_time = 0.0f;
                m_animation_index++;

                if (m_animation_index >= m_animation_frames)
                {
                    m_animation_index = 0;
                }
            }
        }
    }

    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y = m_movement.y * m_speed;
    
    m_velocity += m_acceleration * delta_time;

    // We make two calls to our check_collision methods, one for the collidable objects and one for
    // the map.
    m_position.y += m_velocity.y * delta_time;
    check_collision_y(objects, object_count);
    
    //fireball killing
//    if (m_entity_type == WEAPON && check_collision_x(objects)){
//        deactivate();
//    }

    m_position.x += m_velocity.x * delta_time;
    check_collision_x(objects, object_count);
    check_collision_x(map);

    if (m_is_jumping)
    {
        m_is_jumping = false;

        m_velocity.y += m_jumping_power;
    }
    //std::cout << m_invincible << std::endl;
    if (m_entity_type == PLAYER && m_invincible == true){
       // std::cout << get_buffer() << std::endl;
        float timetoadd = 0.0f;
        timetoadd = get_buffer();
        set_buffer(timetoadd += delta_time);
    }
    
    if(get_buffer() >= 2.0f){
        set_invincible(false);
        set_buffer(0.0f);
    }

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count)
{
    
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];
        if (check_collision(collidable_entity))
        {
            
            //std::cout << "y collision" << std::endl;
            float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->get_height() / 2.0f));
            if (m_velocity.y > 0) {
                //std::cout << "y > 0" << std::endl;
                m_collided_top = true;
                if(m_entity_type == ENEMY && collidable_entity->m_invincible == false){
                    m_position.y -= y_overlap;
                    m_velocity.y = 0;
                    collidable_entity->m_lives -= 1;
                    collidable_entity->set_invincible(true);
                }
                else if(m_entity_type == PLAYER && m_invincible == false){
                    m_position.y -= y_overlap;
                    m_velocity.y = 0;
                    m_lives -= 1; //kill itself
                    set_invincible(true);
                }
                else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == WEAPON){
                    collidable_entity->deactivate();
                    deactivate();
                }
                else if(collidable_entity->get_entity_type() == PLAYER && m_entity_type == HEALTH){
                    collidable_entity->m_lives += 1;
                    deactivate();
                }

            }
            else if (m_entity_type == ENEMY && m_velocity.y < 0 && collidable_entity->m_invincible == false) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
                collidable_entity->m_lives -= 1;
                collidable_entity->set_invincible(true);
            }
            else if (m_entity_type == PLAYER && m_velocity.y < 0 && m_invincible == false) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
                m_lives -= 1;
                set_invincible(true);
            }
            else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == WEAPON){
                collidable_entity->deactivate();
                deactivate();
            }
            else if(collidable_entity->get_entity_type() == PLAYER && m_entity_type == LOOT){
                collidable_entity->transform();
                deactivate();
            }
            else if(collidable_entity->get_entity_type() == PLAYER && m_entity_type == LOOT2){
                collidable_entity->blastfire(); //fireball becomes true
                deactivate();
            }
            else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == BULLET && !nullbullet){
                std::cout << "i hit a zombie " << std::endl;
                collidable_entity->m_lives -= 1;
                m_movement.x  = 0.0f;
                nullbullet = true;
                renderable = false;
            }
            else if(collidable_entity->get_entity_type() == PLAYER && m_entity_type == HEALTH){
                collidable_entity->m_lives += 1;
                deactivate();
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        Entity* collidable_entity = &collidable_entities[i];

        if (check_collision(collidable_entity))
        {
            float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->get_width() / 2.0f));
            if (m_velocity.x > 0 && m_velocity.y > 0) {
                m_collided_right = true;
                if(m_entity_type == PLAYER && m_invincible == false){
                    m_position.x -= x_overlap;
                    m_velocity.x = 0;
                   // std::cout << "player should die from right" << std::endl; //good
                    m_lives -= 1;
                    set_invincible(true);
                }
                else if(m_entity_type == ENEMY && collidable_entity->m_invincible == false){
                    m_position.x -= x_overlap;
                    m_velocity.x = 0;
                    collidable_entity->m_lives -= 1;
                    collidable_entity->set_invincible(true);
                }
                else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == WEAPON){
                    collidable_entity->deactivate();
                    deactivate();
                }
                else if(collidable_entity->get_entity_type() == PLAYER && m_entity_type == HEALTH){
                    collidable_entity->m_lives += 1;
                    deactivate();
                }
//                else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == BULLET ){
//                    collidable_entity->m_lives -= 1;
//                    m_movement.x  = 0.0f;
//                    //nullbullet = true;
//                    renderable = false;
//                }
            }
            else if (m_velocity.x < 0) {
                m_collided_left = true;
                if(m_entity_type == PLAYER && m_invincible == false){
                    m_position.x += x_overlap;
                    m_velocity.x = 0;
                   // std::cout << "player should die from left" << std::endl;
                    m_lives -= 1;
                    set_invincible(true);
                }
                else if(m_entity_type == ENEMY && collidable_entity->m_invincible == false){
                    m_position.x += x_overlap;
                    m_velocity.x = 0;
                    //std::cout << "player should die from left" << std::endl;
                    collidable_entity->m_lives -= 1;
                    collidable_entity->set_invincible(true);
                }
                else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == WEAPON){
                    collidable_entity->deactivate();
                    deactivate();
                }
                else if(collidable_entity->get_entity_type() == PLAYER && m_entity_type == HEALTH){
                    collidable_entity->m_lives += 1;
                    deactivate();
                }
//                else if(collidable_entity->get_entity_type() != PLAYER && m_entity_type == BULLET ){
//                    collidable_entity->m_lives -= 1;
//                    std::cout <<"h"
//                    m_movement.x  = 0.0f;
//                    //nullbullet = true;
//                    renderable = false;
//                }
            }
        }
    }
}


void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
        if (m_entity_type == WEAPON){
            deactivate();
        }
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = -m_velocity.x;
        
        m_collided_right = true;
        if (m_entity_type == WEAPON){
            deactivate();
        }
    }
}



void Entity::render(ShaderProgram* program)
{
     if (!m_is_active) return;
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    if (m_entity_type == BULLET) {
        float bullet_vertices[] = { -0.1, -0.1, 0.1, -0.1, 0.1, 0.1, -0.1, -0.1, 0.1, 0.1, -0.1, 0.1 };
        float bullet_tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };
        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, bullet_vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, bullet_tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    } else {
        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };
        //glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    }
    
    
}


bool const Entity::check_collision(Entity* other) const
{
    if (other == this) return false;
    // If either entity is inactive, there shouldn't be any collision
    if (!m_is_active || !other->m_is_active) return false;

    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);
    return x_distance < 0.0f && y_distance < 0.0f;

}
