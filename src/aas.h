/* q3dm17 and q3dm19 use version 4??? */
#define AAS_SUPPORTV4 1

#ifdef  AAS_SUPPORTV4
#define AAS_VERSION_ 5
#endif

#define AAS_VERSION 5

#define AAS_FILETYPE *(unsigned long *) "EAAS"

extern char *aas_lump_strings[];

typedef enum aas_lump_e {
    ALUMP_CLUSTERS,
    ALUMP_VERTICES,
    ALUMP_PLANES,
    ALUMP_EDGES,
    ALUMP_FACEEDGES,
    ALUMP_FACES,
    ALUMP_AREAFACES,
    ALUMP_AREAS,
    ALUMP_AREASETTINGS,
    ALUMP_REACHABILITIES,
    ALUMP_NODES,
    ALUMP_entry11s,
    ALUMP_entry13entry11s,
    ALUMP_entry13s,
    AAS_NUMLUMPS
} aas_lump_info_t;

typedef struct aas_lump_s {
    unsigned long offset;
    unsigned long size;
} aas_lump_t;

typedef struct aas_header_s {
    unsigned long filetype;
    unsigned long version;      /* 4 or 5 */
    unsigned long crc;
    aas_lump_t lumps[AAS_NUMLUMPS];
} aas_header_t;

/* almost always the same for all maps */
typedef struct aas_cluster_s {
    long un_l0, un_l1;
    /* not entirely sure this is a bbox */
    vec3_t mins;
    vec3_t maxs;
} aas_cluster_t;

typedef vec3_t aas_vertex_t;

typedef struct aas_plane_s {
    vec3_t normal;
    float dist;
    long type;  /* 0,1,2 self-explanatory; what do 3,4,5 mean? */
} aas_plane_t;

typedef struct aas_edge_s {
    long start; /* references back to vertex list, presumably */
    long end;
} aas_edge_t;

typedef long aas_faceedge_t;

typedef struct aas_face_s {
    long planenum;
    long un_l0;
    long numfaceedges;
    long firstfaceedge;
    long un_l1;
    long un_l2;
} aas_face_t;

typedef long aas_areaface_t;

typedef struct aas_area_s {
    long settings;
    long numareafaces;
    long firstareaface;
    vec3_t mins;
    vec3_t maxs;
    vec3_t origin;  /* ??? point is always within the bounding box above */
} aas_area_t;

typedef struct aas_areasetting_s {
    long un_l0; /* is one of these contents? */
    long un_l1;
    long un_l2;
    long un_l3;
    long un_l4; /* connected to num_areas in some way? */
    long numreachabilities;
    long firstreachability;
} aas_areasetting_t;

typedef struct aas_reachability_s {
    long area;
    long un_l0;
    long edge;
    vec3_t start;   /* ??? */
    vec3_t end;
    long un_l1;
    long un_l2;
} aas_reachability_t;

/* similar to clipnodes in Quake 1? */
typedef struct aas_node_s {
    long planenum;
    long children[2];   /* negative children are -(areas+1) */
} aas_node_t;

typedef long aas_entry11_t[5];  /* first entry11 value related to area? */

/* just an index of entry11s for entry13s */
typedef long aas_entry13entry11_t;

typedef struct aas_entry13_s {
    long un_l0; /* related to 4th entry11 value? */
    long un_l1; /* related to 5th entry11 value? */
    long numentry13entry11s;
    long firstentry13entry11;
} aas_entry13_t;

typedef struct aas_s {
    unsigned long num_clusters;
    unsigned long num_vertices;
    unsigned long num_planes;
    unsigned long num_edges;
    unsigned long num_faceedges;
    unsigned long num_faces;
    unsigned long num_areafaces;
    unsigned long num_areas;
    unsigned long num_areasettings;
    unsigned long num_reachabilities;
    unsigned long num_nodes;
    unsigned long num_entry11s;
    unsigned long num_entry13entry11s;
    unsigned long num_entry13s;
    
    aas_cluster_t *clusters;
    aas_vertex_t *vertices;
    aas_plane_t *planes;
    aas_edge_t *edges;
    aas_faceedge_t *faceedges;
    aas_face_t *faces;
    aas_areaface_t *areafaces;
    aas_area_t *areas;
    aas_areasetting_t *areasettings;
    aas_reachability_t *reachabilities;
    aas_node_t *nodes;
    aas_entry11_t *entry11s;
    aas_entry13entry11_t *entry13entry11s;
    aas_entry13_t *entry13s;
} aas_t;

extern aas_t aas;

aboolean AAS_Load_Map (char *mapname);
void	 AAS_Free (void);
