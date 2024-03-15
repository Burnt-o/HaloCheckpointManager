#pragma once

// This class could've been a simple enum class, but wrapping it like this allows deleting default constructor and adding a few helper methods
class GameState
{

public: 
	enum class Value : uint8_t
	{
		// values are equal to MCC's gameIndicator
		Halo1 = 0,
		Halo2 = 1,
		Halo3 = 2,
		Halo4 = 3,
		Halo2MP = 4,
		Halo3ODST = 5,
		HaloReach = 6,
		NoGame = 255,
	};
	GameState() = delete; // no default construction
	constexpr  GameState(const Value val) : value(val) {}
	constexpr bool operator==(const GameState a) const { return value == a.value; }
	constexpr bool operator!=(const GameState a) const { return value != a.value; }
	constexpr operator Value() const { return value; } // allows switching
	explicit operator bool() const = delete; // prevents "if(gameState)"

	constexpr explicit operator int() { return (int)value; } // to int
	constexpr explicit GameState(const int val) : value((Value)val) {} // from int
	std::string toString() const { // to string
		auto it = GameStateStringBiMap.left.find(value);
		if (it == GameStateStringBiMap.left.end()) return "undefined";
		else return GameStateStringBiMap.left.at(value);
	}

	static GameState fromString(const std::string val) // from string (used in pointerDataParser to parse xml)
	{
		auto it = GameStateStringBiMap.right.find(val);
		if (it == GameStateStringBiMap.right.end()) throw HCMInitException(std::format("Bad GameState string value: {}", val));
		else return (GameState)GameStateStringBiMap.right.at(val);
	}

	friend std::ostream& operator<< (std::ostream& out, const GameState& g) // to string stream
	{
		return out << g.toString();
	}

	std::wstring toModuleName() const { // to string
		if (!GameStateModuleNameMap.contains(value)) throw HCMRuntimeException("bad value passed to toModuleName");
		return GameStateModuleNameMap.at(value);
	}

private:
	Value value;
	static inline const boost::bimap<Value, std::string> GameStateStringBiMap = boost::assign::list_of<boost::bimap<Value, std::string>::relation> // used in toString/fromString
		(Value::Halo1, "Halo1")
		(Value::Halo2, "Halo2")
		(Value::Halo3, "Halo3")
		(Value::Halo4, "Halo4")
		(Value::Halo2MP, "Halo2MP")
		(Value::Halo3ODST, "Halo3ODST")
		(Value::HaloReach, "HaloReach")
		(Value::NoGame, "NoGame");

	static inline const std::map<Value, std::wstring> GameStateModuleNameMap =  // used in toModuleName
		boost::assign::map_list_of 
		(Value::Halo1, L"halo1.dll")
		(Value::Halo2, L"halo2.dll")
		(Value::Halo3, L"halo3.dll")
		(Value::Halo4, L"halo4.dll")
		(Value::Halo2MP, L"groundhog.dll")
		(Value::Halo3ODST, L"halo3odst.dll")
		(Value::HaloReach, L"haloreach.dll");
};

static inline const std::vector<GameState> AllGameStateValues
{
	GameState::Value::Halo1,
		GameState::Value::Halo2,
		GameState::Value::Halo2MP,
		GameState::Value::Halo3,
		GameState::Value::Halo4,
		GameState::Value::Halo3ODST,
		GameState::Value::HaloReach,
		GameState::Value::NoGame
};


static inline const std::vector<GameState> AllSupportedGames
{
		GameState::Value::Halo1,
		GameState::Value::Halo2,
		GameState::Value::Halo3,
		GameState::Value::Halo4,
		GameState::Value::Halo3ODST,
		GameState::Value::HaloReach,
};

static inline const std::vector<GameState> DoubleCheckpointGames // all except halo 1
{
	GameState::Value::Halo2,
	GameState::Value::Halo3,
	GameState::Value::Halo4,
	GameState::Value::Halo3ODST,
	GameState::Value::HaloReach,
};





