//
// $Id$
//
#ifndef __stir_LORCoordinates_H__
#define __stir_LORCoordinates_H__

/*!
  \file 
  \ingroup LOR
  \brief defines various classes for specifying a line in 3 dimensions
  \warning This is all preliminary and likely to change.
  \author Kris Thielemans

  $Date$
  $Revision$

*/
/*
    Copyright (C) 2004- $Date$, Hammersmith Imanet Ltd
    This software is distributed under the terms of the GNU Lesser General 
    Public Licence (LGPL).
    See STIR/LICENSE.txt for details
*/

#include "stir/CartesianCoordinate3D.h"

START_NAMESPACE_STIR

class Succeeded;


// predeclarations to allow cross-referencing
template <class coordT> class LOR;
template <class coordT> class LORInAxialAndSinogramCoordinates;
template <class coordT> class LORInCylinderCoordinates;
template <class coordT> class LORInAxialAndNoArcCorrSinogramCoordinates;
template <class coordT> class LORAs2Points;


/*! \ingroup LOR
  \brief A base class for specifying an LOR with geometric coordinates

  A Line-of-Response (LOR) is really just a line in 3D. You can specify 
  a line in several ways, each if which is more convenient for some 
  application. This class provides a common base for all of these.

  \warning This is all preliminary and likely to change.
*/
template <class coordT>
class LOR
{
 public:
  virtual ~LOR() {}

  virtual 
    LOR * clone() const = 0;

  virtual
    Succeeded
    change_representation(LORInCylinderCoordinates<coordT>&,
		       const double radius) const = 0;
  virtual
    Succeeded
    change_representation(LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&,
		       const double radius) const = 0;
  virtual
    Succeeded
    change_representation(LORInAxialAndSinogramCoordinates<coordT>&,
		       const double radius) const = 0;
  virtual
    Succeeded
    get_intersections_with_cylinder(LORAs2Points<coordT>&,
				    const double radius) const = 0;
};
  
/*! \ingroup LOR
  \brief A class for a point on a cylinder.
  \warning This is all preliminary and likely to change.
*/

template <class coordT>
class PointOnCylinder
{
 private:
  // sorry: has to be first to give the compiler a better chance of inlining
  void check_state() const
  {
    assert(0<=_psi);
    assert(_psi<2*_PI);
  }
 public:
  coordT psi() const { check_state(); return _psi; }
  coordT& psi()      { check_state(); return _psi; }
  coordT z() const   { return _z; }
  coordT& z()        { return _z; }

  inline
  PointOnCylinder();
  inline
  PointOnCylinder(const coordT z, const coordT psi);

 private:
  coordT _z;
  coordT _psi;

};

/*! \ingroup LOR
  \brief An internal class for LORs. Do not use.
  \warning This is all preliminary and likely to change.
*/
template <class coordT>
class LORCylindricalCoordinates_z_and_radius
{
 private:
  // sorry: has to be first to give the compiler a better chance of inlining
  void check_state() const
  {
    assert(_radius>0);
  }
 protected:
  explicit 
    LORCylindricalCoordinates_z_and_radius(coordT radius=1)
    : _radius(radius)
  { check_state(); }
  LORCylindricalCoordinates_z_and_radius(coordT z1, coordT z2, coordT radius)
    : _z1(z1), _z2(z2), _radius(radius)
  { check_state(); }

  coordT z1() const     { check_state(); return _z1; }
  coordT& z1()          { check_state(); return _z1; }
  coordT z2() const     { check_state(); return _z2; }
  coordT& z2()          { check_state(); return _z2; }

  coordT radius() const { check_state(); return _radius; }

 protected:

  inline
    void
    set_radius_no_check(const coordT new_radius)
    {
      check_state();
      assert(new_radius>0);
      _z1 *= new_radius/_radius;
      _z2 *= new_radius/_radius;
      _radius = new_radius;
    }
  coordT _z1;
  coordT _z2;
  coordT _radius;
};

/*! \ingroup LOR
  \brief A class for LORs. 
  \warning This is all preliminary and likely to change.
*/
template <class coordT>
class LORInCylinderCoordinates : public LOR<coordT>
{
  typedef  LORInCylinderCoordinates<coordT> self;
  void check_state() const
  {
    assert(_radius>0);
  }
 public:
  const PointOnCylinder<coordT>& p1() const { return _p1; }
  PointOnCylinder<coordT>& p1() { return _p1; }
  const PointOnCylinder<coordT>& p2() const { return _p2; }
  PointOnCylinder<coordT>& p2() { return _p2; }

  void reset(coordT radius=1)
    {
      // set psi such that the new LOR does intersect that cylinder
      _p1.psi()=0; _p2.psi()=_PI; _radius=radius; 
    }
  coordT radius() const { check_state(); return _radius; }

  inline 
    Succeeded
    set_radius(coordT new_radius)
  {
    check_state();
    assert(new_radius>0);
    if (_radius==new_radius)
      return Succeeded::yes;
    // find minimum radius of a cylinder that is intersected by this LOR
    const coordT min_radius = 
      _radius*fabs(cos((_p1.psi()-_p2.psi())/2));
    if (new_radius >= min_radius)
      return Succeeded::no;
    _p1.z() *= new_radius/_radius;
    _p2.z() *= new_radius/_radius;
    _radius = new_radius;
    return Succeeded::yes;
  }


  inline explicit
  LORInCylinderCoordinates(const coordT radius = 1);

  inline
  LORInCylinderCoordinates(const PointOnCylinder<coordT>& p1,
			   const PointOnCylinder<coordT>& p2,
			   const coordT radius = 1);

  inline
    LORInCylinderCoordinates(const LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&);

  inline
    LORInCylinderCoordinates(const LORInAxialAndSinogramCoordinates<coordT>&);

  inline
    LORInCylinderCoordinates(const LORAs2Points<coordT>&);

  virtual
#ifndef STIR_NO_COVARIANT_RETURN_TYPES
    self* 
#else
      LOR<coordT>*
#endif    
    clone() const { return new self(*this); }

  virtual
    Succeeded
    change_representation(LORInCylinderCoordinates<coordT>&,
			     const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&,
						   const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndSinogramCoordinates<coordT>&,
				       const double radius) const;
  virtual
    Succeeded
    get_intersections_with_cylinder(LORAs2Points<coordT>&,
				    const double radius) const;

 private:
  PointOnCylinder<coordT> _p1;
  PointOnCylinder<coordT> _p2;
  coordT _radius;

};

/*! \ingroup LOR
  \brief A class for LORs. 
  \warning This is all preliminary and likely to change.
*/
template <class coordT>
class LORAs2Points : public LOR<coordT>
{
  typedef LORAs2Points<coordT> self;
 public:
  const CartesianCoordinate3D<coordT>& p1() const { return _p1; }
  CartesianCoordinate3D<coordT>& p1() { return _p1; }
  const CartesianCoordinate3D<coordT>& p2() const { return _p2; }
  CartesianCoordinate3D<coordT>& p2() { return _p2; }

  inline
    LORAs2Points();

  inline
    LORAs2Points(const CartesianCoordinate3D<coordT>& p1,
		 const CartesianCoordinate3D<coordT>& p2);

  inline
    LORAs2Points(const LORInCylinderCoordinates<coordT>& cyl_coords);

  inline
    LORAs2Points(const LORInAxialAndSinogramCoordinates<coordT>&);

  inline
    LORAs2Points(const LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&);

  virtual
#ifndef STIR_NO_COVARIANT_RETURN_TYPES
    self* 
#else
      LOR<coordT>*
#endif
      clone() const { return new self(*this); }

  virtual
    Succeeded
    change_representation(LORInCylinderCoordinates<coordT>&,
			     const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&,
						   const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndSinogramCoordinates<coordT>&,
						   const double radius) const;
  virtual
    Succeeded
    get_intersections_with_cylinder(LORAs2Points<coordT>&,
				    const double radius) const;

 private:
  CartesianCoordinate3D<coordT> _p1;
  CartesianCoordinate3D<coordT> _p2;
};

/*! \ingroup LOR
  \brief A class for LORs. 
  \warning This is all preliminary and likely to change.
*/
template <class coordT>
class LORInAxialAndSinogramCoordinates 
  : public LOR<coordT>, private LORCylindricalCoordinates_z_and_radius<coordT>
{
 private:
  typedef LORInAxialAndSinogramCoordinates<coordT> self;

  // sorry: has to be first to give the compiler a better chance of inlining
  void check_state() const
  {
    assert(_radius>0);
    assert(_s>-_radius);
    assert(_s<_radius);
    assert(_phi<_PI);
    assert(_phi>=0);
  }

 public:
  coordT z1() const     { check_state(); return _z1; }
  coordT& z1()          { check_state(); return _z1; }
  coordT z2() const     { check_state(); return _z2; }
  coordT& z2()          { check_state(); return _z2; }
  coordT phi() const    { check_state(); return _phi; }
  coordT& phi()         { check_state(); return _phi; }
  coordT s() const      { check_state(); return _s; }
  coordT& s()           { check_state(); return _s; }

  coordT beta() const   { check_state(); return asin(_s/_radius); }

  inline explicit
  LORInAxialAndSinogramCoordinates(const coordT radius = 1);

  //! Constructor from explicit arguments
  /*! \warning 
    It's a bad idea to use this constructor, as a mistake in the 
    order of arguments is easily made.
  */
  inline
  LORInAxialAndSinogramCoordinates(const coordT z1,
				   const coordT z2,
				   const coordT phi,
				   const coordT s,
				   const coordT radius =1);

  inline
    LORInAxialAndSinogramCoordinates(const LORInCylinderCoordinates<coordT>&);

  inline
    LORInAxialAndSinogramCoordinates(const LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&);

  inline
    LORInAxialAndSinogramCoordinates(const LORAs2Points<coordT>&);

  virtual
#ifndef STIR_NO_COVARIANT_RETURN_TYPES
    self* 
#else
      LOR<coordT>*
#endif
      clone() const { return new self(*this); }

  void reset(coordT radius=1)
    {
      // set such that the new LOR does intersect that cylinder
      _s=0; _radius=radius; 
    }
  coordT radius() const { check_state(); return _radius; }

  inline
    Succeeded
    set_radius(const coordT new_radius)
    {
      if (_radius==new_radius)
	return Succeeded::yes;
      assert(new_radius>0);
      if(fabs(s())>=new_radius)
	return Succeeded::no;
      set_radius_no_check(new_radius);
      return Succeeded::yes;
    }
    

  virtual
    Succeeded
    change_representation(LORInCylinderCoordinates<coordT>&,
			     const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&,
						   const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndSinogramCoordinates<coordT>&,
				       const double radius) const;
  virtual
    Succeeded
    get_intersections_with_cylinder(LORAs2Points<coordT>&,
				    const double radius) const;

 private:
  coordT _phi;
  coordT _s;

};

/*! \ingroup LOR
  \brief A class for LORs. 
  \warning This is all preliminary and likely to change.
*/
template <class coordT>
class LORInAxialAndNoArcCorrSinogramCoordinates
  : public LOR<coordT>, private LORCylindricalCoordinates_z_and_radius<coordT>
{
 private:
  typedef LORInAxialAndNoArcCorrSinogramCoordinates<coordT> self;

  // sorry: has to be first to give the compiler a better chance of inlining
  void check_state() const
  {
    assert(_radius>0);
    assert(_beta>=-_PI/2);
    assert(_beta<_PI/2);
    assert(_phi<_PI);
    assert(_phi>=0);
  }
 public:
  coordT z1() const     { check_state(); return _z1; }
  coordT& z1()          { check_state(); return _z1; }
  coordT z2() const     { check_state(); return _z2; }
  coordT& z2()          { check_state(); return _z2; }
  coordT phi() const    { check_state(); return _phi; }
  coordT& phi()         { check_state(); return _phi; }
  coordT beta() const   { check_state(); return _beta; }
  coordT& beta()        { check_state(); return _beta; }

  coordT s() const      { check_state(); return _radius*sin(_beta); }

  void reset(coordT radius=1)
    {
      // set such that the new LOR does intersect that cylinder
      _beta=0; _radius=radius; 
    }
  coordT radius() const { check_state(); return _radius; }

  inline
    Succeeded
    set_radius(const coordT new_radius)
    {
      if (_radius==new_radius)
	return Succeeded::yes;
      assert(new_radius>0);
      if(fabs(s())>=new_radius)
	return Succeeded::no;
      set_radius_no_check(new_radius);
      return Succeeded::yes;
    }

  inline explicit
  LORInAxialAndNoArcCorrSinogramCoordinates(const coordT radius = 1);

  //! Constructor from explicit arguments
  /*! \warning 
    It's a bad idea to use this constructor, as a mistake in the 
    order of arguments is easily made.
  */
  inline
  LORInAxialAndNoArcCorrSinogramCoordinates(const coordT z1,
				   const coordT z2,
				   const coordT phi,
				   const coordT beta,
				   const coordT radius =1);

  inline
    LORInAxialAndNoArcCorrSinogramCoordinates(const LORInCylinderCoordinates<coordT>&);

  inline
  LORInAxialAndNoArcCorrSinogramCoordinates(const LORInAxialAndSinogramCoordinates<coordT>&);

  inline
    LORInAxialAndNoArcCorrSinogramCoordinates(const LORAs2Points<coordT>&);

  virtual
#ifndef STIR_NO_COVARIANT_RETURN_TYPES
    self* 
#else
      LOR<coordT>*
#endif
      clone() const { return new self(*this); }

  virtual
    Succeeded
    change_representation(LORInCylinderCoordinates<coordT>&,
			     const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndNoArcCorrSinogramCoordinates<coordT>&,
						   const double radius) const;
  virtual
    Succeeded
    change_representation(LORInAxialAndSinogramCoordinates<coordT>&,
				       const double radius) const;
  virtual
    Succeeded
    get_intersections_with_cylinder(LORAs2Points<coordT>&,
				    const double radius) const;

 private:
  coordT _phi;
  coordT _beta;
};

/*! \ingroup LOR
  \brief Given an LOR, find its intersection with a (infintely long) cylinder
  \warning This is all preliminary and likely to change.
*/

template <class coordT1, class coordT2>
inline Succeeded
find_LOR_intersections_with_cylinder(LORInCylinderCoordinates<coordT1>&,
				     const LORAs2Points<coordT2>&,
				     const double radius);
/*! \ingroup LOR
  \brief Given an LOR, find its intersection with a (infintely long) cylinder
  \warning This is all preliminary and likely to change.
*/
template <class coordT1, class coordT2>
inline Succeeded
find_LOR_intersections_with_cylinder(LORAs2Points<coordT1>& intersection_coords,
				     const LORAs2Points<coordT2>& coords,
				     const double radius);
/*! \ingroup LOR
  \brief Given an LOR, find its intersection with a (infintely long) cylinder
  \warning This is all preliminary and likely to change.
*/
template <class coordT1, class coordT2>
inline Succeeded
find_LOR_intersections_with_cylinder(LORInAxialAndNoArcCorrSinogramCoordinates<coordT1>&  lor,
				     const LORAs2Points<coordT2>& cart_coords,
				     const double radius);

/*! \ingroup LOR
  \brief Given an LOR, find its intersection with a (infintely long) cylinder
  \warning This is all preliminary and likely to change.
*/
template <class coordT1, class coordT2>
inline Succeeded
find_LOR_intersections_with_cylinder(LORInAxialAndSinogramCoordinates<coordT1>&  lor,
				     const LORAs2Points<coordT2>& cart_coords,
				     const double radius);
									     
END_NAMESPACE_STIR							     

#include "stir/LORCoordinates.inl"

#endif									     

