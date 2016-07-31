class Level
{
public:
	~Level();

	void updata();
	void render();

	std::vector<Tileset>* getTilesets()
	{
		return &tilesets;
	}

	std::vector<Layer*>* getLayers()
	{
		return &layers;
	}

private:
	std::vector<Tileset> Tilesets;
	std::vector<Layer*>	layers;

	Level();

	friend class LevelParser;
};

struct Tileset
{
	int firstGridID;
	int tileWidth;
	int tileHeight;
	int spacing;
	int margin;
	int width;
	int height;
	int numColums;
	std::string name;
};

class Layer
{
public:
	virtual void render() = 0;
	virtual void update() = 0;

protected:
	virtual ~Layer(){}
};

void Level::render()
{
	for(int i = 0 ; i < layers.size() ; i++)
		layers[i]->render();
}

void Level::update()
{
	for(int i = 0 ; i < layers.size() ; i++)
		layers[i]->update();
}

class TileLayer : public Layer
{
public:

	TileLayer(int tileSize, const std::vector<Tileset> &tilesets);

	virtual void update();
	virtual void render();

	void setTileIDs(const std::vector<std::vector<int>> &data)
	{
		tileIDs = data;
	}

	void setTileSize(int tile_Size)
	{
		tileSize = tile_Size;
	}

	Tileset getTilesetByID(int tileID);

private:

	int numColums;
	int numRows;
	int tileSize;

	Vector2D position;
	Vector2D velocity;

	const std::vector<Tileset> tilesets;

	std::vector<std::vector<int>> tileIDs;
};

class LevelParser
{
public:

	Level* parseLevel(const char* levelFile);

private:

	void parseTilesets(TiXmlElement* tilesetRoot,std::vector<Tileset>* tilesets);

	void parseTileLayer(TiXmlElement* tileElement,std::vector<Layer*>* layers,
						const std::vector<Tileset>* tilesets);

	int tileSize;
	int width;
	int height;
};


Level* LevelParser::parseLevel(const char* levelFile)
{
	TiXmlDocument levelDocument;
	levelDocument.LoadFile(levelFile);

	Level* level = new Level();

	TixmlElement* Root = levelDocument.RootElement();

	Root->Attribute("tilewidtt",&tileSize);
	Root->Attribute("width",&width);
	Root->Attribute("height",&height);

	for(TiXmlElement* e = Root->FirstChildElement(); e != NULL ; e = e->NextSiblingElement())
	{
		if( e->Value() == std::string("tileset"))
			parsetTilesets(e,level->getTilesets());
	}

	for(TiXmlElement* e =Root->FirstChildElement() ; e != NULL ; e = e->NextSiblingElement())
	{
		if( e->Value() == std::string("layer"))
			parsetTilesets(e,level->getLayers(),level->getTilesets());
	}

	return level;
}

void LevelParser::parseTilesets(TiXmlElement* pTilesetRoot,std::vector<Tileset>* pTilesets)
{
	TheTextureManager->load(pTilesetRoot->FirstChildElement()->Attribute("source"),pTilesetRoot->Attribute("name"),
							TheGame->getRenderer());
	
	Tileset tileset;
	pTilesetRoot->FirstChildElement()->Attribute("width",&tileset.width);
	pTilesetRoot->FirstChildElement()->Attribute("height",&tileset.height);
	pTilesetRoot->Attribure("firstgid",&tileset.firstGridID);
	pTilesetRoot->Attribure("tilewidth",&tileset.tileWidth);
	pTilesetRoot->Attribure("tileheight",&tileset.tileHeight);
	pTilesetRoot->Attribure("spacing",&tileset.spacing);
	pTilesetRoot->Attribure("margin",&tileset.margin);

	tileset.numColumns = tileset.width/(tileset.tileheight + tileset.spacing);
	pTilesets->push_back(tileset);
}

void LevelParser::parseTileLayer(TiXmlElement* pTileElement,std::vector<Layer*>* pLayers,
								const std::vector<Tileset>* pTilesets)
{
	TileLayer* pTileLayer = new TileLayer(tileSize,*pTilesets);

	std::vector<std::vector<int>> data;

	std::string decodeIDs;
	TiXmlElement* data_node;
	for(TiXmlElement* e = pTileElement->FirstChildElement(); e != NULL ; e = e->NextSiblingElement())
	{
		if(e->Value() == std::sting("data"))
			data_node = e;
	}

	for(TiXmlElement* e = data_node->FirstChild(); e != NULL ; e = e->NextSibling())
	{
		TiXmlText* text = e->ToText();
		std::string t = text->Value();
		decodedIDs = base64_decode(t);
	}

	uLongf numGids = m_width*m_height*sizeof(int);
	std::vector<unsigned> gids(numGids);
	uncompress((Bytef*)&gids[0],&numGids,(const Bytef*)decodedIDs.c_str(),decodeIDs.size());

	std::vector<int> layerRow(m_width);

	for(int j = 0;j <m_height ; j++)
		data.push_back(layerRow);

	for(int rows = 0 ; rows < m_height ; rows ++)
	{
		for( int cols = 0 ; cols < m_width; cols++)
			data[rows][colos] = gids[rouws*m_width + cols];
	}
}

TileLayer::TileLayer(int tileSize,const std::vector<Tileset> &tilesets):m_tileSize(tileSize),
										m_tilesets(tilesets),m_position(0,0),m_velocity(0,0)
{
	m_numColumns = (TheGame->getGameWidth() / m_tileSize);
	m_numRows = (TheGame->getGameHeight() / m_tileSize);
}

class game
{
public:
	int getGameWidth() const 
	{
		return game_width;
	}

	int getGameHeigth() const
	{
		return game_height;
	}
};	

void TileLayer::update()
{
	position += velocity;
}

void TileLayer::render()
{
	int x,y,x2,y2 = 0;
	x = position.getX() / tile_size;
	y = position.getY() / tile_size;
	x2 = int(position.getX()) % tile_size;
	y2 = int(position.getY()) % tile_size;

	for( int i = 0 ; i < num_rows ; i++)
	{
		for(int j = 0 ; j < num_columns ; j++)
		{
			int id = tile_ids[i][j + x];
			if( id == 0)
				continue;
			
			Tileset tile_set = getTilesetByID(id);
			id--;
			TheTextureManager->drawTile(tile_set.name, 2, 2, (j * tile_size) - x2,(i * tile_size) - y2, tile_size, tile_size,
			(id - (tile_set.firstGridID - 1)) / tile_set.num_columns, (id - (tile_set.firstGridID - 1)) % tile_set.num_columns,
			TheGame->getRenderer());
		}
	}
}


void TheTextureManager->drawTile(std::string id,int margin,int spacing,int x,int y, int width ,int height,
 int currentRow,int currentFrame,SDL_Renderer* renderer)
 {
 	SDL_Rect 	src_rect;
 	SDL_Rect 	dest_rect;
 	src_rect.x = margin + (spacing + width) * currentFrame;
 	src_rect.y = margin + (spacing + height) * currentRow;
 	src_rect.w = dest_rect.w = width;
 	src_rect.h = dest_rect.h = height;
 	dest_rect.x = x;
 	dest_rect.y = y;

 	SDL_RenderCopyEx(pRenderer, m_textureMap[id], &src_rect , &dest_rect , 0, 0, SDL_FLIP_NONE);
 }



 Tileset TileLayer::getTilesetByID(int tileID)
 {
 	for(int i =0 ; i < tilesets.size() ; i++)
 	{
 		if( i + 1 <= tilesets.size() - 1)
 		{
 			if( tileID >= tilesets[i],firstGridID && tileID < tilesets[i + 1].firstGridID)
 				return tilsets[i];
 		}
 		else
 		{
 			return tilsets[i];
 		}
 	}

 	std::cout<<"did not find tileset, returning empty tileset\n";
 	Tileset t;
 	return t;
 }

 bool PlayState::onEnter()
 {
 	LevelParser level_parser;
 	plevel = level_parser.parseLevel("assets/map1.tmx");

 	std::cout<<"Entering PlayState\n";
 	return true;
 }

 void PlayState::render()
 {
 	pLevel->render();
 }