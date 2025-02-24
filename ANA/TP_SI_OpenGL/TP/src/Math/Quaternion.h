#ifndef _Math_Quaternion_H
#define _Math_Quaternion_H

#include <Math/Vector.h>

namespace Math
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \class	Quaternion
	///
	/// \brief	Quaternion.
	///
	/// \author	F. Lamarche, Université de Rennes 1
	/// \date	16/12/2015
	///
	/// \tparam	Float	Type of the floating point value.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class Float>
	class Quaternion
	{
	protected:
		Float   m_s ;
		Vector<Float,3> m_v ;

	public:

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion::Quaternion()
		///
		/// \brief	Default constructor.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion()
		{
			m_v = Math::makeVector(1.0f,0.0f,0.0f) ;
			m_s = 1 ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion::Quaternion(Vector<Float,3> const & v, Float const & angle)
		///
		/// \brief	Constructo of a rotation quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	v	 	The rotation axis.
		/// \param	angle	The rotation angle.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion(Vector<Float,3> const & v, Float const & angle)
		{
			setAxisAngle(v,angle) ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion::Quaternion(Vector<Float,3> const & v)
		///
		/// \brief	Construct a quaternion form a vector (useful to compute vector rotations).
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	v	The vector.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion(Vector<Float,3> const & v)
			: m_s(0.0), m_v(v) 
		{}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion::Quaternion(Quaternion const & q)
		///
		/// \brief	Copy constructor.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	q	The Quaternion to process.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion(Quaternion const & q)
			: m_s(q.m_s), m_v(q.m_v) //, m_angle(q.m_angle), m_axis(q.m_axis)
		{}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion::Quaternion(Float const & s, Vector<Float,3> const & v)
		///
		/// \brief	Constructor. Contruct a quaternion from (v,s) in R^3 x R
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	s	The s parameter.
		/// \param	v	The v parameter.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion(Float const & s, Vector<Float,3> const & v)
			: m_s(s), m_v(v)
		{}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void Quaternion::setAxisAngle(Vector<Float,3> const & v, Float angle)
		///
		/// \brief	Sets a rotation quaternion from the axis and the angle. Note that the quaternion is
		/// 		automatically normalized.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	v	 	The axis.
		/// \param	angle	The angle.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void setAxisAngle(Vector<Float,3> const & v, Float angle)
		{
			m_s = (Float)cos(angle/2.0f) ;
			m_v = v*sin(angle/2.0f) ;
			normalize() ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Float & Quaternion::s()
		///
		/// \brief	Gets the s.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Float & s()
		{ return m_s ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Float Quaternion::s() const
		///
		/// \brief	Gets the s.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Float s() const
		{ return m_s ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Vector<Float,3> Quaternion::v() const
		///
		/// \brief	Gets the v.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Vector<Float,3> v() const
		{ return m_v ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Vector<Float,3> & Quaternion::v()
		///
		/// \brief	Gets the v.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Vector<Float,3> & v()
		{ return m_v ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::operator+ (Quaternion const & q) const
		///
		/// \brief	Addition operator.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	q	The Quaternion const &amp; to process.
		///
		/// \return	The result of the operation.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion operator+ (Quaternion const & q) const
		{	return Quaternion(m_s+q.m_s, m_v+q.m_v) ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::operator- (Quaternion const & q) const
		///
		/// \brief	Negation operator.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	q	The Quaternion const &amp; to process.
		///
		/// \return	The result of the operation.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion operator- (Quaternion const & q) const
		{	return Quaternion(m_s+q.m_s, m_v+q.m_v) ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::operator* (Quaternion const & q) const
		///
		/// \brief	Multiplication operator.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	q	The other Quaternion.
		///
		/// \return	The result of the operation.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion operator* (Quaternion const & q) const
		{
			return Quaternion(m_s*q.m_s-m_v*q.m_v, q.m_v*m_s+m_v*q.m_s+(m_v^q.m_v)) ; 
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::operator* (Float const & v) const
		///
		/// \brief	Multiplication operator.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	v	The scalar used for multiplication.
		///
		/// \return	The result of the operation.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion operator* (Float const & v) const
		{	return Quaternion(m_s*v, m_v*v) ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::operator/ (Float const & v) const
		///
		/// \brief	Division operator.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	v	The scalar used for the division.
		///
		/// \return	The result of the operation.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion operator/ (Float const & v) const
		{	return Quaternion(m_s/v, m_v/v) ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::operator- () const
		///
		/// \brief	Negates the quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	The result of the operation.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion operator- () const
		{   return Quaternion(-m_s, -m_v) ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::inv() const
		///
		/// \brief	Gets the inverse of the quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion inv() const
		{   return Quaternion(m_s, -m_v) ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Float Quaternion::norm2() const
		///
		/// \brief	Returns the squared norm of the quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Float norm2() const
		{   return m_s*m_s+m_v.norm2() ; }

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Float Quaternion::norm() const
		///
		/// \brief	Returns the norm of the quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Float norm() const
		{	return sqrt(this->norm2()) ; }

		Quaternion & operator= (Quaternion const & q)
		{
			m_s = q.m_s ;
			m_v = q.m_v ;
			return (*this) ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion & Quaternion::normalize()
		///
		/// \brief	Normalizes this quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion & normalize() 
		{
			(*this) = (*this)/this->norm() ;
			return *this ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Quaternion Quaternion::rotate(Quaternion const & q) const
		///
		/// \brief	Rotates the given quaternion.
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	q	The Quaternion const &amp; to process.
		///
		/// \return	.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Quaternion rotate(Quaternion const & q) const
		{
			return (*this)*q*this->inv() ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	Vector<Float, 3> Quaternion::rotate(Vector<Float, 3> const & v)
		///
		/// \brief	Rotates the vector v (this method assumes that the current quaternion describes a rotation).
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	17/12/2015
		///
		/// \param	v	The vector to rotate.
		///
		/// \return	The rotated vector.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		Vector<Float, 3> rotate(Vector<Float, 3> const & v) const
		{
			return rotate(Quaternion(v)).v() ;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// \fn	void Quaternion::getAxisAngle(Vector<Float,3> & axis, Float & angle)
		///
		/// \brief	Computes the (axis, angle) representation of this quaternion. Please note that before
		/// 		calling this method, you must ensure that the norm of the quaternion is 1 (i.e. this
		/// 		quaternion is a rotation).
		///
		/// \author	F. Lamarche, Université de Rennes 1
		/// \date	02/03/2016
		///
		/// \param [in,out]	axis 	The axis.
		/// \param [in,out]	angle	The angle.
		////////////////////////////////////////////////////////////////////////////////////////////////////
		void getAxisAngle(Vector<Float,3> & axis, Float & angle) const
		{
			angle = 2*acos(m_s) ;
			if(angle != 0.0)
			{ axis = m_v / (sqrt(1.0-m_s*m_s)) ; }
			else 
			{ axis = Math::makeVector(1.0,0.0,0.0) ; }
		}
		
		/// <summary>
		/// Computes the spherical linear interpolation between two quaternions
		/// </summary>
		/// <param name="v0">The first quaternion.</param>
		/// <param name="v1">The second quaternion.</param>
		/// <param name="t">The interpolation value in interval [0;1].</param>
		/// <returns></returns>
		static Quaternion slerp(Quaternion v0, Quaternion v1, Float t) {
			// Only unit quaternions are valid rotations. Normalize to avoid undefined behavior.
			v0.normalize();
			v1.normalize();
			// Compute the cosine of the angle between the two vectors.
			double dot = v0.s()*v1.s()+v0.v()*v1.v();// dot_product(v0, v1);
			// If the dot product is negative, slerp won't take
			// the shorter path. Note that v1 and -v1 are equivalent when
			// the negation is applied to all four components. Fix by 
			// reversing one quaternion.
			if (dot < 0.0f) {
				v1 = -v1;
				dot = -dot;
			}

			const Float DOT_THRESHOLD = Float(0.9995);
			if (dot > DOT_THRESHOLD) {
				// If the inputs are too close for comfort, linearly interpolate and normalize the result.
				Quaternion result = v0 + (v1 - v0) * t;
				result.normalize();
				return result;
			}
			// Since dot is in range [0, DOT_THRESHOLD], acos is safe
			Float theta_0 = Float(acos(dot));        // theta_0 = angle between input vectors
			Float theta = Float(theta_0 * t);          // theta = angle between v0 and result
			Float sin_theta = Float(sin(theta));     // compute this value only once
			Float sin_theta_0 = Float(sin(theta_0)); // compute this value only once

			Float s0 = Float(cos(theta) - dot * sin_theta / sin_theta_0);  // == sin(theta_0 - theta) / sin(theta_0)
			Float s1 = Float(sin_theta / sin_theta_0);

			return (v0*s0) + (v1*s1);
		}

		/// <summary>
		/// Computes the dot product between two quaternions.
		/// </summary>
		/// <param name="other">The other quaternion.</param>
		/// <returns></returns> 
		Float dot(const Quaternion & other) const
		{
			return m_v * other.m_v + m_s * other.m_s;
		}

		/// <summary>
		/// Equality comparison.
		/// </summary>
		/// <param name="other">The other.</param>
		/// <returns></returns>
		bool operator== (const Quaternion & other) const
		{
			return m_v == other.m_v && m_s == other.m_s;
		}
	};
} 

#endif
