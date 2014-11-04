/*
    libmaus
    Copyright (C) 2009-2014 German Tischler
    Copyright (C) 2011-2014 Genome Research Limited

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#if ! defined(LIBMAUS_MATH_UNSIGNEDINTEGER_HPP)
#define LIBMAUS_MATH_UNSIGNEDINTEGER_HPP

#include <cstdlib>
#include <ostream>
#include <iomanip>
#include <vector>
#include <libmaus/types/types.hpp>

#include <iostream>

namespace libmaus
{
	namespace math
	{
		template<size_t k> struct UnsignedInteger;
		template<size_t k> UnsignedInteger<k> operator*(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
		template<size_t k> UnsignedInteger<k> operator/(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
		template<size_t k> UnsignedInteger<k> operator%(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
		template<size_t k> std::pair< UnsignedInteger<k>,UnsignedInteger<k> > divmod(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
		template<size_t k> std::ostream & operator<<(std::ostream & out, UnsignedInteger<k> const & A);
		
		template<size_t k>
		struct UnsignedInteger
		{
			private:
			uint32_t A[k];
			
			void erase()
			{			
				for ( size_t i = 0; i < k; ++i )
					A[i] = 0;
			}
			
			public:
			template<size_t l>
			friend struct UnsignedInteger;
			
			UnsignedInteger()
			{
				erase();
			}
			
			uint32_t operator[](size_t i) const
			{
				return A[i];
			}
			
			template<size_t l>
			UnsignedInteger(UnsignedInteger<l> const & O)
			{
				for ( size_t i = 0; i < std::min(k,l); ++i )
					A[i] = O.A[i];
				for ( size_t i = l; i < k; ++i )
					A[i] = 0;
			}

			UnsignedInteger(uint64_t const a)
			{
				if ( k >= 2 )
				{
					A[0] = static_cast<uint32_t>((a >>  0) & 0xFFFFFFFFull);
					A[1] = static_cast<uint32_t>((a >> 32) & 0xFFFFFFFFull);
					for ( size_t i = 2; i < k; ++i )
						A[i] = 0;
				}
				else if ( k == 1 )
				{
					A[0] = static_cast<uint32_t>((a >>  0) & 0xFFFFFFFFull);					
				}
				else
				{
				
				}
			}
			
			UnsignedInteger & operator=(UnsignedInteger const & O)
			{
				for ( size_t i = 0; i < k; ++i )
					A[i] = O.A[i];
				return *this;
			}
			
			bool operator==(UnsignedInteger<k> const & O) const
			{
				bool eq = true;
				for ( size_t i = 0; i < k; ++i )
					eq = eq && (A[i] == O.A[i]);
				return eq;
			}

			bool operator!=(UnsignedInteger<k> const & O) const
			{
				return !(operator==(O));
			}
			
			bool operator<(UnsignedInteger<k> const & O) const
			{
				for ( size_t i = 0; i < k; ++i )
					if ( A[k-i-1] != O.A[k-i-1] )
					{
						if ( A[k-i-1] < O.A[k-i-1] )					
							return true;
						else
							return false;
					}
					
				return false;
			}

			bool operator>(UnsignedInteger<k> const & O) const
			{
				for ( size_t i = 0; i < k; ++i )
					if ( A[k-i-1] != O.A[k-i-1] )
					{
						if ( A[k-i-1] < O.A[k-i-1] )					
							return false;
						else
							return true;
					}
					
				return false;
			}

			bool operator>=(UnsignedInteger<k> const & O) const
			{
				for ( size_t i = 0; i < k; ++i )
					if ( A[k-i-1] != O.A[k-i-1] )
					{
						if ( A[k-i-1] < O.A[k-i-1] )					
							return false;
						else
							return true;
					}
					
				return true;
			}

			bool operator<=(UnsignedInteger<k> const & O) const
			{
				for ( size_t i = 0; i < k; ++i )
					if ( A[k-i-1] != O.A[k-i-1] )
					{
						if ( A[k-i-1] < O.A[k-i-1] )					
							return true;
						else
							return false;
					}
					
				return true;
			}
			
			/**
			 * left shift by s bits (multiply by 2^s)
			 **/
			UnsignedInteger<k> & operator<<=(size_t s)
			{
				// shift by this many full words
				size_t const w = (s >> 5);

				// shift by w full words
				if ( w < k )
				{
					for ( size_t i = 0; i < k; ++i )
						A[k-i-1] = A[k-i-1-w];
					for ( size_t i = 0; i < w; ++i )
						A[i] = 0;
				}
				// shift is too large, erase words
				else
				{
					erase();
				}
				
				// bit shift
				size_t const q = s & 31;

				if ( q )
				{
					for ( size_t i = 1; i < k; ++i )
					{
						A[k-i] <<= q;
						A[k-i] |= A[k-i-1] >> (32-q);
					}
					
					A[0] <<= q;
				}
								
				return *this;
			}
			
			UnsignedInteger<k> & operator+=(UnsignedInteger<k> const & O)
			{
				if ( k )
				{
					uint64_t sum = static_cast<uint64_t>(A[0]) + static_cast<uint64_t>(O.A[0]);
					
					A[0] = static_cast<uint32_t>(sum & 0xFFFFFFFFULL);
					
					if ( k > 1 )
					{
						uint64_t carry = (sum >> 32) & 0xFFFFFFFFULL;
						
						for ( size_t i = 1; i < k; ++i )
						{
							sum = static_cast<uint64_t>(A[i]) + static_cast<uint64_t>(O.A[i]) + carry;
							A[i] = static_cast<uint32_t>(sum & 0xFFFFFFFFULL);
							carry = (sum >> 32) & 0xFFFFFFFFULL;
						}
					}
				}
				
				return *this;
			}

			UnsignedInteger<k> & operator-=(UnsignedInteger<k> const & O)
			{
				if ( k )
				{
					int64_t dif = static_cast<int64_t>(A[0]) - static_cast<int64_t>(O.A[0]);
					int64_t carry;
					
					if ( dif < 0 )
					{
						A[0] = static_cast<uint32_t>((dif + 0x100000000ll) & 0xFFFFFFFF);
						carry = 1;
					}
					else
					{
						A[0] = dif;
						carry = 0;
					}
					
					if ( k > 1 )
					{
						for ( size_t i = 1; i < k; ++i )
						{
							dif = static_cast<int64_t>(A[i]) - ( static_cast<int64_t>(O.A[i]) + carry );
							
							if ( dif < 0 )
							{
								A[i] = static_cast<uint32_t>((dif + 0x100000000ll) & 0xFFFFFFFF);
								carry = 1;
							}
							else
							{
								A[i] = dif;
								carry = 0;
							}
						}
					}
				}
				
				return *this;
			}

			friend UnsignedInteger<k> operator*<>(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
			friend UnsignedInteger<k> operator/<>(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
			friend std::pair< UnsignedInteger<k>,UnsignedInteger<k> > divmod<>(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B);
			friend std::ostream & operator<< <>(std::ostream & out, UnsignedInteger<k> const & A);
			
			UnsignedInteger<k> & operator/=(UnsignedInteger<k> const & A)
			{
				UnsignedInteger<k> R = *this / A;
				*this = R;
				return *this;
			}

			UnsignedInteger<k> & operator%=(UnsignedInteger<k> const & A)
			{
				UnsignedInteger<k> R = *this % A;
				*this = R;
				return *this;
			}
		};
		
		template<size_t i, size_t j, size_t k>
		struct UnsignedInteger_multiply_level2
		{
			static void multiply(uint32_t const * const A, uint32_t const * const B, uint32_t * const R)
			{
				uint64_t const prod = 
					static_cast<uint64_t>(A[i]) * static_cast<uint64_t>(B[j]);
				R [ i + j ] += static_cast<uint32_t>(prod & 0xFFFFFFFFULL);
					
				if ( i+j+1 < k )
					R[i+j+1] += static_cast<uint32_t>((prod >> 32) & 0xFFFFFFFFULL);

				UnsignedInteger_multiply_level2<i,j+1,k>::multiply(A,B,R);
			}		
		};
		
		template<size_t i, size_t k>
		struct UnsignedInteger_multiply_level2<i,k,k>
		{
			static void multiply(uint32_t const * const, uint32_t const * const, uint32_t * const)
			{
			}			
		};

		template<size_t i, size_t k>
		struct UnsignedInteger_multiply_level1
		{
			static void multiply(uint32_t const * const A, uint32_t const * const B, uint32_t * const R)
			{
				UnsignedInteger_multiply_level2<i,0,k-i>::multiply(A,B,R);
				UnsignedInteger_multiply_level1<i+1,k>::multiply(A,B,R);
			}
		};

		template<size_t k>
		struct UnsignedInteger_multiply_level1<k,k>
		{
			// level 1 loop termination, intentionally left blank
			static void multiply(uint32_t const * const, uint32_t const * const, uint32_t * const)
			{
			}
		};
		
		template<size_t k>
		UnsignedInteger<k> operator*(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B)
		{
			UnsignedInteger<k> R;			
			UnsignedInteger_multiply_level1<0,k>::multiply(&(A.A[0]),&(B.A[0]),&(R.A[0]));
			return R;
		}

		template<size_t k>
		UnsignedInteger<k> operator+(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B)
		{
			UnsignedInteger<k> R = A;			
			R += B;
			return R;
		}

		template<size_t k>
		UnsignedInteger<k> operator-(UnsignedInteger<k> const & A, UnsignedInteger<k> const & B)
		{
			UnsignedInteger<k> R = A;			
			R -= B;
			return R;
		}

		/**
		 * division
		 **/
		template<size_t k> UnsignedInteger<k> operator/(UnsignedInteger<k> const & rA, UnsignedInteger<k> const & rB)
		{
			if ( rB > rA )
				return UnsignedInteger<k>(0);
		
			UnsignedInteger<k+1> A(rA);
			UnsignedInteger<k+1> B(rB);
			
			std::vector< UnsignedInteger<k+1> > div;
			std::vector< UnsignedInteger<k+1> > sub;
			
			UnsignedInteger<k+1> M(B);
			UnsignedInteger<k+1> D(1);
			
			// generate B * 2^k as long as it does not exceed A
			while ( M <= A )
			{
				div.push_back(M);
				sub.push_back(D);
								
				D <<= 1;
				M <<= 1;
			}
			
			UnsignedInteger<k+1> R;
			while ( div.size() )
			{
				if ( div.back() <= A )
				{
					R += sub.back();
					A -= div.back();					
				}
				
				div.pop_back();
				sub.pop_back();
			}
			
			return UnsignedInteger<k>(R);
		}


		/**
		 * modulo operator
		 **/
		template<size_t k> UnsignedInteger<k> operator%(UnsignedInteger<k> const & rA, UnsignedInteger<k> const & rB)
		{
			if ( rB > rA )
				return rA;
		
			UnsignedInteger<k+1> A(rA);
			UnsignedInteger<k+1> B(rB);
			
			std::vector< UnsignedInteger<k+1> > div;
			
			UnsignedInteger<k+1> M(B);
			
			while ( M <= A )
			{
				div.push_back(M);
				M <<= 1;
			}
			
			while ( div.size() )
			{
				if ( div.back() <= A )
					A -= div.back();
				
				div.pop_back();
			}
			
			return UnsignedInteger<k>(A);
		}

		/**
		 * simultaneously compute result of division and rest of division
		 **/
		template<size_t k> 
		std::pair< UnsignedInteger<k>,UnsignedInteger<k> > divmod(UnsignedInteger<k> const & rA, UnsignedInteger<k> const & rB)
		{
			if ( rB > rA )
				return std::pair< UnsignedInteger<k>,UnsignedInteger<k> > ( UnsignedInteger<k>(0), rA );
		
			UnsignedInteger<k+1> A(rA);
			UnsignedInteger<k+1> B(rB);
			
			std::vector< UnsignedInteger<k+1> > div;
			std::vector< UnsignedInteger<k+1> > sub;
			
			UnsignedInteger<k+1> M(B);
			UnsignedInteger<k+1> D(1);
			
			// generate B * 2^k as long as it does not exceed A
			while ( M <= A )
			{
				div.push_back(M);
				sub.push_back(D);
								
				D <<= 1;
				M <<= 1;
			}
			
			UnsignedInteger<k+1> R;
			while ( div.size() )
			{
				if ( div.back() <= A )
				{
					R += sub.back();
					A -= div.back();					
				}
				
				div.pop_back();
				sub.pop_back();
			}
			
			return std::pair< UnsignedInteger<k>,UnsignedInteger<k> >(
				UnsignedInteger<k>(R),
				UnsignedInteger<k>(A)
			);
		
		}

		template<size_t k> std::ostream & operator<<(std::ostream & out, UnsignedInteger<k> const & A)
		{
			// hex
			if ( out.flags() & std::ios::hex )
			{
				std::streamsize const w = out.width();
				char const f = out.fill();

				for ( unsigned int i = 0; i < k; ++i )
					out << std::setw(0) << std::setfill('0') << std::setw(8) << A.A[k-i-1];
				
				out.width(w);
				out.fill(f);
			}
			// treat rest as decimal, no support for octal yet
			else
			{
				UnsignedInteger<k> V(A);
				UnsignedInteger<k> T(10);
				std::vector<int> D;
				
				while ( V != UnsignedInteger<k>(0) )
				{
					std::pair< UnsignedInteger<k>,UnsignedInteger<k> > M = divmod( V, T );
					D.push_back(M.second.A[0]);
					V = M.first;
				}
				
				while ( D.size() )
				{
					out << D.back();
					D.pop_back();
				}
			}
			
			return out;
		}
	}
}
#endif