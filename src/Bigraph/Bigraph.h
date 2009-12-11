//-----------------------------------------------
// Copyright 2009 Wellcome Trust Sanger Institute
// Written by Jared Simpson (js18@sanger.ac.uk)
// Released under the GPL license
//-----------------------------------------------
//
// Bidirectional graph 
//
#ifndef BIGRAPH_H
#define BIGRAPH_H

#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include <ext/hash_map>
#include "GraphCommon.h"
#include "Vertex.h"
#include "Edge.h"

//
// Typedefs
//
namespace __gnu_cxx {

   template< typename CharT, typename Traits, typename Alloc >
   struct hash< const std::basic_string<CharT, Traits, Alloc> > {
      size_t operator()(const std::basic_string<CharT, Traits, Alloc>& s) const {
         
         const std::collate<CharT>& c = std::use_facet<std::collate<CharT> >(std::locale());
         return c.hash(s.c_str(), s.c_str() + s.length());

      }
   };

   template< typename CharT, typename Traits, typename Alloc >
   struct hash< std::basic_string<CharT, Traits, Alloc> >
	: hash< const std::basic_string<CharT, Traits, Alloc> > {};
};


typedef __gnu_cxx::hash_map<VertexID, Vertex*> VertexPtrMap;
typedef VertexPtrMap::iterator VertexPtrMapIter;
typedef VertexPtrMap::const_iterator VertexPtrMapConstIter;

class Bigraph;
typedef bool(*VertexVisitFunction)(Bigraph*, Vertex*);

typedef EdgePtrVec Path; // alias
typedef std::vector<Path> PathVector;
typedef std::vector<VertexID> VertexIDVec;

class Bigraph
{

	public:
	
		Bigraph();
		~Bigraph();

		// Add a vertex
		void addVertex(Vertex* pVert);
		
		// Remove a vertex
		// removeIslandVertex removes a vertex that is guarenteed to
		// not have edges
		// removeConnectedVertex removes a (possibly) connected vertex and all the edges to/from it
		void removeIslandVertex(Vertex* pVertex);
		void removeConnectedVertex(Vertex* pVertex);

		// Check if a vertex exists
		bool hasVertex(VertexID id);

		// Get a vertex
		Vertex* getVertex(VertexID id) const;

		// Add an edge
		void addEdge(Edge* pEdge);

		// Remove an edge
		void removeEdge(const EdgeDesc& ed);

		// Remove all edges marked by color c
		void sweepVertices(GraphColor c);
		void sweepEdges(GraphColor c);

		// Merge vertices
		void mergeVertices(VertexID id1, VertexID id2);

		// Merge vertices that are joined by the specified edge
		void merge(Edge* pEdge);		

		// Simplify the graph by removing transitive edges
		void simplify();

		// Validate that the graph is sane
		void validate();

		// Flip a given vertex
		void flip(VertexID id);

		// Sort all the vertex adjacency lists
		void sortVertexAdjLists();

		// Get the IDs of the vertices that do not branch (both sense/antisense degree <= 1)
		VertexIDVec getNonBranchingVertices() const;

		// Get the linear components of a non-branching graph
		PathVector getLinearComponents();

		// Return all the path of nodes that can be linearally reached from this node
		// The path expands in both directions so the first node in the path is not necessarily the source
		Path constructLinearPath(VertexID id);

		// Reverse a path
		static Path reversePath(const Path& path);

		// Print simple summary statistics to stdout
		void stats() const;
		
		// Visit each vertex in the graph and perform the visit function
		bool visit(VertexVisitFunction f);

		// Visit each vertex in the graph and call the visit functor object
		template<typename VF>
		bool visit(VF& vf)
		{
			bool modified = false;
			vf.previsit(this);
			VertexPtrMapConstIter iter = m_vertices.begin(); 
			for(; iter != m_vertices.end(); ++iter)
			{
				modified = vf.visit(this, iter->second) || modified;
			}
			vf.postvisit(this);
			return modified;
		}
		
		// Set the colors for the entire graph
		void setColors(GraphColor c);

		// Check the colors for the entire graph
		bool checkColors(GraphColor c);


		// Write the graph to a dot file
		void writeDot(std::string filename, int dotFlags = 0) const;

	private:

		void followLinear(VertexID id, EdgeDir dir, Path& outPath);

		// Vertex collection
		VertexPtrMap m_vertices;
};

#endif
