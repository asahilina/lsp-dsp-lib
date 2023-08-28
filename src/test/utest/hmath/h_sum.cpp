/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>
#include <lsp-plug.in/test-fw/helpers.h>

#ifdef ARCH_ARM
    #define TOLERANCE 1e-3
#endif

#ifndef TOLERANCE
    #define TOLERANCE 1e-4
#endif

namespace lsp
{
    namespace generic
    {
        float h_sum(const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float h_sum(const float *src, size_t count);
        }

        namespace avx
        {
            float h_sum(const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float h_sum(const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float h_sum(const float *src, size_t count);
        }
    )

    typedef float (* h_sum_t)(const float *src, size_t count);
}

UTEST_BEGIN("dsp.hmath", h_sum)

    void call(const char *label, size_t align, h_sum_t func1, h_sum_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize_sign();

                // Call functions
                float a = func1(src, count);
                float b = func2(src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if (!float_equals_adaptive(a, b, TOLERANCE))
                {
                    src.dump("src1");
                    UTEST_FAIL_MSG("Result of function 1 (%f) differs result of function 2 (%f)", a, b);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func);

        IF_ARCH_X86(CALL(generic::h_sum, sse::h_sum, 16));
        IF_ARCH_X86(CALL(generic::h_sum, avx::h_sum, 32));
        IF_ARCH_ARM(CALL(generic::h_sum, neon_d32::h_sum, 16));
        IF_ARCH_AARCH64(CALL(generic::h_sum, asimd::h_sum, 16));
    }
UTEST_END
