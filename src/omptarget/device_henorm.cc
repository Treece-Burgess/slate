// Copyright (c) 2017-2020, University of Tennessee. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// This program is free software: you can redistribute it and/or modify it under
// the terms of the BSD 3-Clause license. See the accompanying LICENSE file.

#include "slate/Exception.hh"
#include "slate/internal/device.hh"

#include "device_util.hh"

#include <cstdio>
#include <complex>

namespace slate {
namespace device {

//------------------------------------------------------------------------------
/// Batched routine that returns the largest absolute value of elements for
/// each tile in Aarray. Sets
///     tiles_maxima[k] = max_{i, j}( abs( A^(k)_(i, j) )),
/// for each tile A^(k), where
/// A^(k) = Aarray[k],
/// k = 0, ..., blockDim.x-1,
/// i = 0, ..., n-1,
/// j = 0, ..., n-1.
///
/// @param[in] n
///     Number of rows and columns of each tile. n >= 0.
///
/// @param[in] Aarray
///     Array in GPU memory of dimension batch_count, containing pointers to tiles,
///     where each Aarray[k] is an n-by-n matrix stored in an lda-by-n array in GPU memory.
///
/// @param[in] lda
///     Leading dimension of each tile. lda >= n.
///
/// @param[out] values
///     Array in GPU memory, dimension batch_count * ldv.
///     - Norm::Max: ldv = 1.
///         On exit, values[k] = max_{i, j} abs( A^(k)_(i, j) )
///         for 0 <= k < batch_count.
///
///     - Norm::One: ldv >= n.
///         On exit, values[k*ldv + j] = sum_{i} abs( A^(k)_(i, j) )
///         for 0 <= k < batch_count, 0 <= j < n.
///
///     - Norm::Inf: for symmetric, same as Norm::One
///
///     - Norm::Max: ldv = 2.
///         On exit,
///             values[k*2 + 0] = scale_k
///             values[k*2 + 1] = sumsq_k
///         where scale_k^2 sumsq_k = sum_{i,j} abs( A^(k)_(i, j) )^2
///         for 0 <= k < batch_count.
///
/// @param[in] ldv
///     Leading dimension of tiles_sums (values) array.
///
/// @param[in] batch_count
///     Size of Aarray. batch_count >= 0.
///
/// @param[in] stream
///     device to execute in.
///
template <typename scalar_t>
void henorm(
    lapack::Norm norm, lapack::Uplo uplo,
    int64_t n,
    scalar_t const* const* Aarray, int64_t lda,
    blas::real_type<scalar_t>* values, int64_t ldv, int64_t batch_count,
    blas::Queue &queue)
{
    using real_t = blas::real_type<scalar_t>;
    ////int64_t nb = 512;

    // quick return
    if (batch_count == 0)
        return;

    //---------
    // max norm
    if (norm == lapack::Norm::Max) {
        if (n == 0) {
            blas::device_memset(values, 0, batch_count, queue);
        }
        else {
            assert(ldv == 1);
            blas::device_memset(values, 0, batch_count, queue);
            queue.sync(); // sync queue before switching to openmp device execution
            // Use omp target offload
            #pragma omp target is_device_ptr(Aarray, values) device(queue.device())
            #pragma omp teams distribute
            for (int64_t k = 0; k < batch_count; ++k) {
                const scalar_t* tileA = Aarray[k];
                // distribute rows (i) to threads
                // note: the max_nan_reduction preserves nans
                #pragma omp parallel for reduction(max_nan_reduction:values[k]) schedule(static, 1)
                for (int64_t i = 0; i < n; ++i) {
                    const scalar_t* rowA = &tileA[i];
                    real_t max = 0;
                    if (uplo == lapack::Uplo::Lower) {
                        for (int64_t j = 0; j < i && j < n; ++j)
                            max = max_nan(max, abs_val(rowA[j*lda]));
                    }
                    else {
                        // Loop backwards (n-1 down to i)
                        for (int64_t j = n-1; j > i; --j)
                            max = max_nan(max, abs_val(rowA[j*lda]));
                    }
                    values[k] = max_nan(values[0], max);
                }
            }
        }
    }
    //---------
    // one norm or inf norm (same values)
    else if (norm == lapack::Norm::One || norm == lapack::Norm::Inf) {
        if (n == 0) {
            blas::device_memset(values, 0, batch_count * n, queue);
        }
        else {
            assert(ldv >= n);
            queue.sync(); // sync queue before switching to openmp device execution
            // use omp target offload
            #pragma omp target is_device_ptr(Aarray, values) device(queue.device())
            #pragma omp teams distribute
            for (int64_t k = 0; k < batch_count; ++k) {
                const scalar_t* tileA = Aarray[k];
                // distribute rows to threads (i)
                #pragma omp parallel for schedule(static, 1)
                for (int64_t idx = 0; idx < n; ++idx) {
                    // get pointer to row_i and corresponding column col_i
                    scalar_t const* row = &tileA[idx];
                    scalar_t const* column = &tileA[lda*idx];
                    real_t sum = 0;
                    // accumulate down row_idx and the symmetric col_idx
                    if (uplo == lapack::Uplo::Lower) {
                        for (int64_t j = 0; j < idx; ++j) // strictly lower
                            sum += abs_val(row[j*lda]);
                        sum += abs_val( std::real( row[idx*lda] )); // diag (real)
                        for (int64_t i = idx + 1; i < n; ++i) // strictly lower
                            sum += abs_val(column[i]);
                    }
                    else {
                        // Loop backwards (n-1 down to i)
                        for (int64_t j = n-1; j > idx; --j) // strictly upper
                            sum += abs_val(row[j*lda]);
                        sum += abs_val( std::real( row[idx*lda] )); // diag (real)
                        for (int64_t i = 0; i < idx && i < n; ++i) // strictly upper
                            sum += abs_val(column[i]);
                    }
                    values[k*ldv + idx] = sum;
                }
            }
        }
    }
    //---------
    // Frobenius norm
    else if (norm == lapack::Norm::Fro) {
        if (n == 0) {
            blas::device_memset(values, 0, batch_count * 2, queue);
        }
        else {
            assert(ldv == 2);
            queue.sync(); // sync queue before switching to openmp device execution
            // use omp target offload
            #pragma omp target is_device_ptr(Aarray, values) device(queue.device())
            #pragma omp teams distribute
            for (int64_t k = 0; k < batch_count; ++k) {
                const scalar_t* tileA = Aarray[k];
                values[k*2 + 0] = 0; // scale_k
                values[k*2 + 1] = 1; // sumsq_k
                // distribute rows to threads (i)
                #pragma omp parallel for schedule(static, 1)
                for (int64_t i = 0; i < n; ++i) {
                    scalar_t const* rowI = &tileA[i];
                    real_t scale_ki = 0;
                    real_t sumsq_ki = 1;
                    if (uplo == lapack::Uplo::Lower) {
                        for (int64_t j = 0; j < i; ++j)
                            add_sumsq(scale_ki, sumsq_ki, abs_val( rowI[j*lda] ));
                        sumsq_ki *= 2; // double sumsq for symmetric entries
                        // add diagonal (real)
                        add_sumsq(scale_ki, sumsq_ki, abs_val( std::real( rowI[i*lda] )));
                    }
                    else {      // uplo == Upper
                        // Loop backwards (n-1 down to i) to maintain coalesced reads.
                        for (int64_t j = n-1; j > i; --j) // strictly upper
                            add_sumsq(scale_ki, sumsq_ki, abs_val(rowI[j*lda]));
                        sumsq_ki *= 2; // double for symmetric entries
                        // add diagonal (real)
                        add_sumsq(scale_ki, sumsq_ki, abs_val( std::real( rowI[i*lda] )));
                    }
                    // accumulate the scale and sumsq for each k
                    #pragma omp critical
                    {
                        real_t scale_k = values[k*2 + 0];
                        real_t sumsq_k = values[k*2 + 1];
                        if (scale_k > scale_ki) {
                            sumsq_k = sumsq_k + sumsq_ki*(scale_ki / scale_k)*(scale_ki / scale_k);
                            // scale_k stays same
                        }
                        else if (scale_ki != 0) {
                            sumsq_k = sumsq_k*(scale_k / scale_ki)*(scale_k / scale_ki) + sumsq_ki;
                            scale_k = scale_ki;
                        }
                        values[k*2 + 0] = scale_k;
                        values[k*2 + 1] = sumsq_k;

                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
// Explicit instantiations.
template
void henorm(
    lapack::Norm norm, lapack::Uplo uplo,
    int64_t n,
    float const* const* Aarray, int64_t lda,
    float* values, int64_t ldv, int64_t batch_count,
    blas::Queue &queue);

template
void henorm(
    lapack::Norm norm, lapack::Uplo uplo,
    int64_t n,
    double const* const* Aarray, int64_t lda,
    double* values, int64_t ldv, int64_t batch_count,
    blas::Queue &queue);

template
void henorm(
    lapack::Norm norm, lapack::Uplo uplo,
    int64_t n,
    std::complex<float> const* const* Aarray, int64_t lda,
    float* values, int64_t ldv, int64_t batch_count,
    blas::Queue &queue);

template
void henorm(
    lapack::Norm norm, lapack::Uplo uplo,
    int64_t n,
    std::complex<double> const* const* Aarray, int64_t lda,
    double* values, int64_t ldv, int64_t batch_count,
    blas::Queue &queue);

} // namespace device
} // namespace slate
