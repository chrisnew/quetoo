/*
 * Copyright(c) 1997-2001 id Software, Inc.
 * Copyright(c) 2002 The Quakeforge Project.
 * Copyright(c) 2006 Quetoo.
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

#include "net_message.h"

typedef union {
	int32_t i;
	vec_t v;
} net_vec_t;

/*
 * @brief
 */
void Net_WriteData(mem_buf_t *msg, const void *data, size_t len) {
	Mem_WriteBuffer(msg, data, len);
}

/*
 * @brief
 */
void Net_WriteChar(mem_buf_t *msg, const int32_t c) {
	byte *buf;

	buf = Mem_AllocBuffer(msg, sizeof(char));
	buf[0] = c;
}

/*
 * @brief
 */
void Net_WriteByte(mem_buf_t *msg, const int32_t c) {
	byte *buf;

	buf = Mem_AllocBuffer(msg, sizeof(byte));
	buf[0] = c;
}

/*
 * @brief
 */
void Net_WriteShort(mem_buf_t *msg, const int32_t c) {
	byte *buf;

	buf = Mem_AllocBuffer(msg, sizeof(int16_t));
	buf[0] = c & 0xff;
	buf[1] = c >> 8;
}

/*
 * @brief
 */
void Net_WriteLong(mem_buf_t *msg, const int32_t c) {
	byte *buf;

	buf = Mem_AllocBuffer(msg, sizeof(int32_t));
	buf[0] = c & 0xff;
	buf[1] = (c >> 8) & 0xff;
	buf[2] = (c >> 16) & 0xff;
	buf[3] = c >> 24;
}

/*
 * @brief
 */
void Net_WriteString(mem_buf_t *msg, const char *s) {
	if (!s)
		Mem_WriteBuffer(msg, "", 1);
	else
		Mem_WriteBuffer(msg, s, strlen(s) + 1);
}

/*
 * @brief
 */
void Net_WriteVector(mem_buf_t *msg, const vec_t v) {

	const net_vec_t vec = {
		.v = v
	};

	Net_WriteLong(msg, vec.i);
}

/*
 * @brief
 */
void Net_WritePosition(mem_buf_t *msg, const vec3_t pos) {
	Net_WriteVector(msg, pos[0]);
	Net_WriteVector(msg, pos[1]);
	Net_WriteVector(msg, pos[2]);
}

/*
 * @brief
 */
void Net_WriteAngle(mem_buf_t *msg, const vec_t v) {
	Net_WriteShort(msg, PackAngle(v));
}

/*
 * @brief
 */
void Net_WriteAngles(mem_buf_t *msg, const vec3_t angles) {
	Net_WriteAngle(msg, angles[0]);
	Net_WriteAngle(msg, angles[1]);
	Net_WriteAngle(msg, angles[2]);
}

/*
 * @brief
 */
void Net_WriteDir(mem_buf_t *msg, const vec3_t dir) {
	int32_t i, best = 0;
	vec_t best_d = 0.0;

	for (i = 0; i < NUM_APPROXIMATE_NORMALS; i++) {
		const vec_t d = DotProduct(dir, approximate_normals[i]);
		if (d > best_d) {
			best_d = d;
			best = i;
		}
	}

	Net_WriteByte(msg, best);
}

/*
 * @brief
 */
void Net_WriteDeltaMoveCmd(mem_buf_t *msg, const pm_cmd_t *from, const pm_cmd_t *to) {

	byte bits = 0;

	if (to->angles[0] != from->angles[0])
		bits |= CMD_ANGLE1;
	if (to->angles[1] != from->angles[1])
		bits |= CMD_ANGLE2;
	if (to->angles[2] != from->angles[2])
		bits |= CMD_ANGLE3;

	if (to->forward != from->forward)
		bits |= CMD_FORWARD;
	if (to->right != from->right)
		bits |= CMD_RIGHT;
	if (to->up != from->up)
		bits |= CMD_UP;

	if (to->buttons != from->buttons)
		bits |= CMD_BUTTONS;

	Net_WriteByte(msg, bits);

	if (bits & CMD_ANGLE1)
		Net_WriteShort(msg, to->angles[0]);
	if (bits & CMD_ANGLE2)
		Net_WriteShort(msg, to->angles[1]);
	if (bits & CMD_ANGLE3)
		Net_WriteShort(msg, to->angles[2]);

	if (bits & CMD_FORWARD)
		Net_WriteShort(msg, to->forward);
	if (bits & CMD_RIGHT)
		Net_WriteShort(msg, to->right);
	if (bits & CMD_UP)
		Net_WriteShort(msg, to->up);

	if (bits & CMD_BUTTONS)
		Net_WriteByte(msg, to->buttons);

	Net_WriteByte(msg, to->msec);
}

/*
 * @brief
 */
void Net_WriteDeltaPlayerState(mem_buf_t *msg, const player_state_t *from, const player_state_t *to) {

	uint16_t bits = 0;

	if (to->pm_state.type != from->pm_state.type)
		bits |= PS_PM_TYPE;

	if (!VectorCompare(to->pm_state.origin, from->pm_state.origin))
		bits |= PS_PM_ORIGIN;

	if (!VectorCompare(to->pm_state.velocity, from->pm_state.velocity))
		bits |= PS_PM_VELOCITY;

	if (to->pm_state.flags != from->pm_state.flags)
		bits |= PS_PM_FLAGS;

	if (to->pm_state.time != from->pm_state.time)
		bits |= PS_PM_TIME;

	if (to->pm_state.gravity != from->pm_state.gravity)
		bits |= PS_PM_GRAVITY;

	if (!VectorCompare(to->pm_state.view_offset, from->pm_state.view_offset))
		bits |= PS_PM_VIEW_OFFSET;

	if (!VectorCompare(to->pm_state.view_angles, from->pm_state.view_angles))
		bits |= PS_PM_VIEW_ANGLES;

	if (!VectorCompare(to->pm_state.kick_angles, from->pm_state.kick_angles))
		bits |= PS_PM_KICK_ANGLES;

	if (!VectorCompare(to->pm_state.delta_angles, from->pm_state.delta_angles))
		bits |= PS_PM_DELTA_ANGLES;

	Net_WriteShort(msg, bits);

	if (bits & PS_PM_TYPE)
		Net_WriteByte(msg, to->pm_state.type);

	if (bits & PS_PM_ORIGIN)
		Net_WritePosition(msg, to->pm_state.origin);

	if (bits & PS_PM_VELOCITY)
		Net_WritePosition(msg, to->pm_state.velocity);

	if (bits & PS_PM_FLAGS)
		Net_WriteShort(msg, to->pm_state.flags);

	if (bits & PS_PM_TIME)
		Net_WriteShort(msg, to->pm_state.time);

	if (bits & PS_PM_GRAVITY)
		Net_WriteShort(msg, to->pm_state.gravity);

	if (bits & PS_PM_VIEW_OFFSET) {
		Net_WriteShort(msg, to->pm_state.view_offset[0]);
		Net_WriteShort(msg, to->pm_state.view_offset[1]);
		Net_WriteShort(msg, to->pm_state.view_offset[2]);
	}

	if (bits & PS_PM_VIEW_ANGLES) {
		Net_WriteShort(msg, to->pm_state.view_angles[0]);
		Net_WriteShort(msg, to->pm_state.view_angles[1]);
		Net_WriteShort(msg, to->pm_state.view_angles[2]);
	}

	if (bits & PS_PM_KICK_ANGLES) {
		Net_WriteShort(msg, to->pm_state.kick_angles[0]);
		Net_WriteShort(msg, to->pm_state.kick_angles[1]);
		Net_WriteShort(msg, to->pm_state.kick_angles[2]);
	}

	if (bits & PS_PM_DELTA_ANGLES) {
		Net_WriteShort(msg, to->pm_state.delta_angles[0]);
		Net_WriteShort(msg, to->pm_state.delta_angles[1]);
		Net_WriteShort(msg, to->pm_state.delta_angles[2]);
	}

	uint32_t stat_bits = 0;

	for (int32_t i = 0; i < MAX_STATS; i++) {
		if (to->stats[i] != from->stats[i]) {
			stat_bits |= 1 << i;
		}
	}

	Net_WriteLong(msg, stat_bits);

	for (int32_t i = 0; i < MAX_STATS; i++) {
		if (stat_bits & (1 << i)) {
			Net_WriteShort(msg, to->stats[i]);
		}
	}
}

/*
 * @brief Writes an entity's state changes to a net message. Can delta from
 * either a baseline or a previous packet_entity
 */
void Net_WriteDeltaEntity(mem_buf_t *msg, const entity_state_t *from, const entity_state_t *to,
		_Bool force) {

	uint16_t bits = 0;

	if (to->number <= 0) {
		Com_Error(ERR_FATAL, "Unset entity number\n");
	}

	if (to->number >= MAX_ENTITIES) {
		Com_Error(ERR_FATAL, "Entity number >= MAX_ENTITIES\n");
	}

	if (!VectorCompare(to->origin, from->origin))
		bits |= U_ORIGIN;

	if (!VectorCompare(from->termination, to->termination))
		bits |= U_TERMINATION;

	if (!VectorCompare(to->angles, from->angles))
		bits |= U_ANGLES;

	if (to->animation1 != from->animation1 || to->animation2 != from->animation2)
		bits |= U_ANIMATIONS;

	if (to->event) // event is not delta compressed, just 0 compressed
		bits |= U_EVENT;

	if (to->effects != from->effects)
		bits |= U_EFFECTS;

	if (to->trail != from->trail)
		bits |= U_TRAIL;

	if (to->model1 != from->model1 || to->model2 != from->model2 || to->model3 != from->model3
			|| to->model4 != from->model4)
		bits |= U_MODELS;

	if (to->client != from->client)
		bits |= U_CLIENT;

	if (to->sound != from->sound)
		bits |= U_SOUND;

	if (to->solid != from->solid)
		bits |= U_SOLID;

	if (!bits && !force)
		return; // nothing to send

	// write the message

	Net_WriteShort(msg, to->number);
	Net_WriteShort(msg, bits);

	if (bits & U_ORIGIN)
		Net_WritePosition(msg, to->origin);

	if (bits & U_TERMINATION)
		Net_WritePosition(msg, to->termination);

	if (bits & U_ANGLES)
		Net_WriteAngles(msg, to->angles);

	if (bits & U_ANIMATIONS) {
		Net_WriteByte(msg, to->animation1);
		Net_WriteByte(msg, to->animation2);
	}

	if (bits & U_EVENT)
		Net_WriteByte(msg, to->event);

	if (bits & U_EFFECTS)
		Net_WriteShort(msg, to->effects);

	if (bits & U_TRAIL)
		Net_WriteByte(msg, to->trail);

	if (bits & U_MODELS) {
		Net_WriteByte(msg, to->model1);
		Net_WriteByte(msg, to->model2);
		Net_WriteByte(msg, to->model3);
		Net_WriteByte(msg, to->model4);
	}

	if (bits & U_CLIENT)
		Net_WriteByte(msg, to->client);

	if (bits & U_SOUND)
		Net_WriteByte(msg, to->sound);

	if (bits & U_SOLID)
		Net_WriteShort(msg, to->solid);
}

/*
 * @brief
 */
void Net_BeginReading(mem_buf_t *msg) {
	msg->read = 0;
}

/*
 * @brief
 */
void Net_ReadData(mem_buf_t *msg, void *data, size_t len) {
	size_t i;

	for (i = 0; i < len; i++) {
		((byte *) data)[i] = Net_ReadByte(msg);
	}
}

/*
 * @brief Returns -1 if no more characters are available.
 */
int32_t Net_ReadChar(mem_buf_t *msg) {
	int32_t c;

	if (msg->read + 1 > msg->size)
		c = -1;
	else
		c = (signed char) msg->data[msg->read];
	msg->read++;

	return c;
}

/*
 * @brief
 */
int32_t Net_ReadByte(mem_buf_t *msg) {
	int32_t c;

	if (msg->read + 1 > msg->size)
		c = -1;
	else
		c = (byte) msg->data[msg->read];
	msg->read++;

	return c;
}

/*
 * @brief
 */
int32_t Net_ReadShort(mem_buf_t *msg) {
	int32_t c;

	if (msg->read + 2 > msg->size)
		c = -1;
	else
		c = (int16_t) (msg->data[msg->read] + (msg->data[msg->read + 1] << 8));

	msg->read += 2;

	return c;
}

/*
 * @brief
 */
int32_t Net_ReadLong(mem_buf_t *msg) {
	int32_t c;

	if (msg->read + 4 > msg->size)
		c = -1;
	else
		c = msg->data[msg->read] + (msg->data[msg->read + 1] << 8) + (msg->data[msg->read + 2]
				<< 16) + (msg->data[msg->read + 3] << 24);

	msg->read += 4;

	return c;
}

/*
 * @brief
 */
char *Net_ReadString(mem_buf_t *msg) {
	static char string[MAX_STRING_CHARS];

	size_t l = 0;
	do {
		const int32_t c = Net_ReadChar(msg);
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string) - 1);

	string[l] = '\0';

	return string;
}

/*
 * @brief
 */
char *Net_ReadStringLine(mem_buf_t *msg) {
	static char string[MAX_STRING_CHARS];

	size_t l = 0;
	do {
		const int32_t c = Net_ReadChar(msg);
		if (c == -1 || c == 0 || c == '\n')
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string) - 1);

	string[l] = '\0';

	return string;
}

/*
 * @brief
 */
vec_t Net_ReadVector(mem_buf_t *msg) {

	const net_vec_t vec = {
		.i = Net_ReadLong(msg)
	};

	return vec.v;
}

/*
 * @brief
 */
void Net_ReadPosition(mem_buf_t *msg, vec3_t pos) {
	pos[0] = Net_ReadVector(msg);
	pos[1] = Net_ReadVector(msg);
	pos[2] = Net_ReadVector(msg);
}

/*
 * @brief
 */
vec_t Net_ReadAngle(mem_buf_t *msg) {
	return UnpackAngle(Net_ReadShort(msg));
}

/*
 * @brief
 */
void Net_ReadAngles(mem_buf_t *msg, vec3_t angles) {
	angles[0] = Net_ReadAngle(msg);
	angles[1] = Net_ReadAngle(msg);
	angles[2] = Net_ReadAngle(msg);
}

/*
 * @brief
 */
void Net_ReadDir(mem_buf_t *msg, vec3_t dir) {

	const int32_t b = Net_ReadByte(msg);

	if (b >= NUM_APPROXIMATE_NORMALS) {
		Com_Error(ERR_DROP, "%d out of range\n", b);
	}

	VectorCopy(approximate_normals[b], dir);
}

/*
 * @brief
 */
void Net_ReadDeltaMoveCmd(mem_buf_t *msg, const pm_cmd_t *from, pm_cmd_t *to) {

	*to = *from;

	const uint8_t bits = Net_ReadByte(msg);

	if (bits & CMD_ANGLE1)
		to->angles[0] = Net_ReadShort(msg);
	if (bits & CMD_ANGLE2)
		to->angles[1] = Net_ReadShort(msg);
	if (bits & CMD_ANGLE3)
		to->angles[2] = Net_ReadShort(msg);

	if (bits & CMD_FORWARD)
		to->forward = Net_ReadShort(msg);
	if (bits & CMD_RIGHT)
		to->right = Net_ReadShort(msg);
	if (bits & CMD_UP)
		to->up = Net_ReadShort(msg);

	if (bits & CMD_BUTTONS)
		to->buttons = Net_ReadByte(msg);

	to->msec = Net_ReadByte(msg);
}

/*
 * @brief
 */
void Net_ReadDeltaPlayerState(mem_buf_t *msg, const player_state_t *from, player_state_t *to) {

	*to = *from;

	const int32_t bits = Net_ReadShort(msg);

	if (bits & PS_PM_TYPE)
		to->pm_state.type = Net_ReadByte(msg);

	if (bits & PS_PM_ORIGIN)
		Net_ReadPosition(msg, to->pm_state.origin);

	if (bits & PS_PM_VELOCITY)
		Net_ReadPosition(msg, to->pm_state.velocity);

	if (bits & PS_PM_FLAGS)
		to->pm_state.flags = Net_ReadShort(msg);

	if (bits & PS_PM_TIME)
		to->pm_state.time = Net_ReadShort(msg);

	if (bits & PS_PM_GRAVITY)
		to->pm_state.gravity = Net_ReadShort(msg);

	if (bits & PS_PM_VIEW_OFFSET) {
		to->pm_state.view_offset[0] = Net_ReadShort(msg);
		to->pm_state.view_offset[1] = Net_ReadShort(msg);
		to->pm_state.view_offset[2] = Net_ReadShort(msg);
	}

	if (bits & PS_PM_VIEW_ANGLES) {
		to->pm_state.view_angles[0] = Net_ReadShort(msg);
		to->pm_state.view_angles[1] = Net_ReadShort(msg);
		to->pm_state.view_angles[2] = Net_ReadShort(msg);
	}

	if (bits & PS_PM_KICK_ANGLES) {
		to->pm_state.kick_angles[0] = Net_ReadShort(msg);
		to->pm_state.kick_angles[1] = Net_ReadShort(msg);
		to->pm_state.kick_angles[2] = Net_ReadShort(msg);
	}

	if (bits & PS_PM_DELTA_ANGLES) {
		to->pm_state.delta_angles[0] = Net_ReadShort(msg);
		to->pm_state.delta_angles[1] = Net_ReadShort(msg);
		to->pm_state.delta_angles[2] = Net_ReadShort(msg);
	}

	const int32_t stat_bits = Net_ReadLong(msg);

	for (int32_t i = 0; i < MAX_STATS; i++) {
		if (stat_bits & (1 << i))
			to->stats[i] = Net_ReadShort(msg);
	}
}

/*
 * @brief
 */
void Net_ReadDeltaEntity(mem_buf_t *msg, const entity_state_t *from, entity_state_t *to,
		uint16_t number, uint16_t bits) {

	*to = *from;

	to->number = number;

	if (bits & U_ORIGIN)
		Net_ReadPosition(msg, to->origin);

	if (bits & U_TERMINATION)
		Net_ReadPosition(msg, to->termination);

	if (bits & U_ANGLES)
		Net_ReadAngles(msg, to->angles);

	if (bits & U_ANIMATIONS) {
		to->animation1 = Net_ReadByte(msg);
		to->animation2 = Net_ReadByte(msg);
	}

	if (bits & U_EVENT)
		to->event = Net_ReadByte(msg);
	else
		to->event = 0;

	if (bits & U_EFFECTS)
		to->effects = Net_ReadShort(msg);

	if (bits & U_TRAIL)
		to->trail = Net_ReadByte(msg);

	if (bits & U_MODELS) {
		to->model1 = Net_ReadByte(msg);
		to->model2 = Net_ReadByte(msg);
		to->model3 = Net_ReadByte(msg);
		to->model4 = Net_ReadByte(msg);
	}

	if (bits & U_CLIENT)
		to->client = Net_ReadByte(msg);

	if (bits & U_SOUND)
		to->sound = Net_ReadByte(msg);

	if (bits & U_SOLID)
		to->solid = Net_ReadShort(msg);
}
