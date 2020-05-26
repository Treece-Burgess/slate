//------------------------------------------------------------------------------
// Copyright (c) 2017, University of Tennessee
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the University of Tennessee nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF TENNESSEE BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------
// This research was supported by the Exascale Computing Project (17-SC-20-SC),
// a collaborative effort of two U.S. Department of Energy organizations (Office
// of Science and the National Nuclear Security Administration) responsible for
// the planning and preparation of a capable exascale ecosystem, including
// software, applications, hardware, advanced system engineering and early
// testbed platforms, in support of the nation's exascale computing imperative.
//------------------------------------------------------------------------------
// For assistance with SLATE, email <slate-user@icl.utk.edu>.
// You can also join the "SLATE User" Google group by going to
// https://groups.google.com/a/icl.utk.edu/forum/#!forum/slate-user,
// signing in with your Google credentials, and then clicking "Join group".
//------------------------------------------------------------------------------

#ifndef SIMPLIFIED_API_HH
#define SIMPLIFIED_API_HH

//------------------------------------------------------------------------------
//
namespace slate {

//------------------------------------------------------------------------------
// Level 3 BLAS

//-----------------------------------------
// multiply()
template <typename scalar_t>
void multiply(scalar_t alpha, BandMatrix<scalar_t>& A,
                                  Matrix<scalar_t>& B,
              scalar_t beta,      Matrix<scalar_t>& C,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gbmm(alpha, A, B, beta, C, opts);
}
template <typename scalar_t>
void multiply(scalar_t alpha, Matrix<scalar_t>& A,
                              Matrix<scalar_t>& B,
              scalar_t beta,  Matrix<scalar_t>& C,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gemm(alpha, A, B, beta, C, opts);
}
template <typename scalar_t>
void multiply(Side side,
              scalar_t alpha, HermitianBandMatrix<scalar_t>& A,
                                           Matrix<scalar_t>& B,
              scalar_t beta,               Matrix<scalar_t>& C,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    hbmm(side, alpha, A, B, beta, C, opts);
}
template <typename scalar_t>
void multiply(Side side,
              scalar_t alpha, HermitianMatrix<scalar_t>& A,
                                       Matrix<scalar_t>& B,
              scalar_t beta,           Matrix<scalar_t>& C,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    hemm(side, alpha, A, B, beta, C, opts);
}
template <typename scalar_t>
void multiply(Side side,
              scalar_t alpha, SymmetricMatrix<scalar_t>& A,
                                       Matrix<scalar_t>& B,
              scalar_t beta,           Matrix<scalar_t>& C,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    symm(side, alpha, A, B, beta, C, opts);
}
template <typename scalar_t>
void multiply(Side side,
              scalar_t alpha, TriangularMatrix<scalar_t>& A,
                                        Matrix<scalar_t>& B,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    trmm(side, alpha, A, B, opts);
}

//-----------------------------------------
// rankkUpdate()
template <typename scalar_t>
void rankkUpdate(blas::real_type<scalar_t> alpha,          Matrix<scalar_t>& A,
                 blas::real_type<scalar_t> beta,  HermitianMatrix<scalar_t>& C,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    herk(alpha, A, beta, C, opts);
}
template <typename scalar_t>
void rankkUpdate(scalar_t alpha,           Matrix<scalar_t>& A,
                 scalar_t beta,   SymmetricMatrix<scalar_t>& C,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    syrk(alpha, A, beta, C, opts);
}

//-----------------------------------------
// rank2kUpdate()
template <typename scalar_t>
void rank2kUpdate(scalar_t alpha,                           Matrix<scalar_t>& A,
                                                            Matrix<scalar_t>& B,
                  blas::real_type<scalar_t> beta,  HermitianMatrix<scalar_t>& C,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    her2k(alpha, A, B, beta, C, opts);
}
template <typename scalar_t>
void rank2kUpdate(scalar_t alpha,           Matrix<scalar_t>& A,
                                            Matrix<scalar_t>& B,
                  scalar_t beta,   SymmetricMatrix<scalar_t>& C,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    syr2k(alpha, A, B, beta, C, opts);
}

//-----------------------------------------
// triangularSolve()
template <typename scalar_t>
void triangularSolve(Side side,
                     scalar_t alpha, TriangularBandMatrix<scalar_t>& A,
                                                   Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    tbsm(side, alpha, A, B, opts);
}
template <typename scalar_t>
void triangularSolve(Side side,
                     scalar_t alpha, TriangularBandMatrix<scalar_t>& A,
                     Pivots& pivots,               Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    tbsm(side, alpha, A, pivots, B, opts);
}
template <typename scalar_t>
void triangularSolve(Side side,
                     scalar_t alpha, TriangularMatrix<scalar_t>& A,
                                               Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    trsm(side, alpha, A, B, opts);
}

//------------------------------------------------------------------------------
// Linear systems

//-----------------------------------------
// LU

//-----------------------------------------
// luFactor()
template <typename scalar_t>
void luFactor(BandMatrix<scalar_t>& A, Pivots& pivots,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gbtrf(A, pivots, opts);
}
template <typename scalar_t>
void luFactor(Matrix<scalar_t>& A, Pivots& pivots,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    getrf(A, pivots, opts);
}
template <typename scalar_t>
void luFactor(Matrix<scalar_t>& A,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    getrf_nopiv(A, opts);
}

//-----------------------------------------
// luSolve()
template <typename scalar_t>
void luSolve(BandMatrix<scalar_t>& A, Pivots& pivots,
                 Matrix<scalar_t>& B,
             const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gbsv(A, pivots, B, opts);
}
template <typename scalar_t>
void luSolve(Matrix<scalar_t>& A, Pivots& pivots,
             Matrix<scalar_t>& B,
             const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gesv(A, pivots, B, opts);
}

//-----------------------------------------
// luSolveUsingFactor()
template <typename scalar_t>
void luSolveUsingFactor(BandMatrix<scalar_t>& A, Pivots& pivots,
                            Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gbtrs(A, pivots, B, opts);
}
template <typename scalar_t>
void luSolveUsingFactor(Matrix<scalar_t>& A, Pivots& pivots,
                        Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    getrs(A, pivots, B, opts);
}
template <typename scalar_t>
void luSolveUsingFactor(Matrix<scalar_t>& A,
                        Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    getrs_nopiv(A, B, opts);
}

//-----------------------------------------
// luInverseUsingFactor()
template <typename scalar_t>
void luInverseUsingFactor(Matrix<scalar_t>& A, Pivots& pivots,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    getri(A, pivots, opts);
}
template <typename scalar_t>
void luInverseUsingFactor(Matrix<scalar_t>& A, Pivots& pivots,
                          Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())

{
    getri(A, pivots, B, opts);
}

//-----------------------------------------
// Cholesky

//-----------------------------------------
// cholFactor()
template <typename scalar_t>
void cholFactor(HermitianMatrix<scalar_t>& A,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    potrf(A, opts);
}
template <typename scalar_t>
void cholFactor(SymmetricMatrix<scalar_t>& A,
                const std::map<Option, Value>& opts = std::map<Option, Value>(),
                enable_if_t< ! is_complex<scalar_t>::value >* = nullptr)
{
    potrf(A, opts);
}

//-----------------------------------------
// cholSolve()
template <typename scalar_t>
void cholSolve(HermitianMatrix<scalar_t>& A,
                        Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    posv(A, B, opts);
}
template <typename scalar_t>
void cholSolve(SymmetricMatrix<scalar_t>& A,
                        Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>(),
                enable_if_t< ! is_complex<scalar_t>::value >* = nullptr)
{
    posv(A, B, opts);
}

//-----------------------------------------
// cholSolveUsingFactor()
template <typename scalar_t>
void cholSolveUsingFactor(HermitianMatrix<scalar_t>& A,
                                   Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    potrs(A, B, opts);
}
template <typename scalar_t>
void cholSolveUsingFactor(SymmetricMatrix<scalar_t>& A,
                                   Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>(),
                enable_if_t< ! is_complex<scalar_t>::value >* = nullptr)
{
    potrs(A, B, opts);
}

//-----------------------------------------
// cholInverseUsingFactor()
template <typename scalar_t>
void cholInverseUsingFactor(HermitianMatrix<scalar_t>& A,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    potri(A, opts);
}

//-----------------------------------------
// Symmetric indefinite -- block Aasen's

//-----------------------------------------
// indefiniteFactor()
template <typename scalar_t>
void indefiniteFactor(HermitianMatrix<scalar_t>& A, Pivots& pivots,
                           BandMatrix<scalar_t>& T, Pivots& pivots2,
                               Matrix<scalar_t>& H,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    hetrf(A, T, H, opts);
}
template <typename scalar_t>
void indefiniteFactor(SymmetricMatrix<scalar_t>& A, Pivots& pivots,
                           BandMatrix<scalar_t>& T, Pivots& pivots2,
                               Matrix<scalar_t>& H,
                const std::map<Option, Value>& opts = std::map<Option, Value>(),
                enable_if_t< ! is_complex<scalar_t>::value >* = nullptr)
{
    hetrf(A, T, H, opts);
}

//-----------------------------------------
// indefiniteSolve()
template <typename scalar_t>
void indefiniteSolve(HermitianMatrix<scalar_t>& A, Pivots& pivots,
                          BandMatrix<scalar_t>& T, Pivots& pivots2,
                              Matrix<scalar_t>& H,
                              Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    hesv(A, B, opts);
}
template <typename scalar_t>
void indefiniteSolve(SymmetricMatrix<scalar_t>& A, Pivots& pivots,
                          BandMatrix<scalar_t>& T, Pivots& pivots2,
                              Matrix<scalar_t>& H,
                              Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>(),
                enable_if_t< ! is_complex<scalar_t>::value >* = nullptr)
{
    hesv(A, B, opts);
}

//-----------------------------------------
// indefiniteSolveUsingFactor()
template <typename scalar_t>
void indefiniteSolveUsingFactor(HermitianMatrix<scalar_t>& A, Pivots& pivots,
                                     BandMatrix<scalar_t>& T, Pivots& pivots2,
                                         Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    hetrs(A, T, B, opts);
}
template <typename scalar_t>
void indefiniteSolveUsingFactor(SymmetricMatrix<scalar_t>& A, Pivots& pivots,
                                     BandMatrix<scalar_t>& T, Pivots& pivots2,
                                         Matrix<scalar_t>& B,
                const std::map<Option, Value>& opts = std::map<Option, Value>(),
                enable_if_t< ! is_complex<scalar_t>::value >* = nullptr)
{
    hetrs(A, T, B, opts);
}

//------------------------------------------------------------------------------
// Least squares

//-----------------------------------------
// leastSquaresSolve()
template <typename scalar_t>
void leastSquaresSolve(Matrix<scalar_t>& A, TriangularFactors<scalar_t>& T,
                       Matrix<scalar_t>& BX,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gels(A, T, BX, opts);
}

//------------------------------------------------------------------------------
// QR, LQ, ...

//-----------------------------------------
// QR

//-----------------------------------------
// qrFactor()
template <typename scalar_t>
void qrFactor(Matrix<scalar_t>& A, TriangularFactors<scalar_t>& T,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    geqrf(A, T, opts);
}

//-----------------------------------------
// qrMultiplyByQ()
template <typename scalar_t>
void qrMultiplyByQ(Side side, Op op,
                   Matrix<scalar_t>& A, TriangularFactors<scalar_t>& T,
                   Matrix<scalar_t>& C,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    unmqr(side, op, A, T, C, opts);
}

//-----------------------------------------
// LQ

//-----------------------------------------
// lqFactor()
template <typename scalar_t>
void lqFactor(Matrix<scalar_t>& A, TriangularFactors<scalar_t>& T,
              const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    gelqf(A, T, opts);
}

//-----------------------------------------
// lqMultiplyByQ()
template <typename scalar_t>
void lqMultiplyByQ(Side side, Op op,
                   Matrix<scalar_t>& A, TriangularFactors<scalar_t>& T,
                   Matrix<scalar_t>& C,
                const std::map<Option, Value>& opts = std::map<Option, Value>())
{
    unmlq(side, op, A, T, C, opts);
}

//------------------------------------------------------------------------------
// Eigenvalues and SVD

//-----------------------------------------
// Eigenvalue decomposition

//-----------------------------------------
// SVD

} // namespace slate

#endif // SIMPLIFIED_API_HH
