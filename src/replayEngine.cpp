#include "replayEngine.h"
#include "hooks.h"
#include <fstream>

Replay replay;
PracticeFix practiceFix;
FrameAdvance frameAdvance;
SpamBot spamBot;
StraightFly straightFly;

unsigned Replay::get_frame()
{
    auto pl = gd::GameManager::sharedState()->getPlayLayer();
    if (pl)
        return static_cast<unsigned>(pl->m_time * fps_value) + practiceFix.frame_offset;
    return 0;
}

void Replay::handle_recording(gd::PlayLayer *self, bool player)
{
    if (!player && !self->m_isDualMode)
        return;

    unsigned frame = get_frame();

    bool frameExists = false;
    for (const auto &data : replay)
    {
        if (data.frame == frame && data.player == player)
        {
            frameExists = true;
            break;
        }
    }

    if (frameExists)
    {
        return;
    }

    replay.push_back({frame,
                      player ? self->m_player1->m_position.x : self->m_player2->m_position.x,
                      player ? self->m_player1->m_position.y : self->m_player2->m_position.y,
                      player ? self->m_player1->getRotation() : self->m_player2->getRotation(),
                      player ? self->m_player1->m_yAccel : self->m_player2->m_yAccel,
                      player});
}

void Replay::handle_recording2(bool player, bool hold)
{
    replay2.push_back({get_frame(), hold, player});
}

void Replay::handle_playing(gd::PlayLayer *self)
{
    unsigned frame = get_frame();
    if (accuracy_fix)
    {
        while (index < (int)replay.size() && frame >= replay[index].frame)
        {
            if (replay[index].player)
            {
                self->m_player1->m_position.x = replay[index].x;
                self->m_player1->m_position.y = replay[index].y;
                if (!disable_rotationfix)
                {
                    self->m_player1->setRotation(replay[index].rotation);
                }
                self->m_player1->m_yAccel = replay[index].y_accel;
            }
            else
            {
                self->m_player2->m_position.x = replay[index].x;
                self->m_player2->m_position.y = replay[index].y;
                if (!disable_rotationfix)
                {
                    self->m_player2->setRotation(replay[index].rotation);
                }
                self->m_player2->m_yAccel = replay[index].y_accel;
            }
            index++;
        }
    }

    while (index2 < (int)replay2.size() && frame >= replay2[index2].frame)
    {
        auto action = replay2[index2].hold;
        if (action)
        {
            hooks::playLayer_pushButton(self, 0, replay2[index2].player);
        }
        else
        {
            hooks::playLayer_releaseButton(self, 0, replay2[index2].player);
        }

        index2++;
    }
}

void Replay::handle_reseting(gd::PlayLayer *self)
{
    if (mode == state::play)
    {
        reset_replay();
    }
    else if (mode == state::record)
    {
        auto zero_frame = [this]()
        {
            if (mode == record)
            {
                Replay::clear();
            }
            practiceFix.clear();
            practiceFix.update_frame_offset();
        };

        if (self->m_isPracticeMode)
        {
            if (practiceFix.fix_respawn(self))
            {
                practiceFix.update_frame_offset();
                if (mode == record)
                {
                    remove_actions(get_frame());
                    bool holding = self->m_player1->m_isHolding;
                    if ((holding && replay2.empty()) || (!replay2.empty() && replay2.back().hold != holding))
                    {
                        handle_recording2(true, holding);
                        if (holding)
                        {
                            hooks::playLayer_releaseButton(self, 0, true);
                            hooks::playLayer_pushButton(self, 0, true);
                            self->m_player1->m_hasJustHeld = true;
                        }
                    }
                    else if (!replay2.empty() && replay2.back().hold && holding && !practiceFix.checkpoints_p1.empty() &&
                             practiceFix.checkpoints_p1.back().has_just_held)
                    {
                        hooks::playLayer_releaseButton(self, 0, true);
                        hooks::playLayer_pushButton(self, 0, true);
                    }
                    if (self->m_levelSettings->m_twoPlayerMode)
                        handle_recording(false, false);
                }
            }
            else
            {
                zero_frame();
            }
        }
        else
            zero_frame();
    }
}

void Replay::remove_actions(unsigned frame)
{
    auto check = [&](ReplayData &action) -> bool
    {
        return action.frame > frame;
    };
    replay.erase(remove_if(replay.begin(), replay.end(), check), replay.end());

    auto check2 = [&](ReplayData2 &action) -> bool
    {
        return action.frame >= frame;
    };
    replay2.erase(remove_if(replay2.begin(), replay2.end(), check2), replay2.end());
}

void Replay::clear()
{
    replay.clear();
    replay2.clear();
}

bool Replay::empty()
{
    return replay.empty() || replay2.empty();
}

void Replay::reset_replay()
{
    index = 0;
    index2 = 0;
}

string Replay::save(string name)
{
    if (replay.empty())
        return "Replay doesn't have actions";

    ofstream file("ReplayEngine/Replays/" + name + ".re", std::ios::binary);

    file.write(reinterpret_cast<char *>(&fps_value), sizeof(fps_value));

    unsigned replay_size = replay.size();
    unsigned replay2_size = replay2.size();

    file.write(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.write(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    file.write(reinterpret_cast<char *>(&replay[0]), sizeof(ReplayData) * replay_size);
    file.write(reinterpret_cast<char *>(&replay2[0]), sizeof(ReplayData2) * replay2_size);

    file.close();
    return "Replay saved";
}

string Replay::load(string name)
{
    if (!replay.empty())
        return "Please clear replay before loading another";

    ifstream file("ReplayEngine/Replays/" + name + ".re", std::ios::binary);

    file.read(reinterpret_cast<char *>(&fps_value), sizeof(fps_value));

    unsigned replay_size = 0;
    unsigned replay2_size = 0;

    file.read(reinterpret_cast<char *>(&replay_size), sizeof(replay_size));
    file.read(reinterpret_cast<char *>(&replay2_size), sizeof(replay2_size));

    replay.resize(replay_size);
    replay2.resize(replay2_size);

    file.read(reinterpret_cast<char *>(&replay[0]), sizeof(ReplayData) * replay_size);
    file.read(reinterpret_cast<char *>(&replay2[0]), sizeof(ReplayData2) * replay2_size);

    file.close();
    return "Replay loaded";
}

void PracticeFix::handle_checkpoint(gd::PlayLayer *self)
{
    checkpoints_p1.push_back({replay.get_frame(),
                              self->m_player1->m_position.x,
                              self->m_player1->m_position.y,
                              self->m_player1->getRotation(),
                              self->m_player1->m_playerSpeed,
                              self->m_player1->m_vehicleSize,
                              self->m_player1->m_xAccel,
                              self->m_player1->m_yAccel,
                              self->m_player1->m_jumpAccel,
                              self->m_player1->m_isUpsideDown,
                              self->m_player1->m_canRobotJump,
                              self->m_player1->m_isOnGround,
                              self->m_player1->m_isDashing,
                              self->m_player1->m_isSliding,
                              self->m_player1->m_isRising,
                              self->m_player1->black_orb,
                              self->m_player1->m_isHolding,
                              self->m_player1->m_isHolding2,
                              self->m_player1->m_hasJustHeld,
                              self->m_player1->m_hasJustHeld2});

    checkpoints_p2.push_back({replay.get_frame(),
                              self->m_player2->m_position.x,
                              self->m_player2->m_position.y,
                              self->m_player2->getRotation(),
                              self->m_player2->m_playerSpeed,
                              self->m_player2->m_vehicleSize,
                              self->m_player2->m_xAccel,
                              self->m_player2->m_yAccel,
                              self->m_player2->m_jumpAccel,
                              self->m_player2->m_isUpsideDown,
                              self->m_player2->m_canRobotJump,
                              self->m_player2->m_isOnGround,
                              self->m_player2->m_isDashing,
                              self->m_player2->m_isSliding,
                              self->m_player2->m_isRising,
                              self->m_player2->black_orb,
                              self->m_player2->m_isHolding,
                              self->m_player2->m_isHolding2,
                              self->m_player2->m_hasJustHeld,
                              self->m_player2->m_hasJustHeld2});
}

bool PracticeFix::fix_respawn(gd::PlayLayer *self)
{
    if (checkpoints_p1.empty())
        return false;

    self->m_player1->m_position.x = checkpoints_p1.back().pos_x;
    self->m_player1->m_position.y = checkpoints_p1.back().pos_y;
    self->m_player1->setRotation(checkpoints_p1.back().rotation);
    self->m_player1->m_playerSpeed = checkpoints_p1.back().player_speed;
    self->m_player1->m_vehicleSize = checkpoints_p1.back().vehicle_size;
    self->m_player1->m_xAccel = checkpoints_p1.back().x_accel;
    self->m_player1->m_yAccel = checkpoints_p1.back().y_accel;
    self->m_player1->m_jumpAccel = checkpoints_p1.back().jump_accel;
    self->m_player1->m_isUpsideDown = checkpoints_p1.back().is_upsidedown;
    // self->m_player1->m_canRobotJump = checkpoints_p1.back().can_robot_jump;
    // self->m_player1->m_isOnGround = checkpoints_p1.back().is_on_ground;
    // self->m_player1->m_isDashing = checkpoints_p1.back().is_dashing;
    // self->m_player1->m_isSliding = checkpoints_p1.back().is_sliding;
    // self->m_player1->m_isRising = checkpoints_p1.back().is_rising;
    self->m_player1->black_orb = checkpoints_p1.back().black_orb;
    // self->m_player1->m_isHolding = checkpoints_p1.back().is_holding;
    // self->m_player1->m_isHolding2 = checkpoints_p1.back().is_holding2;
    // self->m_player1->m_hasJustHeld = checkpoints_p1.back().has_just_held;
    // self->m_player1->m_hasJustHeld2 = checkpoints_p1.back().has_just_held2;

    self->m_player2->m_position.x = checkpoints_p2.back().pos_x;
    self->m_player2->m_position.y = checkpoints_p2.back().pos_y;
    self->m_player2->setRotation(checkpoints_p2.back().rotation);
    self->m_player2->m_playerSpeed = checkpoints_p2.back().player_speed;
    self->m_player2->m_vehicleSize = checkpoints_p2.back().vehicle_size;
    self->m_player2->m_xAccel = checkpoints_p2.back().x_accel;
    self->m_player2->m_yAccel = checkpoints_p2.back().y_accel;
    self->m_player2->m_jumpAccel = checkpoints_p2.back().jump_accel;
    self->m_player2->m_isUpsideDown = checkpoints_p2.back().is_upsidedown;
    // self->m_player2->m_canRobotJump = checkpoints_p2.back().can_robot_jump;
    // self->m_player2->m_isOnGround = checkpoints_p2.back().is_on_ground;
    // self->m_player2->m_isDashing = checkpoints_p2.back().is_dashing;
    // self->m_player2->m_isSliding = checkpoints_p2.back().is_sliding;
    // self->m_player2->m_isRising = checkpoints_p2.back().is_rising;
    self->m_player2->black_orb = checkpoints_p2.back().black_orb;
    // self->m_player2->m_isHolding = checkpoints_p2.back().is_holding;
    // self->m_player2->m_isHolding2 = checkpoints_p2.back().is_holding2;
    // self->m_player2->m_hasJustHeld = checkpoints_p2.back().has_just_held;
    // self->m_player2->m_hasJustHeld2 = checkpoints_p2.back().has_just_held2;
    return true;
}

void PracticeFix::update_frame_offset()
{
    if (!checkpoints_p1.empty())
    {
        frame_offset = checkpoints_p1.back().frame;
    }
    else
    {
        frame_offset = 0;
    }
}

bool SpamBot::next_frame()
{
    if ((downed && ++release_current >= release) || (!downed && ++hold_current >= hold))
    {
        downed = !downed;
        (downed) ? release_current = 0 : hold_current = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void SpamBot::handle_spambot(gd::PlayLayer *self)
{
    if (enabled && next_frame())
    {
        if (downed)
        {
            if (player_p1)
            {
                hooks::playLayer_pushButtonHook(self, 0, 0, true);
            }
            if (player_p2)
            {
                hooks::playLayer_pushButtonHook(self, 0, 0, false);
            }
        }
        else
        {
            if (player_p1)
            {
                hooks::playLayer_releaseButtonHook(self, 0, 0, true);
            }
            if (player_p2)
            {
                hooks::playLayer_releaseButtonHook(self, 0, 0, false);
            }
        }
    }
}

void SpamBot::reset_temp()
{
    hold_current = 0;
    release_current = 0;
}

void StraightFly::handle_straightfly(gd::PlayLayer *self)
{
    if (!enabled)
        return;

    float y = self->m_player1->m_position.y;
    double accel = self->m_player1->m_yAccel;
    bool holding = self->m_player1->m_isHolding || self->m_player1->m_isHolding2;

    if (start_y == 0)
    {
        start(self);
    }

    if (self->m_player1->m_isUpsideDown)
    {
        float delta_y = y - start_y;
        y = start_y - delta_y;
        accel *= -1;
    }

    if (accel < 0 && y < start_y - accel - accuracy / 100 && !holding)
    {
        hooks::playLayer_pushButtonHook(self, 0, 0, true);
    }
    else if (accel > 0 && y > start_y - accel + accuracy / 100 && holding)
    {
        hooks::playLayer_releaseButtonHook(self, 0, 0, true);
    }
}

void StraightFly::start(gd::PlayLayer *self)
{
    start_y = self ? self->m_player1->m_position.y : 0.0f;
}