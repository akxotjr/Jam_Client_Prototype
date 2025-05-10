#pragma once


class SceneManager
{
	DECLARE_SINGLETON(SceneManager)

public:
	void					Init();
	void					Update();
	void					Render();

	void					ChangeScene(SceneType sceneType);

	SceneRef				GetCurrentScene() { return _scene; }

	void					SetRoomId(const uint32& roomId) { _roomId = roomId; }
	void					SetIsInRoom(bool in) { _isInRoom = in; }
	void					SetRoomList(const unordered_map<uint32, Vector<uint32>>& roomList) { _roomList = roomList; }

	uint32					GetRoomId() const { return _roomId; }
	int32					GetPlayerCount() { return _roomList[_roomId].size(); }
	bool					GetIsInRoom() const { return _isInRoom; }
	const unordered_map<uint32, Vector<uint32>>& GetRoomList() { return _roomList; }

private:
	SceneRef				_scene = nullptr;
	SceneType				_sceneType = SceneType::None;

	uint32					_roomId = 0;
	bool					_isInRoom = false;

	unordered_map<uint32, Vector<uint32>> _roomList;
};

