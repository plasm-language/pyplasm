#ifndef _CONFIG_H__
#define _CONFIG_H__

///////////////////////////////////////////////////////////////
//low level configs
///////////////////////////////////////////////////////////////

//spin lock default delta time
#define SPIN_LOCK_DEFAULT_MAX_DELTA_TIME 10

//for memory pool (all malloc with less than 4096 bytes are cached)
#define MEMPOOL_TABLE_SIZE  4096


///////////////////////////////////////////////////////////////
//Graph config
///////////////////////////////////////////////////////////////

// maximum dimension (16 seeems enough)
#define GRAPH_MAX_GEOMETRIC_DIMENSION 16

//for navigator of Graph (each level could contain a maximum number of cells)
#define GRAPH_MAX_CELLS_PER_LEVEL 65536


///////////////////////////////////////////////////////////////
//plasm config
///////////////////////////////////////////////////////////////

#define PLASM_DEFAULT_TOLERANCE ((float)1e-6)

#define PLASM_MAX_NUM_SPLIT 10

///////////////////////////////////////////////////////////////
//viewer/frustum defaults
///////////////////////////////////////////////////////////////
#define DEFAULT_FOV 60

#define DEFAULT_FPS 30

///////////////////////////////////////////////////////////////
//when you have problem with memory
///////////////////////////////////////////////////////////////
#define XGE_TRACK_MEMORY 0

///////////////////////////////////////////////////////////////
//octree
///////////////////////////////////////////////////////////////
#define OCTREE_LOOSE_K_DEFAULT 2.0f
#define OCTREE_MAX_DEPTH_DEFAULT 5



///////////////////////////////////////////////////////////////
//engine configuration
///////////////////////////////////////////////////////////////


#define ENGINE_AUTOMATIC_VBO_CREATION 1
#define ENGINE_ENABLE_TESSELLATOR     1
#define ENGINE_ENABLE_FBO             0
#define ENGINE_ENABLE_SHADERS         0
#define ENGINE_ENABLE_SHADOW_SHADER   0


///////////////////////////////////////////////////////////////
//shadow map config
///////////////////////////////////////////////////////////////

#if ENGINE_ENABLE_SHADOW_SHADER

#define SHADOWMAP_MAXIMIZE_Z_USE 1 //tune up frustum to optimize the light frustum
#define SHADOWMAP_TEXTURE_OFFSET 2 //leave texture0coords and texture1 for mesh, start from 2
#define SHADOWMAP_MAX_SPLITS 4 //max number of split
#define SHADOWMAP_SPLIT_WEIGHT 0.75f //split weight
#define SHADOWMAP_NUM_SPLITS 3 //number of splits
#define SHADOWMAP_DEPTH_SIZE 2048 //depth size

#endif //ENGINE_ENABLE_SHADOW_SHADER

#endif //_CONFIG_H__
