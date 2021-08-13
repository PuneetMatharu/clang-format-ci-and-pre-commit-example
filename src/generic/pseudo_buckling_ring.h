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
#ifndef OOMPH_PSEUDO_BUCKLING_RING_HEADER
#define OOMPH_PSEUDO_BUCKLING_RING_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

// oomph-lib headers
#include "elements.h"
#include "nodes.h"
#include "quadtree.h"
#include "mesh.h"
#include "timesteppers.h"
#include "geom_objects.h"

namespace oomph
{
  //=========================================================================
  /// \short Pseudo buckling ring: Circular ring deformed by the
  /// N-th buckling mode of a thin-wall elastic ring.
  /// \f[
  /// x = R_0 \cos(\zeta) +
  ///     \epsilon \left( \cos(N \zeta) \cos(\zeta) - A \sin(N \zeta)
  ///     \sin(\zeta)
  ///              \right) sin(2 \pi t/T)
  /// \f]
  /// \f[
  /// y = R_0 \sin(\zeta) +
  ///     \epsilon \left( \cos(N \zeta) \sin(\zeta) + A \sin(N \zeta)
  ///     \cos(\zeta)
  ///              \right) sin(2 \pi t/T)
  /// \f]
  /// where A is the ratio of the aziumuthal to the radial buckling
  /// amplitude (A=-1/N for statically buckling rings) and epsilon
  /// is the buckling amplitude.
  ///
  //=========================================================================
  class PseudoBucklingRing : public GeomObject
  {
  public:
    /// Default constructor (empty and broken)
    PseudoBucklingRing()
    {
      throw OomphLibError(
        "Don't call empty constructor for PseudoBucklingRing!",
        OOMPH_CURRENT_FUNCTION,
        OOMPH_EXCEPTION_LOCATION);
    }

    /// \short Constructor: 1 Lagrangian coordinate, 2 Eulerian coords. Pass
    /// buckling amplitude, ratio of of buckling amplitudes, buckling
    /// wavenumber (as a double), undeformed ring radius (all as Data)
    /// and pointer to global timestepper.
    /// \code
    /// Geom_data_pt[0]->value(0) = Eps_buckl;
    /// Geom_data_pt[0]->value(1) = Ampl_ratio;
    /// Geom_data_pt[0]->value(2) = Double_N_buckl;
    /// Geom_data_pt[0]->value(3) = R_0;
    /// Geom_data_pt[0]->value(4) = T;
    /// \endcode
    PseudoBucklingRing(const Vector<Data*>& geom_data_pt,
                       TimeStepper* time_stepper_pt)
      : GeomObject(1, 2, time_stepper_pt)
    {
#ifdef PARANOID
      if (geom_data_pt.size() != 1)
      {
        std::ostringstream error_message;
        error_message << "geom_data_pt should be of size 1, but is of size"
                      << geom_data_pt.size() << std::endl;

        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      if (geom_data_pt[0]->nvalue() != 5)
      {
        std::ostringstream error_message;
        error_message << "geom_data_pt[0] should have 5 values, but has"
                      << geom_data_pt[0]->nvalue() << std::endl;

        throw OomphLibError(error_message.str(),
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif
      Geom_data_pt.resize(1);
      Geom_data_pt[0] = geom_data_pt[0];

      // Data has been created externally: Must not clean up
      Must_clean_up = false;
    }


    /// \short Constructor: 1 Lagrangian coordinate, 2 Eulerian coords. Pass
    /// buckling amplitude, ratio of of buckling amplitudes, buckling
    /// wavenumber, undeformed ring radius, period of osc and pointer
    /// to global timestepper. All geometric data is pinned by default.
    PseudoBucklingRing(const double& eps_buckl,
                       const double& ampl_ratio,
                       const unsigned n_buckl,
                       const double& r_0,
                       const double& T,
                       TimeStepper* time_stepper_pt)
      : GeomObject(1, 2, time_stepper_pt)
    {
      // Number of previous timesteps that need to be stored
      unsigned n_time = time_stepper_pt->nprev_values();

      // Setup geometric data for element: Five items of data live in
      // in the one and only geometric data. Also store time history
      Geom_data_pt.resize(1);
      Geom_data_pt[0] = new Data(time_stepper_pt, 5);

      // I've created the data, I need to clean up
      Must_clean_up = true;

      for (unsigned itime = 0; itime <= n_time; itime++)
      {
        // Buckling amplitude
        Geom_data_pt[0]->set_value(itime, 0, eps_buckl);
        Geom_data_pt[0]->pin(0);

        // Ratio of buckling amplitudes
        Geom_data_pt[0]->set_value(itime, 1, ampl_ratio);
        Geom_data_pt[0]->pin(1);

        // Buckling wavenumber (as double)
        Geom_data_pt[0]->set_value(itime, 2, double(n_buckl));
        Geom_data_pt[0]->pin(2);

        // Radius of undeformed ring
        Geom_data_pt[0]->set_value(itime, 3, r_0);
        Geom_data_pt[0]->pin(3);

        // Period of oscillation
        Geom_data_pt[0]->set_value(itime, 4, T);
        Geom_data_pt[0]->pin(4);
      }
    }


    /// \short Constructor: 1 Lagrangian coordinate, 2 Eulerian coords. Pass
    /// buckling amplitude, h/R, buckling wavenumbe and pointer
    /// to global timestepper. Other parameters get set up to represent
    /// oscillating ring with mode imode (1 or 2). All geometric data is
    /// pinned by  default.
    PseudoBucklingRing(const double& eps_buckl,
                       const double& HoR,
                       const unsigned& n_buckl,
                       const unsigned& imode,
                       TimeStepper* time_stepper_pt)
      : GeomObject(1, 2, time_stepper_pt)
    {
      // Constants in Soedel solution:
      double K1 = (pow(double(n_buckl), 2) + 1.0) *
                  (1.0 / 12.0 * pow(double(n_buckl), 2) * pow(HoR, 2) + 1.0);

      double K2oK1sq =
        1.0 / 12.0 * pow(HoR, 2) * pow(double(n_buckl), 2) *
        pow((pow(double(n_buckl), 2) - 1.0), 2) /
        (pow((pow(double(n_buckl), 2) + 1.0), 2) *
         pow((1.0 / 12.0 * pow(double(n_buckl), 2) * pow(HoR, 2) + 1.0), 2));

      // The two fundamental frequencies:
      double omega1 = sqrt(0.5 * K1 * (1.0 + sqrt(1.0 - 4 * K2oK1sq)));
      double omega2 = sqrt(0.5 * K1 * (1.0 - sqrt(1.0 - 4 * K2oK1sq)));

      // The two amplitude ratios:
      double ampl_ratio1 =
        (double(n_buckl) *
         (1.0 / 12.0 * pow(double(n_buckl), 2) * pow(HoR, 2) + 1.0)) /
        (pow(omega1, 2) -
         pow(double(n_buckl), 2) * (1.0 / 12.0 * pow(HoR, 2) + 1.0));

      double ampl_ratio2 =
        double(n_buckl) *
        (1.0 / 12.0 * pow(double(n_buckl), 2) * pow(HoR, 2) + 1.0) /
        (pow(omega2, 2) -
         pow(double(n_buckl), 2) * (1.0 / 12.0 * pow(HoR, 2) + 1.0));

      double T;
      double ampl_ratio;

      if (n_buckl > 1)
      {
        T = 2.0 * MathematicalConstants::Pi / omega2;
        ampl_ratio = ampl_ratio2;
        if (imode == 1)
        {
          T = 2.0 * MathematicalConstants::Pi / omega1;
          ampl_ratio = ampl_ratio1;
        }
        else if (imode == 2)
        {
          T = 2.0 * MathematicalConstants::Pi / omega2;
          ampl_ratio = ampl_ratio2;
        }
        else
        {
          oomph_info << "wrong imode " << imode << std::endl;
        }
      }
      else
      {
        T = 2.0 * MathematicalConstants::Pi / omega1;
        ampl_ratio = ampl_ratio1;
      }

      // Number of previous timesteps that need to be stored
      unsigned n_time = time_stepper_pt->nprev_values();

      // Setup geometric data for element: Five items of data live in
      // in the one and only geometric data. Also store time history
      Geom_data_pt.resize(1);
      Geom_data_pt[0] = new Data(time_stepper_pt, 5);

      // I've created the data, I need to clean up
      Must_clean_up = true;

      for (unsigned itime = 0; itime <= n_time; itime++)
      {
        // Buckling amplitude
        Geom_data_pt[0]->set_value(itime, 0, eps_buckl);
        Geom_data_pt[0]->pin(0);

        // Ratio of buckling amplitudes
        Geom_data_pt[0]->set_value(itime, 1, ampl_ratio);
        Geom_data_pt[0]->pin(1);

        // Buckling wavenumber (as double)
        Geom_data_pt[0]->set_value(itime, 2, double(n_buckl));
        Geom_data_pt[0]->pin(2);

        // Radius of undeformed ring
        Geom_data_pt[0]->set_value(itime, 3, 1.0);
        Geom_data_pt[0]->pin(3);

        // Period of oscillation
        Geom_data_pt[0]->set_value(itime, 4, T);
        Geom_data_pt[0]->pin(4);
      }
    }

    /// Broken copy constructor
    PseudoBucklingRing(const PseudoBucklingRing& node)
    {
      BrokenCopy::broken_copy("PseudoBucklingRing");
    }

    /// Broken assignment operator
    void operator=(const PseudoBucklingRing&)
    {
      BrokenCopy::broken_assign("PseudoBucklingRing");
    }


    /// Destructor:  Clean up if necessary
    ~PseudoBucklingRing()
    {
      // Do I need to clean up?
      if (Must_clean_up)
      {
        delete Geom_data_pt[0];
        Geom_data_pt[0] = 0;
      }
    }


    /// Access function for buckling amplitude
    double eps_buckl()
    {
      return Geom_data_pt[0]->value(0);
    }

    /// Access function for amplitude ratio
    double ampl_ratio()
    {
      return Geom_data_pt[0]->value(1);
    }

    /// Access function for undeformed radius
    double r_0()
    {
      return Geom_data_pt[0]->value(3);
    }

    /// Access function for period of oscillation
    double T()
    {
      return Geom_data_pt[0]->value(4);
    }

    /// Access function for  buckling wavenumber (as float)
    double n_buckl_float()
    {
      return Geom_data_pt[0]->value(2);
    }

    /// Set buckling amplitude
    void set_eps_buckl(const double& eps_buckl)
    {
      Geom_data_pt[0]->set_value(0, eps_buckl);
    }

    /// \short Set amplitude ratio between radial and azimuthal
    /// buckling displacements
    void set_ampl_ratio(const double& ampl_ratio)
    {
      Geom_data_pt[0]->set_value(1, ampl_ratio);
    }

    /// Set buckling wavenumber
    void set_n_buckl(const unsigned& n_buckl)
    {
      Geom_data_pt[0]->set_value(2, double(n_buckl));
    }

    /// Set undeformed radius of ring
    void set_R_0(const double& r_0)
    {
      Geom_data_pt[0]->set_value(3, r_0);
    }

    /// Set period of oscillation
    void set_T(const double& T)
    {
      Geom_data_pt[0]->set_value(4, T);
    }


    /// \short Position Vector at Lagrangian coordinate zeta at present
    /// time
    void position(const Vector<double>& zeta, Vector<double>& r) const
    {
#ifdef PARANOID
      if (r.size() != Ndim)
      {
        throw OomphLibError("The position vector r has the wrong dimension",
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // Parameter values at present time
      double time = Geom_object_time_stepper_pt->time_pt()->time();
      double Eps_buckl = Geom_data_pt[0]->value(0);
      double Ampl_ratio = Geom_data_pt[0]->value(1);
      double double_N_buckl = Geom_data_pt[0]->value(2);
      double R_0 = Geom_data_pt[0]->value(3);
      double T = Geom_data_pt[0]->value(4);


      // Position Vector
      r[0] = R_0 * cos(zeta[0]) +
             Eps_buckl *
               (cos(double_N_buckl * zeta[0]) * cos(zeta[0]) -
                Ampl_ratio * sin(double_N_buckl * zeta[0]) * sin(zeta[0])) *
               sin(2.0 * MathematicalConstants::Pi * time / T);

      r[1] = R_0 * sin(zeta[0]) +
             Eps_buckl *
               (cos(double_N_buckl * zeta[0]) * sin(zeta[0]) +
                Ampl_ratio * sin(double_N_buckl * zeta[0]) * cos(zeta[0])) *
               sin(2.0 * MathematicalConstants::Pi * time / T);
    }


    ///\short Parametrised velocity on object at current time: veloc = d
    /// r(zeta)/dt.
    void veloc(const Vector<double>& zeta, Vector<double>& veloc) // const
    {
#ifdef PARANOID
      if (veloc.size() != Ndim)
      {
        throw OomphLibError("The vector veloc has the wrong size",
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // Parameter values at present time
      double time = Geom_object_time_stepper_pt->time_pt()->time();
      double Eps_buckl = Geom_data_pt[0]->value(0);
      double Ampl_ratio = Geom_data_pt[0]->value(1);
      double double_N_buckl = Geom_data_pt[0]->value(2);
      // Unused double R_0 = Geom_data_pt[0]->value(3);
      double T = Geom_data_pt[0]->value(4);

      // Veloc
      veloc[0] = Eps_buckl *
                 (cos(double_N_buckl * zeta[0]) * cos(zeta[0]) -
                  Ampl_ratio * sin(double_N_buckl * zeta[0]) * sin(zeta[0])) *
                 cos(2.0 * MathematicalConstants::Pi * time / T) * 2.0 *
                 MathematicalConstants::Pi / T;
      veloc[1] = Eps_buckl *
                 (cos(double_N_buckl * zeta[0]) * sin(zeta[0]) +
                  Ampl_ratio * sin(double_N_buckl * zeta[0]) * cos(zeta[0])) *
                 cos(2.0 * MathematicalConstants::Pi * time / T) * 2.0 *
                 MathematicalConstants::Pi / T;
    }


    /// \short Parametrised acceleration on object at current time:
    /// accel = d^2 r(zeta)/dt^2.
    void accel(const Vector<double>& zeta, Vector<double>& accel) // const
    {
#ifdef PARANOID
      if (accel.size() != Ndim)
      {
        throw OomphLibError("The vector accel has the wrong dimension",
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // Parameter values at present time
      double time = Geom_object_time_stepper_pt->time_pt()->time();
      double Eps_buckl = Geom_data_pt[0]->value(0);
      double Ampl_ratio = Geom_data_pt[0]->value(1);
      double double_N_buckl = Geom_data_pt[0]->value(2);
      // Unused double R_0 = Geom_data_pt[0]->value(3);
      double T = Geom_data_pt[0]->value(4);

      // Accel
      accel[0] = -Eps_buckl *
                 (cos(double_N_buckl * zeta[0]) * cos(zeta[0]) -
                  Ampl_ratio * sin(double_N_buckl * zeta[0]) * sin(zeta[0])) *
                 sin(2.0 * MathematicalConstants::Pi * time / T) * 4.0 *
                 MathematicalConstants::Pi * MathematicalConstants::Pi / T / T;

      accel[1] = -Eps_buckl *
                 (cos(double_N_buckl * zeta[0]) * sin(zeta[0]) +
                  Ampl_ratio * sin(double_N_buckl * zeta[0]) * cos(zeta[0])) *
                 sin(2.0 * MathematicalConstants::Pi * time / T) * 4.0 *
                 MathematicalConstants::Pi * MathematicalConstants::Pi / T / T;
    }


    /// \short Position Vector at Lagrangian coordinate zeta at discrete
    /// previous time (t=0: present time; t>0: previous time)
    void position(const unsigned& t,
                  const Vector<double>& zeta,
                  Vector<double>& r) const
    {
#ifdef PARANOID
      if (r.size() != Ndim)
      {
        throw OomphLibError("The position vector r has the wrong dimension",
                            OOMPH_CURRENT_FUNCTION,
                            OOMPH_EXCEPTION_LOCATION);
      }
      if (t > Geom_object_time_stepper_pt->nprev_values())
      {
        throw OomphLibError(
          "The time value t is greater than the number of previous steps",
          OOMPH_CURRENT_FUNCTION,
          OOMPH_EXCEPTION_LOCATION);
      }
#endif

      // Parameter values at previous time
      double Eps_buckl = Geom_data_pt[0]->value(t, 0);
      double Ampl_ratio = Geom_data_pt[0]->value(t, 1);
      double double_N_buckl = Geom_data_pt[0]->value(t, 2);
      double R_0 = Geom_data_pt[0]->value(t, 3);
      double T = Geom_data_pt[0]->value(4);

      // Present time
      double time = Geom_object_time_stepper_pt->time_pt()->time();

      // Recover time at previous timestep
      for (unsigned i = 0; i < t; i++)
      {
        time -= Geom_object_time_stepper_pt->time_pt()->dt(i);
      }

      // Position Vector
      r[0] = R_0 * cos(zeta[0]) +
             Eps_buckl *
               (cos(double_N_buckl * zeta[0]) * cos(zeta[0]) -
                Ampl_ratio * sin(double_N_buckl * zeta[0]) * sin(zeta[0])) *
               sin(2.0 * MathematicalConstants::Pi * time / T);

      r[1] = R_0 * sin(zeta[0]) +
             Eps_buckl *
               (cos(double_N_buckl * zeta[0]) * sin(zeta[0]) +
                Ampl_ratio * sin(double_N_buckl * zeta[0]) * cos(zeta[0])) *
               sin(2.0 * MathematicalConstants::Pi * time / T);
    }


    /// \short j-th time-derivative on object at current time:
    /// \f$ \frac{d^{j} r(\zeta)}{dt^j} \f$.
    void dposition_dt(const Vector<double>& zeta,
                      const unsigned& j,
                      Vector<double>& drdt)
    {
      switch (j)
      {
          // Current position
        case 0:
          position(zeta, drdt);
          break;

          // Velocity:
        case 1:
          veloc(zeta, drdt);
          break;

          // Acceleration:
        case 2:
          accel(zeta, drdt);
          break;

        default:
          std::ostringstream error_message;
          error_message << j << "th derivative not implemented\n";

          throw OomphLibError(error_message.str(),
                              OOMPH_CURRENT_FUNCTION,
                              OOMPH_EXCEPTION_LOCATION);
      }
    }


    /// How many items of Data does the shape of the object depend on?
    unsigned ngeom_data() const
    {
      return Geom_data_pt.size();
    }

    /// \short Return pointer to the j-th Data item that the object's
    /// shape depends on
    Data* geom_data_pt(const unsigned& j)
    {
      return Geom_data_pt[j];
    }


  protected:
    /// \short Vector of pointers to Data items that affects the object's shape
    Vector<Data*> Geom_data_pt;

    /// Do I need to clean up?
    bool Must_clean_up;
  };


  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  // Pseudo buckling ring as element
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////


  //=========================================================================
  /// \short Pseudo buckling ring: Circular ring deformed by the
  /// N-th buckling mode of a thin-wall elastic ring.
  /// \f[
  /// x = R_0 \cos(\zeta) +
  ///     \epsilon \left( \cos(N \zeta) \cos(\zeta) - A \sin(N \zeta)
  ///     \sin(\zeta)
  ///              \right) sin(2 \pi t/T)
  /// \f]
  /// \f[
  /// y = R_0 \sin(\zeta) +
  ///     \epsilon \left( \cos(N \zeta) \sin(\zeta) + A \sin(N \zeta)
  ///     \cos(\zeta)
  ///              \right) sin(2 \pi t/T)
  /// \f]
  /// where A is the ratio of the aziumuthal to the radial buckling
  /// amplitude (A=-1/N for statically buckling rings) and epsilon
  /// is the buckling amplitude.
  /// Scale R_0 is adjusted to ensure conservation of (computational)
  /// volume/area. This is implemented by a
  /// pseudo-elasticity approach: The governing equation for \f$ R_0 \f$ is:
  /// \f[
  /// p_{ref} = R_0 - 1.0
  /// \f]
  /// The pointer to the reference pressure needs to be set with
  /// reference_pressure_pt().
  //=========================================================================
  class PseudoBucklingRingElement : public GeneralisedElement,
                                    public PseudoBucklingRing
  {
  private:
    /// \short Index of the value stored in the single geometric object that has
    /// become an unknown
    unsigned Internal_geometric_variable_index;

    /// \short The Data object that represents the reference pressure is stored
    /// at the location indexed by this integer in the external data storage.
    unsigned External_reference_pressure_index;

    /// Pointer to the data object that represents the external reference
    /// pressure
    Data* External_reference_pressure_pt;

    /// Return the local equation number of the internal geometric variable
    inline int geometric_local_eqn()
    {
      return internal_local_eqn(0, Internal_geometric_variable_index);
    }

    /// Return the local equation number of the reference pressure variable
    inline int reference_pressure_local_eqn()
    {
      return external_local_eqn(External_reference_pressure_index, 0);
    }


  public:
    /// \short Constructor: Build  pseudo buckling ring
    /// from doubles that describe the geometry.
    PseudoBucklingRingElement(const double& eps_buckl,
                              const double& ampl_ratio,
                              const unsigned n_buckl,
                              const double& r_0,
                              const double& T,
                              TimeStepper* time_stepper_pt)
      : PseudoBucklingRing(
          eps_buckl, ampl_ratio, n_buckl, r_0, T, time_stepper_pt),
        External_reference_pressure_pt(0)
    {
      // Geom data for geom object has been setup (and pinned) in
      // constructor for geometric object. Now free the scale for the half axes
      // because we want to determine it as an unknown
      Geom_data_pt[0]->unpin(3);

      // Record that the geometric variable is value 3 in the geometric data
      Internal_geometric_variable_index = 3;

      // The geometric data is internal to the element -- this
      // ensures that any unknown pieces of geom_data get global equation
      // numbers. There should only be one piece of internal data
      unsigned n_geom_data = Geom_data_pt.size();
      for (unsigned i = 0; i < n_geom_data; i++)
      {
        add_internal_data(Geom_data_pt[i]);
      }
    }


    /// \short Constructor: Pass
    /// buckling amplitude, h/R, buckling wavenumbe and pointer
    /// to global timestepper. Other parameters get set up to represent
    /// oscillating ring with mode imode (1 or 2). All geometric data is
    /// pinned by  default.
    PseudoBucklingRingElement(const double& eps_buckl,
                              const double& HoR,
                              const unsigned& n_buckl,
                              const unsigned& imode,
                              TimeStepper* time_stepper_pt)
      : PseudoBucklingRing(eps_buckl, HoR, n_buckl, imode, time_stepper_pt),
        External_reference_pressure_pt(0)
    {
      // Geom data for geom object has been setup (and pinned) in
      // constructor for geometric object. Now free the scale for the half axes
      // because we want to determine it as an unknown
      Geom_data_pt[0]->unpin(3);

      // Record that the geometric variable is value 3 in the geometric data
      Internal_geometric_variable_index = 3;

      // The geometric data is internal to the element -- this
      // ensures that any unknown pieces of geom_data get global equation
      // numbers. There should only be one piece of internal data
      unsigned n_geom_data = Geom_data_pt.size();
      for (unsigned i = 0; i < n_geom_data; i++)
      {
        add_internal_data(Geom_data_pt[i]);
      }
    }


    /// Broken copy constructor
    PseudoBucklingRingElement(const PseudoBucklingRingElement& dummy)
    {
      BrokenCopy::broken_copy("PseudoBucklingRingElement");
    }

    /// Broken assignment operator
    void operator=(const PseudoBucklingRingElement&)
    {
      BrokenCopy::broken_assign("PseudoBucklingRingElement");
    }

    /// Destructor: Kill internal data and set to NULL
    virtual ~PseudoBucklingRingElement()
    {
      // The GeomElement's GeomData is mirrored in the element's
      // Internal Data and therefore gets wiped in the
      // destructor of GeneralisedElement --> No need to
      // kill it in PseudoBucklingRing()
      Must_clean_up = false;
    }


    /// Compute element residual Vector (wrapper)
    inline virtual void get_residuals(Vector<double>& residuals)
    {
      // Create a dummy matrix
      DenseMatrix<double> dummy(1);

      // Call the generic residuals function with flag set to 0
      get_residuals_generic(residuals, dummy, 0);
    }


    /// Compute element residual Vector and element Jacobian matrix (wrapper)
    inline virtual void get_jacobian(Vector<double>& residuals,
                                     DenseMatrix<double>& jacobian)
    {
      // Call the generic routine with the flag set to 1
      get_residuals_generic(residuals, jacobian, 1);
    }

    /// \short Pointer to pressure data that is used as reference pressure
    Data* const& reference_pressure_pt() const
    {
      return external_data_pt(0);
    }

    /// \short Return the reference pressure
    double reference_pressure() const
    {
      // If there is no external pressure, return 0.0
      if (External_reference_pressure_pt == 0)
      {
        return 0.0;
      }
      else
      {
        return External_reference_pressure_pt->value(0);
      }
    }

    /// \short Set the pressure data that is used as reference pressure
    void set_reference_pressure_pt(Data* const& data_pt)
    {
      // Clear the existing external data, if there is any
      flush_external_data(External_reference_pressure_pt);
      // Set the new External reference pointer
      External_reference_pressure_pt = data_pt;
      // Add it to the external data
      External_reference_pressure_index = add_external_data(data_pt);
    }

  protected:
    /// \short Compute element residual Vector (only if flag=0) and also
    /// element Jacobian matrix (if flag=1)
    virtual void get_residuals_generic(Vector<double>& residuals,
                                       DenseMatrix<double>& jacobian,
                                       unsigned flag)
    {
      // Initialise the residuals to zero
      residuals.initialise(0.0);
      // If computing the Jacobian initialise to zero
      if (flag)
      {
        jacobian.initialise(0.0);
      }

      // There is only one equation, which is due to the internal degree
      // of freedom
      int local_eqn = geometric_local_eqn();

      // If it's not a boundary condition
      if (local_eqn >= 0)
      {
        // Pseudo force balance
        residuals[local_eqn] = reference_pressure() - (r_0() - 1.0);

        // Work out jacobian: d residual[0]/d r_0
        if (flag)
        {
          // The derivative wrt the internal unknown is
          // Derivative residual w.r.t. scale
          jacobian(local_eqn, local_eqn) = -1.0;

          int local_unknown = reference_pressure_local_eqn();
          if (local_unknown >= 0)
          {
            jacobian(local_eqn, local_unknown) = 1.0;
          }
        }
      }
    }
  };

} // namespace oomph

#endif