#pragma once

#include "Terrain.h"
#include "Prerequisites.h"
#include "ComputeShader.h"


#include <wrl/client.h>
#include <vector>
#include <thread>


#define TERRAIN_MAP_SIZE_MAX (64)

struct SeamRenderVectors
{
	std::vector<TerrainPtr> high;
	std::vector<TerrainPtr>	highmid;
	std::vector<TerrainPtr>	midhigh;
	std::vector<TerrainPtr>	mid;
	std::vector<TerrainPtr>	midlow;
	std::vector<TerrainPtr>	lowmid;
	std::vector<TerrainPtr>	low;
};

#define LOD_THRESHHOLD_HIGH (1)
#define LOD_THRESHHOLD_MID (3)
//#define LOD_THRESHHOLD_LOW (4)

//class for easier management of terrain loading threads
class TerrainChunk
{
public:
	//when adding new chunks while moving is_init should be false as the chunk data needs to be loaded.
	//when initializing the terrain manager partially out of map bounds, set is_init to tell the manager there is no need to load it.
	TerrainChunk(bool is_init) : m_is_loaded(is_init), m_is_queued(false) {}
	TerrainChunk(Vector2D map_pos) : m_is_loaded(true), m_is_queued(false), m_chunk(TerrainPtr(new Terrain(map_pos))) {}

	TerrainChunk(TerrainPtr chunk, Vector2D map_pos) : m_chunk(chunk), m_is_loaded(true), m_is_queued(false), m_is_culled(false) {}

	void reset()
	{
		m_is_loaded = false;
		m_is_queued = false;
		m_is_culled = false;
		m_chunk.reset();
	}

public:
	TerrainPtr m_chunk;
	bool m_is_loaded;
	bool m_is_queued;
	bool m_is_culled;
};


/*
right now the terrain manager loads data in one chunk at a time per thread.  we can probably input 
just one single structure into the terrain generator which contains the dimension of chunks we want to load and the starting position
of the load.  just pull from input[0] in each thread.
*/

class TerrainManager
{
public:
	//constructor for terrain manager that creates new terrain on the GPU and can continue infinitely in all directions.
	/* WAY faster than the other two methods */
	TerrainManager(Vector2D visible_chunk_count);

	//constructor which reads terrain data directly from text, much faster than from a bitmap
	//MUST be initialized with an odd number of visible chunks on X and Z axis
	TerrainManager(Vector2D visible_chunk_count, Vector2D map_size, Vector2D center_chunk);

	//load a terrain manager from a bitmap (very slow, used mainly just to output text files to load with the other constructor
	TerrainManager(const char* filename_heightmap, const char* filename_terraintype, Vector2D chunk_count, Vector2D offset, Vector2D chunkstart = Vector2D(0,0));
	~TerrainManager();

public:
	//update necessary resources before the render function
	void update();
	//function which is called to update resources whenever the camera enters a new chunk location
	void onNewChunk(int xdif, int ydif);
	//function which retrieves vertex data from the gpu when creating terrain from noise
	void onNewChunkCompute(Vector2D pos_change);

	//render the terrain chunks stored in m_map after sorting them into temporary vectors
	void render(int shader = -1, float bumpiness = 0.5f, bool is_wireframe = false, int is_HD = 0);

	//render terrain only in on LOD
	void renderInLOD(int shader, float bumpiness, bool is_wireframe = false, int is_HD = 0);

public:
	//output text files for every terrain chunk in the terrain manager
	void outputFiles();
	//update the texture splat data for every terrain chunk in the terrain manager
	void updateTerrainTypes();

private:
	//initialize index buffers for different LODs of terrain
	void initChunkIndexes();
	//initialize index buffers for the seams that connect adjacent chunks and close resolution gaps (this creates old indexes still used by text and cpu loading)
	void initSeamIndexesOld();
	//initialize index buffers for the seams that connect adjacent chunks and close resolution gaps (this creates indexes updated for the compute shader)
	void initSeamIndexesNew();
	//initialize rasterizers
	void initRasterizersAndStencil();
	//initialize terrain textures
	void initTexturePointers();

private: //functions for preparing the render process
	//toggle culling for each chunk
	void cullChunksByFrustum();
	//prepare LOD arrays for rendering order on this frame
	void prepareLODArrays(std::vector<TerrainPtr>* high, std::vector<TerrainPtr>* mid, std::vector<TerrainPtr>* low,
		SeamRenderVectors* forward, SeamRenderVectors* right);
	//select a seam render vector and input the appropriate terrain
	void chooseSeamArray(Vector2D pos, int source, int comp, SeamRenderVectors* srv);

private:
	//check the threads to see if anything should begin loading for text based terrain manager
	void checkThreadsForText();
	//different version of the check threads function for text manager that is based on the compute shader
	void checkThreadsForComputeShader();
	//generate new chunk data on the GPU
	void runComputeShader(Vector2D origin, Vector2D numchunks, std::vector<VertexMesh>& verts);

	//load chunk data on a seperate thread to be added to m_map when it is finished
	void threadLoadChunkTxt(Vector2D location, int thread_num);
	//load chunk data using a std::vector of data that was constructed on the GPU
	void threadLoadChunkCompute(Vector2D location, Vector2D relative_location, int thread_num);

	enum TerrainManagerMode
	{
		LOAD_BITMAP,
		USE_TXT,
		USE_COMPUTE_SHADER,
	};

	int m_mode;
	Vector2D m_file_output_offset;

public:
	static ComputeShader* getComputeShader() { return m_compute_terrain; }
	static Vector2D getComputeOrigin() { return m_compute_origin; }
	static Vector2D getComputeNumChunks() { return m_compute_numchunks; }
	static std::vector<VertexMesh>& getComputeVertexesX() { return m_compute_vertexes_x; }
	static std::vector<VertexMesh>& getComputeVertexesZ() { return m_compute_vertexes_z; }

	static void setMultithreadIsBusy(bool b) { m_multithread_compute_is_busy = b; }
private:
//================================
//  Chunk Thread Management Members
//--------------------------------
	//chunk that the player was in as of last frame
	Vector2D m_player_chunk;
	//viewable terrain chunks
	std::vector<std::vector<TerrainChunk>> m_map;
	//size of the entire map
	Vector2D m_mapsize;
	//reference point which serves as the "center" chunk
	Vector2D m_center;
	//amount of chunks actively renderable when in view
	Vector2D m_visible_chunks;
	//number of unloaded chunks that are awaiting a thread
	int m_num_unloaded_chunks;


	//compute shader for creating terrain on the GPU
	static ComputeShader* m_compute_terrain;
	static Vector2D m_compute_origin;
	static Vector2D m_compute_numchunks;
	static bool m_multithread_compute_is_busy;

	//vector of vertexes to save outputs seperately for when the compute shader outputs chunks along the x axis or z axis
	static std::vector<VertexMesh> m_compute_vertexes_x;
	static std::vector<VertexMesh> m_compute_vertexes_z;
	int m_remaining_compute_chunks_x = 0;
	int m_remaining_compute_chunks_z = 0;


	static const int THREADCOUNT = 5;
	//thread which loads chunk data as necessary
	std::shared_ptr<std::thread> m_chunk_thread[THREADCOUNT];
	//bool used to identify whether the thread is busy
	bool m_thread_is_busy[THREADCOUNT];
	//chunk that is currently being threaded
	Vector2D m_threaded_chunk_pos[THREADCOUNT];
	//temporary terrain pointer to store the threaded terrain
	TerrainPtr m_temp_terrain[THREADCOUNT];

//================================
//  Chunk Indexes
//--------------------------------

	IndexBufferPtr m_LOD_high;
	IndexBufferPtr m_LOD_mid;
	IndexBufferPtr m_LOD_low;

//================================

//================================
//  Seam Indexes
//--------------------------------

	IndexBufferPtr m_LOD_seam_high;
	IndexBufferPtr m_LOD_seam_mid;
	IndexBufferPtr m_LOD_seam_low;

	IndexBufferPtr m_LOD_midToHigh;
	IndexBufferPtr m_LOD_highToMid;

	IndexBufferPtr m_LOD_midToLow;
	IndexBufferPtr m_LOD_lowToMid;

//================================

//================================
//  D3D Resources
//--------------------------------
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rs_fill;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rs_wire;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_stencil;
//================================
//	Texture Pointers
//--------------------------------
	TexturePtr m_displace1_1;
	TexturePtr m_tex1;
	TexturePtr m_norm1;
	TexturePtr m_rough1;
	TexturePtr m_ambient_occ1;

	TexturePtr m_displace2_1;
	TexturePtr m_tex2;
	TexturePtr m_norm2;
	TexturePtr m_rough2;
	TexturePtr m_ambient_occ2;

	TexturePtr m_displace3_1;
	TexturePtr m_tex3;
	TexturePtr m_norm3;
	TexturePtr m_rough3;
	TexturePtr m_ambient_occ3;

	TexturePtr m_displace4_1;
	TexturePtr m_tex4;
	TexturePtr m_norm4;
	TexturePtr m_rough4;
	TexturePtr m_ambient_occ4;
};

