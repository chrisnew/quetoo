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

#include "g_local.h"


/*
 * P_PickupWeapon
 */
qboolean P_PickupWeapon(edict_t *ent, edict_t *other){
	int index, ammoindex;
	g_item_t *ammo;

	index = ITEM_INDEX(ent->item);

	// add ammo
	ammo = G_FindItem(ent->item->ammo);
	ammoindex = ITEM_INDEX(ammo);

	if(!(ent->spawn_flags & SF_ITEM_DROPPED) && other->client->locals.inventory[index]){
		if(other->client->locals.inventory[ammoindex] >= ammo->quantity)
			G_AddAmmo(other, ammo, ent->item->quantity);  // q3 style
		else
			G_AddAmmo(other, ammo, ammo->quantity);
	}
	else
		G_AddAmmo(other, ammo, ammo->quantity);

	// setup respawn if it's not a dropped item
	if(!(ent->spawn_flags & SF_ITEM_DROPPED))
		G_SetRespawn(ent, 5);

	// add the weapon to inventory
	other->client->locals.inventory[index]++;

	// auto-change if it's the first weapon we pick up
	if(other->client->locals.weapon != ent->item &&
			(other->client->locals.weapon == G_FindItem("Shotgun")))
		other->client->new_weapon = ent->item;

	return true;
}


/*
 * P_ChangeWeapon
 *
 * The old weapon has been put away, so make the new one current
 */
void P_ChangeWeapon(edict_t *ent){

	// change weapon
	ent->client->locals.last_weapon = ent->client->locals.weapon;
	ent->client->locals.weapon = ent->client->new_weapon;
	ent->client->new_weapon = NULL;

	// update weapon state
	ent->client->weapon_fire_time = g_level.time + 0.4;

	// resolve ammo
	if(ent->client->locals.weapon && ent->client->locals.weapon->ammo)
		ent->client->ammo_index = ITEM_INDEX(G_FindItem(ent->client->locals.weapon->ammo));
	else
		ent->client->ammo_index = 0;

	// set visible model
	if(ent->client->locals.weapon)
		ent->s.model2 = gi.ModelIndex(ent->client->locals.weapon->model);
	else
		ent->s.model2 = 0;

	if(ent->health < 1)
		return;

	// play a sound
	gi.Sound(ent, gi.SoundIndex("weapons/common/switch"), ATTN_NORM);
}


/*
 * P_NoAmmoWeaponChange
 */
void P_NoAmmoWeaponChange(g_client_t *client){

	if(client->locals.inventory[ITEM_INDEX(G_FindItem("nukes"))]
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("bfg10k"))]){
		client->new_weapon = G_FindItem("bfg10k");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("slugs"))]
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("railgun"))]){
		client->new_weapon = G_FindItem("railgun");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("bolts"))]
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("lightning"))]){
		client->new_weapon = G_FindItem("lightning");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("cells"))]
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("hyperblaster"))]){
		client->new_weapon = G_FindItem("hyperblaster");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("rockets"))]
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("rocket launcher"))]){
		client->new_weapon = G_FindItem ("rocket launcher");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("bullets"))]
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("machinegun"))]){
		client->new_weapon = G_FindItem("machinegun");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("shells"))] > 1
			&& client->locals.inventory[ITEM_INDEX(G_FindItem("super shotgun"))]){
		client->new_weapon = G_FindItem("super shotgun");
		return;
	}
	if(client->locals.inventory[ITEM_INDEX(G_FindItem("shells"))]){
		client->new_weapon = G_FindItem("shotgun");
		return;
	}
}


/*
 * P_UseWeapon
 */
void P_UseWeapon(edict_t *ent, g_item_t *item){

	// see if we're already using it
	if(item == ent->client->locals.weapon)
		return;

	// change to this weapon when down
	ent->client->new_weapon = item;
}


/*
 * P_DropWeapon
 */
void P_DropWeapon(edict_t *ent, g_item_t *item){
	int index;

	index = ITEM_INDEX(item);

	// see if we're already using it and we only have one
	if((item == ent->client->locals.weapon || item == ent->client->new_weapon) &&
			(ent->client->locals.inventory[index] == 1)){
		gi.ClientPrint(ent, PRINT_HIGH, "Can't drop current weapon\n");
		return;
	}

	G_DropItem(ent, item);
	ent->client->locals.inventory[index]--;
}


/*
 * P_FireWeapon
 */
static void P_FireWeapon(edict_t *ent, float interval, void (*fire)(edict_t *ent)){
	int n, m;
	int buttons;

	buttons = (ent->client->latched_buttons | ent->client->buttons);

	if(!(buttons & BUTTON_ATTACK))
		return;

	ent->client->latched_buttons &= ~BUTTON_ATTACK;

	// use small epsilon for low server_frame rates
	if(ent->client->weapon_fire_time > g_level.time + 0.001)
		return;

	ent->client->weapon_fire_time = g_level.time + interval;

	// determine if ammo is required, and if the quantity is sufficient
	n = ent->client->locals.inventory[ent->client->ammo_index];
	m = ent->client->locals.weapon->quantity;

	// they are out of ammo
	if(ent->client->ammo_index && n < m){

		if(g_level.time >= ent->pain_time){  // play a click sound
			gi.Sound(ent, gi.SoundIndex("weapons/common/no_ammo"), ATTN_NORM);
			ent->pain_time = g_level.time + 1;
		}

		P_NoAmmoWeaponChange(ent->client);
		return;
	}

	// they've pressed their fire button, and have ammo, so fire
	G_SetAnimation(ent, ANIM_TORSO_ATTACK1);

	if(ent->client->locals.inventory[quad_damage_index]){  // quad sound

		if(ent->client->quad_attack_time < g_level.time){
			gi.Sound(ent, gi.SoundIndex("quad/attack"), ATTN_NORM);

			ent->client->quad_attack_time = g_level.time + 0.5;
		}
	}

	fire(ent);  // fire the weapon

	// and decrease their inventory
	ent->client->locals.inventory[ent->client->ammo_index] -= m;
}


/*
 * P_WeaponThink
 */
void P_WeaponThink(edict_t *ent){

	if(ent->health < 1)
		return;

	ent->client->weapon_think_time = g_level.time;

	if(ent->client->new_weapon){
		P_ChangeWeapon(ent);
		return;
	}

	// call active weapon think routine
	if(ent->client->locals.weapon && ent->client->locals.weapon->weapon_think)
		ent->client->locals.weapon->weapon_think(ent);
}


/*
 * P_FireShotgun
 */
static void P_FireShotgun_(edict_t *ent){
	vec3_t start, offset;
	vec3_t forward, right;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireShotgun(ent, start, forward, 6, 4, DEFAULT_SHOTGUN_HSPREAD,
			DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_SHOTGUN);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireShotgun(edict_t *ent){
	P_FireWeapon(ent, 0.65, P_FireShotgun_);
}


/*
 * P_FireSuperShotgun
 */
static void P_FireSuperShotgun_(edict_t *ent){
	vec3_t start;
	vec3_t forward, right;
	vec3_t offset;
	vec3_t v;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	v[PITCH] = ent->client->angles[PITCH];
	v[YAW] = ent->client->angles[YAW] - 5;
	v[ROLL] = ent->client->angles[ROLL];
	AngleVectors(v, forward, NULL, NULL);

	G_FireShotgun(ent, start, forward, 4, 4, DEFAULT_SHOTGUN_HSPREAD,
			DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);

	v[YAW] = ent->client->angles[YAW] + 5;
	AngleVectors(v, forward, NULL, NULL);

	G_FireShotgun(ent, start, forward, 4, 4, DEFAULT_SHOTGUN_HSPREAD,
			DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT / 2, MOD_SSHOTGUN);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_SSHOTGUN);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireSuperShotgun(edict_t *ent){
	P_FireWeapon(ent, 0.85, P_FireSuperShotgun_);
}


/*
 * P_FireMachinegun
 */
static void P_FireMachinegun_(edict_t *ent){
	vec3_t start, offset;
	vec3_t forward, right;

	// get start / end positions
	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireBullet(ent, start, forward, 8, 4, DEFAULT_BULLET_HSPREAD,
			DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_MACHINEGUN);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireMachinegun(edict_t *ent){
	P_FireWeapon(ent, 0.04, P_FireMachinegun_);
}


/*
 * P_FireGrenadeLauncher
 */
static void P_FireGrenadeLauncher_(edict_t *ent){
	vec3_t start, offset;
	vec3_t forward, right;

	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	AngleVectors(ent->client->angles, forward, right, NULL);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireGrenadeLauncher(ent, start, forward, 900, 100, 100, 185.0, 2.0);

	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_GRENADE);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireGrenadeLauncher(edict_t *ent){
	P_FireWeapon(ent, 0.6, P_FireGrenadeLauncher_);
}


/*
 * P_FireRocketLauncher
 */
static void P_FireRocketLauncher_(edict_t *ent){
	vec3_t offset, start;
	vec3_t forward, right;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireRocketLauncher(ent, start, forward, 1250, 120, 120, 150.0);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_ROCKET);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireRocketLauncher(edict_t *ent){
	P_FireWeapon(ent, 0.8, P_FireRocketLauncher_);
}


/*
 * P_FireHyperblaster
 */
static void P_FireHyperblaster_(edict_t *ent){
	vec3_t forward, right;
	vec3_t offset, start;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireHyperblaster(ent, start, forward, 2000, 16, 6);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_HYPERBLASTER);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireHyperblaster(edict_t *ent){
	P_FireWeapon(ent, 0.1, P_FireHyperblaster_);
}


/*
 * P_FireLightning
 */
static void P_FireLightning_(edict_t *ent){
	vec3_t start, offset;
	vec3_t forward, right;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireLightning(ent, start, forward, 10, 12);

	// if the client has just begun to attack, send the muzzle flash
	if(ent->client->muzzle_flash_time < g_level.time){
		gi.WriteByte(svc_muzzle_flash);
		gi.WriteShort(ent - g_game.edicts);
		gi.WriteByte(MZ_LIGHTNING);
		gi.Multicast(ent->s.origin, MULTICAST_PVS);

		ent->client->muzzle_flash_time = g_level.time + 0.25;
	}
}

void P_FireLightning(edict_t *ent){
	P_FireWeapon(ent, 0.1, P_FireLightning_);
}


/*
 * P_FireRailgun
 */
static void P_FireRailgun_(edict_t *ent){
	vec3_t start, offset;
	vec3_t forward, right;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireRailgun(ent, start, forward, 120, 80);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_RAILGUN);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireRailgun(edict_t *ent){
	P_FireWeapon(ent, 1.5, P_FireRailgun_);
}


/*
 * P_FireBFG
 */
static void P_FireBFG_(edict_t *ent){
	vec3_t offset, start;
	vec3_t forward, right;

	AngleVectors(ent->client->angles, forward, right, NULL);
	VectorSet(offset, 30.0, 6.0, ent->view_height - 10.0);
	G_ProjectSource(ent->s.origin, offset, forward, right, start);

	G_FireBFG(ent, start, forward, 800, 100, 100, 1024.0);

	// send muzzle flash
	gi.WriteByte(svc_muzzle_flash);
	gi.WriteShort(ent - g_game.edicts);
	gi.WriteByte(MZ_BFG);
	gi.Multicast(ent->s.origin, MULTICAST_PVS);
}

void P_FireBFG(edict_t *ent){
	P_FireWeapon(ent, 2.0, P_FireBFG_);
}
