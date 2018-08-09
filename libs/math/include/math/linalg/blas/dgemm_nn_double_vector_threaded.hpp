#pragma once

#include"math/linalg/prototype.hpp"
#include"math/linalg/matrix.hpp"
#include"math/linalg/blas/base.hpp"

namespace fetch
{
namespace math
{
namespace linalg 
{


template<>
class Blas< double, Computes( _C <= _C = _alpha * _A * _B + _beta * _C ),platform::Parallelisation::VECTORISE | platform::Parallelisation::THREADING>
{
public:
  using vector_register_type = typename Matrix< double >::vector_register_type;

  void operator()(double const &alpha, Matrix< double > const &a, Matrix< double > const &b, double const &beta, Matrix< double > &c ) 
  {
    std::size_t j;
    if( (c.height() == 0) || ((c.width() == 0) || (((alpha == 0.0) || (a.width() == 0)) && (beta == 1.0))) ) 
    {
      return;
    } 
    
    if( alpha == 0.0 ) 
    {
      if( beta == 0.0 ) 
      {
        for(j = 0 ; j < c.width(); ++j )
        {
          
          vector_register_type vec_zero(0.0);
          
          auto ret_slice = c.data().slice( c.padded_height() * j, c.height());
          ret_slice.in_parallel().Apply([vec_zero](vector_register_type &vw_c ){
            
            vw_c = vec_zero;  
          });
        }
      }
      else 
      {
        for(j = 0 ; j < c.width(); ++j )
        {
          
          vector_register_type vec_beta(beta);
          
          auto ret_slice = c.data().slice( c.padded_height() * j, c.height());
          auto slice_c = c.data().slice( c.padded_height() * j, c.height());
          ret_slice.in_parallel().Apply([vec_beta](vector_register_type const &vr_c, vector_register_type &vw_c ){
            
            vw_c = vec_beta * vr_c;  
          }, slice_c);
        }
      } 
      
      return;
    } 
    
    
    for(j = 0 ; j < c.width(); ++j )
    {
      pool_.Dispatch([j, alpha, a, b, beta, &c]() {    std::size_t l;
        if( beta == 0.0 ) 
        {
          
          vector_register_type vec_zero(0.0);
          
          auto ret_slice = c.data().slice( c.padded_height() * j, c.height());
          ret_slice.in_parallel().Apply([vec_zero](vector_register_type &vw_c ){
            
            vw_c = vec_zero;  
          });
        }
        else if ( beta != 1.0 )
        {
          
          vector_register_type vec_beta(beta);
          
          auto ret_slice = c.data().slice( c.padded_height() * j, c.height());
          auto slice_c = c.data().slice( c.padded_height() * j, c.height());
          ret_slice.in_parallel().Apply([vec_beta](vector_register_type const &vr_c, vector_register_type &vw_c ){
            
            vw_c = vec_beta * vr_c;  
          }, slice_c);
        } 
        
        for(l = 0 ; l < a.width(); ++l )
        {
          double temp;
          temp = alpha * b(l, j);
          
          vector_register_type vec_temp(temp);
          
          auto ret_slice = c.data().slice( c.padded_height() * j, c.height());
          auto slice_c = c.data().slice( c.padded_height() * j, c.height());
          auto slice_a = a.data().slice( a.padded_height() * l, c.height());
          ret_slice.in_parallel().Apply([vec_temp](vector_register_type const &vr_c, vector_register_type const &vr_a, vector_register_type &vw_c ){
            
            vw_c = vr_c + vec_temp * vr_a;  
          }, slice_c, slice_a);
        }
      });
    
      pool_.Wait();}
    return;
    
  }
private:
  threading::Pool pool_;
};


} // namespace linalg
} // namespace math
} // namepsace fetch