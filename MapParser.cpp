#include "MapParser.hpp"

LoadedStage MapParser::parse(const String& csvPath)
{
	LoadedStage out;
	const CSV csv{ csvPath };

	if (not csv)
	{
		throw Error{ U"Failed to load CSV {}"_fmt(csvPath) };
	}

	out.mapSize = Point(static_cast<int32>(csv.columns(0)),
		static_cast<int32>(csv.rows())) * kBlockSize;

	for (size_t row = 0; row < csv.rows(); ++row)
	{
		for (size_t col = 0; col < csv.columns(row); ++col)
		{
			const String cell = Parse<String>(csv[row][col]);
			if (cell.isEmpty()) continue;

			const Vec2 cellPos{
				static_cast<double>(col) * kBlockSize,
				static_cast<double>(row) * kBlockSize
			};
			const CellContext ctx{ cell, cellPos, col, row, out };

			if (auto handler = lookupHandler(cell))
			{
				handler(ctx);
			}
		}
	}

	return out;
}

MapParser::Handler MapParser::lookupHandler(const String& cell)
{
	if (cell.isEmpty()) return nullptr;
	switch (cell[0])
	{
	case U'B': return &handleBlock;
	case U'N': return &handleNonCollidable;
	case U'H': return &handleHalfBlock;
	case U'p': return &handlePlayerStart;
	case U'i': return &handleItem;
	case U'E': return &handleEnemy;
	case U'G': return &handleGoal;
	case U'C': return &handleBossArea;
	default:
		if (cell == U"1") return &handlePlainOne;
		return nullptr;
	}
}

int MapParser::parseTwoDigit(const String& cell, size_t start)
{
	if (cell.size() < start + 2) return 0;
	return (cell[start] - U'0') * 10 + (cell[start + 1] - U'0');
}

void MapParser::handleBlock(const CellContext& c)
{
	const int num = parseTwoDigit(c.cell, 1);
	c.out.blocks << BlockSpawn{ c.cellPos, num, 1 };
}

void MapParser::handleNonCollidable(const CellContext& c)
{
	const int num = parseTwoDigit(c.cell, 1);
	c.out.blocks << BlockSpawn{ c.cellPos, num, -1 };
}

void MapParser::handleHalfBlock(const CellContext& c)
{
	const int num = parseTwoDigit(c.cell, 1);
	c.out.blocks << BlockSpawn{ c.cellPos, num, 2 };
}

void MapParser::handlePlayerStart(const CellContext& c)
{
	c.out.playerStart = c.cellPos;
	c.out.playerStartSet = true;
}

void MapParser::handleItem(const CellContext& c)
{
	if (c.cell.size() < 2) return;
	c.out.items << ItemSpawn{ c.cellPos, c.cell.narrow()[1] - '0' };
}

void MapParser::handleEnemy(const CellContext& c)
{
	if (c.cell.size() < 2) return;
	int num = 1;
	switch (c.cell[1])
	{
	case U'1': num = 1; break;
	case U'2': num = 2; break;
	case U'3': num = 3; break;
	case U'9': num = 9; break;
	default:   num = 1; break;
	}
	c.out.enemies << EnemySpawn{ c.cellPos, num };
}

void MapParser::handleGoal(const CellContext& c)
{
	const int num = parseTwoDigit(c.cell, 1);
	c.out.blocks << BlockSpawn{ c.cellPos, num, 4 };
}

void MapParser::handleBossArea(const CellContext& c)
{
	if (c.cell.size() < 2) return;
	const Vec2 bossCenter{
		(static_cast<double>(c.col) - 0.5) * kBlockSize,
		(static_cast<double>(c.row) - 0.5) * kBlockSize
	};
	c.out.bossArea << bossCenter;
	if (c.cell[1] == U'N' && c.cell.size() >= 4)
	{
		const int num = parseTwoDigit(c.cell, 2);
		c.out.blocks << BlockSpawn{ c.cellPos, num, -1 };
	}
}

void MapParser::handlePlainOne(const CellContext& c)
{
	c.out.blocks << BlockSpawn{ c.cellPos, 1, 1 };
}
