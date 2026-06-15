#include "coach.h"
#include "core/constants.h"
#include "entities/ball.h"
#include "entities/team.h"
#include "game/scene.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
// Set to false to let the other team use their own logic (if you implement it)
// Set to true to test your logic on both teams
bool coach_both_teams = true;

/* -------------------------------------------------------------------------
 * Logic Functions
 *  TODO 1: You must implement the following functions in Phase 2.
 *        Each player in each team has its own functions.
 *        You can add new functions, but are NOT ALLOWED to remove
 *        the existing functions or change their structure.
 * -------------------------------------------------------------------------
 * ⚠️ STUDENT RULES FOR PHASE 2:
 * You are restricted to modifying ONLY specific variables in each function:
 *
 * 1. MOVEMENT FUNCTIONS (movement_logic_X_Y):
 * Allowed: player->velocity
 * Goal:    Determine the direction and speed of movement.
 *
 * 2. SHOOTING FUNCTIONS (shooting_logic_X_Y):
 * Allowed: ball->velocity
 * Goal:    Determine the direction and power of the kick/pass.
 *
 * 3. CHANGE STATE FUNCTIONS (change_state_logic_X_Y):
 * Allowed: player->state
 * Goal:    Switch between IDLE, MOVING, SHOOTING, or INTERCEPTING.
 *
 * NOTE: Directly modifying any other attributes will be flagged as a violation.
 * Thank you for your attention to this matter!
 * ------------------------------------------------------------------------- */
int own_half(int team, struct Scene *scene)
{
    if (team == 1)
    {
        if (scene->ball->position.x < CENTER_X)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if (team == 2)
    {
        if (scene->ball->position.x > CENTER_X)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
int gk_half(int team, struct Scene *scene)
{
    if (team == 1)
    {
        if (scene->ball->position.x < PITCH_X + 140.0f)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    if (team == 2)
    {
        if (scene->ball->position.x > PITCH_X + PITCH_W - 140.0f)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
void outha(struct Player *self)
{
    float left = PITCH_X;
    float right = PITCH_X + PITCH_W;
    float up = PITCH_Y;
    float down = PITCH_Y + PITCH_H;
    if (self->position.x < left + PLAYER_RADIUS)
    {
        float a = ((rand() % 91) - 45) * (3.14 / 180.0f);

        self->position.x = left + PLAYER_RADIUS;
        self->velocity.x = -fabsf(cosf(a)) * (self->velocity.x) / 2;
    }
    if (self->position.x > right - PLAYER_RADIUS)
    {
        float a = ((rand() % 91) - 45) * (3.14 / 180.0f);
        self->position.x = right - PLAYER_RADIUS;
        self->velocity.x = -fabsf(cosf(a)) * (self->velocity.x) / 2;
    }
    if (self->position.y < up + PLAYER_RADIUS)
    {
        float a = ((rand() % 91) - 45) * (3.14 / 180.0f);
        self->position.y = up + PLAYER_RADIUS;
        self->velocity.y = -fabsf(cosf(a)) * (self->velocity.y) / 2;
    }
    if (self->position.y > down - PLAYER_RADIUS)
    {
        float a = ((rand() % 91) - 45) * (3.14 / 180.0f);
        self->position.y = down - PLAYER_RADIUS;
        self->velocity.y = -fabsf(cosf(a)) * (self->velocity.y) / 2;
    }
    if (fabsf(self->velocity.x) < 2.0f)
        self->velocity.x = 0;
    if (fabsf(self->velocity.y) < 2.0f)
        self->velocity.y = 0;
}
int maxvelocity(struct Player *self, struct Scene *scene)
{
    return MAX_PLAYER_VELOCITY * self->talents.agility / MAX_TALENT_PER_SKILL;
}
void tadakhol(struct Player *self, struct Scene *scene)
{
    /*for (int i = 0; i < 2; i++)
    {
        struct Team *team = (i == 0) ? scene->first_team : scene->second_team;

        for (int j = 0; j < 6; j++)
        {
            struct Player *p = team->players[j];

            if (p == self)
                continue;

            float dx = self->position.x - p->position.x;
            float dy = self->position.y - p->position.y;
            float d = hypotf(dx, dy);
            if (d > 0.0f && d < PLAYER_RADIUS * 2.0f)
            {
                float nx = dx / d;
                float ny = dy / d;
                self->position.x += nx * (PLAYER_RADIUS * 2.0f - d) * 0.3f;
                self->position.y += ny * (PLAYER_RADIUS * 2.0f - d) * 0.3f;
                self->velocity.x *= 0.3f;
                self->velocity.y *= 0.3f;
            }
        }
    }*/
}
int ballowner(struct Player *self, struct Scene *scene)
{
    if (self->team == 1)
    {
        int flag6 = 0;
        for (int i = 0; i < 6; i++)
        {
            if (scene->first_team->players[i] == self)
                continue;
            else
            {
                if (scene->ball->possessor == scene->first_team->players[i])
                {
                    flag6 = 1;
                    break;
                }
            }
        }
        if (flag6 == 1)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else if (self->team == 2)
    {
        int flag6 = 0;
        for (int i = 0; i < 6; i++)
        {
            if (scene->second_team->players[i] == self)
                continue;
            else
            {
                if (scene->ball->possessor == scene->second_team->players[i])
                {
                    flag6 = 1;
                    break;
                }
            }
        }
        if (flag6 == 1)
            return 1;
        else
            return 2;
    }
}
void pass(struct Ball *ball, struct Player *target, struct Player *self)
{
    float predict_x = target->position.x + target->velocity.x * 0.3f;
    float predict_y = target->position.y + target->velocity.y * 0.3f;

    float dx = predict_x - self->position.x;
    float dy = predict_y - self->position.y;

    float distance = hypotf(dx, dy);

    if (distance > 16.0f)
    {
        float speed = MAX_BALL_VELOCITY * self->talents.shooting / MAX_TALENT_PER_SKILL;
        ball->velocity.x = speed * (dx / distance);
        ball->velocity.y = speed * (dy / distance);
    }
    ball->last_team = self->team;
}
void kickoff(struct Player *self, struct Scene *scene)
{
    struct Ball *ball = scene->ball;
    if (ball->position.x == CENTER_X &&
        ball->position.y == CENTER_Y &&
        ball->velocity.x == 0.0f &&
        ball->velocity.y == 0.0f)
    {
        ball->velocity.x = (self->team == 1) ? -350.0f : 350.0f;
    }
}
int freesight(struct Player *self, struct Scene *scene)
{
    if (self->team == 1)
    {
        for (int i = 1; i < 6; i++)
        {
            int flag = 0;
            if (scene->first_team->players[i] == self)
                continue;
            else
            {
                for (int j = 0; j < 6; j++)
                {
                    if (self->position.x == scene->first_team->players[i]->position.x)
                        continue;
                    else
                    {
                        if (scene->first_team->players[i]->position.x > self->position.x)
                        {
                            float a = (scene->first_team->players[i]->position.y - self->position.y) / (scene->first_team->players[i]->position.x - self->position.x) * (scene->second_team->players[j]->position.x - self->position.x) - self->position.y;
                            if (a - BALL_RADIUS - 5.0f <= scene->second_team->players[j]->position.y && a + BALL_RADIUS + 5.0f >= scene->second_team->players[j]->position.y)
                            {
                                flag = 1;
                                break;
                            }
                        }
                    }
                }
                if (flag == 1)
                    continue;
                else if (flag == 0)
                {
                    pass(scene->ball, scene->first_team->players[i], self);
                    return 1;
                }
            }
        }
        return 2;
    }
    else if (self->team == 2)
    {
        for (int i = 1; i < 6; i++)
        {
            int flag = 0;
            if (scene->second_team->players[i] == self)
                continue;
            else
            {
                for (int j = 0; j < 6; j++)
                {
                    if (self->position.x == scene->second_team->players[i]->position.x)
                        continue;
                    else
                    {
                        if (self->position.x > scene->second_team->players[i]->position.x)
                        {
                            float a = (scene->second_team->players[i]->position.y - self->position.y) / (scene->second_team->players[i]->position.x - self->position.x) * (scene->first_team->players[j]->position.x - self->position.x) - self->position.y;
                            if (a - BALL_RADIUS - 5.0f <= scene->first_team->players[j]->position.y && a + BALL_RADIUS + 5.0f >= scene->first_team->players[j]->position.y)
                            {
                                flag = 1;
                                break;
                            }
                        }
                    }
                }
                if (flag == 1)
                    continue;
                else if (flag == 0)
                {
                    pass(scene->ball, scene->second_team->players[i], self);
                    return 1;
                }
            }
        }
        return 2;
    }
}
int freesight2(struct Player *self, struct Scene *scene, struct Player *target)
{
    if (self->team == 1)
    {
        for (int j = 0; j < 6; j++)
        {
            if (target->position.x > self->position.x)
            {
                float a = (target->position.y - self->position.y) / (target->position.x - self->position.x) * (scene->second_team->players[j]->position.x - self->position.x) - self->position.y;
                if (a - BALL_RADIUS - 5.0f <= scene->second_team->players[j]->position.y && a + BALL_RADIUS + 5.0f >= scene->second_team->players[j]->position.y)
                {
                    return 2;
                }
                else
                {
                    return 1;
                }
            }
        }
    }
    else if (self->team == 2)
    {

        for (int j = 0; j < 6; j++)
        {

            if (self->position.x > target->position.x)
            {
                float a = (target->position.y - self->position.y) / (target->position.x - self->position.x) * (scene->first_team->players[j]->position.x - self->position.x) - self->position.y;
                if (a - BALL_RADIUS - 5.0f <= scene->first_team->players[j]->position.y && a + BALL_RADIUS + 5.0f >= scene->first_team->players[j]->position.y)
                {
                    return 2;
                }
                else
                {
                    return 1;
                }
            }
        }
    }
}
int isout(struct Player *self, struct Scene *scene)
{
    int flag = 0;
    for (int i = 0; i < 6; i++)
    {
        if (self != scene->first_team->players[i] && scene->first_team->players[i]->position.x < PITCH_X || scene->first_team->players[i]->position.x > PITCH_X + PITCH_W || scene->first_team->players[i]->position.y < PITCH_Y || scene->first_team->players[i]->position.y > PITCH_Y + PITCH_H)
        {
            flag = 1;
            break;
        }
        else if (self != scene->second_team->players[i] && scene->second_team->players[i]->position.x < PITCH_X || scene->second_team->players[i]->position.x > PITCH_X + PITCH_W || scene->second_team->players[i]->position.y < PITCH_Y || scene->second_team->players[i]->position.y > PITCH_Y + PITCH_H)
        {
            flag = 1;
            break;
        }
    }
    if (flag == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
void out(struct Player *self, struct Scene *scene)
{
    struct Player *nazdik;
    struct Player *min;
    int j = 0;
    int flag = 0;
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W || self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        if (self->team == 1)
        {
            for (int i = 0; i < 6; i++)
            {
                min = scene->first_team->players[j];
                if (min == self)
                {
                    if (j == 5)
                    {
                        j = 0;
                    }
                    j++;
                    min = scene->first_team->players[j];
                }
                if (scene->first_team->players[i] == self)
                    continue;
                nazdik = scene->first_team->players[i];
                float dx = nazdik->position.x - self->position.x;
                float dy = nazdik->position.y - self->position.y;
                float d = hypotf(dx, dy);
                float d2x = min->position.x - self->position.x;
                float d2y = min->position.y - self->position.y;
                float d2 = hypotf(d2x, d2y);
                if (d < d2)
                {
                    min = nazdik;
                    flag = 1;
                    pass(scene->ball, min, self);
                    break;
                }
            }
            if (flag == 0)
            {

                if (scene->ball->position.y < CENTER_Y)
                {
                    scene->ball->velocity.y = maxvelocity(self, scene);
                }
                else if (scene->ball->position.y > CENTER_Y)
                {
                    scene->ball->velocity.y = -maxvelocity(self, scene);
                }
                // pass(scene->ball, scene->first_team->players[3], self);
            }
        }
        else if (self->team == 2)
        {
            for (int i = 0; i < 6; i++)
            {
                min = scene->second_team->players[j];
                if (min == self)
                {
                    if (j == 5)
                    {
                        j = 0;
                    }
                    j++;
                    min = scene->second_team->players[j];
                }
                if (scene->second_team->players[i] == self)
                    continue;
                nazdik = scene->second_team->players[i];
                float dx = nazdik->position.x - self->position.x;
                float dy = nazdik->position.y - self->position.y;
                float d = hypotf(dx, dy);
                float d2x = min->position.x - self->position.x;
                float d2y = min->position.y - self->position.y;
                float d2 = hypotf(d2x, d2y);
                if (d < d2)
                {
                    min = nazdik;
                    flag = 1;
                    pass(scene->ball, min, self);
                    break;
                }
            }
            if (flag == 0)
            {
                if (scene->ball->position.y < CENTER_Y)
                {
                    scene->ball->velocity.y = maxvelocity(self, scene);
                }
                else if (scene->ball->position.y > CENTER_Y)
                {
                    scene->ball->velocity.y = -maxvelocity(self, scene);
                }
                // pass(scene->ball, scene->second_team->players[3], self);
            }
        }
    }
}
static bool is_nearest_to_ball(struct Player *self, struct Scene *scene)
{
    struct Team *my_team = (self->team == 1) ? scene->first_team : scene->second_team;
    float my_dist = hypotf(self->position.x - scene->ball->position.x,
                           self->position.y - scene->ball->position.y);

    for (int i = 0; i < 6; i++)
    {
        struct Player *teammate = my_team->players[i];
        if (teammate == self)
            continue;

        float d = hypotf(teammate->position.x - scene->ball->position.x,
                         teammate->position.y - scene->ball->position.y);
        if (d < my_dist)
            return false;
    }
    return true;
}
void movement_logic_gk(struct Player *self, struct Scene *scene)
{
    outha(self);
    float tirbala = CENTER_Y - GOAL_HEIGHT / 2;
    float tirpayin = CENTER_Y + GOAL_HEIGHT / 2;
    float dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
    float dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
    float d2 = hypotf(dx, dy);
    if (scene->ball->possessor == self)
    {
        if (self->team == 1)
        {
            if (self->position.x > 100.0f)
            {
                self->velocity.x = -maxvelocity(self, scene);
            }
            else
                self->velocity.x = 0;
        }
        else if (self->team == 2)
        {
            if (self->position.x < 940.0f)
            {
                self->velocity.x = maxvelocity(self, scene);
            }
            else
                self->velocity.x = 0;
        }

        if (self->position.y > tirpayin)
        {
            self->velocity.y = -maxvelocity(self, scene);
        }
        else if (self->position.y < tirbala)
        {
            self->velocity.y = maxvelocity(self, scene);
        }
        else
        {
            self->velocity.y = 0;
        }
    }
    else if (scene->ball->possessor != self)
    {
        if (own_half(self->team, scene))
        {
            if (scene->ball->position.y < tirbala)
            {
                if (self->position.y < tirbala)
                {
                    self->velocity.y = maxvelocity(self, scene);
                }
                else if (self->position.y > tirbala)
                {
                    self->velocity.y = -maxvelocity(self, scene);
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            else if (scene->ball->position.y > tirpayin)
            {
                if (self->position.y > tirpayin)
                {
                    self->velocity.y = -maxvelocity(self, scene);
                }
                else if (self->position.y < tirpayin)
                {
                    self->velocity.y = maxvelocity(self, scene);
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            else
            {
                if (self->position.y < scene->ball->position.y)
                {
                    self->velocity.y = maxvelocity(self, scene);
                }
                else if (self->position.y > scene->ball->position.y)
                {
                    self->velocity.y = -maxvelocity(self, scene);
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            if (self->team == 1)
            {
                if (self->position.x < 60.0f)
                {
                    self->velocity.x = maxvelocity(self, scene) / 2.0f;
                }
                if (self->position.x > 100.0f)
                {
                    self->velocity.x = -maxvelocity(self, scene) / 2.0f;
                }
                if (self->position.x < 100.0f && self->position.x > 60.0f && scene->ball->position.x < 270.0f && scene->ball->position.x - 100.0f > self->position.x)
                {
                    self->velocity.x = maxvelocity(self, scene);
                }
                else if (self->position.x < 100.0f && self->position.x > 60.0f && scene->ball->position.x < 270.0f && scene->ball->position.x - 100.0f <= self->position.x)
                {
                    if (d2 < 80.0f)
                    {
                        self->velocity.x = maxvelocity(self, scene) * (dx / d2);
                    }
                    else
                        self->velocity.x = -maxvelocity(self, scene);
                }
                else if (self->position.x < 100.0f && self->position.x > 60.0f && scene->ball->position.x >= 270.0f)
                {
                    self->velocity.x = -maxvelocity(self, scene);
                }
            }
            else if (self->team == 2)
            {
                if (self->position.x < 900.0f)
                {
                    self->velocity.x = maxvelocity(self, scene) / 2.0f;
                }
                if (self->position.x > 940.0f)
                {
                    self->velocity.x = -maxvelocity(self, scene) / 2.0f;
                }
                if (self->position.x < 940.0f && self->position.x > 900.0f && scene->ball->position.x < 730.0f)
                {
                    self->velocity.x = maxvelocity(self, scene);
                }
                else if (self->position.x < 940.0f && self->position.x > 900.0f && scene->ball->position.x >= 730.0f && scene->ball->position.x + 100.0f < self->position.x)
                {
                    self->velocity.x = -maxvelocity(self, scene);
                }
                else if (self->position.x < 940.0f && self->position.x > 900.0f && scene->ball->position.x >= 730.0f && scene->ball->position.x + 100.f >= self->position.x)
                {
                    if (d2 < 80.0f)
                    {
                        self->velocity.x = maxvelocity(self, scene) * (dx / d2);
                    }
                    else
                        self->velocity.x = maxvelocity(self, scene);
                }
            }
        }
    }
}
void movement_logic_def(struct Player *self, struct Scene *scene)
{
    outha(self);
    float goal_x = ((self->team == 1) ? PITCH_X + PITCH_W : PITCH_X);
    float upgoal = CENTER_Y - GOAL_WIDTH / 2.0f - 20.0f;
    float downgoal = CENTER_Y + GOAL_WIDTH / 2.0f + 20.0f;
    float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
    float dx = goal_x - self->position.x;
    float dy = goal_y - self->position.y;
    float d = hypotf(dx, dy);
    if (self->team == 1)
    {
        if (scene->ball->possessor == self)
        {
            self->velocity.x = maxvelocity(self, scene) * (dx / d);
            self->velocity.y = maxvelocity(self, scene) * (dy / d);
        }
        else if (scene->ball->possessor != self)
        {
            if (scene->ball->possessor == NULL)
            {
                dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                float d2 = hypotf(dx, dy);
                float ball_speed = hypotf(scene->ball->velocity.x, scene->ball->velocity.y);
                if (d2 < 50.0f && ball_speed < 200.0f)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d2;
                    self->velocity.y = maxvelocity(self, scene) * dy / d2;
                }
            }
            if (self->position.x > CENTER_X)
            {
                self->velocity.x = -maxvelocity(self, scene);
            }
            if (scene->ball->position.x > CENTER_X && self->position.x <= CENTER_X)
            {
                self->velocity.x = 0;
                if ((CENTER_Y + scene->ball->position.y) / 2 < self->position.y)
                {
                    self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                }
                else if ((CENTER_Y + scene->ball->position.y) / 2 > self->position.y)
                {
                    self->velocity.y = maxvelocity(self, scene) / 2.0f;
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            else if (scene->ball->position.x < CENTER_X)
            {
                if (scene->ball->possessor == scene->first_team->players[3])
                {
                    self->velocity.x = maxvelocity(self, scene);
                    self->velocity.y = 0;
                }
                else if (scene->ball->possessor != scene->first_team->players[3])
                {
                    if (self->position.x < ((scene->ball->position.x + PITCH_X) / 2))
                    {
                        self->velocity.x = maxvelocity(self, scene);
                    }
                    else if (self->position.x > ((scene->ball->position.x + PITCH_X) / 2))
                    {
                        self->velocity.x = -maxvelocity(self, scene);
                    }
                    else
                    {
                        self->velocity.x = 0;
                    }
                    if ((CENTER_Y + scene->ball->position.y) / 2 < self->position.y)
                    {
                        self->velocity.y = -maxvelocity(self, scene);
                    }
                    else if ((CENTER_Y + scene->ball->position.y) / 2 > self->position.y)
                    {
                        self->velocity.y = maxvelocity(self, scene);
                    }
                    else
                    {
                        self->velocity.y = 0;
                    }
                }
            }
        }
    }
    else if (self->team == 2)
    {
        if (scene->ball->possessor == self)
        {
            self->velocity.x = maxvelocity(self, scene) * (dx / d);
            self->velocity.y = maxvelocity(self, scene) * (dy / d);
        }
        else
        {
            if (scene->ball->possessor == NULL)
            {
                dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                float d2 = hypotf(dx, dy);
                if (d2 < 60.0f)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d2;
                    self->velocity.y = maxvelocity(self, scene) * dy / d2;
                }
            }
            if (self->position.x < CENTER_X)
            {
                self->velocity.x = maxvelocity(self, scene);
            }
            else if (scene->ball->position.x < CENTER_X && self->position.x >= CENTER_X)
            {
                if ((CENTER_Y + scene->ball->position.y) / 2 < self->position.y)
                {
                    self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                }
                else if ((CENTER_Y + scene->ball->position.y) / 2 > self->position.y)
                {
                    self->velocity.y = maxvelocity(self, scene) / 2.0f;
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            else if (scene->ball->position.x > CENTER_X)
            {
                if (scene->ball->possessor == scene->second_team->players[3])
                {
                    self->velocity.x = -maxvelocity(self, scene);
                    self->velocity.y = 0;
                }
                else if (scene->ball->possessor != scene->second_team->players[3])
                {
                    if (self->position.x > (PITCH_X + PITCH_W + scene->ball->position.x) / 2)
                    {
                        self->velocity.x = -maxvelocity(self, scene);
                    }
                    else if (self->position.x < (PITCH_X + PITCH_W + scene->ball->position.x) / 2)
                    {
                        self->velocity.x = maxvelocity(self, scene);
                    }
                    else
                    {
                        self->velocity.x = 0;
                    }
                    if ((CENTER_Y + scene->ball->position.y) / 2 < self->position.y)
                    {
                        self->velocity.y = -maxvelocity(self, scene);
                    }
                    else if ((CENTER_Y + scene->ball->position.y) / 2 > self->position.y)
                    {
                        self->velocity.y = maxvelocity(self, scene);
                    }
                    else
                    {
                        self->velocity.y = 0;
                    }
                }
            }
        }
    }
}
void movement_logic_mid(struct Player *self, struct Scene *scene)
{
    outha(self);
    tadakhol(self, scene);
    float goal_x = ((self->team == 1) ? PITCH_X + PITCH_W : PITCH_X);
    float upgoal = CENTER_Y - GOAL_WIDTH / 2.0f;
    float downgoal = CENTER_Y + GOAL_WIDTH / 2.0f;
    float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
    float dx = goal_x - self->position.x;
    float dy = goal_y - self->position.y;
    float d = hypotf(dx, dy);
    float d2x = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
    float d2y = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
    float d2 = hypotf(d2x, d2y);
    struct Team *my_team;
    if (self->team == 1)
    {
        my_team = scene->first_team;
    }
    else
    {
        my_team = scene->second_team;
    }
    int flag = 0;
    for (int i = 0; i < 6; i++)
    {
        flag = 0;
        if (my_team->players[i] == scene->ball->possessor && scene->ball->possessor != self)
        {
            flag = 1;
            break;
        }
    }
    if (flag == 1)
    {
        if (self->team == 1)
        {
            if (scene->ball->possessor == scene->first_team->players[3])
            {
                if (self->position.x > CENTER_X - 200.0f)
                    self->velocity.x = -maxvelocity(self, scene) / 3.0f;
                else if (self->position.x < CENTER_X - 200.0f && self->position.x > CENTER_X - 400.0f)
                    self->velocity.x = -maxvelocity(self, scene) / 9.0f;
                else
                    self->velocity.x = 0.0f;
                float d3 = scene->first_team->players[2]->position.y - scene->first_team->players[4]->position.y;
                if (abs(d3) < 250.0f)
                {
                    if (self == scene->first_team->players[2])
                    {
                        self->velocity.y = maxvelocity(self, scene) / 2.0f;
                    }
                    else
                    {
                        self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                    }
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            else
            {
                if (self->position.x < 150.0f)
                {
                    self->velocity.x = maxvelocity(self, scene) / 2.0f;
                }
                if (self->position.x > CENTER_X && scene->ball->position.x < CENTER_X)
                {
                    self->velocity.x = -maxvelocity(self, scene);
                }
                if (self->position.x < CENTER_X && scene->ball->position.x > CENTER_X && self->position.x > PITCH_X + 200.0f)
                {
                    self->velocity.x = -maxvelocity(self, scene) / 3.0f;
                }
                if (self->position.x < CENTER_X && scene->ball->position.x > CENTER_X && self->position.x <= PITCH_X + 200.0f)
                {
                    self->velocity.x = 0.0f;
                }
                float d3 = scene->first_team->players[2]->position.y - scene->first_team->players[4]->position.y;
                if (abs(d3) < 250.0f)
                {
                    if (self == scene->first_team->players[2])
                    {
                        self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                    }
                    else
                    {
                        self->velocity.y = maxvelocity(self, scene) / 2.0f;
                    }
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
        }
        else if (self->team == 2)
        {
            if (scene->ball->possessor == scene->second_team->players[3])
            {
                if (self->position.x < CENTER_X + 200.0f)
                    self->velocity.x = -maxvelocity(self, scene) / 3.0f;
                else if (self->position.x > CENTER_X + 200.0f && self->position.x < CENTER_X + 400.0f)
                    self->velocity.x = -maxvelocity(self, scene) / 9.0f;
                else
                    self->velocity.x = 0.0f;
                float d3 = scene->second_team->players[2]->position.y - scene->second_team->players[4]->position.y;
                if (abs(d3) < 250.0f)
                {
                    if (self == scene->second_team->players[2])
                    {
                        self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                    }
                    else
                    {
                        self->velocity.y = maxvelocity(self, scene) / 2.0f;
                    }
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
            else
            {
                if (self->position.x > PITCH_X + PITCH_W - 160.0f)
                {
                    self->velocity.x = -maxvelocity(self, scene) / 2.0f;
                }
                if (self->position.x < CENTER_X && scene->ball->position.x > CENTER_X)
                {
                    self->velocity.x = maxvelocity(self, scene);
                }
                if (self->position.x > CENTER_X && scene->ball->position.x < CENTER_X && self->position.x < PITCH_X + PITCH_W - 200.0f)
                {
                    self->velocity.x = maxvelocity(self, scene) / 3.0f;
                }
                if (self->position.x > CENTER_X && scene->ball->position.x < CENTER_X && self->position.x >= PITCH_X + PITCH_W - 200.0f)
                {
                    self->velocity.x = 0.0f;
                }
                float d3 = scene->second_team->players[2]->position.y - scene->second_team->players[4]->position.y;
                if (abs(d3) < 250.0f)
                {
                    if (self == scene->second_team->players[2])
                    {
                        self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                    }
                    else
                    {
                        self->velocity.y = maxvelocity(self, scene) / 2.0f;
                    }
                }
                else
                {
                    self->velocity.y = 0;
                }
            }
        }
    }
    else
    {
        if (self->team == 1)
        {
            if (self->position.x > CENTER_X && self->position.x < CENTER_X + 200.0f)
            {
                if (scene->ball->possessor == self)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else
                {
                    self->velocity.x = -maxvelocity(self, scene);
                    self->velocity.y = 0;
                }
            }
            else if (self->position.x <= CENTER_X)
            {
                if (scene->ball->possessor == self)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else
                {
                    if (d2 >= 200.0f)
                    {
                        self->velocity.x = 0;
                        float d3 = scene->first_team->players[2]->position.y - scene->first_team->players[4]->position.y;
                        if (abs(d3) < 250.0f)
                        {
                            if (self == scene->first_team->players[2])
                            {
                                self->velocity.y = maxvelocity(self, scene) / 2.0f;
                            }
                            else
                            {
                                self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                            }
                        }
                        else
                        {
                            self->velocity.y = 0;
                        }
                    }
                    else if (d2 < 200.0f)
                    {
                        float ball_speed = hypotf(scene->ball->velocity.x, scene->ball->velocity.y);
                        if (!(ball_speed > 250.0f && d2 < 40.0f))
                        {
                            dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                            dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                            self->velocity.x = maxvelocity(self, scene) * dx / d2;
                            self->velocity.y = maxvelocity(self, scene) * dy / d2;
                        }
                        if (self->position.x < 160.0f && self->position.x > scene->ball->position.x)
                        {
                            self->velocity.x = 0.0f;
                            if (self->position.y < CENTER_Y - 40.0f)
                                self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                            else if (self->position.y > CENTER_Y + 40.0f)
                                self->velocity.y = maxvelocity(self, scene) / 2.0f;
                            else
                                self->velocity.y = 0;
                        }
                        else
                        {
                            dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                            dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                            self->velocity.x = maxvelocity(self, scene) * dx / d2;
                            self->velocity.y = maxvelocity(self, scene) * dy / d2;
                        }
                    }
                }
            }
        }
        else if (self->team == 2)
        {
            if (self->position.x < CENTER_X && self->position.x > CENTER_X - 200.0f)
            {
                if (scene->ball->possessor == self)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else
                {
                    self->velocity.x = maxvelocity(self, scene);
                    self->velocity.y = 0;
                }
            }
            else if (self->position.x >= CENTER_X)
            {
                if (scene->ball->possessor == self)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else
                {
                    if (d2 >= 200.0f)
                    {
                        self->velocity.x = 0;
                        float d3 = scene->second_team->players[2]->position.y - scene->second_team->players[4]->position.y;
                        if (abs(d3) < 250.0f)
                        {
                            if (self == scene->second_team->players[2])
                            {
                                self->velocity.y = maxvelocity(self, scene) / 2.0f;
                            }
                            else
                            {
                                self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                            }
                        }
                        else
                        {
                            self->velocity.y = 0;
                        }
                    }
                    else if (d2 < 200.0f)
                    {
                        float ball_speed = hypotf(scene->ball->velocity.x, scene->ball->velocity.y);
                        if (!(ball_speed > 250.0f && d2 < 40.0f))
                        {
                            dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                            dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                            self->velocity.x = maxvelocity(self, scene) * dx / d2;
                            self->velocity.y = maxvelocity(self, scene) * dy / d2;
                        }
                        if (self->position.x > PITCH_X + PITCH_W - 120.0f && self->position.x < scene->ball->position.x)
                        {
                            self->velocity.x = -maxvelocity(self, scene);
                            if (self->position.y < CENTER_Y - 40.0f)
                                self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                            else if (self->position.y > CENTER_Y + 40.0f)
                                self->velocity.y = maxvelocity(self, scene) / 2.0f;
                            else
                                self->velocity.y = 0;
                        }
                        else
                        {
                            dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                            dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                            self->velocity.x = maxvelocity(self, scene) * dx / d2;
                            self->velocity.y = maxvelocity(self, scene) * dy / d2;
                        }
                    }
                }
            }
        }
    }
}
void movement_logic_att(struct Player *self, struct Scene *scene)
{
    outha(self);
    tadakhol(self, scene);
    float goal_x = ((self->team == 1) ? PITCH_X + PITCH_W : PITCH_X);
    float upgoal = CENTER_Y - GOAL_WIDTH / 2.0f - 20.0f;
    float downgoal = CENTER_Y + GOAL_WIDTH / 2.0f + 20.0f;
    float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
    float dx = goal_x - self->position.x;
    float dy = goal_y - self->position.y;
    float d = hypotf(dx, dy);
    float d2x = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
    float d2y = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
    float d2 = hypotf(d2x, d2y);
    if (self->team == 1)
    {
        if (scene->ball->possessor == self)
        {

            if (self->position.x < CENTER_X && self->position.x > PITCH_X)
            {
                self->velocity.x = maxvelocity(self, scene) * dx / d;
                self->velocity.y = maxvelocity(self, scene) * dy / d;
            }
            else if (self->position.x >= CENTER_X)
            {
                if (self == scene->first_team->players[1] && scene->first_team->players[5]->position.x <= self->position.x)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else if (self == scene->first_team->players[5] && scene->first_team->players[1]->position.x <= self->position.x)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
            }
            else
            {
                self->velocity.x = maxvelocity(self, scene) * dx / d;
                self->velocity.y = maxvelocity(self, scene) * dy / d;
            }
        }
        else if (scene->ball->possessor != self)
        {
            if (self->position.x < CENTER_X && ballowner(self, scene) == 2)
            {
                if (d2 < 200.0f)
                {
                    float ball_speed = hypotf(scene->ball->velocity.x, scene->ball->velocity.y);
                    if (ball_speed < 300.0f || d2 > 50.0f)
                    {
                        dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                        dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                        self->velocity.x = maxvelocity(self, scene) * dx / d2;
                        self->velocity.y = maxvelocity(self, scene) * dy / d2;
                    }
                }
                else if (d2 >= 200.0f)
                {
                    dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                    dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                    self->velocity.y = 0;
                    self->velocity.x = maxvelocity(self, scene) * (dx / d2) / 2.0f;
                }
            }
            else if (self->position.x < CENTER_X && ballowner(self, scene) == 1)
            {
                if (scene->ball->possessor == scene->first_team->players[3])
                {
                    if (self->position.x < CENTER_X - 200.0f)
                    {
                        self->velocity.x = maxvelocity(self, scene);
                    }
                    else if (self->position.x >= CENTER_X - 200.0f && (self->position.x < CENTER_X))
                    {
                        self->velocity.x = maxvelocity(self, scene) / 1.5f;
                    }
                    else
                    {
                        self->velocity.x = -maxvelocity(self, scene) / 3.0f;
                    }
                    float d3 = scene->first_team->players[1]->position.y - scene->first_team->players[5]->position.y;
                    if (self == scene->first_team->players[5] && abs(d3) < 250.0f)
                    {
                        self->velocity.y = maxvelocity(self, scene) / 2.0f;
                    }
                    else if (self == scene->first_team->players[1] && abs(d3) < 250.0f)
                    {
                        self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                    }
                    else
                    {
                        self->velocity.y = 0.0f;
                    }
                }
                else
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
            }
            else if (self->position.x >= CENTER_X && ballowner(self, scene) == 2)
            {
                if (d2 < 200.0f && scene->ball->possessor != scene->second_team->players[3])
                {
                    dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                    dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                    self->velocity.x = maxvelocity(self, scene) * dx / d2;
                    self->velocity.y = maxvelocity(self, scene) * dy / d2;
                }
                else if (d2 < 200.0f && scene->ball->possessor == scene->second_team->players[3])
                {
                    self->velocity.x = -maxvelocity(self, scene);
                    if (self == scene->first_team->players[1])
                    {
                        self->velocity.y = -maxvelocity(self, scene);
                    }
                    else
                    {
                        self->velocity.y = maxvelocity(self, scene);
                    }
                }
                else if (d2 >= 200.0f && scene->ball->possessor != scene->second_team->players[3])
                {
                    dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                    dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                    self->velocity.y = 0;
                    self->velocity.x = maxvelocity(self, scene) * (dx / d2) / 2.0f;
                }
                else if (d2 >= 200.0f && scene->ball->possessor == scene->second_team->players[3])
                {
                    self->velocity.y = 0;
                    self->velocity.x = -maxvelocity(self, scene);
                }
            }
            else if (self->position.x >= CENTER_X && ballowner(self, scene) == 1)
            {
                self->velocity.x = maxvelocity(self, scene) * dx / d;
                if (self->position.y < CENTER_Y)
                {
                    self->velocity.y = -maxvelocity(self, scene) / 10.0f;
                }
                else
                {
                    self->velocity.y = maxvelocity(self, scene) / 10.0f;
                }
            }
        }
        else
        {
            self->velocity.x = maxvelocity(self, scene) * dx / d;
            self->velocity.y = maxvelocity(self, scene) * dy / d;
        }
    }
    else if (self->team == 2)
    {
        if (scene->ball->possessor == self)
        {
            if (self->position.x > CENTER_X && self->position.x < PITCH_X + PITCH_W)
            {
                self->velocity.x = maxvelocity(self, scene) * dx / d;
                self->velocity.y = maxvelocity(self, scene) * dy / d;
            }
            else if (self->position.x <= CENTER_X)
            {
                if (self == scene->second_team->players[1] && scene->second_team->players[5]->position.x >= self->position.x)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else if (self == scene->second_team->players[5] && scene->second_team->players[1]->position.x >= self->position.x)
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
                else
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
            }
            else
            {
                self->velocity.x = maxvelocity(self, scene) * dx / d;
                self->velocity.y = maxvelocity(self, scene) * dy / d;
            }
        }
        else if (scene->ball->possessor != self)
        {
            if (self->position.x > CENTER_X && ballowner(self, scene) == 2)
            {
                if (d2 < 200.0f)
                {
                    float ball_speed = hypotf(scene->ball->velocity.x, scene->ball->velocity.y);
                    if (ball_speed < 300.0f || d2 > 50.0f)
                    {
                        dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                        dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                        self->velocity.x = maxvelocity(self, scene) * dx / d2;
                        self->velocity.y = maxvelocity(self, scene) * dy / d2;
                    }
                }
                else if (d2 >= 200.0f)
                {
                    dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                    dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                    self->velocity.y = 0;
                    self->velocity.x = maxvelocity(self, scene) * (dx / d2) / 2.0f;
                }
            }
            else if (self->position.x > CENTER_X && ballowner(self, scene) == 1)
            {
                if (scene->ball->possessor == scene->second_team->players[3])
                {
                    if (self->position.x > CENTER_X + 200.0f)
                    {
                        self->velocity.x = -maxvelocity(self, scene);
                    }
                    else if (self->position.x <= CENTER_X + 200.0f && (self->position.x > CENTER_X))
                    {
                        self->velocity.x = -maxvelocity(self, scene) / 1.5f;
                    }
                    else
                    {
                        self->velocity.x = maxvelocity(self, scene) / 3.0f;
                    }
                    float d3 = scene->second_team->players[1]->position.y - scene->second_team->players[5]->position.y;

                    if (self == scene->second_team->players[5] && abs(d3) < 250.0f)
                    {
                        self->velocity.y = maxvelocity(self, scene) / 2.0f;
                    }
                    else if (self == scene->second_team->players[1] && abs(d3) < 250.0f)
                    {
                        self->velocity.y = -maxvelocity(self, scene) / 2.0f;
                    }
                    else
                    {
                        self->velocity.y = 0.0f;
                    }
                }
                else
                {
                    self->velocity.x = maxvelocity(self, scene) * dx / d;
                    self->velocity.y = maxvelocity(self, scene) * dy / d;
                }
            }
            else if (self->position.x <= CENTER_X && ballowner(self, scene) == 2)
            {
                if (d2 < 200.0f && scene->ball->possessor != scene->first_team->players[3])
                {
                    dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                    dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                    self->velocity.x = maxvelocity(self, scene) * dx / d2;
                    self->velocity.y = maxvelocity(self, scene) * dy / d2;
                }
                else if (d2 < 200.0f && scene->ball->possessor == scene->first_team->players[3])
                {
                    self->velocity.x = maxvelocity(self, scene);
                    if (self == scene->second_team->players[1])
                    {
                        self->velocity.y = -maxvelocity(self, scene);
                    }
                    else
                    {
                        self->velocity.y = maxvelocity(self, scene);
                    }
                }
                else if (d2 >= 200.0f && scene->ball->possessor != scene->first_team->players[3])
                {
                    dx = scene->ball->position.x - self->position.x + scene->ball->velocity.x * 0.3f;
                    dy = scene->ball->position.y - self->position.y + scene->ball->velocity.y * 0.3f;
                    self->velocity.y = 0;
                    self->velocity.x = maxvelocity(self, scene) * (dx / d2) / 2.0f;
                }
                else if (d2 >= 200.0f && scene->ball->possessor == scene->first_team->players[3])
                {
                    self->velocity.y = 0;
                    self->velocity.x = maxvelocity(self, scene);
                }
            }
            else if (self->position.x <= CENTER_X && ballowner(self, scene) == 1)
            {
                self->velocity.x = maxvelocity(self, scene) * dx / d;
                if (self->position.y < CENTER_Y)
                {
                    self->velocity.y = -maxvelocity(self, scene) / 10.0f;
                }
                else
                {
                    self->velocity.y = maxvelocity(self, scene) / 10.0f;
                }
            }
        }
        else
        {
            self->velocity.x = maxvelocity(self, scene) * dx / d;
            self->velocity.y = maxvelocity(self, scene) * dy / d;
        }
    }
}
void shooting_logic_mid(struct Player *self, struct Scene *scene)
{
    kickoff(self, scene);
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W ||
        self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        out(self, scene);
    }
    else
    {
        float goal_x;
        if (self->team == 1)
        {
            goal_x = PITCH_X + PITCH_W;
            if (self->position.x <= CENTER_X)
            {
                if (scene->first_team->players[1]->position.x > self->position.x)
                {
                    pass(scene->ball, scene->first_team->players[1], self);
                }
                else if (scene->first_team->players[5]->position.x > self->position.x)
                {
                    pass(scene->ball, scene->first_team->players[5], self);
                }
                else
                {
                    if (self == scene->first_team->players[2])
                        pass(scene->ball, scene->first_team->players[4], self);
                    else
                        pass(scene->ball, scene->first_team->players[2], self);
                }
            }
            else if (self->position.x > CENTER_X && self->position.x < CENTER_X + 200.0f)
            {
                if (scene->first_team->players[1]->position.x > self->position.x)
                {
                    pass(scene->ball, scene->first_team->players[1], self);
                }
                else if (scene->first_team->players[5]->position.x > self->position.x)
                {
                    pass(scene->ball, scene->first_team->players[5], self);
                }
            }
            else
            {
                float upgoal = CENTER_Y - GOAL_WIDTH / 2.0f;
                float downgoal = CENTER_Y + GOAL_WIDTH / 2.0f;
                float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
                float dx = goal_x - self->position.x;
                float dy = goal_y - self->position.y;
                float d = hypotf(dx, dy);
                scene->ball->velocity.x = MAX_BALL_VELOCITY * (dx / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
                scene->ball->velocity.y = MAX_BALL_VELOCITY * (dy / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
            }
        }
        else if (self->team == 2)
        {
            goal_x = PITCH_X;
            if (self->position.x >= CENTER_X)
            {
                if (scene->second_team->players[1]->position.x < self->position.x)
                {
                    pass(scene->ball, scene->second_team->players[1], self);
                }
                else if (scene->second_team->players[5]->position.x < self->position.x)
                {
                    pass(scene->ball, scene->second_team->players[5], self);
                }
                else
                {
                    if (self == scene->second_team->players[2])
                        pass(scene->ball, scene->second_team->players[4], self);
                    else
                        pass(scene->ball, scene->second_team->players[2], self);
                }
            }
            else if (self->position.x < CENTER_X && self->position.x > CENTER_X - 200.0f)
            {
                if (scene->second_team->players[1]->position.x < self->position.x)
                {
                    pass(scene->ball, scene->second_team->players[1], self);
                }
                else if (scene->second_team->players[5]->position.x < self->position.x)
                {
                    pass(scene->ball, scene->second_team->players[5], self);
                }
            }
            else
            {
                float upgoal = CENTER_Y - GOAL_WIDTH / 2.0f;
                float downgoal = CENTER_Y + GOAL_WIDTH / 2.0f;
                float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
                float dx = goal_x - self->position.x;
                float dy = goal_y - self->position.y;
                float d = hypotf(dx, dy);
                scene->ball->velocity.x = MAX_BALL_VELOCITY * (dx / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
                scene->ball->velocity.y = MAX_BALL_VELOCITY * (dy / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
            }
        }
    }
}
void shooting_logic_gk(struct Player *self, struct Scene *scene)
{
    kickoff(self, scene);
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W ||
        self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        out(self, scene);
    }
    else
    {
        int randomdef = 0;
        randomdef = rand() % 2;
        int j = 0;
        if (randomdef == 0)
            j = 2;
        else
            j = 4;
        if (self->team == 1)
        {
            pass(scene->ball, scene->first_team->players[j], self);
        }
        if (self->team == 2)
        {
            pass(scene->ball, scene->second_team->players[j], self);
        }
    }
}
void shooting_logic_def(struct Player *self, struct Scene *scene)
{
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W ||
        self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        out(self, scene);
    }
    else
    {
        if (hypotf(self->position.x - CENTER_X, 1) < 10.0f)
        {
            if (self->team == 1)
            {
                pass(scene->ball, scene->first_team->players[5], self);
            }
            else if (self->team == 2)
            {
                pass(scene->ball, scene->second_team->players[1], self);
            }
        }
        else
        {
            shooting_logic_gk(self, scene);
        }
    }
}
void shooting_logic_att(struct Player *self, struct Scene *scene)
{
    kickoff(self, scene);
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W ||
        self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        out(self, scene);
    }
    else
    {
        if (scene->ball->possessor == self)
        {
            if (scene->ball->position.x == CENTER_X && scene->ball->position.y == CENTER_Y)
            {
                shooting_logic_gk(self, scene);
                return;
            }
            float goal_x;
            if (self->team == 1)
            {
                goal_x = PITCH_X + PITCH_W;
                if (scene->ball->position.x < CENTER_X)
                {
                    if (self == scene->first_team->players[1] && scene->first_team->players[5]->position.x >= self->position.x)
                    {
                        pass(scene->ball, scene->first_team->players[5], self);
                    }
                    else if (self == scene->first_team->players[5] && scene->first_team->players[1]->position.x >= self->position.x)
                    {
                        pass(scene->ball, scene->first_team->players[1], self);
                    }
                    else
                    {
                        for (int i = 0; i < 6; i++)
                        {
                            if (scene->first_team->players[i]->position.x > self->position.x)
                            {
                                pass(scene->ball, scene->first_team->players[i], self);
                                break;
                            }
                        }
                    }
                }
                else if (self->position.x >= CENTER_X && scene->ball->position.x < PITCH_X + PITCH_W - 200.0f)
                {
                    if (self == scene->first_team->players[1] && scene->first_team->players[5]->position.x > self->position.x)
                    {
                        pass(scene->ball, scene->first_team->players[5], self);
                    }
                    else if (self == scene->first_team->players[5] && scene->first_team->players[1]->position.x > self->position.x)
                    {
                        pass(scene->ball, scene->first_team->players[1], self);
                    }
                }
                else
                {
                    float upgoal;
                    float downgoal;
                    if (scene->second_team->players[3]->position.y < CENTER_Y)
                    {
                        upgoal = CENTER_Y + 25.0f;
                        downgoal = CENTER_Y + GOAL_WIDTH / 2.0f + 30.0f;
                    }
                    else if (scene->second_team->players[3]->position.y >= CENTER_Y)
                    {
                        upgoal = CENTER_Y - GOAL_WIDTH / 2.0f - 30.0f;
                        downgoal = CENTER_Y - 25.0f;
                    }
                    float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
                    float dx = goal_x - self->position.x;
                    float dy = goal_y - self->position.y;
                    float d = hypotf(dx, dy);
                    scene->ball->velocity.x = MAX_BALL_VELOCITY * (dx / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
                    scene->ball->velocity.y = MAX_BALL_VELOCITY * (dy / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
                    self->velocity.x = self->velocity.x / 2.0f;
                    self->velocity.y = self->velocity.y / 2.0f;
                }
            }
            else if (self->team == 2)
            {
                goal_x = PITCH_X;
                if (scene->ball->position.x > CENTER_X)
                {
                    if (self == scene->second_team->players[1] && scene->second_team->players[5]->position.x < self->position.x)
                    {
                        pass(scene->ball, scene->second_team->players[5], self);
                    }
                    else if (self == scene->second_team->players[5] && scene->second_team->players[1]->position.x < self->position.x)
                    {
                        pass(scene->ball, scene->second_team->players[1], self);
                    }
                    else
                    {
                        for (int i = 0; i < 6; i++)
                        {
                            if (scene->second_team->players[i]->position.x < self->position.x)
                            {
                                pass(scene->ball, scene->second_team->players[i], self);
                                break;
                            }
                        }
                    }
                }
                else if (self->position.x <= CENTER_X && scene->ball->position.x > PITCH_X + 200.0f)
                {
                    if (self == scene->second_team->players[1] && scene->second_team->players[5]->position.x <= self->position.x)
                    {
                        pass(scene->ball, scene->second_team->players[5], self);
                    }
                    else if (self == scene->second_team->players[5] && scene->second_team->players[1]->position.x <= self->position.x)
                    {
                        pass(scene->ball, scene->second_team->players[1], self);
                    }
                }
                else
                {
                    float upgoal;
                    float downgoal;
                    if (scene->first_team->players[3]->position.y < CENTER_Y)
                    {
                        upgoal = CENTER_Y + 25.0f;
                        downgoal = CENTER_Y + GOAL_WIDTH / 2.0f + 30.0f;
                    }
                    else if (scene->first_team->players[3]->position.y >= CENTER_Y)
                    {
                        downgoal = CENTER_Y - 25.0f;
                        upgoal = CENTER_Y - GOAL_WIDTH / 2.0f - 30.0f;
                    }
                    float goal_y = upgoal + (float)rand() / RAND_MAX * (downgoal - upgoal);
                    float dx = goal_x - self->position.x;
                    float dy = goal_y - self->position.y;
                    float d = hypotf(dx, dy);
                    scene->ball->velocity.x = MAX_BALL_VELOCITY * (dx / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
                    scene->ball->velocity.y = MAX_BALL_VELOCITY * (dy / d) * self->talents.shooting / MAX_TALENT_PER_SKILL;
                    self->velocity.x = self->velocity.x / 2.0f;
                    self->velocity.y = self->velocity.y / 2.0f;
                }
            }
        }
    }
}
void change_state_gk(struct Player *self, struct Scene *scene)
{
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W || self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        if (scene->ball->possessor == self)
        {
            self->state = SHOOTING;
            return;
        }
    }
    if (scene->ball->possessor == self)
    {
        if (freesight(self, scene) == 2)
        {
            self->state = MOVING;
            return;
        }
        else
        {
            if (self->team == 1)
            {
                if (scene->second_team->players[1]->position.x < PITCH_X + 300.0f || scene->second_team->players[5]->position.x < PITCH_X + 300.0f)
                {
                    self->state = MOVING;
                    return;
                }
                else
                {
                    self->state = SHOOTING;
                    return;
                }
            }
            else if (self->team == 2)
            {
                if (scene->first_team->players[1]->position.x > PITCH_X + PITCH_W - 300.0f || scene->first_team->players[5]->position.x > PITCH_X + PITCH_W - 300.0f)
                {
                    self->state = MOVING;
                    return;
                }
                else
                {
                    self->state = SHOOTING;
                    return;
                }
            }
        }
    }
    if (own_half(self->team, scene) != 1)
    {
        if (self->team == 1)
        {
            if (self->position.x < 100.0f && self->position.x > 60.0f)
            {
                self->state = IDLE;
                return;
            }
            else
            {
                self->state = MOVING;
                return;
            }
        }
        else if (self->team == 2)
        {
            if (self->position.x < 940.0f && self->position.x > 900.0f)
            {
                self->state = IDLE;
                return;
            }
            else
            {
                self->state = MOVING;
                return;
            }
        }
    }
    else if (gk_half(self->team, scene) == 1 && (hypotf(self->position.x - scene->ball->position.x, self->position.y - scene->ball->position.y) < 50.0f))
    {
        if (ballowner(self, scene) == 2)
        {
            if (isout == 1)
            {
                self->state = IDLE;
                return;
            }
            self->state = INTERCEPTING;
            return;
        }
        else
        {
            self->state = MOVING;
            return;
        }
    }
    else
    {
        self->state = MOVING;
        return;
    }
}
void change_state_def(struct Player *self, struct Scene *scene)
{
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W || self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        if (scene->ball->possessor == self)
        {
            self->state = SHOOTING;
            return;
        }
    }
    if (self->team == 1)
    {
        if (scene->ball->position.x > CENTER_X)
        {
            self->state = MOVING;
            return;
        }
    }
    else if (self->team == 2)
    {
        if (scene->ball->position.x < CENTER_X)
        {
            self->state = MOVING;
            return;
        }
    }
    if (scene->ball->possessor == self)
    {
        if (freesight(self, scene) == 1)
        {
            self->state = SHOOTING;
            return;
        }
        else
        {
            self->state = MOVING;
            return;
        }
    }
    else if (hypotf(self->position.x - scene->ball->position.x, self->position.y - scene->ball->position.y) < 50.0f && own_half(self->team, scene) == 1)
    {
        if (scene->ball->possessor == NULL)
        {
            if (is_nearest_to_ball(self, scene) && ballowner(self, scene) != 1)
            {
                if (isout == 1)
                {
                    self->state = IDLE;
                    return;
                }
                self->state = INTERCEPTING;
                return;
            }
        }
        if (ballowner(self, scene) == 2)
        {
            if (isout == 1)
            {
                self->state = IDLE;
                return;
            }
            self->state = INTERCEPTING;
            return;
        }
        else
        {
            self->state = MOVING;
            return;
        }
    }
    else
    {
        self->state = MOVING;
        return;
    }
}
void change_state_mid(struct Player *self, struct Scene *scene)
{
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W || self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        if (scene->ball->possessor == self)
        {
            self->state = SHOOTING;
            return;
        }
    }
    struct Team *my_team;
    if (self->team == 1)
    {
        my_team = scene->first_team;
    }
    else
    {
        my_team = scene->second_team;
    }
    int flag = 0;
    for (int i = 0; i < 6; i++)
    {
        flag = 0;
        if (my_team->players[i] == scene->ball->possessor && scene->ball->possessor != self)
        {
            flag = 1;
            break;
        }
    }
    if (flag == 1)
    {
        if (self->team == 1)
        {
            if (self->position.x < scene->ball->possessor->position.x)
            {
                float d3 = scene->first_team->players[2]->position.y - scene->first_team->players[4]->position.y;
                if (abs(d3) < 250.0f)
                {
                    self->state = MOVING;
                    return;
                }
                else
                {
                    self->state = IDLE;
                    return;
                }
            }
            else
            {
                self->state = MOVING;
                return;
            }
        }
        else if (self->team == 2)
        {
            if (self->position.x > scene->ball->possessor->position.x)
            {
                float d3 = scene->second_team->players[2]->position.y - scene->second_team->players[4]->position.y;
                if (abs(d3) < 250.0f)
                {
                    self->state = MOVING;
                    return;
                }
                else
                {
                    self->state = IDLE;
                    return;
                }
            }
            else
            {
                self->state = MOVING;
                return;
            }
        }
    }
    if (scene->ball->possessor == NULL)
    {
        if (is_nearest_to_ball(self, scene) && ballowner(self, scene) != 1)
        {
            if (isout == 1)
            {
                self->state = IDLE;
                return;
            }
            self->state = INTERCEPTING;
            return;
        }
    }
    if (scene->ball->possessor == self)
    {
        if (self->team == 1)
        {
            if (self->position.x > CENTER_X && self->position.x < CENTER_X + 200.0f)
            {
                if (scene->first_team->players[1]->position.x > self->position.x)
                {
                    self->state = SHOOTING;
                    return;
                }
                else if (scene->first_team->players[5]->position.x > self->position.x)
                {
                    self->state = SHOOTING;
                    return;
                }
                else
                {
                    self->state = MOVING;
                    return;
                }
            }
            else if (self->position.x <= CENTER_X)
            {
                if (freesight2(self, scene, scene->first_team->players[5]) == 1 || freesight2(self, scene, scene->first_team->players[1]) == 1)
                {
                    self->state = SHOOTING;
                    return;
                }
                else
                {
                    self->state = MOVING;
                    return;
                }
            }
            else
            {
                self->state = SHOOTING;
                return;
            }
        }
        if (self->team == 2)
        {
            if (self->position.x < CENTER_X && self->position.x > CENTER_X - 200.0f)
            {
                if (scene->second_team->players[1]->position.x < self->position.x)
                {
                    self->state = SHOOTING;
                    return;
                }
                else if (scene->second_team->players[5]->position.x < self->position.x)
                {
                    self->state = SHOOTING;
                    return;
                }
                else
                {
                    self->state = MOVING;
                    return;
                }
            }
            else if (self->position.x >= CENTER_X)
            {
                if (freesight2(self, scene, scene->second_team->players[5]) == 1 || freesight2(self, scene, scene->second_team->players[1]) == 1)
                {
                    self->state = SHOOTING;
                    return;
                }
                else
                {
                    self->state = MOVING;
                    return;
                }
            }
            else
            {
                self->state = SHOOTING;
                return;
            }
        }
    }
    else if (hypotf(self->position.x - scene->ball->position.x, self->position.y - scene->ball->position.y) <= 100.0f)
    {
        if (ballowner(self, scene) == 2 || scene->ball->possessor == NULL)
        {
            if (self->team == 1)
            {
                if (self->position.x < scene->ball->position.x - 10.0f)
                {
                    if (isout == 1)
                    {
                        self->state = IDLE;
                        return;
                    }
                    self->state = INTERCEPTING;
                    return;
                }
                else
                {
                    if (ballowner(self, scene) == 2)
                    {
                        if (isout == 1)
                        {
                            self->state = IDLE;
                            return;
                        }
                        self->state = INTERCEPTING;
                        return;
                    }
                    else
                    {
                        self->state = IDLE;
                        return;
                    }
                }
            }
            else if (self->team == 2)
            {
                if (self->position.x > scene->ball->position.x + 10.0f)
                {
                    if (isout == 1)
                    {
                        self->state = IDLE;
                        return;
                    }
                    self->state = INTERCEPTING;
                    return;
                }
                else
                {
                    if (ballowner(self, scene) == 2)
                    {
                        if (isout == 1)
                        {
                            self->state = IDLE;
                            return;
                        }
                        self->state = INTERCEPTING;
                        return;
                    }
                    else
                    {
                        self->state = IDLE;
                        return;
                    }
                }
            }
        }
        else
        {
            self->state = IDLE;
            return;
        }
    }
    else
    {
        if (self->team == 1 && scene->ball->position.x > CENTER_X)
        {
            self->state = IDLE;
            return;
        }
        else if (self->team == 2 && scene->ball->position.x < CENTER_X)
        {
            self->state = IDLE;
            return;
        }
        else
        {
            self->state = MOVING;
            return;
        }
    }
}
void change_state_att(struct Player *self, struct Scene *scene)
{
    float goal_x = (self->team == 1) ? PITCH_X + PITCH_W : PITCH_X;
    float dx = goal_x - self->position.x;
    float dy = CENTER_Y - self->position.y;
    float d = hypotf(dx, dy);
    if (self->position.x < PITCH_X || self->position.x > PITCH_X + PITCH_W || self->position.y < PITCH_Y || self->position.y > PITCH_Y + PITCH_H)
    {
        if (scene->ball->possessor == self)
        {
            self->state = SHOOTING;
            return;
        }
    }
    if (scene->ball->possessor == NULL)
    {
        if (is_nearest_to_ball(self, scene) && ballowner(self, scene) != 1)
        {
            if (isout == 1)
            {
                self->state = IDLE;
                return;
            }
            self->state = INTERCEPTING;
            return;
        }
    }
    if (scene->ball->possessor == self)
    {
        if (d < 200.0f)
        {
            self->state = SHOOTING;
            return;
        }
    }
    if (self->team == 1)
    {
        if (scene->ball->position.x < self->position.x && scene->ball->position.x < CENTER_X - 150.0f && scene->ball->possessor != self && (scene->second_team->players[1]->position.x < self->position.x || scene->second_team->players[5]->position.x < self->position.x))
        {
            if (scene->ball->possessor == scene->first_team->players[3])
            {
                self->state = MOVING;
                return;
            }
            else
            {
                if (scene->ball->possessor != NULL && self->position.x < CENTER_X)
                {
                    dx = scene->ball->position.x - self->position.x;
                    dy = scene->ball->position.y - self->position.y;
                    d = hypotf(dx, dy);
                    if (d < 16.0f)
                    {
                        self->state = MOVING;
                        return;
                    }
                    else
                    {
                        self->state = IDLE;
                        return;
                    }
                }
                else
                {
                    if (scene->ball->possessor == NULL)
                    {
                        self->state = MOVING;
                        return;
                    }
                    else
                    {
                        float d3 = scene->first_team->players[1]->position.y - scene->first_team->players[5]->position.y;
                        if (abs(d3) < 150.0f)
                        {
                            self->state = MOVING;
                            return;
                        }
                        else
                        {
                            self->state = IDLE;
                            return;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (scene->ball->position.x > self->position.x && scene->ball->position.x > CENTER_X + 150.0f && scene->ball->possessor != self && (scene->first_team->players[1]->position.x > self->position.x || scene->first_team->players[5]->position.x > self->position.x))
        {
            if (scene->ball->possessor == scene->second_team->players[3])
            {
                self->state = MOVING;
                return;
            }
            else
            {
                if (scene->ball->possessor != NULL && self->position.x > CENTER_X)
                {
                    dx = scene->ball->position.x - self->position.x;
                    dy = scene->ball->position.y - self->position.y;
                    d = hypotf(dx, dy);
                    if (d < 10.0f)
                    {
                        self->state = MOVING;
                        return;
                    }
                    else
                    {
                        self->state = IDLE;
                        return;
                    }
                }
                else
                {
                    if (scene->ball->possessor == NULL)
                    {
                        self->state = MOVING;
                        return;
                    }
                    else
                    {
                        float d3 = scene->second_team->players[1]->position.y - scene->second_team->players[5]->position.y;
                        if (abs(d3) < 150.0f)
                        {
                            self->state = MOVING;
                            return;
                        }
                        else
                        {
                            self->state = IDLE;
                            return;
                        }
                    }
                }
            }
        }
    }
    if (self->team == 1)
    {
        if (scene->ball->possessor == self)
        {
            if (self->position.x < CENTER_X && self->position.x > PITCH_X)
            {
                for (int i = 0; i < 6; i++)
                {
                    if (scene->first_team->players[i]->position.x > self->position.x)
                    {
                        if (i == 0)
                            continue;
                        self->state = SHOOTING;
                        return;
                    }
                }
                self->state = MOVING;
                return;
            }
            else if (self->position.x >= CENTER_X && self->position.x < CENTER_X + 200.0f)
            {
                if (self == scene->first_team->players[1] && scene->first_team->players[5]->position.x < self->position.x)
                {
                    self->state = MOVING;
                    return;
                }
                else if (self == scene->first_team->players[5] && scene->first_team->players[1]->position.x < self->position.x)
                {
                    self->state = MOVING;
                    return;
                }
                else if (self == scene->first_team->players[1] && scene->first_team->players[5]->position.x > self->position.x)
                {
                    if (freesight2(self, scene, scene->first_team->players[5]) == 1)
                    {
                        self->state = SHOOTING;
                        return;
                    }
                    else
                    {
                        self->state = MOVING;
                        return;
                    }
                }
                else if (self == scene->first_team->players[5] && scene->first_team->players[1]->position.x > self->position.x)
                {
                    if (freesight2(self, scene, scene->first_team->players[1]) == 1)
                    {
                        self->state = SHOOTING;
                        return;
                    }
                    else
                    {
                        self->state = MOVING;
                        return;
                    }
                }
            }
        }
        else if (hypotf(self->position.x - scene->ball->position.x, self->position.y - scene->ball->position.y) < 30.0f)
        {
            if (ballowner(self, scene) == 2 || scene->ball->possessor == NULL)
            {
                if (scene->ball->possessor == scene->second_team->players[3])
                {
                    self->state = MOVING;
                    return;
                }
                else
                {
                    if (isout == 1)
                    {
                        self->state = IDLE;
                        return;
                    }
                    self->state = INTERCEPTING;
                    return;
                }
            }
            else
            {
                self->state = MOVING;
                return;
            }
        }
        else if (scene->ball->possessor != self)
        {
            self->state = MOVING;
            return;
        }
        else
        {
            self->state = MOVING;
            return;
        }
    }
    if (self->team == 2)
    {
        if (scene->ball->possessor == self)
        {
            if (self->position.x > CENTER_X && self->position.x < PITCH_X + PITCH_W)
            {
                for (int i = 0; i < 6; i++)
                {
                    if (scene->second_team->players[i]->position.x < self->position.x)
                    {
                        if (i == 0)
                            continue;
                        self->state = SHOOTING;
                        return;
                    }
                }
                self->state = MOVING;
                return;
            }
            else if (self->position.x <= CENTER_X)
            {
                if (self == scene->second_team->players[1] && scene->second_team->players[5]->position.x > self->position.x)
                {
                    self->state = MOVING;
                    return;
                }
                else if (self == scene->second_team->players[5] && scene->second_team->players[1]->position.x > self->position.x)
                {
                    self->state = MOVING;
                    return;
                }
                else if (self == scene->second_team->players[1] && scene->second_team->players[5]->position.x < self->position.x)
                {
                    if (freesight2(self, scene, scene->second_team->players[5]) == 1)
                    {
                        self->state = SHOOTING;
                        return;
                    }
                    else
                    {
                        self->state = MOVING;
                        return;
                    }
                }
                else if (self == scene->second_team->players[5] && scene->second_team->players[1]->position.x < self->position.x)
                {
                    if (freesight2(self, scene, scene->second_team->players[1]) == 1)
                    {
                        self->state = SHOOTING;
                        return;
                    }
                    else
                    {
                        self->state = MOVING;
                        return;
                    }
                }
            }
        }
        else if (hypotf(self->position.x - scene->ball->position.x, self->position.y - scene->ball->position.y) < 30.0f)
        {
            if (ballowner(self, scene) == 2 || scene->ball->possessor == NULL)
            {
                if (scene->ball->possessor == scene->first_team->players[3])
                {
                    self->state = MOVING;
                    return;
                }
                else
                {
                    if (isout == 1)
                    {
                        self->state = IDLE;
                        return;
                    }
                    self->state = INTERCEPTING;
                    return;
                }
            }
            else
            {
                self->state = MOVING;
                return;
            }
        }
        else if (scene->ball->possessor != self)
        {
            self->state = MOVING;
            return;
        }
        else
        {
            self->state = MOVING;
            return;
        }
    }
}
/* Team 1 movement logic */
void movement_logic_1_0(struct Player *self, struct Scene *scene) { movement_logic_def(self, scene); }
void movement_logic_1_1(struct Player *self, struct Scene *scene) { movement_logic_att(self, scene); }
void movement_logic_1_2(struct Player *self, struct Scene *scene) { movement_logic_mid(self, scene); }
void movement_logic_1_3(struct Player *self, struct Scene *scene) { movement_logic_gk(self, scene); }
void movement_logic_1_4(struct Player *self, struct Scene *scene) { movement_logic_mid(self, scene); }
void movement_logic_1_5(struct Player *self, struct Scene *scene) { movement_logic_att(self, scene); }

/* Team 2 movement logic */
void movement_logic_2_0(struct Player *self, struct Scene *scene) { movement_logic_def(self, scene); }
void movement_logic_2_1(struct Player *self, struct Scene *scene) { movement_logic_att(self, scene); }
void movement_logic_2_2(struct Player *self, struct Scene *scene) { movement_logic_mid(self, scene); }
void movement_logic_2_3(struct Player *self, struct Scene *scene) { movement_logic_gk(self, scene); }
void movement_logic_2_4(struct Player *self, struct Scene *scene) { movement_logic_mid(self, scene); }
void movement_logic_2_5(struct Player *self, struct Scene *scene) { movement_logic_att(self, scene); }

/* Team 1 shooting logic */
void shooting_logic_1_0(struct Player *self, struct Scene *scene) { shooting_logic_def(self, scene); }
void shooting_logic_1_1(struct Player *self, struct Scene *scene) { shooting_logic_att(self, scene); }
void shooting_logic_1_2(struct Player *self, struct Scene *scene) { shooting_logic_mid(self, scene); }
void shooting_logic_1_3(struct Player *self, struct Scene *scene) { shooting_logic_gk(self, scene); }
void shooting_logic_1_4(struct Player *self, struct Scene *scene) { shooting_logic_mid(self, scene); }
void shooting_logic_1_5(struct Player *self, struct Scene *scene) { shooting_logic_att(self, scene); }

/* Team 2 shooting logic */
void shooting_logic_2_0(struct Player *self, struct Scene *scene) { shooting_logic_def(self, scene); }
void shooting_logic_2_1(struct Player *self, struct Scene *scene) { shooting_logic_att(self, scene); }
void shooting_logic_2_2(struct Player *self, struct Scene *scene) { shooting_logic_mid(self, scene); }
void shooting_logic_2_3(struct Player *self, struct Scene *scene) { shooting_logic_gk(self, scene); }
void shooting_logic_2_4(struct Player *self, struct Scene *scene) { shooting_logic_mid(self, scene); }
void shooting_logic_2_5(struct Player *self, struct Scene *scene) { shooting_logic_att(self, scene); }

/* Team 1 change_state logic */
void change_state_logic_1_0(struct Player *self, struct Scene *scene) { change_state_def(self, scene); }
void change_state_logic_1_1(struct Player *self, struct Scene *scene) { change_state_att(self, scene); }
void change_state_logic_1_2(struct Player *self, struct Scene *scene) { change_state_mid(self, scene); }
void change_state_logic_1_3(struct Player *self, struct Scene *scene) { change_state_gk(self, scene); }
void change_state_logic_1_4(struct Player *self, struct Scene *scene) { change_state_mid(self, scene); }
void change_state_logic_1_5(struct Player *self, struct Scene *scene) { change_state_att(self, scene); }

/* Team 2 change_state logic */
void change_state_logic_2_0(struct Player *self, struct Scene *scene) { change_state_def(self, scene); }
void change_state_logic_2_1(struct Player *self, struct Scene *scene) { change_state_att(self, scene); }
void change_state_logic_2_2(struct Player *self, struct Scene *scene) { change_state_mid(self, scene); }
void change_state_logic_2_3(struct Player *self, struct Scene *scene) { change_state_gk(self, scene); }
void change_state_logic_2_4(struct Player *self, struct Scene *scene) { change_state_mid(self, scene); }
void change_state_logic_2_5(struct Player *self, struct Scene *scene) { change_state_att(self, scene); }

/* -------------------------------------------------------------------------
 * Lookup tables for factory
 * ------------------------------------------------------------------------- */
static PlayerLogicFn team1_movement[6] = {
    movement_logic_1_0, movement_logic_1_1, movement_logic_1_2,
    movement_logic_1_3, movement_logic_1_4, movement_logic_1_5};

static PlayerLogicFn team2_movement[6] = {
    movement_logic_2_0, movement_logic_2_1, movement_logic_2_2,
    movement_logic_2_3, movement_logic_2_4, movement_logic_2_5};

static PlayerLogicFn team1_shooting[6] = {
    shooting_logic_1_0, shooting_logic_1_1, shooting_logic_1_2,
    shooting_logic_1_3, shooting_logic_1_4, shooting_logic_1_5};

static PlayerLogicFn team2_shooting[6] = {
    shooting_logic_2_0, shooting_logic_2_1, shooting_logic_2_2,
    shooting_logic_2_3, shooting_logic_2_4, shooting_logic_2_5};

static PlayerLogicFn team1_change_state[6] = {
    change_state_logic_1_0, change_state_logic_1_1, change_state_logic_1_2,
    change_state_logic_1_3, change_state_logic_1_4, change_state_logic_1_5};

static PlayerLogicFn team2_change_state[6] = {
    change_state_logic_2_0, change_state_logic_2_1, change_state_logic_2_2,
    change_state_logic_2_3, change_state_logic_2_4, change_state_logic_2_5};

/* -------------------------------------------------------------------------
 * Factory functions
 * ------------------------------------------------------------------------- */
PlayerLogicFn get_movement_logic(int team, int kit)
{
    if (coach_both_teams)
        return team1_movement[kit];
    return (team == 1) ? team1_movement[kit] : team2_movement[kit];
}

PlayerLogicFn get_shooting_logic(int team, int kit)
{
    if (coach_both_teams)
        return team1_shooting[kit];
    return (team == 1) ? team1_shooting[kit] : team2_shooting[kit];
}

PlayerLogicFn get_change_state_logic(int team, int kit)
{
    if (coach_both_teams)
        return team1_change_state[kit];
    return (team == 1) ? team1_change_state[kit] : team2_change_state[kit];
}

/* -------------------------------------------------------------------------
 * TALENTS
 *  TODO 2: Replace these default values with your desired skill points.
 * ------------------------------------------------------------------------- */
/* Team 1 */
static struct Talents team1_talents[6] = {
    {6, 6, 4, 4},
    {2, 8, 2, 8},
    {5, 5, 3, 7},
    {6, 4, 2, 8},
    {6, 6, 2, 6},
    {3, 7, 4, 6},
};

/* Team 2 */
static struct Talents team2_talents[6] = {
    {6, 6, 4, 4},
    {3, 6, 5, 6},
    {7, 4, 3, 6},
    {4, 4, 3, 9},
    {6, 5, 3, 6},
    {1, 7, 4, 8},
};

struct Talents get_talents(int team, int kit)
{
    if (coach_both_teams)
        return team1_talents[kit];
    return (team == 1) ? team1_talents[kit] : team2_talents[kit];
}

/* -------------------------------------------------------------------------
 * Positioning
 *  TODO 3: Decide players positions at kick-off.
 *        Players must stay on their half, outside the center circle.
 *        Keep in mind that the kick-off team's first player will automatically
 *             be placed at the center of the pitch.
 * ------------------------------------------------------------------------- */
/* Team 1 */
static struct Vec2 team1_positions[6] = {
    {350, CENTER_Y},
    {400, CENTER_Y - 150},
    {200, CENTER_Y - 175},
    {75, CENTER_Y},
    {200, CENTER_Y + 175},
    {400, CENTER_Y + 150},
};

/* Team 2 */
static struct Vec2 team2_positions[6] = {
    {750, CENTER_Y},
    {700, CENTER_Y - 150},
    {850, CENTER_Y - 125},
    {925, CENTER_Y},
    {870, CENTER_Y + 125},
    {700, CENTER_Y + 150},
};

struct Vec2 get_positions(int team, int kit)
{
    return (team == 1) ? team1_positions[kit] : team2_positions[kit];
}
