// LIC// ====================================================================
// LIC// This file forms part of oomph-lib, the object-oriented,
// LIC// multi-physics finite-element library, available
// LIC// at http://www.oomph-lib.org.
// LIC//
// LIC// Copyright (C) 2006-2021 Matthias Heil and Andrew Hazel
// LIC//
// LIC// This library is free software; you can redistribute it and/or
// LIC// modify it under the terms of the GNU Lesser General Public
// LIC// License as published by the Free Software Foundation; either
// LIC// version 2.1 of the License, or (at your option) any later version.
// LIC//
// LIC// This library is distributed in the hope that it will be useful,
// LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
// LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// LIC// Lesser General Public License for more details.
// LIC//
// LIC// You should have received a copy of the GNU Lesser General Public
// LIC// License along with this library; if not, write to the Free Software
// LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// LIC// 02110-1301  USA.
// LIC//
// LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
// LIC//
// LIC//====================================================================
// Header file for generic tree structures
#ifndef OOMPH_TREE_HEADER
#define OOMPH_TREE_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

#ifdef OOMPH_HAS_MPI
#include "mpi.h"
#endif

// OOMPH-LIB headers
#include "Vector.h"
#include "map_matrix.h"

namespace oomph
{
  // Forward class definition for class representing the root of a Tree
  class TreeRoot;

  class RefineableElement;

  class Mesh;

  //========================================================================
  /// A generalised tree base class that abstracts the common functionality
  /// between the quad- and octrees used in mesh adaptation in two and
  /// three dimensions, respectively.
  ///
  /// The tree can also be part of a forest. If that is the case, the root
  /// of the tree will have pointers to the roots of neighbouring trees.
  ///
  /// The objects contained in the tree must be RefineableElements.
  ///
  /// The tree can be traversed and actions performed
  /// at all its "nodes" or only at the leaf "nodes" ("nodes" without sons).
  ///
  /// Finally, the leaf "nodes" can be split depending on
  /// a criteria defined by the object.
  ///
  /// Note that Trees are only generated by splitting existing
  /// Trees. Therefore, the constructors are protected. The
  /// only Tree that "Joe User" can create is
  /// the (derived) class TreeRoot.
  //=================================================================
  class Tree
  {
  public:
    /// \short Destructor. Note: Deleting a tree also deletes the
    /// objects associated with its non-leave nodes.
    virtual ~Tree();

    /// Broken copy constructor
    Tree(const Tree& dummy)
    {
      BrokenCopy::broken_copy("Tree");
    }

    /// Broken assignment operator
    void operator=(const Tree&)
    {
      BrokenCopy::broken_assign("Tree");
    }

    /// \short Return the pointer to the object (RefineableElement)
    /// represented by the tree
    RefineableElement* object_pt() const
    {
      return Object_pt;
    }

    /// Flush the object represented by the tree
    void flush_object()
    {
      Object_pt = 0;
    }

    /// \short Return pointer to the son for a given index. Note that
    /// to aid code readability specific enums have been defined for
    /// specific trees. However, these are simply aliases for ints and
    /// the general interface can be implemented once, here.
    Tree* son_pt(const int& son_index) const
    {
      // If there are no sons, return NULL (0)
      if (Son_pt.size() == 0)
      {
        return 0;
      }
      // Otherwise, return the pointer to the appropriate son
      else
      {
        return Son_pt[son_index];
      }
    }


    /// \short Set vector of pointers to sons, indexed by the
    /// appropriate enum that identies son types.
    /// (To aid code readability specific enums have been defined for
    /// specific trees. However, these are simply aliases for ints and
    /// the general interface can be implemented once, here).
    void set_son_pt(const Vector<Tree*>& son_pt)
    {
      Son_pt = son_pt;
    }

    /// Return number of sons (zero if it's a leaf node)
    unsigned nsons() const
    {
      return Son_pt.size();
    }

    /// Flush the sons
    void flush_sons()
    {
      Son_pt.clear();
    }

    /// Return pointer to root of the tree
    TreeRoot*& root_pt()
    {
      return Root_pt;
    }

    /// Return pointer to root of the tree (const version)
    TreeRoot* root_pt() const
    {
      return Root_pt;
    }

    ///\short  If required, split the leaf and create its sons --
    /// criterion: bool object_pt()-> to_be_refined() = true
    template<class ELEMENT>
    void split_if_required();

    ///\short  If required, p-refine the leaf --
    /// criterion: bool object_pt()-> to_be_p_refined() = true
    /// or bool object_pt()-> to_be_p_unrefined() = true
    template<class ELEMENT>
    void p_refine_if_required(Mesh*& mesh_pt);

    /// \short If required, merge the four sons for unrefinement --
    /// criterion: bool object_pt()-> sons_to_be_unrefined() = true
    void merge_sons_if_required(Mesh*& mesh_pt);

    /// \short Call the RefineableElement's deactivate_element() function.
    void deactivate_object();

    /// \short A function that constructs a specific type of tree. This
    /// MUST be overloaded for each specific tree type. The use of such a
    /// function allows the generic implementation of split_if_required().
    virtual Tree* construct_son(RefineableElement* const& object_pt,
                                Tree* const& father_pt,
                                const int& son_type) = 0;

    /// Function pointer to argument-free void Tree member function
    typedef void (Tree::*VoidMemberFctPt)();

    /// Function pointer to a void Tree member function that takes a
    /// pointer to a mesh as its argument
    typedef void (Tree::*VoidMeshPtArgumentMemberFctPt)(Mesh*& mesh_pt);


    /// \short Traverse the tree and execute void Tree member function
    /// member_function() at all its "nodes"
    void traverse_all(Tree::VoidMemberFctPt member_function);

    /// \short Traverse the tree and excute void Tree member function
    /// that takes a pointer to a mesh as an argument
    void traverse_all(Tree::VoidMeshPtArgumentMemberFctPt member_function,
                      Mesh*& mesh_pt);

    /// \short Traverse the tree and execute void Tree member function
    /// member_function() at all its "nodes" aparat from the leaves
    void traverse_all_but_leaves(Tree::VoidMemberFctPt member_function);

    /// \short  Traverse the tree and execute void Tree member function
    /// member_function() only at its leaves
    void traverse_leaves(Tree::VoidMemberFctPt member_function);

    /// \short  Traverse the tree and execute void Tree member function
    /// that takes a pointer to a mesh as an argument only at its leaves
    void traverse_leaves(Tree::VoidMeshPtArgumentMemberFctPt member_function,
                         Mesh*& mesh_pt);

    /// Traverse tree and stick pointers to leaf "nodes" (only) into Vector
    void stick_leaves_into_vector(Vector<Tree*>&);

    /// Traverse and stick pointers to all "nodes" into Vector
    void stick_all_tree_nodes_into_vector(Vector<Tree*>&);

    /// Return son type
    int son_type() const
    {
      return Son_type;
    }

    /// Return true if the tree is a leaf node
    bool is_leaf()
    {
      // If there are no sons, it's a leaf, return true
      if (Son_pt.size() == 0)
      {
        return true;
      }
      // Otherwise return false
      else
      {
        return false;
      }
    }

    /// Return pointer to father: NULL if it's a root node
    Tree* father_pt() const
    {
      return Father_pt;
    }

    /// Set the father
    void set_father_pt(Tree* const& father_pt)
    {
      Father_pt = father_pt;
    }

    /// \short Return the level of the Tree (root=0)
    unsigned level() const
    {
      return Level;
    }

    /// \short Max. allowed discrepancy in neighbour finding routine
    /// (distance between points when identified from two neighbouring
    /// elements)
    static double& max_neighbour_finding_tolerance()
    {
      return Max_neighbour_finding_tolerance;
    }

  public:
    /// Default value for an unassigned neighbour
    static const int OMEGA;

  protected:
    /// Default constructor (empty and broken)
    Tree()
    {
      // Throw an error
      throw OomphLibError("Don't call an empty constructor for a Tree object",
                          OOMPH_CURRENT_FUNCTION,
                          OOMPH_EXCEPTION_LOCATION);
    }

    /// \short Default constructor for empty (root) tree:
    /// no father, no sons; just pass a pointer to its object
    /// Protected because Trees can only be created internally,
    /// during the split operation. Only TreeRoots can be
    /// created externally.
    Tree(RefineableElement* const& object_pt);

    /// \short Constructor for tree that has a father: Pass it the pointer
    /// to its object, the pointer to its father and tell it what type
    /// of son it is.
    /// Protected because Trees can only be created internally,
    /// during the split operation.  Only TreeRoots can be
    /// created externally.
    Tree(RefineableElement* const& object_pt,
         Tree* const& father_pt,
         const int& son_type);

    /// Pointer to the root of the tree
    TreeRoot* Root_pt;

  protected:
    /// Pointer to the Father of the Tree
    Tree* Father_pt;

    /// Vector of pointers to the sons of the Tree
    Vector<Tree*> Son_pt;

    /// Level of the Tree (level 0 = root)
    int Level;

    /// Son type (e.g. SW/SE/NW/NE in a quadtree)
    int Son_type;

    /// Pointer to the object represented by the tree
    RefineableElement* Object_pt;

    /// \short Max. allowed discrepancy in neighbour finding routine
    /// (distance between points when identified from two neighbouring
    /// elements)
    static double Max_neighbour_finding_tolerance;
  };


  //===================================================================
  /// TreeRoot is a Tree that forms the root of a (recursive)
  /// tree. The "root node" is special as it holds additional
  /// information about its neighbours and their relative
  /// rotation (inside a TreeForest).
  //==================================================================
  class TreeRoot : public virtual Tree
  {
  protected:
    /// \short Map of pointers to the neighbouring TreeRoots:
    /// Neighbour_pt[direction] returns the pointer to the
    /// TreeRoot's neighbour in the (enumerated) direction.
    /// Returns NULL if there's no neighbour in this direction.
    std::map<int, TreeRoot*> Neighbour_pt;


    /// \short Map of booleans used for periodic boundaries:
    /// Neighbour_periodic_direction[directon] returns true if the
    /// neighbour in that direction is actually a periodic neighbour
    /// --- shared data values, but independent position.
    /// The default return of the map is false.
    std::map<int, bool> Neighbour_periodic;

  public:
    /// Constructor for the (empty) root tree
    TreeRoot(RefineableElement* const& object_pt) : Tree(object_pt)
    {
      // TreeRoot is the Root
      Root_pt = this;
    }


    /// Broken copy constructor
    TreeRoot(const TreeRoot& dummy)
    {
      BrokenCopy::broken_copy("TreeRoot");
    }

    /// Broken assignment operator
    void operator=(const TreeRoot&)
    {
      BrokenCopy::broken_assign("TreeRoot");
    }

    /// \short Return the pointer to the neighbouring TreeRoots in specified
    /// direction.  Returns NULL if there's no neighbour in this direction.
    TreeRoot*& neighbour_pt(const int& direction)
    {
      return Neighbour_pt[direction];
    }

    /// \short Return whether the neighbour in the particular direction
    /// is periodic.
    bool is_neighbour_periodic(const int& direction)
    {
      return Neighbour_periodic[direction];
    }

    ///\short Set the neighbour in particular direction to be periodic
    void set_neighbour_periodic(const int& direction)
    {
      Neighbour_periodic[direction] = true;
    }

    ///\short Set the neighbour in particular direction to be nonperiodic
    void set_neighbour_nonperiodic(const int& direction)
    {
      Neighbour_periodic[direction] = false;
    }

    /// Return the number of neighbours
    unsigned nneighbour()
    {
      // Loop over the neighbours and test whether they are non-null
      unsigned count = 0;
      for (std::map<int, TreeRoot*>::iterator it = Neighbour_pt.begin();
           it != Neighbour_pt.end();
           it++)
      {
        if (Neighbour_pt[it->first] != 0)
        {
          count++;
        }
      }
      // Return number counted
      return count;
    }
  };


  //================================================================
  /// A TreeForest consists of a collection of TreeRoots.
  /// Each member tree can have neighbours in various enumerated
  /// directions (e.g. S/W/N/E for a QuadTreeForest)
  /// and the orientation of their compasses can differ, allowing
  /// for complex, unstructured meshes.
  //=================================================================
  class TreeForest
  {
  public:
    /// \short Constructor for Tree forest: Pass Vector of
    /// (pointers to) constituents trees.
    TreeForest(Vector<TreeRoot*>& trees_pt);

    /// Default constructor (empty and broken)
    TreeForest()
    {
      // Throw an error
      throw OomphLibError(
        "Don't call an empty constructor for a TreeForest object",
        OOMPH_CURRENT_FUNCTION,
        OOMPH_EXCEPTION_LOCATION);
    }

    /// Broken copy constructor
    TreeForest(const TreeForest& dummy)
    {
      BrokenCopy::broken_copy("TreeForest");
    }

    /// Broken assignment operator
    void operator=(const TreeForest&)
    {
      BrokenCopy::broken_assign("TreeForest");
    }

    /// \short Destructor: Delete the constituent trees (and thus
    /// the objects associated with its non-leaf nodes!)
    virtual ~TreeForest();

    /// Traverse forst and stick pointers to leaf "nodes" into Vector
    void stick_leaves_into_vector(Vector<Tree*>& forest_nodes);

    /// Traverse forest and stick pointers to all "nodes" into Vector
    void stick_all_tree_nodes_into_vector(Vector<Tree*>& all_forest_nodes);

    /// \short Document/check the neighbours of all the nodes in the forest.
    /// This must be overloaded for different types of forest.
    virtual void check_all_neighbours(DocInfo& doc_info) = 0;

    /// \short Open output files that will store any hanging nodes in the
    /// forest. Return a vector of the output streams. This is included in
    /// the tree structure, so that we can use generic routines for
    /// mesh adaptation in two and three dimensions. The need for pointers
    /// to the output streams is because one cannot copy a stream!
    virtual void open_hanging_node_files(
      DocInfo& doc_info, Vector<std::ofstream*>& output_stream) = 0;

    /// \short Close output files that will store any hanging nodes in the
    /// forest and delete any associated storage.
    /// This can be performed genercially in this base class.
    void close_hanging_node_files(DocInfo& doc_info,
                                  Vector<std::ofstream*>& output_stream);

    /// Number of trees in forest
    unsigned ntree()
    {
      return Trees_pt.size();
    }

    /// Return pointer to i-th tree in forest
    TreeRoot* tree_pt(const unsigned& i) const
    {
      return Trees_pt[i];
    }

    /// Flush trees from forest
    void flush_trees()
    {
      // Clear Trees_pt vector
      Trees_pt.clear();
    }

  protected:
    /// Vector containing the pointers to the trees
    Vector<TreeRoot*> Trees_pt;
  };

} // namespace oomph

#endif