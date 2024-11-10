/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 нояб. 2024 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PRIVATE_DSP_ARCH_GENERIC_PAN_H_
#define PRIVATE_DSP_ARCH_GENERIC_PAN_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace generic
    {

        void depan_lin(float *dst, const float *l, const float *r, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float sl  = fabsf(l[i]);
                const float sr  = fabsf(r[i]);
                const float den = sl + sr;
                dst[i]          = (den > 1e-5f) ? sr / den : 0.5f;
            }
        }

        void depan_panl(float *dst, const float *l, const float *r, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float sl  = l[i] * l[i];
                const float sr  = r[i] * r[i];
                const float den = sl + sr;
                dst[i]          = (den > 1e-10f) ? sr / den : 0.5f;
            }
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PAN_H_ */
