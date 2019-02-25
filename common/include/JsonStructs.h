#pragma once

#include "PrimitiveDef.h"
#include "share_def.h"

#include "../iguana/iguana/json.hpp"
#include <string>

struct UserHallLoginRequest
{
	UserIDType userid;
	std::string password;
};
REFLECTION(UserHallLoginRequest, userid, password);

struct HallLoginResponseBase
{
	UserIDType userid;
	HallResponseStatusCode status;
};
REFLECTION(HallLoginResponseBase, userid, status);

struct UserHallLoginResponse : HallLoginResponseBase
{
	std::string avatarUrl;
	uint8_t avatarId;
};
REFLECTION(UserHallLoginResponse, userid, status, avatarUrl, avatarId);