#include "pch.h"
#include <fstream>

enum state
{
    disable,
    record,
    play
};

struct ReplayData
{
    unsigned frame;
    float x;
    float y;
    float rotation;
    double y_accel;
    bool player;
};

struct ReplayData2
{
    unsigned frame;
    bool hold;
    bool player;
};

struct CheckpointData
{
    unsigned frame;
    float pos_x, pos_y, rotation, player_speed, vehicle_size;
    double x_accel, y_accel, jump_accel;
    bool is_upsidedown, can_robot_jump, is_on_ground, is_dashing, is_sliding, is_rising, black_orb,
        is_holding, is_holding2, has_just_held, has_just_held2;
};

class Replay
{
public:
    int index = 0;
    vector<ReplayData> replay;
    int index2 = 0;
    vector<ReplayData2> replay2;

    state mode = disable;
    char replay_name[128];
    float fps_value = 60.f;
    float speed_value = 1.f;

    bool ignore_input = true;
    bool real_time = true;
    bool dual_clicks = false;

    bool accuracy_fix = true;
    bool disable_rotationfix = true;

    unsigned get_frame();
    void handle_recording(gd::PlayLayer *self, bool player);
    void handle_recording2(bool player, bool hold);
    void handle_playing(gd::PlayLayer *self);
    void handle_reseting(gd::PlayLayer *self);
    void remove_actions(unsigned frame);
    void clear();
    bool empty();
    void reset_replay();
    string save(string name);
    string load(string name);
    size_t replay_size() { return replay.size() + replay2.size(); }
    string replay_size_text()
    {
        string data2;
        for (const auto &data : replay2)
        {
            data2 += +"Frame: " + to_string(data.frame) + " Hold: " + to_string(data.hold) + " Player: " + to_string(data.player) + "\n";
        }
        return data2;
    }
};

extern Replay replay;

class PracticeFix
{
public:
    vector<CheckpointData> checkpoints_p1;
    vector<CheckpointData> checkpoints_p2;

    void handle_checkpoint(gd::PlayLayer *self);
    bool fix_respawn(gd::PlayLayer *self);
    void delete_last()
    {
        if (!checkpoints_p1.empty())
        {
            checkpoints_p1.pop_back();
            checkpoints_p2.pop_back();
        }
    }
    void clear()
    {
        checkpoints_p1.clear();
        checkpoints_p2.clear();
    }
    void update_frame_offset();
    unsigned frame_offset = 0;
};

extern PracticeFix practiceFix;

class FrameAdvance
{
public:
    bool enabled = false;
    bool triggered = false;
};

extern FrameAdvance frameAdvance;

class SpamBot
{
public:
    bool enabled = false;
    int hold = 5;
    int release = 5;
    int hold_current = 0;
    int release_current = 0;
    bool player_p1 = true;
    bool player_p2 = true;

    bool downed = false;

    bool next_frame();
    void handle_spambot(gd::PlayLayer *self);
    void reset_temp();
};

extern SpamBot spamBot;

class StraightFly
{
public:
    bool enabled = false;
    int accuracy = 40;

    void handle_straightfly(gd::PlayLayer *self);
    void start(gd::PlayLayer *self);

private:
    float start_y = 0.0f;
};

extern StraightFly straightFly;
