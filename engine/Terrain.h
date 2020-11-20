#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include <vector>
#include "VertexMesh.h"


struct HeightMapInfo {        // Heightmap structure
	int terrainWidth;        // Width of heightmap
	int terrainHeight;        // Height (Length) of heightmap
	Vector3D* height_map;    // Array to store terrain's vertex positions
	Vector3D* forward_seam_hm;  //  Array to store the vertex positions of the seam along the upper edge of chunk
	Vector3D* right_seam_hm; // Array to store the vertex positions of the seamalong the rightward edge of chunk
	Vector4D* terrain_color; //Array to store terrain's type 
	Vector4D* forward_seam_tc; //Array to store terrain's type 
	Vector4D* right_seam_tc; //Array to store terrain's type 
};

//Terrain Chunk Size
/*
Terrain Chunk size is the origin offset value per chunk.
This is calculated at time of terrain generation to avoid needing to update constant buffers for every terrain render call.
The terrain chunk size INCLUDES the size of the per-chunk seam which is used to blend different LOD chunks together without tearing.
*/
#define CHUNK_AND_SEAM_SIZE (33) //32x32 bitmap chunks + 1 unit wide seam
#define SEAMLESS_CHUNK (CHUNK_AND_SEAM_SIZE - 1) //size of a chunk excluding the seam
//

//Y vertex erosion////////////////////////
#define EROSION (1)
//XZ vertex scale/////////////////////////
#define TERRAIN_XZ_SCALE (1)
//how much to scale terrain that is loaded from a prepared text file
#define PRELOADED_SCALE (4)

enum SeamLOD
{
	HIGH,
	HIGH_MID,
	MID,
	MID_LOW,
	LOW
};

/* TODO: adjust terrain generation to read from a second heightmap which works as a multiplier to provide a more smooth mesh */
class Terrain
{
public:
	//constructor to load terrain data directly from a prepared text file
	Terrain(Vector2D map_pos, bool from_generated = false);
	//constructor to load terrain piece by piece from image file.  The data can then be saved to a text file with outputFile().
	Terrain(const char* filename_heightmap, const char* filename_terraintype, Vector2D map_pos);

	Terrain(const std::vector<VertexMesh>& verts);
	~Terrain();
	//vb_id is the indicator about which vertex buffer to load (the terrain chunk, the forward seam, or the right seam in that order)
	void render(int lod = 0, int vb_id = 0, int seam_lod = HIGH);

public:
	//Vector3D clampToTerrain(const Vector3D& pos);
	float getTerrainY(const Vector3D& pos);
	//Get the world position x/z of the currently loaded chunk's center
	Vector2D getCenter();

	//Get the world position x/y/z of the currently loaded chunk's 4 corners
	void getCorners(Vector3D* corner_array);

public:
	//output the loaded data to a text file
	void outputFile(Vector2D output_offset);
	//update the currently loaded texture splat data from a new image
	void updateTextureSplat(const char* file, Vector2D max);

private:

	bool fetchMaps(const char* height_filename, const char* tex_filename, HeightMapInfo& hminfo, Vector2D chunk_id);
	//function which reads the entirety of an image file to create a heightmap
	bool fetchHeightMap(const char* filename, HeightMapInfo& hminfo);
	//function which reads a one-chunk section of an image file to create a height map for only one section
	bool fetchHeightMap(const char* filename, HeightMapInfo& hminfo, Vector2D chunk_id);
	//retrieve the terrain balances at each vertex (Very important that the terrain type file is the SAME size as the height map!)
	bool fetchTerrainTypes(const char* filename, HeightMapInfo& hminfo, Vector2D chunk_id);

	//sample a bitmap image in several locations to provide the average surrounding height
	float sampleHeight(const unsigned char* bmp, int step, int start_offset, int row_step, int bmp_max);
	//calculate the averaged height of seam caps for a seam
	void findCaps(const std::vector<VertexMesh> chunk, int height, int width, VertexMesh* topleft, VertexMesh* topright, VertexMesh* right);

private:
	void loadChunkFromText(bool from_generated);
	void loadSeamRFromText(bool from_generated);
	void loadSeamFFromText(bool from_generated);

private:
	HeightMapInfo m_heightmap_info;
	int m_num_faces;
	int m_num_vertexes;

private:
	VertexBufferPtr m_vertex_buffer;
	VertexBufferPtr m_seam_vb_right;   // seam which runs along the right edge of chunk
	VertexBufferPtr m_seam_vb_forward; // seam which runs along the top edge of chunk

	int loaded_cols = 0;
	int loaded_rows = 0;
	bool m_has_right_seam = false;
	bool m_has_forward_seam = false;
	bool m_left_buffer = false;
	bool m_bottom_buffer = false;

	std::vector<VertexMesh> m_vec_vm_chunk;
	std::vector<VertexMesh> m_vec_vm_seamR;
	std::vector<VertexMesh> m_vec_vm_seamF;

	Vector2D m_pos;
};

