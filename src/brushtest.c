/* Aftershock 3D rendering engine
 * Copyright (C) 1999 Stephen C. Taylor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "a_shared.h"
#include "util.h"
#include "pak.h"
#include "cmap.h"
#include "entity.h"
#include "vec.h"
#include "brushtest.h"


static unsigned char brushflag[ 1025 ];

static vec3_t smins , smaxs ;
static int content_mask ;
static float radius ;
static vec3_t ray ;
static vec3_t org;
static vec3_t startmins , startmaxs ,endmins ,endmaxs ;
static trace_t * res ;

static float col_dist ;
static vec3_t col_start ,col_end ;
static cbrushside_t * col_bside ;
static cbrush_t * brush_hit =NULL;
static cplane_t * plane_hit =NULL;
static int model_index =-1;
static int collided =0;
static int col_start_solid=0;


/*
void Clip_CheckBrushes(float *start, float *end)
{
	model_t *M;
	ClipBSPModel *cm;
	brush_t *brush;
	brushside_t *bside;
	cplane_t *plane;
	vec3_t p1, p2, p;
	float  d1, d2, d;
	float dot1, dot2;
	int i, j, n, num, start_solid;
	word *pClipBrush;


	VectorCopy(end,col_end);	// end will not be used anywhere after this
	col_dist=1.0;
	col_bside=-1;
	col_object=-1;
	col_start_solid=0;

	cm=ClipBSPModels;
	pClipBrush=ClipBrushes;
	for(n=0;n<NumClipBSPModels;n++,cm++) {
		M=&cbsp->Models[cm->model_idx];

		VectorSubtract(start,M->origin,start);
		VectorSubtract(col_end,M->origin,col_end);

		for(j=0;j<cm->numClipBrushes;j++,pClipBrush++) {

			d1=0.0; d2=col_dist;
			VectorCopy(start,p1);
			VectorCopy(col_end,p2);

			start_solid=1;

			brush=&cbsp->Brushes[*pClipBrush];

			if ((brush->contents&CONTENTS_SOLID)==0) continue;

			bside=&cbsp->BrushSides[brush->firstside];
			num=brush->numsides;

			for(i=0;i<num;i++,bside++) {

				plane=&cbsp->Planes[bside->planenum];

				dot1=DotProduct(plane->normal,p1)
					-plane->dist-oradius;
				dot2=DotProduct(plane->normal,p2)
					-plane->dist-oradius;

				if (dot1>=0&&dot2>=0) {
					break;
				}

				if (dot1<0&&dot2<0) {
					continue;
				}

				d = dot1/(dot1-dot2);
				VectorSubtract(p2,p1,p);
				VectorScale(p,d,p);
				VectorAdd(p,p1,p);
				d = d1+(d2-d1)*d;

				if (dot1>=0) {
					VectorCopy(p,p1); d1=d;
					col_bside=i+brush->firstside;
					start_solid=0;
				}
				else {
					VectorCopy(p,p2); d2=d;
				}
			}

			if (i==num) {

				if (start_solid) {
					VectorAdd(start,M->origin,start);
					VectorCopy(start,col_end);
					col_dist=0.0;
					col_bside=-1;
					col_start_solid=1;
					col_object=cm->obj_idx;
					return;
				}

				VectorCopy(p1,col_end);
				col_dist=d1;
				col_object=cm->obj_idx;
			}
		}

		VectorAdd(start,M->origin,start);
		VectorAdd(col_end,M->origin,col_end);
	}
}*/

// TODO !!!!!
float BoxToPlane_Dist (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	float	dist1, dist2;
	int		sides;
	float dist ;
	int which ;


	if (DistanceSquared(col_start ,emins ) < DistanceSquared ( col_start , emaxs ))
	{
		which =1;
		// use mins 
	}
	else 
	{
		which =0;
		// use maxs 
	}

// fast axial cases
	if (p->type < 3)
	{
		if (which )
		{
			return 	emins[p->type] - p->dist ;
		}
		else 
		{
			return  emaxs[p->type] - p->dist;
		}
	}

// general case
	switch (p->signbits)
	{
	case 0:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 1:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 2:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 3:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 4:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 5:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 6:
		dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	case 7:
		dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		break;
	}

	sides = 0;
	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	return sides;
}

// Returns the Plane which has the smallest angle to ray 
// Any better way to find the plane ???

static cplane_t *  Find_Plane_Hit ( cbrush_t * brush )
{
	int i,planenum=0 ; 
	float dot;
	float bestangle = -1.0;

	
	for (i=0;i<brush->numsides;i++)
	{

		dot = DotProduct (brush->sides[i].plane->normal , ray );
		if ( dot > bestangle )
		{
			bestangle = dot ;
			planenum = i; 	
		}


	}
	
	return brush->sides[planenum].plane;
}



void Check_Brush ( cbrush_t *brush)
{


	cbrushside_t	*bside;
	cplane_t		*plane;
	vec3_t			p1;
	vec3_t			p2;
	vec3_t			p;
	float			d1;
	float			d2;
	float			d;
	float			dot1;
	float			dot2;
	int				i;
	int				start_solid;

	d1 = 0.f;
	d2 = col_dist;
	VectorCopy(col_start, p1);
	VectorCopy(col_end, p2);

	start_solid = 1;

	if ( !(brush->shaderref->contents & content_mask) ) return;

	bside = brush->sides;
	for (i = 0; i < brush->numsides; i++, bside++) {
		plane = bside->plane;

		dot1 = DotProduct(plane->normal, p1) - plane->dist - radius;
		dot2 = DotProduct(plane->normal, p2) - plane->dist - radius;

		if ((dot1 >= 0.f) && (dot2 >= 0.f))
			break;
		if ((dot1 < 0.f) && (dot2 < 0.f))
			continue;

		d = dot1 / (dot1 - dot2);
		VectorSubtract(p2, p1, p);
		VectorMA(p1, d, p, p);
		d = d1 + (d2 - d1) * d;

		if (dot1 >= 0.f) {
			VectorCopy(p, p1);
			d1 = d;
			col_bside = bside;
			start_solid = 0;
		} else {
			VectorCopy(p, p2);
			d2 = d;
		}
	}

	if (i == brush->numsides) {
		collided = 1;

		if (start_solid) {
			VectorCopy(col_start, col_end);
			col_dist = 0.f;
			col_bside = NULL;
			col_start_solid = 1;
		}

		plane_hit = Find_Plane_Hit ( brush );
		
		if (model_index > 0 )
		{
			// TODO !!!!
			// Translate the Plane 
		}


		brush_hit= brush;
		VectorCopy(p1, col_end);
		col_dist = d1;
	}







}


void Check_Leaf ( int num )
{
	cleaf_t * leaf = &cm.leaves[num];
	int i , brushnum,idx;
	unsigned char bit ;
	// Needed ??? 
	if ((smins[0] > leaf->maxs[0]) || (smaxs[0] < leaf->mins[0])) return;
	if ((smins[1] > leaf->maxs[1]) || (smaxs[1] < leaf->mins[1])) return;
	if ((smins[2] > leaf->maxs[2]) || (smaxs[2] < leaf->mins[2])) return;

	
	for (i=0;i<leaf->numbrushes ; i++ )
	{
		brushnum = cm.lbrushes[leaf->firstbrush +i ];
		idx = brushnum >> 3;
		bit = (unsigned char) (1 << (brushnum & 7));
		if (!(brushflag[idx] & bit)) {
			brushflag[idx] |= bit;
			Check_Brush(&cm.brushes[brushnum]);
		}
		
	}


}

void Check_Node ( int num )
{
	cnode_t * node = &cm.nodes[num];
	
	if ((smins[0] > node->maxs[0]) || (smaxs[0] < node->mins[0])) return;
	if ((smins[1] > node->maxs[1]) || (smaxs[1] < node->mins[1])) return;
	if ((smins[2] > node->maxs[2]) || (smaxs[2] < node->mins[2])) return;


	if (node->children[0] >0 )
	{
		Check_Node (node->children[0]);
	}
	else 
		Check_Leaf ( - (node ->children[0] +1 ));

	if (node->children[1] >0 )
	{
		Check_Node (node->children[1]);
	}
	else 
		Check_Leaf ( - (node ->children[1] +1 ));


}

void Check_Model ( int num )
{
	vec3_t origin ;
	cmodel_t * model = &cm.models[num];
	cbrush_t * brush;
	int i;
	
	// Is this right ??

	VectorAdd (model->mins ,model->maxs ,origin );
	VectorScale ( origin ,0.5 ,origin );

	VectorCopy ( origin , org );

	VectorAdd (col_start , origin ,col_start );
	VectorAdd (col_end ,origin ,col_end );

	brush= model->brushes;
	for (i=0;i<model->numbrushes;i++,brush++)
	{
		Check_Brush(brush);
	}

	VectorSubtract ( col_start, origin ,col_start );
	VectorSubtract ( col_end , origin,col_end );

	VectorClear ( org );

}


void Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask )
{
	int i;
	vec3_t min1 ,max1 , min2 ,max2;

	VectorAdd (mins ,start , min1 );
	VectorAdd (maxs ,start , max1 );

	VectorAdd (mins ,end , min2 );
	VectorAdd (maxs ,end , max2 );

	Merge_BoundingBoxes (min1,max1,min2,max2,smins,smaxs );

	 radius =RadiusFromBounds(mins, maxs );

	res = results ;
	content_mask = contentmask ;

	col_dist =1.0;
	collided =0;
	col_start_solid=0;

	brush_hit = NULL;

	VectorCopy ( start ,col_start );
	VectorCopy (end ,col_end );

	VectorSubtract (end,start , ray );
	VectorNormalize (ray );

	VectorClear(org );

	memset ( brushflag,0,1025);

	model_index =0 ;
	Check_Node (0);

	for (i=1; i< cm.num_models;i++ )
	{
			model_index = i;
			Check_Model ( i);
	}

	memset ( results,0,sizeof (trace_t ));
	results->fraction = col_dist ;
	results->startsolid =col_start_solid ;
	VectorCopy ( col_end , results->endpos);
	
	// TODO !!!
	results->entityNum = ENTITYNUM_WORLD ;
	
		
	if (brush_hit)
	{
		results->contents= brush_hit->shaderref->contents;
	}

	//results->Point_Contents (col_end );

}



// TODO  : optimize 
// debug ? not tested yet 

int Point_Contents (const vec3_t pos ,int passEntityNum)
{
	
	int i,j,k;
	cplane_t * plane=NULL;
	cleaf_t * leaf =NULL;
	cnode_t * node= cm.nodes;
	float dist ;
	cbrush_t * brush=NULL;
	cbrushside_t *  side =NULL;
	cmodel_t * model ;
	vec3_t pos2,origin ;
	int contents =0;
	
	while (1 )
	{
		dist =DotProduct (node->plane->normal, pos) - node->plane->dist;
		
		if (dist <=0 ) 
		{
			if (node->children[0]<0)
			{
				leaf=&cm.leaves [~node->children[0]];
				break;
			}
			else 
			{
				node= &cm.nodes[node->children[0]];

			}


		}
		else 
		{
			if (node->children[1]<0)
			{
				leaf=&cm.leaves [~node->children[1]];
				break;
			}
			else 
			{
				node= &cm.nodes[node->children[1]];

			}



		}
	}

		if (!leaf) return 0;


		for (i=0;i<leaf->numbrushes;i++)
		{
			brush=&cm.brushes[cm.lbrushes[leaf->firstbrush+i]];

			
			side=brush->sides;
			for (j=0;j<brush->numsides;j++,side++)
			{
				plane=side->plane;
				
				if (plane->type < 3)
				{
					dist = pos[plane->type] - plane->dist;
				}
				else
				{
					dist = DotProduct (plane->normal, pos) - plane->dist;
				}
		
				if (dist>=0 )
					break;

			}
			
			if (j== brush->numsides)
			{
				contents |= brush->shaderref->contents;
				return contents ;
			}
		}



	for ( i=1 ; i<cm.num_models;i++)
	{
		model=&cm.models[i];
		VectorAdd (model->mins ,model->maxs ,origin );
		VectorScale (origin ,0.5 ,origin );

		VectorAdd ( pos , origin , pos2);


		for (j=0;j<model->numbrushes;j++ )
		{

			brush= model->brushes+ j;

			side= brush->sides;
			for (k=0;k<brush->numsides;k++,side++ )
			{
				
					plane=side->plane;
				
				if (plane->type < 3)
				{
					dist = pos2[plane->type] - plane->dist;
				}
				else
				{
					dist = DotProduct (plane->normal, pos2) - plane->dist;
				}
		
				if (dist>=0 )
					break;


			}

			if (k == brush->numsides )
			{
				contents |= brush->shaderref->contents;
				return contents ;
			}

		}









	}


	return 0 ;

}

/*
qboolean SV_RecursiveHullCheck (hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace)
{
	dclipnode_t	*node;
	mplane_t	*plane;
	float		t1, t2;
	float		frac;
	int			i;
	vec3_t		mid;
	int			side;
	float		midf;

// check for empty
	if (num < 0)
	{
		if (num != CONTENTS_SOLID)
		{
			trace->allsolid = false;
			if (num == CONTENTS_EMPTY)
				trace->inopen = true;
			else
				trace->inwater = true;
		}
		else
			trace->startsolid = true;
		return true;		// empty
	}

	if (num < hull->firstclipnode || num > hull->lastclipnode)
		Sys_Error ("SV_RecursiveHullCheck: bad node number");

//
// find the point distances
//
	node = hull->clipnodes + num;
	plane = hull->planes + node->planenum;

	if (plane->type < 3)
	{
		t1 = p1[plane->type] - plane->dist;
		t2 = p2[plane->type] - plane->dist;
	}
	else
	{
		t1 = DotProduct (plane->normal, p1) - plane->dist;
		t2 = DotProduct (plane->normal, p2) - plane->dist;
	}
	
#if 1
	if (t1 >= 0 && t2 >= 0)
		return SV_RecursiveHullCheck (hull, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 < 0 && t2 < 0)
		return SV_RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);
#else
	if ( (t1 >= DIST_EPSILON && t2 >= DIST_EPSILON) || (t2 > t1 && t1 >= 0) )
		return SV_RecursiveHullCheck (hull, node->children[0], p1f, p2f, p1, p2, trace);
	if ( (t1 <= -DIST_EPSILON && t2 <= -DIST_EPSILON) || (t2 < t1 && t1 <= 0) )
		return SV_RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);
#endif

// put the crosspoint DIST_EPSILON pixels on the near side
	if (t1 < 0)
		frac = (t1 + DIST_EPSILON)/(t1-t2);
	else
		frac = (t1 - DIST_EPSILON)/(t1-t2);
	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;
		
	midf = p1f + (p2f - p1f)*frac;
	for (i=0 ; i<3 ; i++)
		mid[i] = p1[i] + frac*(p2[i] - p1[i]);

	side = (t1 < 0);

// move up to the node
	if (!SV_RecursiveHullCheck (hull, node->children[side], p1f, midf, p1, mid, trace) )
		return false;

#ifdef PARANOID
	if (SV_HullPointContents (sv_hullmodel, mid, node->children[side])
	== CONTENTS_SOLID)
	{
		Con_Printf ("mid PointInHullSolid\n");
		return false;
	}
#endif
	
	if (SV_HullPointContents (hull, node->children[side^1], mid)
	!= CONTENTS_SOLID)
// go past the node
		return SV_RecursiveHullCheck (hull, node->children[side^1], midf, p2f, mid, p2, trace);
	
	if (trace->allsolid)
		return false;		// never got out of the solid area
		
//==================
// the other side of the node is solid, this is the impact point
//==================
	if (!side)
	{
		VectorCopy (plane->normal, trace->plane.normal);
		trace->plane.dist = plane->dist;
	}
	else
	{
		VectorSubtract (vec3_origin, plane->normal, trace->plane.normal);
		trace->plane.dist = -plane->dist;
	}

	while (SV_HullPointContents (hull, hull->firstclipnode, mid)
	== CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy (mid, trace->endpos);
			Con_DPrintf ("backup past 0\n");
			return false;
		}
		midf = p1f + (p2f - p1f)*frac;
		for (i=0 ; i<3 ; i++)
			mid[i] = p1[i] + frac*(p2[i] - p1[i]);
	}

	trace->fraction = midf;
	VectorCopy (mid, trace->endpos);

	return false;
}

*/
