#pragma once
#include <algorithm>
#include <array>
#include <vector>
#include <cmath>
#include <limits>
#include <numeric>
#undef max
#undef min

namespace geo
{
	class vector2
	{
	public:
		vector2( )
		{
			x = y = 0.f;
		}
		vector2( float fx, float fy )
		{
			x = fx;
			y = fy;
		}

		float x, y;

		vector2 operator+( const vector2& input ) const
		{
			return vector2{ x + input.x, y + input.y };
		}
		vector2 operator-( const vector2& input ) const
		{
			return vector2{ x - input.x, y - input.y };
		}
		vector2 operator/( float input ) const
		{
			return vector2{ x / input, y / input };
		}
		vector2 operator*( float input ) const
		{
			return vector2{ x * input, y * input };
		}
		vector2& operator-=( const vector2& v )
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}
		vector2& operator/=( float input )
		{
			x /= input;
			y /= input;
			return *this;
		}
		vector2& operator*=( float input )
		{
			x *= input;
			y *= input;
			return *this;
		}

		float length( ) const
		{
			return std::sqrt( ( x * x ) + ( y * y ) );
		}

		vector2 normalized( ) const
		{
			return { x / length( ), y / length( ) };
		}

		float dot_product( vector2 input ) const
		{
			return ( x * input.x ) + ( y * input.y );
		}

		float distance( vector2 input ) const
		{
			return ( *this - input ).length( );
		}

		bool empty( ) const
		{
			return x == 0.f && y == 0.f;
		}
	};

	class vector3
	{
	public:
		vector3( )
		{
			x = y = z = 0.f;
		}
		vector3( float fx, float fy, float fz )
		{
			x = fx;
			y = fy;
			z = fz;
		}

		float x, y, z;

		vector3 operator+( const vector3& input ) const
		{
			return vector3{ x + input.x, y + input.y, z + input.z };
		}
		vector3 operator-( const vector3& input ) const
		{
			return vector3{ x - input.x, y - input.y, z - input.z };
		}
		vector3 operator/( float input ) const
		{
			return vector3{ x / input, y / input, z / input };
		}
		vector3 operator*( float input ) const
		{
			return vector3{ x * input, y * input, z * input };
		}
		vector3& operator-=( const vector3& v )
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;

			return *this;
		}
		vector3& operator/=( float input )
		{
			x /= input;
			y /= input;
			z /= input;
			return *this;
		}
		vector3& operator*=( float input )
		{
			x *= input;
			y *= input;
			z *= input;
			return *this;
		}
		bool operator==( const vector3& input ) const
		{
			return x == input.x && y == input.y && z == input.z;
		}

		float length_sqr( ) const
		{
			return ( x * x ) + ( y * y ) + ( z * z );
		}

		float length( ) const
		{
			return std::sqrt( length_sqr( ) );
		}

		float length_2d( ) const
		{
			return std::sqrt( ( x * x ) + ( y * y ) );
		}

		vector3 normalized( ) const
		{
			return { x / length( ), y / length( ), z / length( ) };
		}

		float dot_product( vector3 input ) const
		{
			return ( x * input.x ) + ( y * input.y ) + ( z * input.z );
		}

		float distance( vector3 input ) const
		{
			return ( *this - input ).length( );
		}

		float distance_2d( vector3 input ) const
		{
			return ( *this - input ).length_2d( );
		}

		void clamp( )
		{
			std::clamp( x, -89.f, 89.f );
			std::clamp( y, -180.f, 180.f );

			z = 0.f;
		}

		bool empty( ) const
		{
			return x == 0.f && y == 0.f && z == 0.f;
		}

		bool empty_euler( ) const
		{
			return x == 0.f || y == 0.f;
		}
	};

	using vec2_t = vector2;
	using vec3_t = vector3;
}