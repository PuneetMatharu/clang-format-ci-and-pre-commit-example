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
// Header file for SpaceTimeNavierStokes elements
#ifndef OOMPH_MIXED_ORDER_PETROV_GALERKIN_SPACE_TIME_BLOCK_PRECONDITIONABLE_ELEMENTS_HEADER
#define OOMPH_MIXED_ORDER_PETROV_GALERKIN_SPACE_TIME_BLOCK_PRECONDITIONABLE_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

// Space-time block preconditionable elements machinery
#include "mixed_order_petrov_galerkin_space_time_navier_stokes_elements.h"
#include "../space_time_block_preconditioner/general_purpose_space_time_block_preconditionable_elements.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

namespace oomph
{
  //======start_of_BlockPrecQTaylorHoodMixedOrderSpaceTimeElement================
  /// Block preconditionable version of the
  /// QTaylorHoodMixedOrderSpaceTimeElement
  //=============================================================================
  class BlockPrecQTaylorHoodMixedOrderSpaceTimeElement
    : public virtual QTaylorHoodMixedOrderSpaceTimeElement<2>,
      public virtual BlockPreconditionableSpaceTimeElementBase
  {
  public:
    /// Empty constructor
    BlockPrecQTaylorHoodMixedOrderSpaceTimeElement()
      : QTaylorHoodMixedOrderSpaceTimeElement<2>(),
        BlockPreconditionableSpaceTimeElementBase()
    {
    }

    /// Empty destructor
    ~BlockPrecQTaylorHoodMixedOrderSpaceTimeElement() {}

    /// \short Overload the pure virtual base class implementation.
    /// Create a list of pairs for all unknowns in this element,
    /// so the first entry in each pair contains the global equation
    /// number of the unknown, while the second one contains the number
    /// of the "DOF type" that this unknown is associated with.
    void get_dof_numbers_for_unknowns(
      std::list<std::pair<unsigned long, unsigned>>& dof_lookup_list) const;
  };
} // End of namespace oomph
#endif