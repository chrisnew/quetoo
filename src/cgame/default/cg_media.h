/*
 * Copyright(c) 1997-2001 Id Software, Inc.
 * Copyright(c) 2002 The Quakeforge Project.
 * Copyright(c) 2006 Quake2World.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __CG_MEDIA_H__
#define __CG_MEDIA_H__

#include "cg_types.h"

#ifdef __CG_LOCAL_H__
extern s_sample_t *cg_sample_blaster_fire;
extern s_sample_t *cg_sample_blaster_hit;
extern s_sample_t *cg_sample_shotgun_fire;
extern s_sample_t *cg_sample_supershotgun_fire;
extern s_sample_t *cg_sample_machinegun_fire[4];
extern s_sample_t *cg_sample_machinegun_hit[3];
extern s_sample_t *cg_sample_grenadelauncher_fire;
extern s_sample_t *cg_sample_rocketlauncher_fire;
extern s_sample_t *cg_sample_hyperblaster_fire;
extern s_sample_t *cg_sample_hyperblaster_hit;
extern s_sample_t *cg_sample_lightning_fire;
extern s_sample_t *cg_sample_lightning_fly;
extern s_sample_t *cg_sample_lightning_discharge;
extern s_sample_t *cg_sample_railgun_fire;
extern s_sample_t *cg_sample_bfg_fire;
extern s_sample_t *cg_sample_bfg_hit;

extern s_sample_t *cg_sample_explosion;
extern s_sample_t *cg_sample_teleport;
extern s_sample_t *cg_sample_respawn;
extern s_sample_t *cg_sample_sparks;
extern s_sample_t *cg_sample_footsteps[4];
extern s_sample_t *cg_sample_rain;
extern s_sample_t *cg_sample_snow;

extern r_image_t *cg_particle_normal;
extern r_image_t *cg_particle_explosion;
extern r_image_t *cg_particle_teleporter;
extern r_image_t *cg_particle_smoke;
extern r_image_t *cg_particle_steam;
extern r_image_t *cg_particle_bubble;
extern r_image_t *cg_particle_rain;
extern r_image_t *cg_particle_snow;
extern r_image_t *cg_particle_beam;
extern r_image_t *cg_particle_burn;
extern r_image_t *cg_particle_blood;
extern r_image_t *cg_particle_lightning;
extern r_image_t *cg_particle_rail_trail;
extern r_image_t *cg_particle_flame;
extern r_image_t *cg_particle_spark;
extern r_image_t *cg_particle_bullet[3];

void Cg_UpdateMedia(void);
#endif /* __CG_LOCAL_H__ */

#endif /* __CG_MEDIA_H__ */
