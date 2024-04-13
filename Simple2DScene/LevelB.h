#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 2;
    
    // ————— CONSTRUCTOR ————— //
    ~LevelB();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    //void set_lives_left(float lives) override;
};
