#include "LuaChannel.h"

LuaChannel::LuaChannel(Network::EventPoller* poller,int fd, LuaFish* lua, uint32_t header) : Network::Channel(poller, fd) {
	lua_ = lua;
	header_ = header;
	need_ = 0;
	reference_ = 0;
	dataReference_ = 0;
	closeRefernce_ = 0;
	errorRefernce_ = 0;
}

LuaChannel::~LuaChannel() {

}

void LuaChannel::HandleRead() {
	lua_State* L = lua_->GetScript().state();

	lua_rawgeti(L, LUA_REGISTRYINDEX, dataReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
	
	if (header_ == 0) {
		if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
			LOG_ERROR(fmt::format("HandleRead error:{}", lua_tostring(L, -1)));
		}
	} else {
		while(IsAlive()) {
			if (need_ == 0) {
				if (reader_->total_ < header_) {
					break;
				}
				if (header_ == 2) {
					uint8_t header[2];
					reader_->ReadData((char*)header, 2);
					need_ = header[0] | header[1] << 8;
				} else if (header_ == 4) {
					uint8_t header[4];
					reader_->ReadData((char*)header, 4);
					need_ = header[0] | header[1] << 8 | header[2] << 16 | header[3] << 24;
				}
			} else {
				if (reader_->total_ < need_) {
					break;
				}

				char* data = malloc(need_);
				reader_->ReadData(data, need_);
				
				lua_pushlightuserdata(L, data);
				lua_pushinteger(L, need_);

				if (LUA_OK != lua_pcall(L, 3, 0, 0)) {
					LOG_ERROR(fmt::format("HandleRead error:{}", lua_tostring(L, -1)));
				}

				need_ = 0;
				free(data);
			}
		}
	}
}

void LuaChannel::HandleClose() {
	lua_State* L = lua->GetScript()->state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, closeReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("HandleClose error:{}", lua_tostring(L, -1)));
	}
}

void LuaChannel::HandleError() {
	lua_State* L = lua->GetScript()->state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, errorReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("HandleError error:{}", lua_tostring(L, -1)));
	}
}

void LuaChannel::SetReference(int reference) {
	reference_ = reference;
}

int LuaChannel::GetReference() {
	return reference_;
}

void LuaChannel::SetDataReference(int reference) {
	dataReference_ = reference;
}

int LuaChannel::GetDataReference() {
	return dataReference_;
}

void LuaChannel::SetCloseReference(int reference) {
	closeReference_ = reference;
}

int LuaChannel::GetCloseReference() {
	return closeReference_;
}

void LuaChannel::SetErrorReference(int reference) {
	errorReference_ = reference;
}

int LuaChannel::GetErrorReference() {
	return errorReference_;
}

int LuaChannel::Read(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	int size = luaL_optinteger(L, 2, 0);

	int total = reader_->total_;
	if (total == 0) {
		return 0;
	}

	if (size == 0 || size > total) {
		size = total;
	}

	char* data = mallo(size);
	reader_->ReadData(data, size);
	lua_pushlstring(L, data, size);
	free(data);

	return 1;
}

int LuaChannel::Write(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	int header = luaL_optinteger(L, 2, 0);
	int vt = lua_type(L, 3);

	char* data;
	size_t size;
	switch(vt) {
		case LUA_TSTRING: {
			char* data = (char*) lua_tolstring(L, 3, &size);
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			data = (char*)lua_touserdata(L, 3);
			size = luaL_checkinteger(L, 4);
		}
		default: {
			luaL_error(L, "channel:%p write error:unknow lua type:%s", channel, lua_typename(L,vt));
		}
	}

	if (size <= 0) {
		luaL_error(L, "channel:%p write error:empty content", channel);
	}

	char* content = NULL;
	if (header == 0) {
		content = data;
		if (vt == LUA_TSTRING) {
			content = malloc(size);
			memcpy(content, data, size);
		}
	} else if (header == 2) {
		ushort length = size + header;
		content = malloc(size + header);
		memcpy(content, &length, 2);
		memcpy(content, data, size);
		if (vt == LUA_TLIGHTUSERDATA) {
			free(data);
		}
		size = length;
	} else if (header == 4) {
		uint32_t length = size + header;
		content = malloc(size + header);
		memcpy(content, &length, 4);
		memcpy(content, data, size);
		if (vt == LUA_TLIGHTUSERDATA) {
			free(data);
		}
		size = length;
	} else {
		luaL_error(L, "channel:%p write error:error header size:%d", channel, header);
	}

	int status = channel->Write(content, size);

	lua_pushboolean(L, status == 0);
	return 1;
}

int LuaChannel::Close(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	bool rightnow = luaL_optinteger(L, 2, 1);
	channel->Close(rightnow);
	luaL_unref(L, LUA_REGISTRYINDEX, channel->GetReference());
	return 0;
}

int LuaChannel::Release(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	luaL_unref(L, LUA_REGISTRYINDEX, channel->GetDataReference());
	luaL_unref(L, LUA_REGISTRYINDEX, channel->GetCloseReference());
	luaL_unref(L, LUA_REGISTRYINDEX, channel->GetErrorReference());
	channel->~LuaChannel();
	return 0;
}
