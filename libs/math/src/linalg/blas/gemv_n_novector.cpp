//------------------------------------------------------------------------------
//
//   Copyright 2018-2020 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#include "math/linalg/blas/base.hpp"
#include "math/linalg/blas/gemv_n.hpp"
#include "math/linalg/prototype.hpp"
#include "math/tensor/tensor_view.hpp"

namespace fetch {
namespace math {
namespace linalg {

template <typename S, uint64_t V>
void Blas<S, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
          Computes(_y <= _alpha * _A * _x + _beta * _y), V>::operator()(Type const alpha,
                                                                        TensorView<Type> const a,
                                                                        TensorView<Type> const x,
                                                                        int const              incx,
                                                                        Type const             beta,
                                                                        TensorView<Type>       y,
                                                                        int const incy) const
{
  Type temp;
  int  i;
  int  iy;
  int  j;
  int  jx;
  int  kx;
  int  ky;
  int  lenx;
  int  leny;
  if ((int(a.height()) == 0) ||
      ((int(a.width()) == 0) || ((alpha == Type{0}) && (beta == Type{1}))))
  {
    return;
  }

  lenx = int(a.width());
  leny = int(a.height());
  if (incx > 0)
  {
    kx = 1;
  }
  else
  {
    kx = 1 + (-(-1 + lenx) * incx);
  }

  if (incy > 0)
  {
    ky = 1;
  }
  else
  {
    ky = 1 + (-(-1 + leny) * incy);
  }

  if (beta != Type{1})
  {
    if (incy == 1)
    {
      if (beta == Type{0})
      {
        for (i = 0; i < leny; ++i)
        {
          y[i] = Type{0};
        }
      }
      else
      {
        for (i = 0; i < leny; ++i)
        {
          y[i] = beta * y[i];
        }
      }
    }
    else
    {
      iy = -1 + ky;
      if (beta == Type{0})
      {
        for (i = 0; i < leny; ++i)
        {
          y[iy] = Type{0};
          iy    = iy + incy;
        }
      }
      else
      {
        for (i = 0; i < leny; ++i)
        {
          y[iy] = beta * y[iy];
          iy    = iy + incy;
        }
      }
    }
  }

  if (alpha == Type{0})
  {
    return;
  }

  jx = -1 + kx;
  if (incy == 1)
  {
    for (j = 0; j < int(a.width()); ++j)
    {
      temp = alpha * x[jx];
      for (i = 0; i < int(a.height()); ++i)
      {
        y[i] = y[i] + temp * a(i, j);
      }

      jx = jx + incx;
    }
  }
  else
  {
    for (j = 0; j < int(a.width()); ++j)
    {
      temp = alpha * x[jx];
      iy   = -1 + ky;
      for (i = 0; i < int(a.height()); ++i)
      {
        y[iy] = y[iy] + temp * a(i, j);
        iy    = iy + incy;
      }

      jx = jx + incx;
    }
  }
}

template class Blas<double, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
                    Computes(_y <= _alpha * _A * _x + _beta * _y),
                    platform::Parallelisation::NOT_PARALLEL>;
template class Blas<float, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
                    Computes(_y <= _alpha * _A * _x + _beta * _y),
                    platform::Parallelisation::NOT_PARALLEL>;
template class Blas<
    fetch::fixed_point::FixedPoint<16, 16>, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
    Computes(_y <= _alpha * _A * _x + _beta * _y), platform::Parallelisation::NOT_PARALLEL>;
template class Blas<
    fetch::fixed_point::FixedPoint<32, 32>, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
    Computes(_y <= _alpha * _A * _x + _beta * _y), platform::Parallelisation::NOT_PARALLEL>;
template class Blas<
    fetch::fixed_point::FixedPoint<64, 64>, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
    Computes(_y <= _alpha * _A * _x + _beta * _y), platform::Parallelisation::NOT_PARALLEL>;
template class Blas<double, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
                    Computes(_y <= _alpha * _A * _x + _beta * _y),
                    platform::Parallelisation::THREADING>;
template class Blas<float, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
                    Computes(_y <= _alpha * _A * _x + _beta * _y),
                    platform::Parallelisation::THREADING>;
template class Blas<
    fetch::fixed_point::FixedPoint<16, 16>, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
    Computes(_y <= _alpha * _A * _x + _beta * _y), platform::Parallelisation::THREADING>;
template class Blas<
    fetch::fixed_point::FixedPoint<32, 32>, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
    Computes(_y <= _alpha * _A * _x + _beta * _y), platform::Parallelisation::THREADING>;
template class Blas<
    fetch::fixed_point::FixedPoint<64, 64>, Signature(_y <= _alpha, _A, _x, _n, _beta, _y, _m),
    Computes(_y <= _alpha * _A * _x + _beta * _y), platform::Parallelisation::THREADING>;

}  // namespace linalg
}  // namespace math
}  // namespace fetch
